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

#ifndef HATN_VALIDATOR_CONFIG_HPP
#define HATN_VALIDATOR_CONFIG_HPP

#include <boost/hana.hpp>

#define HATN_VALIDATOR_NAMESPACE_BEGIN namespace hatn { namespace validator {
#define HATN_VALIDATOR_NAMESPACE_END }}

HATN_VALIDATOR_NAMESPACE_BEGIN

namespace hana=boost::hana;

HATN_VALIDATOR_NAMESPACE_END

#define HATN_VALIDATOR_NAMESPACE hatn::validator
#define HATN_VALIDATOR_USING using namespace hatn::validator;
#define HATN_VALIDATOR_NS validator

#if __cplusplus >= 201703L
    #define HATN_VALIDATOR_INLINE_LAMBDA inline
#else
    #define HATN_VALIDATOR_INLINE_LAMBDA static
#endif

#endif // HATN_VALIDATOR_CONFIG_HPP
