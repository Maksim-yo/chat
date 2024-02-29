#include <chrono>
#include <condition_variable>
#include <shared_mutex>
#include <type_traits>

using namespace std::chrono_literals;

class Timer
{

    using Lock = std::shared_mutex;
    using WriteLock = std::unique_lock<Lock>;
    using ReadLock = std::shared_lock<Lock>;

    class TimerAlreadyStartException : public std::runtime_error
    {
    public:
        using runtime_error::runtime_error;
    };

private:
    std::condition_variable_any cv;
    Lock cv_m;
    bool isRunning{false};
    std::thread startThread;
    std::thread abortThread;

private:
    template <typename Return, typename Object, typename... Args>
    void _start(int seconds, Return (Object::*callback)(Args...), Object* objInstance, Args... args)
    {
        WriteLock lk(cv_m);
        isRunning = true;
        auto status = cv.wait_for(lk, seconds * 1s);
        if (status == std::cv_status::timeout)
            (objInstance->*callback)(args...);
        isRunning = false;
    }
    template <typename func, typename... Args>
    void _start(int seconds, func callback, Args... args)
    {
        WriteLock lk(cv_m);
        isRunning = true;
        auto status = cv.wait_for(lk, seconds * 1s);
        if (status == std::cv_status::timeout)
            callback(args...);
        isRunning = false;
    }
    void _abort()
    {
        cv.notify_one();
    }

public:
    template <typename Return, typename Object, typename... Args>
    void start(int seconds, Return (Object::*callback)(Args...), Object* objInstance, Args... args)
    {   
        if constexpr (std::is_member_function_pointer_v<decltype(callback)>)
            startThread = std::thread{&Timer::_start<Return, Object, Args...>, this, seconds, callback, objInstance, args...};
    }

    template <typename Return, typename... Args>
    void start(int seconds, Return (*callback)(Args...), Args... args)
    {
        startThread = std::thread{&Timer::_start<decltype(callback), Args...>, this, seconds, callback, args...};
    }

    void abort()
    {
        abortThread = std::thread{&Timer::_abort, this};
    }

    bool isRun()
    {
        ReadLock r_lock(cv_m);
        return isRunning;
    }

    ~Timer(){
        abort();
    }
};
