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

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

struct type_p_value;
struct property_validator_tag;

/**
 * @brief Property validator functor.
 */
template <typename HandlerT, typename PropT, typename CheckExistsT=hana::false_>
struct property_validator : public base_validator<HandlerT,CheckExistsT>
{
    using hana_tag=property_validator_tag;
    using property_type=PropT;

    using base_validator<HandlerT,CheckExistsT>::base_validator;
};

/**
 * @brief Make property validator functor.
 * @param prop Property.
 * @param args Arguments to forward to validator.
 */
template <typename PropT, typename OpT, typename OperandT>
constexpr auto make_property_validator(PropT&& prop, OpT&& op, OperandT&& operand)
{
    auto fn=hana::reverse_partial(dispatch,std::forward<PropT>(prop),std::forward<OpT>(op),
                                  adjust_storable(std::forward<OperandT>(operand)));

    return hana::if_(
            hana::and_(
                std::is_base_of<exists_tag,std::decay_t<OpT>>{}
            ),
            [](auto&& prop, auto&& fn, auto&& operand)
            {
                static_assert(std::is_same<std::decay_t<decltype(prop)>,type_p_value>::value,"Operator \"exists\" can be used only with property \"value\"");
                static_assert(!hana::is_a<lazy_tag,decltype(operand)>,"Operator \"exists\" can not be used with lazy operands");
                static_assert(std::is_same<std::decay_t<decltype(operand)>,bool>::value,"Operator \"exists\" can be used only with boolean operands");
                return property_validator<
                        std::decay_t<decltype(fn)>,
                        PropT,
                        hana::true_
                    >(std::forward<decltype(fn)>(fn),operand);
            },
            [](auto&&, auto&& fn, auto&&)
            {
                return property_validator<
                        std::decay_t<decltype(fn)>,
                        PropT
                    >{std::forward<decltype(fn)>(fn)};
            }
    )(prop,std::move(fn),operand);
}

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_PROPERTY_VALIDATOR_HPP
