# April 原生编译（AOT / LLVM）可行性调研

> 状态：调研结论 + 原型计划。数据来自对源码的静态阅读，**没有构建或运行过 april**（沙箱缺 boost），
> 所有工作量为估算，需在原型阶段验证。

## 1. 结论

1. **方向可行**：XIL 是约 16 类指令族的栈式 IR，对象布局、虚表、数组头已是原生内存布局，GC（保守、不移动）与原生代码天然兼容。
2. **泛型比预想的容易**：用户可见的泛型用法很少，核心库的泛型集中在 5 个 `[Internal]` 类型，且都是**编译期可确定**的（见 §2、§3），单态化可行。
3. **真正的工作量在三处**：
   - 解释器**内建命令**（委托调用、Tuple/Array 内部函数）要改成编译器内建，不能直接翻译；
   - **委托**保存的是运行期 `rt_method_base_t*`（带调用类型的指针标记），要换成函数指针/thunk 表示；
   - **异常**与**元数据**（`typeof`、反射）要决定原生侧的表示。
4. rtlib 的 49 个函数可以**原样复用**：它们通过 `ctx.args<T>()` 读解释器栈上的参数块，AOT 调用点用同样布局的栈上参数块调用即可（见 §4.3）。

## 2. 泛型使用统计

| 类别 | 数量 / 位置 | 说明 |
|---|---|---|
| 核心库泛型类型 | `Array<T>`、`Tuple<TItems...>`、`ITuple<TItems...>`、`Delegate<TReturn, TArgs...>`、`MulticastDelegate<TReturn, TArgs...>`（`core_lib/System/ 下的 Object.cs、Array.cs、Tuple.cs、Delegate.cs 等`） | 全部 `[Internal]`；后 4 个是**变参泛型** |
| 核心库泛型方法 | `Array.New<T>`、`ITuple.Get<T>`、`Internal.Array_GetValue<T>`、`Array_SetValue<T>`、`Delegate_Invoke<T>`、`MulticastDelegate_Get<TDelegate>` | `Internal.*` 均为 `extern`，由运行时实现 |
| 用户代码泛型 | `test/unittest/interface/generic_interface.cs`（泛型接口）；`test/projects/Project1`（`Call2<T1>` → `Call3<T2>` 泛型方法链、`Call4<T>`） | 单元测试里仅 2 处 |
| 泛型约束（`where`） | 0 处 | core_lib 与测试里都没有；约束检查在编译器里（最近的提交），不影响代码生成 |
| 核心库规模 | `core_lib` 共 1359 行 C#（`System.cs` 355、`Internal.cs` 208 等） | 小，可全量编译 |

## 3. 对 AOT 的影响

### 3.1 泛型：单态化可行

- 泛型实例在解释器里是命令创建时惰性生成（`rt_generic_type_t` / `rt_generic_method_t`，`commands.cpp` 约 17 处引用）。调用点的类型实参来自 XIL 里的类型引用，**编译期已知**。
- 做法：从入口方法出发做**工作表闭包**——遇到泛型调用/类型实例化，代入类型实参生成实例，再继续扫描其方法体，直到不再产生新实例。`Call2<T1>` → `Call3<T2>` 这种嵌套靠代入完成。
- 变参泛型按**实际元数**展开（`Delegate<int, int, int>` 与 `Delegate<void>` 各是独立实例），与现有的 `params` 泛型参数处理逻辑一致。
- 跨程序集：`System.apl` 里的泛型被用户代码实例化时，实例要生成在**使用方**（类似 C++ 模板），所以 `.apl` 必须继续保留 System 的 XIL。
- **不支持/首版回退到解释器**：运行期才确定类型实参的实例化（例如由 `Type` 值驱动的构造）。目前没看到这种用法，需在原型阶段确认。

### 3.2 内建命令与 `Internal` 函数

| 项 | 现状 | AOT 处理 |
|---|---|---|
| `Delegate_Init` / `Delegate_InitWithCallType` / `Delegate_Invoke` | 解释器**内建命令**（`commands.cpp` ~4490-4780），按字段名 "Method"/"Object" 找偏移，读调用栈布局 | 编译器内建：生成构造与间接调用代码 |
| `MulticastDelegate_GetCount/Get` | 同上 | 编译器内建 |
| `Array_GetValue<T>/SetValue<T>` | 泛型 extern | 编译器内建：内联为带边界检查的下标访问 |
| `Array_GetRank/GetLength/GetLengthOfDimension`、`Tuple_*`、`Reflection_*`、`Math_*`、`Console_*` | 49 个 libfunc（`rtlib.cpp` 注册） | 复用，见 §4.3 |

