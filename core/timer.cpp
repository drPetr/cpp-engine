#include "timer.h"
#include <core/assert.h>
#include <windows.h>
namespace engine {
namespace core {

float timer::ticksToSec {0.0};
float timer::ticksToMsec {0.0};

/* timer::timer */
timer::timer() : lastTicks{0} {
    if( static bool isInit = false; !isInit ) {
        auto freq = timer::get_ticks_per_sec();
        assert( (freq != 0) && "timer::timer() error: QueryPerformanceFrequency()" );
        ticksToSec = 1.0 / static_cast<float>(freq);
        ticksToMsec = 1000.0 / static_cast<float>(freq);
    }
    start();
}

/* timer::get_ticks_per_sec */
timer::ticks timer::get_ticks_per_sec() {
    if( LARGE_INTEGER i; QueryPerformanceFrequency( &i ) ) {
        return static_cast<ticks>( i.QuadPart );
    }
    return 0;
}

/* timer::get_ticks */
timer::ticks timer::get_ticks() {
    LARGE_INTEGER i;
    QueryPerformanceCounter( &i );
    return static_cast<ticks>( i.QuadPart );
}

} /* namespace core */
} /* namespace engine */