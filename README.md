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
InstantiatingHermesRuntime           19971121 ns     19572338 ns           35
CallHostFunctionFromJS1000Times        908896 ns       903850 ns          773
CallJSFunctionWithoutJSI1000Times      773631 ns       763131 ns          914
ExecuteJSIFunction1000Times            837376 ns       832555 ns          827
AccessHostObjectAttribute1000Times    1166008 ns      1150051 ns          608
AccessNativeStateData1000Times        1406017 ns      1393046 ns          501
GetNativeState                            268 ns          268 ns      2614592
EvaluatePreparedCode                    12253 ns        12253 ns        92156
```
