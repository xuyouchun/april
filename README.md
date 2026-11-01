# A new language: April

文档地址：https://github.com/xuyouchun/april/wiki

核心算法：https://github.com/xuyouchun/april/wiki/Algorithms

源码导读：https://github.com/xuyouchun/april/wiki/Source-guidance

## 构建

依赖：g++ (C++20)、CMake ≥ 3.16、Python3、Boost (log/system/filesystem)。

```sh
# CMake（推荐）
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build -j        # 产物: bin/april，以及同目录的 april_base / april_compiler / april_runtime 共享库
                              # (和 april_jit，找到 LLVM 时)；-DAPRIL_SHARED_MODULES=OFF 则全部静态链接进 april

# 运行单元测试（需先构建，并在仓库根目录准备好 System.apl）
ctest --test-dir build --output-on-failure

# 命令行：System.apl 默认包含（不要在命令行里写）
bin/april run hello.cs                      # native 构建（缓存）并运行；--interpret 用解释器
bin/april compile --native -o hello hello.cs # 生成可执行文件
bin/april compile core_lib/System/System.project -o .   # 编译 System.apl

# 传统 Makefile 方式仍然可用
./make
```
