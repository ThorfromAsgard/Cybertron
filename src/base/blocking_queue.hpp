/**
 * @file blocking_queue.hpp
 * @author FengWenxi (ThorfromAsgard@outlook.com)
 * @brief An implementation of blocking queue using std::condition_variable #NOTE This only works on C++11 standard.
 * @version 0.1
 * @date 2024-10-08
 *
 * <========================================================================>
 *                   © 2024 FengWenxi. All Rights Reserved.
 * <========================================================================>
 *
 */
#ifndef CYBERTRON_BASE_BLOCKING_QUEUE_HPP
#define CYBERTRON_BASE_BLOCKING_QUEUE_HPP

#include "noncopyable.hpp"

#include <mutex>
#include <deque>
#include <iostream>
#include <condition_variable>

namespace cybertron::base {
template <typename T>
/**
 * @brief An implementation of blocking queue. The queue works in two modes by specifying the {push_block} parameter.
 * #NOTE Use it carefully when set the parameter {max_capacity} to 0 because it may lead to unlimited memory
 * consumption.
 *
 */
// TODO: add timeout for push method
class BlockingQueue : public Noncopyable {
public:
    /**
     * @brief Construct a new Blocking Queue object, use it carefully when set the parameter {max_capacity} to 0.
     *
     * @param push_block Whether the push method will work in blocking mode or
     * not.
     *
     * @param max_capacity Again, you should be very aware of what you are doing when set it to 0, if you are not sure,
     * then you are not aware !!!!!!
     *
     */
    explicit BlockingQueue(bool push_block = false, size_t max_capacity = 0)
        : push_block_(push_block), capacity_limit_(max_capacity), mutex_(), consumer_(mutex_), deque_(), active_(true) {
        std::cout << "Blocking Queue capacity: " << capacity_limit_ << std::endl;
        if (capacity_limit_ == 0) {
            // TODO: Find another way to warning!
            std::cout << "Warning! Blocking queue parameter {max_capacity} is set to 0, may cause out of memory."
                      << std::endl;
        }
    }

    /**
     * @brief Destroy the Blocking Queue object.
     *
     * #TODO: Do tests to make sure this works fine!
     */
    ~BlockingQueue() {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            deque_.clear();
            active_ = false;
        }
        producer_.notify_all();
        consumer_.notify_all();
    }

    /**
     * @brief Push element to the back of the queue within {timeout} microseconds in a blocking way.If the {timeout}
     * parameter is set to 0,then it will always try to push until success or the queue is closed.
     *
     * @param element Input element that is going to be pushed.
     * @param timeout in microseconds.
     * @return true if the function is not always full in {timeout} microseconds and successfully pushes one element to
     * the back of the queue,
     * @return false if it fails.
     */
    bool PushBack(const T& element, const int64_t& timeout = 0) {
        std::unique_lock<std::mutex> lock(mutex_);
        if (push_block_) {
            std::cv_status is_timeout = std::cv_status::no_timeout;
            if (timeout) {
                is_timeout = producer_.wait_for(lock, std::chrono::microseconds(timeout), [&] {
                    return ((!active_) || (!capacity_limit_) || (capacity_limit_ && (deque_.size() < capacity_limit_)));
                });
            } else {
                producer_.wait(lock, [&] {
                    return ((!active_) || (!capacity_limit_) || (capacity_limit_ && (deque_.size() < capacity_limit_)));
                });
            }
            if ((!active_) || (is_timeout == std::cv_status::timeout)) {
                return false;
            }
        } else {
            while (capacity_limit_ && deque_.size() >= capacity_limit_) {
                deque_.pop_front();
            }
        }
        deque_.push_back(element);
        consumer_.notify_one();
        return true;
    }

    /**
     * @brief Push element to the back of the queue within {timeout} microseconds in a blocking way.If the {timeout}
     * parameter is set to 0,then it will always try to push until success or the queue is closed.
     *
     * @param element Input elemet that is going to be pushed.
     * @param timeout in microseconds.
     * @return true if the function is not always empty in {timeout} microseconds and successfully pushes one element to
     * the back of the queue,
     * @return false if it fails.
     */
    bool PushBack(T&& element, const uint64_t& timeout = 0) {
        std::unique_lock<std::mutex> lock(mutex_);
        if (push_block_) {
            std::cv_status is_timeout = std::cv_status::no_timeout;
            if (timeout) {
                is_timeout = producer_.wait_for(lock, std::chrono::microseconds(timeout), [&] {
                    return ((!active_) || (!capacity_limit_) || (capacity_limit_ && (deque_.size() < capacity_limit_)));
                });
            } else {
                producer_.wait(lock, [&] {
                    return ((!active_) || (!capacity_limit_) || (capacity_limit_ && (deque_.size() < capacity_limit_)));
                });
            }
            if ((!active_) || (is_timeout == std::cv_status::timeout)) {
                return false;
            }
        } else {
            while (capacity_limit_ && deque_.size() >= capacity_limit_) {
                deque_.pop_front();
            }
        }
        deque_.push_back(std::move(item));
        consumer_.notify_one();
        return true;
    }

