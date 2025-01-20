/**
 * @file blocking_queue.hpp
 * @author FengWenxi (ThorfromAsgard@outlook.com)
 * @brief An implementation of blocking queue using std::condition_variable #NOTE This only works on C++11 standard.
 * @version 0.1
 * @date 2024-06-08
 *
 * <========================================================================>
 *          github page: https://github.com/ThorfromAsgard/cybertron
 *                   © 2024 FengWenxi. All Rights Reserved.
 * <========================================================================>
 *
 */
#ifndef CYBERTRON_BASE_BLOCKING_QUEUE_HPP
#define CYBERTRON_BASE_BLOCKING_QUEUE_HPP

#include <mutex>
#include <deque>
#include <iostream>
#include <condition_variable>

#include "noncopyable.hpp"

namespace cybertron::base {
template <typename T>
/**
 * @brief An implementation of blocking queue. The queue works in two modes by specifying the {push_block} parameter.
 * #NOTE Use it carefully when set the parameter {capacity_limit} to 0 because it may lead to unlimited memory
 * consumption.
 *
 */
class BlockingQueue : public Noncopyable {
public:
    /**
     * @brief Construct a new Blocking Queue object, use it carefully when set the parameter {capacity_limit} to 0.
     *
     * @param push_block Whether the push method will work in blocking mode or not.
     *
     * @param capacity_limit The capacity limit of the queue, again, you should be very aware of what you are doing when
     * set it to 0, if you are not sure, then you are not aware !!!!!!
     *
     */
    explicit BlockingQueue(size_t capacity_limit = 0, bool push_block = false)
        : _push_block(push_block),
          _capacity_limit(capacity_limit),
          _mutex(),
          _producer(),
          _consumer(),
          _dequeue(),
          _active(true) {
        std::cout << "Blocking Queue capacity: " << _capacity_limit << std::endl;
        if (_capacity_limit == 0) {
            // TODO: Find another way to warning! Use glog instead.
            std::cerr << "Warning! Blocking queue parameter {capacity_limit} is set to 0, may cause out of memory."
                      << std::endl;
        }
    }

    /**
     * @brief Destroy the Blocking Queue object.
     *
     * #TODO: Do tests to make sure this works fine!
     */
    ~BlockingQueue() { close(); }

    void close() {
        {
            std::lock_guard<std::mutex> lock(_mutex);
            _dequeue.clear();
            _active = false;
        }
        _producer.notify_all();
        _consumer.notify_all();
    }

    /**
     * @brief Push element to the back of the queue within {timeout} microseconds in a blocking way. If the {timeout}
     * parameter is set to 0, then it will always try to push until success or the queue is closed.
     *
     * @param element Input element that is going to be pushed.
     * @param timeout in microseconds.
     * @return true if the function is not always full during {timeout} microseconds and successfully pushes one element
     * to the back of the queue,
     * @return false if it fails.
     */
    bool push_back(const T& element, const int64_t& timeout = 0) {
        std::unique_lock<std::mutex> lock(_mutex);
        if (_push_block) {
            bool is_woken_up = false;
            if (timeout) {
                is_woken_up = _producer.wait_for(lock, std::chrono::microseconds(timeout), [&] {
                    return ((!_active) || (!_capacity_limit) ||
                            (_capacity_limit && (_dequeue.size() < _capacity_limit)));
                });
            } else {
                _producer.wait(lock, [&] {
                    return ((!_active) || (!_capacity_limit) ||
                            (_capacity_limit && (_dequeue.size() < _capacity_limit)));
                });
                is_woken_up = true;
            }
            if ((!_active) || (!is_woken_up)) {
                return false;
            }
        } else {
            while (_capacity_limit && _dequeue.size() >= _capacity_limit) {
                _dequeue.pop_front();
            }
        }
        _dequeue.push_back(element);
        _consumer.notify_one();
        return true;
    }

    /**
     * @brief Push element to the back of the queue within {timeout} microseconds in a blocking way. If the {timeout}
     * parameter is set to 0, then it will always try to push until success or the queue is closed.
     *
     * @param element Input element that is going to be pushed.
     * @param timeout in microseconds.
     * @return true if the function is not always full during {timeout} microseconds and successfully pushes one element
     * to the back of the queue,
     * @return false if it fails.
     */
    bool push_back(T&& element, const int64_t& timeout = 0) {
        std::unique_lock<std::mutex> lock(_mutex);
        if (_push_block) {
            bool is_woken_up = false;
            if (timeout) {
                is_woken_up = _producer.wait_for(lock, std::chrono::microseconds(timeout), [&] {
                    return ((!_active) || (!_capacity_limit) ||
                            (_capacity_limit && (_dequeue.size() < _capacity_limit)));
                });
            } else {
                _producer.wait(lock, [&] {
                    return ((!_active) || (!_capacity_limit) ||
                            (_capacity_limit && (_dequeue.size() < _capacity_limit)));
                });
                is_woken_up = true;
            }
            if ((!_active) || (!is_woken_up)) {
                return false;
            }
        } else {
            while (_capacity_limit && _dequeue.size() >= _capacity_limit) {
                _dequeue.pop_front();
            }
        }
        _dequeue.push_back(std::move(element));
        _consumer.notify_one();
        return true;
    }

