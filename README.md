# ray-tracing-practice
reference: https://raytracing.github.io/books/RayTracingInOneWeekend.html#outputanimage/creatinganimagefile

# ビルド

## 外部ファイル
リポジトリでは動作に必要な次を省いています。
- `images/earthmap.jpg`

## 実行環境
```
❯ g++ --version
Apple clang version 15.0.0 (clang-1500.3.9.4)
Target: arm64-apple-darwin23.5.0
Thread model: posix
InstalledDir: /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin

❯ cmake --version
cmake version 3.27.7

CMake suite maintained and supported by Kitware (kitware.com/cmake).
```

## 手順
プロジェクトフォルダ`ray-tracing-practice`をカレントディレクトリとした上で
```
cd ./build
cmake ../
cmake --build build --target main
mkdir dst
./build/main > dst/hello.ppm
```