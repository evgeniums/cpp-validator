/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/property_validator.hpp
*
*  Defines functor for property validation.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_PROPERTY_VALIDATOR_HPP
#define DRACOSHA_VALIDATOR_PROPERTY_VALIDATOR_HPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/dispatcher.hpp>
#include <dracosha/validator/utils/wrap_object.hpp>
#include <dracosha/validator/utils/adjust_storable_type.hpp>
#include <dracosha/validator/base_validator.hpp>
#include <dracosha/validator/operators/exists.hpp>
#include <dracosha/validator/lazy.hpp>
#include <dracosha/validator/utils/copy.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

struct type_p_value;
struct property_validator_tag;

/**
 * @brief Property validator functor.
 */
template <typename HandlerT, typename PropT, typename CheckExistsT=hana::false_, typename ExistsOperatorT=exists_t>
struct property_validator : public base_validator<HandlerT,CheckExistsT,ExistsOperatorT>
{
    using hana_tag=property_validator_tag;
    using property_type=PropT;

    using base_validator<HandlerT,CheckExistsT,ExistsOperatorT>::base_validator;
};

/**
 * @brief Make property validator functor.
 * @param prop Property.
 * @param args Arguments to forward to validator.
 */
template <typename PropT, typename OpT, typename OperandT>
constexpr auto make_property_validator(PropT&& prop, OpT&& op, OperandT&& operand)
{
    return hana::if_(
            hana::and_(
                std::is_base_of<exists_tag,std::decay_t<OpT>>{}
            ),
            [](auto&& prop, auto&& op, auto&& operand)
            {
                static_assert(std::is_same<std::decay_t<decltype(prop)>,type_p_value>::value,"Operator \"exists\" can be used only with property \"value\"");
                static_assert(!hana::is_a<lazy_tag,decltype(operand)>,"Operator \"exists\" can not be used with lazy operands");
                static_assert(std::is_same<std::decay_t<decltype(operand)>,bool>::value,"Operator \"exists\" can be used only with boolean operands");

                auto op1=copy(op);
                auto operand1=copy(operand);
                auto fn=hana::reverse_partial(dispatch,std::forward<decltype(prop)>(prop),std::forward<decltype(op)>(op),
                                              adjust_storable(std::forward<decltype(operand)>(operand)));

                return property_validator<
                        decltype(fn),
                        PropT,
                        hana::true_,
                        decltype(op1)
                    >{std::move(fn),std::move(operand1),std::move(op1)};
            },
            [](auto&& prop, auto&& op, auto&& operand)
            {
                auto fn=hana::reverse_partial(dispatch,std::forward<decltype(prop)>(prop),std::forward<decltype(op)>(op),
                                              adjust_storable(std::forward<decltype(operand)>(operand)));

                return property_validator<
                        decltype(fn),
                        PropT
                    >{std::move(fn)};
            }
    )(std::forward<PropT>(prop),std::forward<OpT>(op),std::forward<OperandT>(operand));
}

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_PROPERTY_VALIDATOR_HPP