### 3.3 委托

- 委托值 = `{ Object instance; Method method }`，`method` 是 `rt_method_base_t*` 且低位带调用类型标记（`al::incorp_v/incorp_p`），调用时 `exec_method_of(method)` 取解释器方法。
- AOT 方案：`Method` 字段改存**原生函数指针**（虚/接口调用另存 thunk 或 vtable 槽号），调用类型标记保留。反射里的 `Method` 对象需要从函数指针反查元数据，用一张方法表解决。

## 4. 设计要点

### 4.1 流水线

```
april compile --native
  .cs ─► XIL (现有) ─► 闭包+单态化 ─► LLVM IR ─► 优化 ─► .o ─► 链接 (runtime 静态库) ─► 可执行文件
april run          （默认：编译成 native 后运行，构建结果缓存；`--interpret` 用解释器，保留作开发模式与差分测试基准）
```

### 4.2 XIL → LLVM IR

- 栈式指令按基本块做**符号栈**消去（push/pop 变成 SSA 值），局部变量用 `alloca`（后续由 mem2reg 提升）。
- 类型映射：整数/浮点直接映射；引用类型 `ptr`；struct 按 `core` 里算好的字段偏移生成 LLVM struct；对象头（`rt_type_t*`，数组再加长度）布局保持不变，所以 `get_object_type` 等 mm 辅助函数可继续在 runtime 里使用。
- 虚方法/接口：沿用现有 vtable 布局（`rt_vtable_t`），虚调用 = 加载对象头→类型→vtable→槽；接口调用先沿用现有查找，后续再优化。
- 分配：调用 runtime 的 `new_obj`/`new_array`（已在 `mm`，参数是 `rt_type_t*`，原生代码需要这些类型对象存在 → 见 §4.5）。

### 4.3 复用 rtlib（49 个函数）

**已确认的调用约定**（`rtlib.h` 的 `rtlib_context_t`、`commands.cpp` 的内部调用命令）：

- 函数签名 `void f(rtlib_context_t & ctx)`；参数块是**连续的 8 字节槽**，按声明顺序排布，每个参数占 `ceil(size/8)` 个槽（`rt_arg_t`），基类结构的成员在前。
- 解释器的做法：`argument = 栈顶 - 参数槽总数`；函数返回后**返回值写回参数块的第一个槽**（`ctx.return_`），再弹出 `参数槽数 - 返回值槽数` 个槽。
- 内部函数按**方法名**查表（`get_libfunc("Math_Sqrt")`）。

AOT 调用点：`alloca [N x i64]` → 按声明顺序存入各参数 → 调用同一个函数 → 从槽 0 取返回值。**不需要改任何 rtlib 代码**。`rtlib_context_t` 还带一个 `assembly_analyzer_t &`，只有 3 个函数真正用到：

| 函数 | 用到元数据 | 现状 |
|---|---|---|
| `Tuple_SetRange` | 是（遍历 tuple 字段信息，按偏移/大小拷贝参数） | 实现完整 → AOT 改为**编译期展开**（单态化后字段偏移已知，直接生成 store） |
| `Tuple_GetCount` | 是 | 实现完整 → 编译期常量 |
| `Reflection_GetTypeName` | 是 | **空桩**（返回硬编码 `"AAAA"`） |
| `Tuple_Get` | — | **空桩**（只读了参数，什么都不做） |

其余 45 个（Console 24、Array 3、Math 15、Reflection_GetType 等）完全不依赖元数据，`Reflection_GetType` 只读对象头。热路径函数（`Math_*`）之后可换成 LLVM intrinsic。

### 4.4 GC 对接

- 保守扫描 native 栈 + 寄存器溢出（`setjmp`）已实现，原生代码无需 stack map。
- 需要保证：**内部指针**可识别（已支持）；不把引用藏在非字对齐/加密形式；LLVM 优化不丢弃"仅在栈上存活"的引用（以保守扫描为前提，通常安全，用 `APRIL_GC_STRESS=1` 全量回归）。
- 扫描信息：Phase 2a 的类型解析器现在读 `rt_type_t` 元数据；AOT 若不保留这些元数据，需要改为编译期生成每个类型的引用位图并随类型描述符发出。

