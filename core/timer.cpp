#include "timer.hpp"
#include <core/assert.hpp>

namespace engine::core
{

float timer::m_ticksToSec {0.0};
float timer::m_ticksToMsec {0.0};

/* timer::timer */
timer::timer()
{
    if (static bool isInit = false; !isInit) {
        auto freq = timer::get_ticks_per_sec();
        core_asserta( freq >= 1000, "get_ticks_per_sec() returns %ld", static_cast<ticks>(freq) );
        m_ticksToSec = 1.0 / static_cast<float>(freq);
        m_ticksToMsec = 1000.0 / static_cast<float>(freq);
    }
    start();
}

/* timer::get_ticks_per_sec */
timer::ticks timer::get_ticks_per_sec()
{
    return engine::core::platform::get_ticks_per_sec();
}

/* timer::get_ticks */
timer::ticks timer::get_ticks()
{
    return engine::core::platform::get_current_ticks();
}

} /* namespace engine::core */
