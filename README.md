This project is a React Native app that performs benchmarks oriented at different JSI-related tasks.

Currently it only supports Android and uses [google benchmark](https://github.com/google/benchmark) library to run a series of tests written [here](blob/main/android/app/src/main/cpp/benchmark_jsi.h).

In order to run benchmarks on your own you need to build and install the app. Preferably run benchmarks on a real device and not emulator to get a better sense of the real-life values of the things under test.

## Step 1: Install project dependencies

```bash
yarn start
```

## Step 2: Build Android's app for release

```bash
cd android && ./gradlew assembleRelease
```

## Step 3: Install on Android device

Connect your android phone, and install using ADB:

```bash
adb install ./app/build/outputs/apk/release/app-release.apk
```

## Step 4: Launch the app and see results

Launch AndroidBenchmark app installed in the previous step on your phone.
Once the app opens, the benchmark results will be writted to ADB logcat.
Use the following command to filter the results:

```bash
adb logcat | grep BENCHMARK
```

The output will looks something like that (the below results are from Samsung Galaxy J3 device):

```
-----------------------------------------------------------------------------
Benchmark                                   Time             CPU   Iterations
-----------------------------------------------------------------------------
InstantiatingHermesRuntime           21586660 ns     20671476 ns           35
CallHostFunctionFromJS1000Times        945949 ns       916524 ns          765
CallJSFunctionWithoutJSI1000Times      821180 ns       767397 ns          904
ExecuteJSIFunction1000Times            860390 ns       832706 ns          819
AccessHostObjectAttribute1000Times    1165616 ns      1148329 ns          609
AccessNativeStateData1000Times        1435801 ns      1390908 ns          495
GetNativeState                            279 ns          269 ns      2608473
GetHostObject                            69.3 ns         67.6 ns     10342067
EvaluatePreparedCode                    13979 ns        13159 ns        90922
```
