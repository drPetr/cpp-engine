#include <core/platform/api.hpp>
#include <windows.h>

namespace engine::core::platform
{

/* get_ticks_per_sec */
timer_ticks get_ticks_per_sec()
{
    if (LARGE_INTEGER i; QueryPerformanceFrequency( &i )) {
        return static_cast<timer_ticks>( i.QuadPart );
    }
    return 0;
}

/* get_current_ticks */
timer_ticks get_current_ticks()
{
    LARGE_INTEGER i;
    QueryPerformanceCounter( &i );
    return static_cast<timer_ticks>( i.QuadPart );
}

} /* namespace engine::core::platform */
