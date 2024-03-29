/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/utils/enable_to_string.hpp
*
*  Defines base template for types that can be string convertible using description.
*
*/

/****************************************************************************/

#ifndef HATN_VALIDATOR_ENABLE_TO_STRING_HPP
#define HATN_VALIDATOR_ENABLE_TO_STRING_HPP

#include <string>
#include <hatn/validator/config.hpp>

HATN_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

/**
 * @brief Base template for types that can be string convertible using static description.
 */
template <typename DerivedT>
struct enable_to_string
{
    operator std::string () const
    {
        return std::string(DerivedT::description);
    }
};

//-------------------------------------------------------------

HATN_VALIDATOR_NAMESPACE_END

#endif // HATN_VALIDATOR_ENABLE_TO_STRING_HPP
