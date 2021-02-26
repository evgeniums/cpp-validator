/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/config.hpp
  *
  *  Defines configuration macros used throughout the library.
  *
  */

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_CONFIG_HPP
#define DRACOSHA_VALIDATOR_CONFIG_HPP

#include <boost/hana.hpp>

#define DRACOSHA_VALIDATOR_NAMESPACE_BEGIN namespace dracosha { namespace validator {
#define DRACOSHA_VALIDATOR_NAMESPACE_END }}

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

namespace hana=boost::hana;

DRACOSHA_VALIDATOR_NAMESPACE_END

#define DRACOSHA_VALIDATOR_NAMESPACE dracosha::validator

#if __cplusplus >= 201703L
    #define DRACOSHA_VALIDATOR_INLINE_LAMBDA inline
#else
    #define DRACOSHA_VALIDATOR_INLINE_LAMBDA static
#endif

#endif // DRACOSHA_VALIDATOR_CONFIG_HPP
