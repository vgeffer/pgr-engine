#include "debug.hpp"

#include <array>
#include <cassert>
#include <iostream>
#include <ostream>
#include <stdexcept>
#include <vector>

using namespace pgreng::common;

debug::debug() {
    s_instance = this;
}

debug::~debug() {

}


