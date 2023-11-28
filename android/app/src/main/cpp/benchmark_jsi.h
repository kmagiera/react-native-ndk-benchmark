#include <benchmark/benchmark.h>
#include <hermes/hermes.h>
#include <jsi/jsi.h>

using namespace facebook;

static void InstantiatingHermesRuntime(benchmark::State &state)
{
  for (auto _ : state)
  {
    auto runtime =
        facebook::hermes::makeHermesRuntime();
  }
}
BENCHMARK(InstantiatingHermesRuntime);

static void CallHostFunctionFromJS1000Times(benchmark::State &state)
{
  auto hermesRuntime = facebook::hermes::makeHermesRuntime();
  jsi::Runtime &rt = *hermesRuntime;
  jsi::Value hostFunction = jsi::Function::createFromHostFunction(
      rt,
      jsi::PropNameID::forAscii(rt, "foo"),
      1,
      [](jsi::Runtime &rt,
         const jsi::Value &thisValue,
         const jsi::Value *args,
         size_t count) -> jsi::Value
      {
        return jsi::Value(7);
      });
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

  for (auto _ : state)
  {
    loop.call(rt);
  }
}
BENCHMARK(CallHostFunctionFromJS1000Times);

static void CallJSFunctionWithoutJSI1000Times(benchmark::State &state)
{
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

  for (auto _ : state)
  {
    loop.call(rt);
  }
}
BENCHMARK(CallJSFunctionWithoutJSI1000Times);

static void ExecuteJSIFunction1000Times(benchmark::State &state)
{
  auto hermesRuntime = facebook::hermes::makeHermesRuntime();
  jsi::Runtime &rt = *hermesRuntime;

  auto code = std::make_shared<const jsi::StringBuffer>(R"(
(function foo(i) {
  return 7;
}))");
  auto func = rt.evaluateJavaScript(code, "https://swmansion.com/index.php").asObject(rt).asFunction(rt);

  for (auto _ : state)
  {
    int sum = 0;
    for (int i = 0; i < 1000; i++)
    {
      sum += func.call(rt, jsi::Value(i)).asNumber();
    }
  }
}
BENCHMARK(ExecuteJSIFunction1000Times);

class FooHostObject : public jsi::HostObject
{
public:
  FooHostObject() {}
  virtual ~FooHostObject() {}
  jsi::Value get(jsi::Runtime &rt, const jsi::PropNameID &name) override
  {
    if (name.utf8(rt) == "foo")
    {
      return jsi::Value(7);
    }
    return {};
  }
  void set(jsi::Runtime &rt, const jsi::PropNameID &name, const jsi::Value &value) override {}
};

static void AccessHostObjectAttribute1000Times(benchmark::State &state)
{
  auto hermesRuntime = facebook::hermes::makeHermesRuntime();
  jsi::Runtime &rt = *hermesRuntime;

  rt.global().setProperty(rt, "HostObject", jsi::Object::createFromHostObject(rt, std::make_shared<FooHostObject>()));

  auto code = std::make_shared<const jsi::StringBuffer>(R"(
(function loop() {
  let sum = 0;
  for (var i = 0; i < 1000; i++) {
    sum += HostObject.foo;
  }
  return sum;
}))");
  auto loop = rt.evaluateJavaScript(code, "https://swmansion.com/index.php").asObject(rt).asFunction(rt);

  for (auto _ : state)
  {
    loop.call(rt);
  }
}
BENCHMARK(AccessHostObjectAttribute1000Times);

class FooNativeState : public jsi::NativeState
{
public:
  FooNativeState() {}
  virtual ~FooNativeState() {}
  int foo = 7;
};

static void
AccessNativeStateData1000Times(benchmark::State &state)
{
  auto hermesRuntime = facebook::hermes::makeHermesRuntime();
  jsi::Runtime &rt = *hermesRuntime;

  auto objectWithNativeState = jsi::Object(rt);
  objectWithNativeState.setNativeState(rt, std::make_shared<FooNativeState>());
  rt.global().setProperty(rt, "objectWithNativeState", objectWithNativeState);

  jsi::Value accessFunction = jsi::Function::createFromHostFunction(
      rt,
      jsi::PropNameID::forAscii(rt, "foo"),
      1,
      [](jsi::Runtime &rt,
         const jsi::Value &thisValue,
         const jsi::Value *args,
         size_t count) -> jsi::Value
      {
        auto nativeState = args[0].getObject(rt).getNativeState<FooNativeState>(rt);
        return jsi::Value(nativeState->foo);
      });
  rt.global().setProperty(rt, "foo", accessFunction);

  auto code = std::make_shared<const jsi::StringBuffer>(R"((() => {
var sum = 0;
return function loop() {
  for (var i = 0; i < 1000; i++) {
    sum += foo(objectWithNativeState);
  }
}
})())");
  auto loop = rt.evaluateJavaScript(code, "https://swmansion.com/index.php").asObject(rt).asFunction(rt);

  for (auto _ : state)
  {
    loop.call(rt);
  }
}
BENCHMARK(AccessNativeStateData1000Times);

static void
GetNativeState(benchmark::State &state)
{
  auto hermesRuntime = facebook::hermes::makeHermesRuntime();
  jsi::Runtime &rt = *hermesRuntime;

  auto objectWithNativeState = jsi::Object(rt);
  objectWithNativeState.setNativeState(rt, std::make_shared<FooNativeState>());

  for (auto _ : state)
  {
    objectWithNativeState.getNativeState<FooNativeState>(rt);
  }
}
BENCHMARK(GetNativeState);

static void
GetHostObject(benchmark::State &state)
{
  auto hermesRuntime = facebook::hermes::makeHermesRuntime();
  jsi::Runtime &rt = *hermesRuntime;

  auto hostObject = jsi::Object::createFromHostObject(rt, std::make_shared<FooHostObject>());

  for (auto _ : state)
  {
    hostObject.getHostObject<FooHostObject>(rt);
  }
}
BENCHMARK(GetHostObject);

static void EvaluatePreparedCode(benchmark::State &state)
{
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

  for (auto _ : state)
  {
    rt.evaluatePreparedJavaScript(preparedJavaScript);
  }
}
BENCHMARK(EvaluatePreparedCode);