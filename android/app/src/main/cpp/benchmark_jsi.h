#include <benchmark/benchmark.h>
#include <hermes/hermes.h>
#include <jsi/jsi.h>

using namespace facebook;

static void InstantiatingHermesRuntime(benchmark::State& state) {
    for (auto _ : state) {
        auto runtime =
                facebook::hermes::makeHermesRuntime();
    }
}
BENCHMARK(InstantiatingHermesRuntime);

static void CallHostFunctionFromJS1000Times(benchmark::State& state) {
    auto hermesRuntime = facebook::hermes::makeHermesRuntime();
    jsi::Runtime &rt = *hermesRuntime;
    jsi::Value hostFunction = jsi::Function::createFromHostFunction(
            rt,
            jsi::PropNameID::forAscii(rt, "foo"),
            1,
            [](jsi::Runtime &rt,
                const jsi::Value &thisValue,
                const jsi::Value *args,
                size_t count) -> jsi::Value {
                return jsi::Value(7);
            }
    );
    rt.global().setProperty(rt, "foo", hostFunction);

    auto code = std::make_shared<const jsi::StringBuffer>(R"((() => {
var sum = 0;
return function loop() {
  for (var i = 0; i < 1000; i++) {
    sum += foo(i);
  }
}
})())");
    auto loop = rt.evaluateJavaScript(code, "https://swmansion.com/index.php").asObject(rt).asFunction(rt);

    for (auto _ : state) {
        loop.call(rt);
    }
}
BENCHMARK(CallHostFunctionFromJS1000Times);


static void CallJSFunctionWithoutJSI1000Times(benchmark::State& state) {
    auto hermesRuntime = facebook::hermes::makeHermesRuntime();
    jsi::Runtime &rt = *hermesRuntime;

    auto code = std::make_shared<const jsi::StringBuffer>(R"((() => {
var sum = 0;
function foo(i) {
  return 7;
}
return function loop() {
  for (var i = 0; i < 1000; i++) {
    sum += foo(i);
  }
}
})())");
    auto loop = rt.evaluateJavaScript(code, "https://swmansion.com/index.php").asObject(rt).asFunction(rt);

    for (auto _ : state) {
        loop.call(rt);
    }
}
BENCHMARK(CallJSFunctionWithoutJSI1000Times);


static void ExecuteJSIFunction1000Times(benchmark::State& state) {
    auto hermesRuntime = facebook::hermes::makeHermesRuntime();
    jsi::Runtime &rt = *hermesRuntime;

    auto code = std::make_shared<const jsi::StringBuffer>(R"(
(function foo(i) {
  return 7;
}))");
    auto func = rt.evaluateJavaScript(code, "https://swmansion.com/index.php").asObject(rt).asFunction(rt);

    for (auto _ : state) {
        int sum = 0;
        for (int i = 0; i < 1000; i++) {
            sum += func.call(rt, jsi::Value(i)).asNumber();
        }
    }
}
BENCHMARK(ExecuteJSIFunction1000Times);

static void EvaluatePreparedCode(benchmark::State& state) {
    auto hermesRuntime = facebook::hermes::makeHermesRuntime();
    jsi::Runtime &rt = *hermesRuntime;

    auto code = std::make_shared<const jsi::StringBuffer>(R"(
function foo(i) {
  return 7;
}
function bar() {
  for (var i = 0; i < 1000; i++) {
    sum += foo(i);
  }
}
)");
    auto preparedJavaScript = rt.prepareJavaScript(code, "https://swmansion.com/index.php");

    for (auto _ : state) {
        rt.evaluatePreparedJavaScript(preparedJavaScript);
    }
}
BENCHMARK(EvaluatePreparedCode);