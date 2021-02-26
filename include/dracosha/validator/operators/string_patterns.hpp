/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/operators/string_patterns.hpp
*
*  Defines operator to validate some string patterns.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_STRING_PATTERNS_HPP
#define DRACOSHA_VALIDATOR_STRING_PATTERNS_HPP

#include <boost/regex.hpp>

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/operators/operator.hpp>
#include <dracosha/validator/operators/op_report_without_operand.hpp>
#include <dracosha/validator/operators/regex.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

/**
 * @brief Definition of operator "must contain only alpha symbols".
 */
struct str_alpha_t : public op_report_without_operand<str_alpha_t>
{
    constexpr static const char* description="must contain only letters and digits";
    constexpr static const char* n_description="must contain not only letters and digits";

    template <typename T1, typename T2>
    bool operator() (const T1& a, const T2& b) const
    {
        static const std::regex e("[0-9a-zA-Z_]*");
        return regex_match(a,e)==b;
    }
};

/**
    @brief Operator "must contain only alpha symbols".
*/
constexpr str_alpha_t str_alpha{};

/**
 * @brief Definition of operator "must be hexadecimal number".
 */
struct str_hex_t : public op_report_without_operand<str_hex_t>
{
    constexpr static const char* description="must be a hexadecimal number";
    constexpr static const char* n_description="must be not a hexadecimal number";

    template <typename T1, typename T2>
    bool operator() (const T1& a, const T2& b) const
    {
        static const std::regex e("[0-9a-fA-F]+");
        return regex_match(a,e)==b;
    }
};

/**
    @brief Operator "must be hexadecimal number".
*/
constexpr str_hex_t str_hex{};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_STRING_PATTERNS_HPP
