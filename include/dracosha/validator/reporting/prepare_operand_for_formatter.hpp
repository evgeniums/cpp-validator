/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/reporting/prepare_operand_for_formatter.hpp
*
* Defines helper for preparing operand for formatter.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_PREPARE_OPERAND_FORMATTER_HPP
#define DRACOSHA_VALIDATOR_PREPARE_OPERAND_FORMATTER_HPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/member.hpp>
#include <dracosha/validator/make_member.hpp>
#include <dracosha/validator/operators/contains.hpp>
#include <dracosha/validator/reporting/member_operand.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

/**
 * @brief Default helper to prepare operand for formatter.
 */
template <typename OpT, typename T, typename =hana::when<true>>
struct prepare_operand_for_formatter_t
{
    auto operator () (OpT&&, T&& b) const -> decltype(auto)
    {
        return hana::id(std::forward<T>(b));
    }
};

/**
 * @brief Helper to prepare operand for formatter for operator contains.
 */
template <typename OpT, typename T>
struct prepare_operand_for_formatter_t<OpT,T,
            hana::when<
                std::is_same<std::decay_t<OpT>,contains_t>::value
            >
        >
{
    auto operator () (OpT&&, T&& b) const -> decltype(auto)
    {
        return make_member_operand(
                        make_plain_member(std::forward<T>(b))
                    );
    }
};

/**
 * @brief Template instance of helper for preparing operand for formatter.
 */
template <typename OpT, typename T>
constexpr prepare_operand_for_formatter_t<OpT,T> prepare_operand_for_formatter_inst{};

/**
 * @brief Prepare operand for formatter taking into account operator.
 * @param op Operator.
 * @param b Original operand.
 * @return Operand prepared for formatting.
 */
template <typename OpT, typename T>
auto prepare_operand_for_formatter(OpT&& op, T&& b)
{
    return prepare_operand_for_formatter_inst<OpT,T>(std::forward<OpT>(op),std::forward<T>(b));
}

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_PREPARE_OPERAND_FORMATTER_HPP
