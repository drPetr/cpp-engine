#pragma once
namespace engine {
namespace core {

class timer {
public:
    typedef long long int   ticks;
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
    ticks           lastTicks;
    static float    ticksToSec;
    static float    ticksToMsec;
};



/* timer::start */
inline void timer::start() {
    lastTicks = get_ticks();
}

/* timer::time_ticks */
inline timer::ticks timer::time_ticks() {
    auto time = lastTicks;
    lastTicks = get_ticks();
    return lastTicks - time;
}

/* timer::time_msec */
inline float timer::time_msec() {
    auto time = lastTicks;
    lastTicks = get_ticks();
    return static_cast<float>(lastTicks - time) * ticksToMsec;
}

/* timer::time_sec */
inline float timer::time_sec() {
    auto time = lastTicks;
    lastTicks = get_ticks();
    return static_cast<float>(lastTicks - time) * ticksToSec;
}

/* timer::get_elapsed_ticks */
inline timer::ticks timer::get_elapsed_ticks() const {
    auto time = get_ticks() - lastTicks;
    return time;
}

/* timer::get_elapsed_msec */
inline float timer::get_elapsed_msec() const {
    auto time = get_ticks() - lastTicks;
    return static_cast<float>(time) * ticksToMsec;
}

/* timer::get_elapsed_sec */
inline float timer::get_elapsed_sec() const {
    auto time = get_ticks() - lastTicks;
    return static_cast<float>(time) * ticksToSec;
}

} /* namespace core */
} /* namespace engine */
