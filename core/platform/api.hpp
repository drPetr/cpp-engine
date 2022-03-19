#pragma once
#include <core/config.hpp>
#include <core/platform/types.hpp>

namespace engine::core::platform
{

timer_ticks get_ticks_per_sec();
timer_ticks get_current_ticks();

} /* namespace engine::core::platform */