    /**
     * @brief Push element to the front of the queue within {timeout} microseconds in a blocking way. If the {timeout}
     * parameter is set to 0, then it will always try to push until success or the queue is closed.
     *
     * @param element Input element that is going to be pushed.
     * @param timeout Timeout in microseconds.
     * @return true if the function is not always full during {timeout} microseconds and successfully pushes one element
     * to the front of the queue,
     * @return false if it fails.
     */
    bool push_front(const T& element, const int64_t& timeout = 0) {
        std::unique_lock<std::mutex> lock(_mutex);
        if (_push_block) {
            bool is_woken_up = false;
            if (timeout) {
                is_woken_up = _producer.wait_for(lock, std::chrono::microseconds(timeout), [&] {
                    return ((!_active) || (!_capacity_limit) ||
                            (_capacity_limit && (_dequeue.size() < _capacity_limit)));
                });
            } else {
                _producer.wait(lock, [&] {
                    return ((!_active) || (!_capacity_limit) ||
                            (_capacity_limit && (_dequeue.size() < _capacity_limit)));
                });
                is_woken_up = true;
            }
            if ((!_active) || (!is_woken_up)) {
                return false;
            }
        } else {
            while (_capacity_limit && _dequeue.size() >= _capacity_limit) {
                _dequeue.pop_back();
            }
        }
        _dequeue.push_front(element);
        _consumer.notify_one();
    }

    /**
     * @brief Push element to the front of the queue within {timeout} microseconds in a blocking way. If the {timeout}
     * parameter is set to 0, then it will always try to push until success or the queue is closed.
     *
     * @param element Input element that is going to be pushed.
     * @param timeout Timeout in microseconds.
     * @return true if the function is not always full during {timeout} microseconds and successfully pushes one element
     * to the front of the queue,
     * @return false if it fails.
     */
    bool push_front(T&& item, const int64_t& timeout = 0) {
        std::unique_lock<std::mutex> lock(_mutex);
        if (_push_block) {
            bool is_woken_up = false;
            if (timeout) {
                is_woken_up = _producer.wait_for(lock, std::chrono::microseconds(timeout), [&] {
                    return ((!_active) || (!_capacity_limit) ||
                            (_capacity_limit && (_dequeue.size() < _capacity_limit)));
                });
            } else {
                _producer.wait(lock, [&] {
                    return ((!_active) || (!_capacity_limit) ||
                            (_capacity_limit && (_dequeue.size() < _capacity_limit)));
                });
                is_woken_up = true;
            }
            if ((!_active) || (!is_woken_up)) {
                return false;
            }
        } else {
            while (_capacity_limit && _dequeue.size() >= _capacity_limit) {
                _dequeue.pop_back();
            }
        }
        _dequeue.push_front(std::move(item));
        _consumer.notify_one();
    }

    /**
     * @brief Pop the front element of the queue within {timeout} microseconds in a blocking way. If the {timeout}
     * parameter is set to 0, then it will always try to pop until success or the queue is closed.
     *
     * @param element Output element.
     * @param timeout Timeout in microseconds.
     * @return true if the function is not always empty during {timeout} microseconds and successfully pops one element
     * from the front of the queue,
     * @return false if it fails.
     */
    bool pop_front(T& element, const int64_t& timeout = 0) {
        std::unique_lock<std::mutex> lock(_mutex);
        bool is_woken_up = false;
        if (timeout) {
            is_woken_up = _consumer.wait_for(lock, std::chrono::microseconds(timeout),
                                             [&] { return ((!_active) || (!_dequeue.empty())); });
        } else {
            _consumer.wait(lock, [&] { return ((!_active) || (!_dequeue.empty())); });
            is_woken_up = true;
        }
        if ((!_active) || (!is_woken_up)) {
            return false;
        };
        element = _dequeue.front();
        _dequeue.pop_front();
        if (_push_block) {
            _producer.notify_one();
        }
        return true;
    }

    /**
     * @brief Pop the back element of the queue within {timeout} microseconds in a blocking way. If the {timeout}
     * parameter is set to 0, then it will always try to pop until success or the queue is closed.
     *
     * @param element Output element.
     * @param timeout Timeout in microseconds.
     * @return true if the function is not always empty during {timeout} microseconds and successfully pops one element
     * from the back of the queue,
     * @return false if it fails.
     */
    bool pop_back(T& element, const int64_t& timeout = 0) {
        std::unique_lock<std::mutex> lock(_mutex);
        bool is_woken_up = false;
        if (timeout) {
            is_woken_up = _consumer.wait_for(lock, std::chrono::microseconds(timeout),
                                             [&] { return ((!_active) || (!_dequeue.empty())); });
        } else {
            _consumer.wait(lock, [&] { return ((!_active) || (!_dequeue.empty())); });
            is_woken_up = true;
        }
        if ((!_active) || (!is_woken_up)) {
            return false;
        };
        element = _dequeue.back();
        _dequeue.pop_back();
        if (_push_block) {
            _producer.notify_one();
        }
        return true;
    }

    size_t size() {
        std::lock_guard<std::mutex> lock(_mutex);
        return _dequeue.size();
    }

    size_t capacity() { return _capacity_limit; }

    bool empty() {
        std::lock_guard<std::mutex> lock(_mutex);
        return _dequeue.empty();
    }

    bool full() {
        std::lock_guard<std::mutex> lock(_mutex);
        return (_dequeue.size() >= _capacity_limit);
    }

    T front() {
        std::lock_guard<std::mutex> lock(_mutex);
        return _dequeue.front();
    }

    T back() {
        std::lock_guard<std::mutex> lock(_mutex);
        return _dequeue.back();
    }

    void clear() {
        std::lock_guard<std::mutex> lock(_mutex);
        while (_dequeue.size()) {}
        _dequeue.clear();
    }

private:
    const bool _push_block;
    bool _active;
    const size_t _capacity_limit;
    std::mutex _mutex;
    std::condition_variable _consumer;  // guarded by _mutex
    std::condition_variable _producer;  // guarded by _mutex
    std::deque<T> _dequeue;             // guarded by _mutex
};

}  // namespace cybertron::base
#endif  // CYBERTRON_BASE_BLOCKING_QUEUE_HPP