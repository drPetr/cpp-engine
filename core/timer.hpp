#pragma once
#include <core/platform/api.hpp>

namespace engine::core
{

class timer
{
public:
    typedef platform::timer_ticks   ticks;

public:
                    timer();
    void            start();
    ticks           time_ticks();
    float           time_msec();
    float           time_sec();
    ticks           get_elapsed_ticks() const;
    float           get_elapsed_msec() const;
    float           get_elapsed_sec() const;

    static ticks    get_ticks_per_sec();
    static ticks    get_ticks();
private:
    ticks           m_lastTicks {0};
    static float    m_ticksToSec;
    static float    m_ticksToMsec;
}; /* class timer */



/* timer::start */
inline void timer::start()
{
    m_lastTicks = get_ticks();
}

/* timer::time_ticks */
inline timer::ticks timer::time_ticks()
{
    auto time = m_lastTicks;
    m_lastTicks = get_ticks();
    return m_lastTicks - time;
}

/* timer::time_msec */
inline float timer::time_msec()
{
    auto time = m_lastTicks;
    m_lastTicks = get_ticks();
    return static_cast<float>(m_lastTicks - time) * m_ticksToMsec;
}

/* timer::time_sec */
inline float timer::time_sec()
{
    auto time = m_lastTicks;
    m_lastTicks = get_ticks();
    return static_cast<float>(m_lastTicks - time) * m_ticksToSec;
}

/* timer::get_elapsed_ticks */
inline timer::ticks timer::get_elapsed_ticks() const
{
    auto time = get_ticks() - m_lastTicks;
    return time;
}

/* timer::get_elapsed_msec */
inline float timer::get_elapsed_msec() const
{
    auto time = get_ticks() - m_lastTicks;
    return static_cast<float>(time) * m_ticksToMsec;
}

/* timer::get_elapsed_sec */
inline float timer::get_elapsed_sec() const
{
    auto time = get_ticks() - m_lastTicks;
    return static_cast<float>(time) * m_ticksToSec;
}

} /* namespace engine::core */
