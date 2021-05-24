#pragma once
#include <core/types.h>
#include <iostream>
namespace engine {

class common {
public:
    static ostream      &error();
    static ostream      &log();
};

inline ostream &common::error() {
    return std::cerr;
}

inline ostream &common::log() {
    return std::cout;
}

}