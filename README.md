# A new language: April

文档地址：https://github.com/xuyouchun/april/wiki

核心算法：https://github.com/xuyouchun/april/wiki/Algorithms

源码导读：https://github.com/xuyouchun/april/wiki/Source-guidance

## 构建

依赖：g++ (C++20)、CMake ≥ 3.16、Python3、Boost (log/system/filesystem)。

```sh
# CMake（推荐）
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build -j        # 产物: bin/april

# 运行单元测试（需先构建，并在仓库根目录准备好 System.apl）
ctest --test-dir build --output-on-failure

# 传统 Makefile 方式仍然可用
./make
```