### 4.5 运行时元数据

原生代码仍需要：`rt_type_t` 描述符（分配、`typeof`、反射、GC 扫描信息）和方法表（委托、反射）。两种做法：
- **A（低风险）**：可执行文件内嵌 `.apl`，启动时加载元数据，原生代码通过符号表绑定到这些对象。
- **B（独立）**：编译期把类型描述符发成静态数据，脱离 `.apl` 与 `rt`。

建议先做 A，B 作为后续优化。

### 4.6 异常

解释器用 `exception_stack_t` + 方法内 try 区间。原生有两条路：
- LLVM 异常处理（`invoke`/`landingpad` + 自带 personality），与 C++ 运行时配合，最规范，工作量较大；
- 以 `setjmp/longjmp` 风格实现，简单，代价是每个 try 区有开销且需要和 GC 根/栈状态配合。

首版**不支持异常**（遇到包含 try/throw 的方法回退或报错），第二阶段再选方案。

## 5. 风险

| 风险 | 影响 | 缓解 |
|---|---|---|
| 隐含的运行期泛型实例化路径 | 单态化不完整，运行时缺实例 | 原型里加"闭包完整性检查"，缺失时明确报错而不是崩溃 |
| 内建命令数量比预期多 | 编译器内建工作量增加 | 先统计 `commands.cpp` 里按 xil 命令族的分类（见 §6 M0） |
| 解释器与原生行为分歧 | 难以定位 | 同一份单测同时跑两种后端，**输出逐行比对** |
| LLVM 依赖与构建复杂度 | 构建、CI 变重 | LLVM 作为可选组件（CMake option），默认不开 |
| 元数据/反射语义 | 与解释器行为不一致 | 方案 A 保持同一套元数据，行为一致 |

## 6. 原型阶段计划（M0 后修订）

| 阶段 | 内容 | 验收 | 估算 |
|---|---|---|---|
| M0 调研补完 | **已完成**，见 §8 | — | 已用 |
| M1 骨架 | CMake 可选 LLVM；`april compile --native` 入口；闭包扫描（不含泛型）；rtlib 适配器；`Main` 里 `Console.WriteLine` 端到端 | hello world 原生可执行文件 | 2 到 3 周 |
| M2 核心语义 | 局部变量/表达式/控制流（含 switch）、静态/实例方法、struct、数组、字符串、装箱、`cast/is/as` 的运行期类型检查；元数据方案 A | `basic/`（除 `cast.cs` 里 try/catch 部分）、`box/`、`custom_struct/`（除委托）单测原生通过，输出与解释器一致 | 3 到 4 周 |
| M3 接口与委托 | 接口分派、委托/多播委托内建、泛型接口 | `interface/`、`method/`、`custom_struct/delegate` | 3 到 4 周 |
| M4 泛型 | 单态化闭包、泛型方法、变参泛型、Tuple 编译期展开与合成的 `get_ItemN`、跨程序集实例 | `Project1` 泛型链、Tuple 相关用例 | **2 到 4 周**（由 3 到 5 周下调：闭包是静态的，合成方法很简单） |
| M5 异常与收尾 | 异常模型选型与实现（`cast.cs` 需要运行时抛出的 `InvalidCastException`）、GC stress 全量回归、性能基准 | 全部单测双后端一致，stress 通过 | 4 到 6 周 |

合计约 **4 到 5 个月**，不确定性比之前小：泛型和 rtlib 两个最大的未知项已经落实，剩余风险集中在委托、异常和元数据表示。

## 6.1 M1 实现状态

**设计决定**：编译器输出**文本 LLVM IR（.ll）**，不链接 LLVM 的 C++ API。april 本身不依赖 LLVM，不受其 API 变动影响；IR 生成逻辑可以脱离 LLVM 单测。代价是 IR 的有效性由 `clang` 最终校验；需要 **LLVM 15+**（IR 使用不透明指针 `ptr`）。