    /**
     * @brief Push item to the front of the queue within {timeout} microseconds in a blocking way.
     *
     * @param item Input elemet that is going to be pushed.
     * @param timeout Timeout in microseconds.
     * @return true if the function successfully pushes one element to the front of the queue,
     * @return false if it fails.
     */
    bool PushFront(const T& item, const uint64_t& timeout = 0) {
        std::unique_lock<std::mutex> lock(mutex_);
        if (push_block_) {
            std::cv_status is_timeout = std::cv_status::no_timeout;
            if (timeout) {
                is_timeout = producer_.wait_for(lock, std::chrono::microseconds(timeout), [&] {
                    return ((!active_) || (!capacity_limit_) || (capacity_limit_ && (deque_.size() < capacity_limit_)));
                });
            } else {
                producer_.wait(lock, [&] {
                    return ((!active_) || (!capacity_limit_) || (capacity_limit_ && (deque_.size() < capacity_limit_)));
                });
            }
            if ((!active_) || (is_timeout == std::cv_status::timeout)) {
                return false;
            }
        } else {
            while (capacity_limit_ && deque_.size() >= capacity_limit_) {
                deque_.pop_back();
            }
        }
        deque_.push_front(item);
        consumer_.notify_one();
    }

    /**
     * @brief Push item to the front of the queue within {timeout} microseconds in a blocking way.
     *
     * @param item Input elemet that is going to be pushed.
     * @param timeout Timeout in microseconds.
     * @return true if the function successfully pushes one element to the front of the queue,
     * @return false if it fails.
     */
    bool PushFront(T&& item, const uint64_t& timeout = 0) {
        std::unique_lock<std::mutex> lock(mutex_);
        if (push_block_) {
            std::cv_status is_timeout = std::cv_status::no_timeout;
            if (timeout) {
                is_timeout = producer_.wait_for(lock, std::chrono::microseconds(timeout), [&] {
                    return ((!active_) || (!capacity_limit_) || (capacity_limit_ && (deque_.size() < capacity_limit_)));
                });
            } else {
                producer_.wait(lock, [&] {
                    return ((!active_) || (!capacity_limit_) || (capacity_limit_ && (deque_.size() < capacity_limit_)));
                });
            }
            if ((!active_) || (is_timeout == std::cv_status::timeout)) {
                return false;
            }
        } else {
            if (capacity_limit_ && deque_.size() >= capacity_limit_) {
                deque_.pop_back();
            }
        }
        deque_.push_front(std::move(item));
        consumer_.notify_one();
    }

    /**
     * @brief Pop front element within {timeout} microseconds in a blocking way.
     *
     * @param item output item.
     * @param timeout in microseconds.
     * @return true if the function is not always empty in {timeout} microseconds and successfully pops one element from
     * the front of the queue,
     * @return false if it fails.
     */
    bool PopFront(T& item, const int& timeout = 0) {
        std::unique_lock<std::mutex> lock(mutex_);
        std::cv_status is_timeout = std::cv_status::no_timeout;
        if (timeout) {
            is_timeout = consumer_.wait_for(lock, std::chrono::microseconds(timeout),
                                            [&] { return ((!active_) || (deque_.empty())); });
        } else {
            consumer_.wait(lock, [&] { return ((!active_) || (!deque_.empty())); });
        }
        if ((!active_) || (is_timeout == std::cv_status::timeout)) {
            return false;
        };
        item = deque_.front();
        deque_.pop_front();
        if (push_block_) {
            producer_.notify_one();
        }
        return true;
    }

    /**
     * @brief Pop back element within {timeout} microseconds in a blocking way.
     *
     * @param item output item.
     * @param timeout in microseconds.
     * @return true if the function is not empty in {timeout} microseconds and successfully pops one element from the
     * back of the queue,
     * @return false if it fails.
     */
    bool PopBack(T& item, const int& timeout = 0) {
        std::cv_status is_timeout = std::cv_status::no_timeout;
        if (timeout) {
            is_timeout = consumer_.wait_for(lock, std::chrono::microseconds(timeout),
                                            [&] { return ((!active_) || (deque_.empty())); });
        } else {
            consumer_.wait(lock, [&] { return ((!active_) || (!deque_.empty())); });
        }
        if ((!active_) || (is_timeout == std::cv_status::timeout)) {
            return false;
        };
        item = deque_.back();
        deque_.pop_back();
        if (push_block_) {
            producer_.notify_one();
        }
        return true;
    }

    size_t Size() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return deque_.size();
    }

    size_t Capacity() {
        std::lock_guard<std::mutex> lock(mutex_);
        return capacity_limit_;
    }

    bool Empty() {
        std::lock_guard<std::mutex> lock(mutex_);
        return deque_.empty();
    }

    bool Full() {
        std::lock_guard<std::mutex> lock(mutex_);
        return (deque_.size() >= capacity_limit_);
    }

    T Front() {
        std::lock_guard<std::mutex> lock(mutex_);
        return deque_.front();
    }

    T Back() {
        std::lock_guard<std::mutex> lock(mutex_);
        return deque_.back();
    }

    void Clear() {
        std::lock_guard<std::mutex> lock(mutex_);
        deque_.clear();
    }

private:
    const bool push_block_;
    bool active_;
    const size_t capacity_limit_;
    std::mutex mutex_;
    std::condition_variable consumer_;  // guarded by mutex_
    std::condition_variable producer_;  // guarded by mutex_
    std::deque<T> deque_;               // guarded by mutex_
};
}  // namespace cybertron::base

#endif