/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/operators/exists.hpp
*
*  Defines operator "exists".
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

struct exists_tag{};

/**
 * @brief Definition of operator "exists" for checking if member exists in object.
 */
struct exists_t : public exists_tag,
                  public adjust_storable_ignore
{
    using hana_tag=operator_tag;

    constexpr static const char* description="must exist";
    constexpr static const char* n_description="must not exist";

    static const char* str(const bool& b)
    {
        return b?description:n_description;
    }

    template <typename T1, typename T2>
    constexpr bool operator() (const T1& a, const T2& b) const
    {
        return check_exists(a,b);
    }

    template <typename T>
    constexpr bool operator == (const T&) const
    {
        return false;
    }
    constexpr bool operator == (const exists_t&) const
    {
        return true;
    }

    template <typename T>
    constexpr bool operator != (const T&) const
    {
        return true;
    }
    constexpr bool operator != (const exists_t&) const
    {
        return false;
    }
};

/**
 * @brief Operator for checking if member exists in object.
 */
constexpr exists_t exists{};

/**
 * @brief Wrapper of operator "exists" with explicit string description.
 */
struct exists_op_with_string_t : public exists_tag,
                                 public adjust_storable_ignore
{
    using hana_tag=operator_tag;

    exists_op_with_string_t(std::string str):_description(std::move(str))
    {}

    std::string str(const bool&) const
    {
        return _description;
    }

    template <typename T1, typename T2>
    constexpr bool operator() (const T1& a, const T2& b) const
    {
        return exists(a,b);
    }

    std::string _description;
};

//-------------------------------------------------------------

/**
 * @brief String descriptions helper for operator "exists".
 */
struct string_exists_t : public enable_to_string<string_exists_t>
{
    constexpr static const char* description="must exist";
};

/**
 * @brief Instance of string descriptions helper for operator "exists".
 */
constexpr string_exists_t string_exists{};

//-------------------------------------------------------------

/**
 * @brief String descriptions helper for invertion of operator "exists".
 */
struct string_not_exists_t : public enable_to_string<string_not_exists_t>
{
    constexpr static const char* description="must not exist";
};

/**
 * @brief Instance of string descriptions helper for invertion of operator "exists".
 */
constexpr string_not_exists_t string_not_exists{};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_EXISTS_HPP
