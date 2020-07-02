/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** \file validator/if_bool.hpp
*
*  Defines helper to check and format boolean values
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_IF_BOOL_HPP
#define DRACOSHA_VALIDATOR_IF_BOOL_HPP

#include <dracosha/validator/config.h>
#include <dracosha/validator/enable_to_string.hpp>
#include <dracosha/validator/operators/comparison.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

struct string_must_be_t : public enable_to_string<string_must_be_t>
{
    constexpr static const char* description="must be";
};
constexpr string_must_be_t string_must_be{};

struct string_must_not_be_t : public enable_to_string<string_must_not_be_t>
{
    constexpr static const char* description="must not be";
};
constexpr string_must_not_be_t string_must_not_be{};

struct string_true_t : public enable_to_string<string_true_t>
{
    constexpr static const char* description="true";
};
constexpr string_true_t string_true{};
struct string_false_t : public enable_to_string<string_false_t>
{
    constexpr static const char* description="false";
};
constexpr string_false_t string_false{};

/**
 * @brief Helper for adjusting presentation of operation when operand is not boolean
 */
template <typename T2, typename OpT, typename =hana::when<true>>
struct if_bool_t
{
    auto operator () (OpT&& op) const -> decltype(auto)
    {
        return hana::id(std::forward<OpT>(op));
    }
};

/**
 * @brief Helper for adjusting presentation of operation when operand is boolean and operation is "eq"
 */
template <typename T2, typename OpT>
struct if_bool_t<T2,OpT,
            hana::when<std::is_same<std::remove_reference_t<T2>,bool>::value
            &&
            std::is_same<std::remove_reference_t<OpT>,eq_t>::value>
        >
{
    auto operator () (OpT&&) const -> decltype(auto)
    {
        return string_must_be;
    }
};

/**
 * @brief Helper for adjusting presentation of operation when operand is boolean and operation is "ne"
 */
template <typename T2, typename OpT>
struct if_bool_t<T2,OpT,
        hana::when<std::is_same<std::remove_reference_t<T2>,bool>::value
        &&
        std::is_same<std::remove_reference_t<OpT>,ne_t>::value>
        >
{
    auto operator () (OpT&&) const -> decltype(auto)
    {
        return string_must_not_be;
    }
};

template <typename T2, typename OpT>
constexpr if_bool_t<T2,OpT> if_bool{};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_IF_BOOL_HPP
