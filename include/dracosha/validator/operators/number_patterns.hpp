/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/operators/number_patterns.hpp
*
*  Defines operator to validate some number patterns in strings.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_NUMBER_PATTERNS_HPP
#define DRACOSHA_VALIDATOR_NUMBER_PATTERNS_HPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/operators/regex.hpp>
#include <dracosha/validator/operators/op_report_without_operand.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

namespace detail
{
// from https://stackoverflow.com/a/2845275
inline bool is_integer(const std::string& s)
{
   if(s.empty() || ((!isdigit(s[0])) && (s[0] != '-') && (s[0] != '+'))) return false;

   char * p;
   strtol(s.c_str(), &p, 10);

   return (*p == 0);
}
}

/**
 * @brief Definition of operator "must be integer".
 */
struct str_int_t : public op_report_without_operand<str_int_t>
{
    constexpr static const char* description="must be integer";
    constexpr static const char* n_description="must not be integer";

    template <typename T1, typename T2>
    bool operator() (const T1& a, const T2& b) const
    {
        return detail::is_integer(a)==b;
    }
};

/**
    @brief Operator "must be integer".
*/
constexpr str_int_t str_int{};


/**
 * @brief Definition of operator "must be a floating point number".
 */
struct str_float_t : public op_report_without_operand<str_float_t>
{
    constexpr static const char* description="must be a floating point number";
    constexpr static const char* n_description="must be not a floating point number";

    template <typename T1, typename T2>
    bool operator() (const T1& a, const T2& b) const
    {
        static const std::regex e("[-+]?[0-9]*\\.?[0-9]+([eE][-+]?[0-9]+)?");
        return regex_match(a,e)==b;
    }
};

/**
    @brief Operator "must be a floating point number".
*/
constexpr str_float_t str_float{};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_NUMBER_PATTERNS_HPP
