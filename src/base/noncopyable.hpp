/**
 * @file noncopyable.hpp
 * @author FengWenxi (ThorfromAsgard@outlook.com)
 * @brief A noncopyable base class
 * @version 1.0
 * @date 2024-06-08
 *
 * <========================================================================>
 *          github page: https://github.com/ThorfromAsgard/cybertron
 *                   © 2024 FengWenxi. All Rights Reserved.
 * <========================================================================>
 *
 */
#ifndef CYBERTRON_BASE_NONCOPYABLE_HPP
#define CYBERTRON_BASE_NONCOPYABLE_HPP

namespace cybertron::base {
/**
 * @brief A tag class emphasises that the objects are noncopyable. Derived class should be noncopyable, too.
 *
 */
class Noncopyable {
public:
    Noncopyable(const Noncopyable&) = delete;
    void operator=(const Noncopyable&) = delete;

protected:
    Noncopyable() = default;
    virtual ~Noncopyable() = default;
};

}  // namespace cybertron::base

#endif  // BASE_NONCOPYABLE_H
