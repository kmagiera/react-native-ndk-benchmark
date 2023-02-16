#include <iostream>
#include <jni.h>
#include <benchmark/benchmark.h>
#include <android/log.h>

#define TAG "BENCHMARK"

#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,    TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN,     TAG, __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,     TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,    TAG, __VA_ARGS__)

#include "benchmark_jsi.h"

struct BenchmarkStream : std::ostream, std::streambuf
{
    BenchmarkStream() : std::ostream(this) {}

    std::vector<char> buffer;

    int overflow(int c)
    {
        buffer.push_back(c);
        return 0;
    }
};

std::string benchmarkResults = "";

void performTests() {
    int argc = 1;
    std::string argument = "";
    char* argv[1] = {"test"};
    benchmark::ConsoleReporter reporter(benchmark::ConsoleReporter::OutputOptions::OO_None);
    BenchmarkStream outStream;
    reporter.SetOutputStream(&outStream);
    benchmark::Initialize(&argc, argv);
    if (benchmark::ReportUnrecognizedArguments(argc, argv)) return;
    benchmark::RunSpecifiedBenchmarks(&reporter, nullptr);

    benchmarkResults = "";
    for (auto c : outStream.buffer) {
        benchmarkResults += c;
    }

    LOGI("%s", benchmarkResults.c_str());
}

jint JNI_OnLoad(JavaVM *vm, void *reserved) {
    // Add your initialization code here
    performTests();

    return JNI_VERSION_1_6;
}
