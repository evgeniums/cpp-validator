/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/utils/optional.hpp
*
*  Defines alias for optional: boost::optional is used for C++14 standard and std::optional is used for C++17 and newer stansards.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_OPTIONAL_HPP
#define DRACOSHA_VALIDATOR_OPTIONAL_HPP

#if __cplusplus < 201703L || (defined (IOS_SDK_VERSION_X10) && IOS_SDK_VERSION_X10<120)
#include <boost/optional.hpp>
#else
#include <optional>
#endif

#include <dracosha/validator/config.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

#if __cplusplus < 201703L || (defined (IOS_SDK_VERSION_X10) && IOS_SDK_VERSION_X10<120)
template <typename T>
using optional=boost::optional<T>;
#else
template <typename T>
using optional=std::optional<T>;
#endif

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_OPTIONAL_HPP
