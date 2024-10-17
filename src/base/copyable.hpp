/**
 * @file copyable.hpp
 * @author FengWenxi (ThorfromAsgard@outlook.com)
 * @brief A copyable base class
 * @version 1.0
 * @date 2024-10-08
 * 
 * <========================================================================>
 *                   © 2024 FengWenxi. All Rights Reserved.                
 * <========================================================================>
 * 
 */
#ifndef CYBERTRON_BASE_COPYABLE_HPP
#define CYBERTRON_BASE_COPYABLE_HPP

namespace cybertron::base {
/**
 * @brief A tag class emphasises the objects are copyable. Derived class should be copyable, too.
 *
 */
class Copyable {
protected:
    Copyable() = default;
    ~Copyable() = default;
};

}  // namespace cybertron::base

#endif  // BASE_COPYABLE_H
