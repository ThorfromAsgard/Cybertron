/*
 * @Author: ThorfromAsgard ThorfromAsgard@outlook.com
 * @Date: 2024-05-07 13:19:53
 * @LastEditors: ThorfromAsgard ThorfromAsgard@outlook.com
 * @LastEditTime: 2024-05-07 13:21:04
 * @FilePath: /cybertron/src/base/singleton.hpp
 * @Description: Write your file description here.
 * Copyright (c) 2024 by ThorfromAsgard@outlook.com, All Rights Reserved.
 */

namespace cybertron::base {
    class Singleton {
    private:
        Singleton(){};
        ~Singleton(){};
        Singleton(const Singleton &);
        Singleton &operator=(const Singleton &);

    public:
        static Singleton &get_instance() {
            static Singleton instance;
            return instance;
        }
    }
}