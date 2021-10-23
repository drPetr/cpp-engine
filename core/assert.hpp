#pragma once
#include <core/config.hpp>

/* undefine assert macro from 'cassert' */
#ifdef assert
#   undef assert
#endif /* assert */

void __impl_asserta( int e, const char *expr, const char *file, int line, const char *function, const char *fmt, ... );
void __impl_assert( int e, const char *expr, const char *file, int line, const char *function );
void __impl_verify( int e, const char* expr, const char* file, int line, const char *function );


#define verify(e)   \
        (!(e)?__impl_verify(static_cast<int>(!!(e)),#e,__FILE__,__LINE__,__FUNCTION__):((void)0))

/* assertion macros */
#define always_asserta(e,fmt,...)   \
        (!(e)?__impl_asserta(static_cast<int>(!!(e)),#e,__FILE__,__LINE__,__FUNCTION__,fmt,##__VA_ARGS__):((void)0))

#define always_assert(e)   \
        (!(e)?__impl_assert(static_cast<int>(!!(e)),#e,__FILE__,__LINE__,__FUNCTION__):((void)0))

#define do_nothing()                    ((void)0)

/* assertion macros */
#if ASSERT_ENABLED
#   define asserta(e,fmt,...)           always_asserta(e, fmt, ##__VA_ARGS__)
#   define assert(e)                    always_assert(e)
#else
#   define asserta(e,fmt,...)           do_nothing()
#   define assert(e)                    do_nothing()
#endif /* ASSERT_ENABLED */

#if CORE_ASSERT_ENABLED
#   define core_asserta(e,fmt,...)      always_asserta(e, fmt, ##__VA_ARGS__)      
#   define core_assert(e)               always_assert(e)
#else
#   define core_asserta(e,fmt,...)      do_nothing()
#   define core_assert(e)               do_nothing()
#endif /* CORE_ASSERT_ENABLED */

#if MATH_ASSERT_ENABLED
#   define math_asserta(e,fmt,...)      always_asserta(e, fmt, ##__VA_ARGS__)      
#   define math_assert(e)               always_assert(e)
#else
#   define math_asserta(e,fmt,...)      do_nothing()
#   define math_assert(e)               do_nothing()
#endif /* MATH_ASSERT_ENABLED */

#if CONTAINERS_ASSERT_ENABLED
#   define container_asserta(e,fmt,...) always_asserta(e, fmt, ##__VA_ARGS__)      
#   define container_assert(e)          always_assert(e)
#else
#   define container_asserta(e,fmt,...) do_nothing()
#   define container_assert(e)          do_nothing()
#endif /* CONTAINERS_ASSERT_ENABLED */

