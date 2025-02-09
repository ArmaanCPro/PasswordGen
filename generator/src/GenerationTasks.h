#pragma once

#include <coroutine>
#include <future>

// for multithreaded async coroutines.
template<typename T>
struct ThreadedTask
{
    struct promise_type
    {
        std::promise<T> promise;

        ThreadedTask get_return_object() { return ThreadedTask(promise.get_future()); }
        std::suspend_never initial_suspend() { return {}; }
        std::suspend_never final_suspend() noexcept { return {}; }

        void return_value(T value) { promise.set_value(value); }
        void unhandled_exception() { promise.set_exception(std::current_exception()); }
    };

    std::future<T> future;

    explicit ThreadedTask(std::future<T> future) : future(std::move(future)) {}
    ThreadedTask(const ThreadedTask&) = delete;
    ThreadedTask& operator=(const ThreadedTask&) = delete;
    ThreadedTask(ThreadedTask&&) = default;
    ThreadedTask& operator=(ThreadedTask&&) = default;

    T get() { return future.get(); }
};

/* implementation:
ThreadedTask<std::vector<std::string>> PasswordGenerator::GenerateIntermediatePasswordsAsyncThreaded(int numPasswords) const {
    std::promise<std::vector<std::string>> promise;
    std::future<std::vector<std::string>> fut = promise.get_future();

    std::thread([this, numPasswords, promise = std::move(promise)]() mutable {
        try {
            std::vector<std::string> passwords = GenerateIntermediatePasswords(numPasswords);
            promise.set_value(passwords);
        } catch (...) {
            promise.set_exception(std::current_exception());
        }
    }).detach();

    co_return fut.get(); // This will wait for the thread to finish
}

 */