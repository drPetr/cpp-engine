#pragma once
#include <cassert>

void __impl_assert( int e, const char* expr, const char* file, int line, const char *function );
void __impl_verify( int e, const char* expr, const char* file, int line, const char *function );

/* verification macros */
#define verify(e)   \
    (!(e)?__impl_verify((int)(!!(e)),#e,__FILE__,__LINE__,__FUNCTION__):((void)0))

#ifdef assert
#undef assert
#endif /* assert */

/* assertion macros */
#if defined(_DEBUG) || defined(DEBUG)
    #define assert(e)   \
        (!(e)?__impl_assert((int)(!!(e)),#e,__FILE__,__LINE__,__FUNCTION__):((void)0))
#else
    #define assert(e)
#endif