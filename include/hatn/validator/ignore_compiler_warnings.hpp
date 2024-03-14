/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/ignore_compiler_warnings.hpp
  *
  *  Defines macros used to ignore some false or unimportant compiler warnings.
  *
  */

/****************************************************************************/

#ifndef HATN_VALIDATOR_IGNORE_COMPILER_WARNINGS_HPP
#define HATN_VALIDATOR_IGNORE_COMPILER_WARNINGS_HPP

#ifndef HATN_IGNORE_MAYBE_UNINITIALIZED_BEGIN

#if __GNUC__
    #if defined(__has_warning)
        #if __has_warning("-Wmaybe-uninitialized")
            #define HATN_IGNORE_MAYBE_UNINITIALIZED
        #endif
    #else
            #define HATN_IGNORE_MAYBE_UNINITIALIZED
    #endif
#endif // __GNUC__

#ifdef HATN_IGNORE_MAYBE_UNINITIALIZED
    #define HATN_IGNORE_MAYBE_UNINITIALIZED_BEGIN \
        _Pragma("GCC diagnostic push") \
        _Pragma("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
    #define HATN_IGNORE_MAYBE_UNINITIALIZED_END \
        _Pragma("GCC diagnostic pop")
#else
    #define HATN_IGNORE_MAYBE_UNINITIALIZED_BEGIN
    #define HATN_IGNORE_MAYBE_UNINITIALIZED_END
#endif // HATN_IGNORE_MAYBE_UNINITIALIZED

#endif // HATN_IGNORE_MAYBE_UNINITIALIZED_BEGIN

#endif // HATN_VALIDATOR_IGNORE_COMPILER_WARNINGS_HPP
