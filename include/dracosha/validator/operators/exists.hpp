/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** \file validator/operators/exists.hpp
*
*  Defines operator "exists"
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_EXISTS_HPP
#define DRACOSHA_VALIDATOR_EXISTS_HPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/operators/operator.hpp>
#include <dracosha/validator/check_exists.hpp>
#include <dracosha/validator/utils/enable_to_string.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

/**
 * @brief Operator to check if member exists in object
 */
struct exists_t
{
    using hana_tag=operator_tag;

    template <typename T1, typename T2>
    constexpr bool operator() (const T1& a, const T2& b) const
    {
        return check_exists(a,b);
    }
};
constexpr exists_t exists{};

//-------------------------------------------------------------
struct string_exists_t : public enable_to_string<string_exists_t>
{
    constexpr static const char* description="exists";
};
constexpr string_exists_t string_exists{};

//-------------------------------------------------------------
struct string_not_exists_t : public enable_to_string<string_not_exists_t>
{
    constexpr static const char* description="does not exist";
};
constexpr string_not_exists_t string_not_exists{};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_EXISTS_HPP
