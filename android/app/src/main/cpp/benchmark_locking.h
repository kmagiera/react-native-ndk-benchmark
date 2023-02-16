#include <benchmark/benchmark.h>
#include <atomic>
#include <mutex>
#include <semaphore>

static void AcquiringMutex(benchmark::State& state) {
    std::mutex mx;
    int id = 0;
    for (auto _ : state) {
        std::lock_guard<std::mutex> lock{mx};
        id++;
        id--;
    }
}
BENCHMARK(AcquiringMutex);

static void IncrementAtomic(benchmark::State& state) {
    std::atomic<int> id{};
    for (auto _ : state) {
        id++;
        // id--;
    }
}
BENCHMARK(IncrementAtomic);


static void AtomicLockTestAndSet(benchmark::State& state) {
    std::atomic_flag lock = ATOMIC_FLAG_INIT;
    for (auto _ : state) {
        lock.test_and_set(std::memory_order_acquire);
    }
}
BENCHMARK(AtomicLockTestAndSet);


//static void SemaphoreAcquire(benchmark::State& state) {
//    std::counting_semaphore<1000000> semaphore;
//    for (auto _ : state) {
//        semaphore.acquire();
//    }
//}
//BENCHMARK(SemaphoreAcquire);

