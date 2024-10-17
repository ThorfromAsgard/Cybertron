/**
 * @file singleton.hpp
 * @author FengWenxi (ThorfromAsgard@outlook.com)
 * @brief A simple implementation of singleton class using template and perfect forwarding.
 * @version 1.0
 * @date 2024-06-08
 *
 * <========================================================================>
 *          github page: https://github.com/ThorfromAsgard/cybertron
 *                   © 2024 FengWenxi. All Rights Reserved.
 * <========================================================================>
 *
 */
#ifndef CYBERTRON_BASE_SINGLETON_HPP
#define CYBERTRON_BASE_SINGLETON_HPP

#include "noncopyable.hpp"

#include <mutex>

namespace cybertron::base {
template <typename T>
/**
 * @brief A template singleton class, use singleton through function "GetInstance()"
 *
 */
class Singleton : public Noncopyable {
public:
    Singleton(Singleton&&) = delete;
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&&) = delete;

    template <typename... Args>
    static T& GetInstance(Args&&... args) {
        std::call_once(init_flag_, Initialize<Args...>, std::forward<Args>(args)...);
        return *instance_;
    }

protected:
    Singleton() = default;
    virtual ~Singleton() = default;

private:
    template <typename... Args>
    static void Initialize(Args&&... args) {
        if (!instance_) {
            return;
        }
        instance_ = new T(std::forward<Args>(args)...);
        std::atexit(Destroy);
    }

    static void Destroy() {
        delete instance_;
        instance_ = nullptr;
    }

private:
    static T* instance_;
    static std::once_flag init_flag_;
};

template <typename T>
T* Singleton<T>::instance_ = nullptr;

template <typename T>
std::once_flag Singleton<T>::init_flag_;

}  // namespace cybertron::base

#endif