| 组成 | 位置 | 状态 |
|---|---|---|
| IR 生成器（只依赖标准库，带结构校验） | `modules/native/ll_builder.h` | 完成，`test/native_standalone` 35 项断言通过（ASAN/UBSAN） |
| 原生运行时支撑（C ABI：init / 按名查 rtlib 函数 / 调用） | `modules/native/native_abi.h`、`native_rt.cpp` | 完成，通过编译检查，**未运行** |
| XIL → IR 降级 + 可达方法闭包 | `modules/native/native.cpp`、`include/modules/native.h` | 完成，通过编译检查，**未运行** |
| CLI：`april compile --native`：生成 `.ll` 并构建可执行文件（中间产物在缓存目录） | `modules/april/*` | 完成，已运行 |
| 链接脚本（备用） | `tools/april-native-link.sh` | 完成，**未运行** |
| 构建/运行驱动：clang 版本检查、编译 `.ll`、链接静态库、运行并转发退出码 | `modules/native/driver.{h,cpp}` | 完成，`test/native_standalone/driver_test.cpp`（假 clang++ 脚本）48 项通过；真实 clang **未运行** |
| CLI：`april run` 默认 native（缓存），不支持的程序回退到解释器 | `modules/april/*` | 完成，已运行 |

**M1 支持的子集**：静态方法；基本类型（整数、浮点、bool、char）与字符串；局部变量与实参；`push`（常量/实参/局部）、`pop`（局部/实参/丢弃）、静态调用、内部函数（`Console_*`、`Math_*` 等 rtlib）调用、`ret`。其余一律在编译期报错，错误信息指出不支持的 XIL，例如 `native: unsupported xil: push field`。

**用法**（需要先构建 april，且本机有 clang 15+）：

```sh
bin/april run test/unittest/basic/simple.cs               # 构建（有缓存则跳过）并运行，转发退出码
bin/april run --interpret simple.cs                       # 用解释器运行
bin/april compile --native simple.cs                      # 生成可执行文件 <程序集名>(.exe) 在当前目录
bin/april compile --native -o prog -O2 simple.cs          # -o 指定可执行文件，-O 为 clang 优化级别
```

**JIT（LLVM）**：`bin/april_jit.dll`（Linux 为 `libapril_jit.so`）是链接 LLVM 的共享库，CMake 找到 LLVM（`find_package(LLVM)`，15+）时构建；运行时由 april 加载（先 `$APRIL_JIT`，再 april 同目录、`../lib`）。`april run` 优先用它：IR 文本在内存里交给 `LLVMParseIRInContext`，`default<O2>` 优化，生成宿主机的目标文件（缓存目录里的 `program.o`），再用 ORC JIT 在 april 进程内运行，native 运行时的符号（`april_native_*`）由 april 提供，不需要 clang，也不需要静态库。库缺失、LLVM 的动态库不在 PATH、或者程序用到 native 后端不支持的功能时，依次退到 clang 构建的可执行文件、解释器（`--verbose` 说明原因）。`compile --native` 仍用 clang 做最后的链接。

System.apl 默认包含，不能在命令行里指定（找不到时设置 `APRIL_SYSTEM`；依次找 `$APRIL_SYSTEM`、`<april>/System.apl`、`<april>/../System.apl`、`<april>/../lib/System.apl`、`./System.apl`）。`april run` 的构建放在缓存目录（`$APRIL_CACHE`，默认 Windows 为 `%LOCALAPPDATA%\april\cache`，否则 `$XDG_CACHE_HOME/april` 或 `~/.cache/april`），以源文件内容、编译器、System.apl、静态库和选项为键，没有变化就不重新编译。程序用到 native 后端还不支持的功能，或没有 clang，会在 stderr 提示后回退到解释器。

选项：`--clang <path>`（默认 `$CLANG`，再默认 `clang++`）、`--libs <dir>`（静态库目录，默认依次找 `$APRIL_LIBS`、`<april>/../libs`、`<april>/../build`）、`--no-pie`（链接报重定位错误时使用）。构建时会打印执行的 clang 命令；clang 对 `.ll` 的报错带行号。需要 clang 15+（版本不够会直接报错）。

`tools/april-native-link.sh` 仍可作为手工链接的备用方式。

**已知限制 / 下一步**（M2）：基本块与跳转（`jmp`）、算术/比较指令、实例方法与字段、数组、struct、装箱、类型检查；`Tuple_*` 与 `Reflection_GetTypeName` 需要元数据，目前被明确拒绝。

## 7. 仍待验证的问题

1. **现有单测在解释器上是否全部通过**（决定差分测试基线是否可信）——需要你本机运行。
2. `xil_command_t::external` 的含义和用法（本轮没找到使用点，需要读 compile 端的生成逻辑）。
3. 虚方法调用在测试里如何覆盖（测试和 core_lib 中 C# 关键字 `virtual`/`override` 都是 0 次，疑似 April 用其他写法，或只靠接口/抽象类）。
4. 元数据方案 A 下，原生可执行文件启动时加载 `.apl` 的开销与部署形态（单文件还是附带 `.apl`）。
5. 目标平台：是否仍只做 linux/x86_64。

