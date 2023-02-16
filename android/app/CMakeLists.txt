cmake_minimum_required(VERSION 3.4.1)

set(ANDROID_STL c++_shared)

set(lib-name benchmark-lib)

add_library(
        ${lib-name}
        SHARED
        src/main/cpp/benchmark-lib.cpp)


# benchmark
set(BENCHMARK_DOWNLOAD_DEPENDENCIES ON)
add_subdirectory(src/main/cpp/benchmark)
include_directories(src/main/cpp/benchmark/include)
target_link_libraries(${lib-name} benchmark)

#logging
find_library(log-lib log)
target_link_libraries(${lib-name} ${log-lib})

#react-native
set(REACT_NATIVE_DIR "../../node_modules/react-native/")
target_include_directories(
        ${lib-name}
        PRIVATE
        "${REACT_NATIVE_DIR}/ReactAndroid/src/main/jni/react/turbomodule"
        "${REACT_NATIVE_DIR}/ReactCommon"
        "${REACT_NATIVE_DIR}/ReactCommon/callinvoker"
        "${REACT_NATIVE_DIR}/ReactCommon/react/renderer/graphics/platform/cxx"
        "${REACT_NATIVE_DIR}/ReactCommon/runtimeexecutor"
        "${REACT_NATIVE_DIR}/ReactCommon/yoga"
)

#hermes
find_package(fbjni REQUIRED CONFIG)
find_package(ReactAndroid REQUIRED CONFIG)
find_package(hermes-engine REQUIRED CONFIG)
target_link_libraries(${lib-name} hermes-engine::libhermes ReactAndroid::hermes_executor ReactAndroid::jsi)