## 8. M0 调研结论

### 8.1 XIL 指令清单与 LLVM 降级难度

| 指令族 | 子类型 | 降级方式 | 难度 |
|---|---|---|---|
| `push` / `pop` / `pick` | 存储类型 20 种：local、argument、field、array_element、各自的 `_addr`、`_content`，以及 constant、object（type_info/method_info）、params、duplicate、convert、box、unbox | 符号栈消去；addr 类取地址；`params` 是指向变参数据的指针（`Tuple_SetRange` 用）；box/unbox 需要装箱布局 | 中 |
| `call` | `virtual_`、`instance`、`static_`、`internal`、`command`（`delegate_init` / `delegate_init_with_call_type` / `delegate_invoke`） | static/instance 直接调用；virtual 走 vtable；internal 走 rtlib 适配器；command 为委托内建 | 中到高 |
| `jmp` | near/far；`true_`、`false_`、`switch_`、`leave` | `br` / `switch`；`leave` 与异常配合 | 低 |
| `smp` | `label`、`ret`、`throw_`、`rethrow`、`end_finally` | label/ret 直接；后三个属于异常 | ret 低，其余高 |
| `al` / `cmp` / `logic` / `bit` | 8 / 7 / 4 / 7 个操作，对 16 种 xil_type（int8~uint64、float、double、bool、char、ptr、object、string） | 逐一映射到 LLVM 整数/浮点指令（注意有符号与无符号除法、移位） | 低 |
| `cast` | 数值转换 + 类型检查（`is`/`as`/强转） | 数值转换直接；类型检查调 runtime 的子类型判断（需要 `rt_type_t`） | 中 |
| `new_` | `default_`、`array`、`generic`、`stack_alloc`、`stack_allocs` | 调 runtime `new_obj`/`new_array`；`stack_alloc` 用 `alloca`；`generic` 单态化后等同 default | 中 |
| `copy` | `object_copy`、`generic_copy`；kind：`default_`、`reverse`、`zero`、`ret` | `memcpy`/`memset`，注意 struct 按值语义 | 中 |
| `init` | `array_begin`、`array_element`、`array_end` | 数组初始化器，逐元素 store | 低 |
| `external` | （含义待查） | — | 未知 |

### 8.2 语言特性在测试与 core_lib 中的使用（按行数，词边界匹配）

| 特性 | 用量 | 对计划的影响 |
|---|---|---|
| `try/catch` | 仅 `basic/cast.cs`（3 组 `catch (InvalidCastException)`），用户 `throw`/`finally` 为 0 | 异常只有"运行时抛出的转换异常"这一种被测到，但 `cast.cs` 离不开它 |
| `switch` | 2 | M2 需要支持 |
| `for` / `while` / `do-while` | 有 | 基础控制流 |
| `interface` | 10 | M3 |
| `delegate`、`ref`/`out` | 1 / 12 | M3 |
| `struct` | 31 | 重点（`custom_struct/` 整个目录） |
| `typeof`、`is`/`as` | 2 / 6 | 需要运行期类型信息 |
| `foreach`、lambda（`=>`）、`yield`、`async`、`unsafe`、`lock` | 0 | 语言未使用，不用管 |

### 8.3 运行期泛型实例化：确认由静态信息驱动

实例化入口是 `rt_pool_t::to_generic_type` / `to_generic_method`（`rt.cpp`），调用方是 `assembly_analyzer_t::get_type` / `__to_generic` 与 `command_creating_context_t::to_generic_type`。它们的输入只有两类：**XIL 里的类型引用**（`mt->args`）和**当前方法的类型实参绑定**（`gp_manager`，取自调用方实例）。没有任何路径由运行期的对象值或 `Type` 值决定要实例化什么。所以从入口方法出发、以 (方法, 类型实参绑定) 为节点的闭包是完整的。

另外：`rt_dynamic_method_t`（"动态方法"）只用于 **Tuple 实例**：每个 `Tuple<...>` 实例按元数合成 `get_Item1..N`，方法体就是 `push argument 0 → push field → ret`。编译器在生成 Tuple 实例时直接合成这几条即可。
