/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/operators/all.hpp
*
*  Defines aggregation pseudo operator ALL.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_ALL_HPP
#define DRACOSHA_VALIDATOR_ALL_HPP

#include <iostream>

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/utils/get_it.hpp>
#include <dracosha/validator/make_validator.hpp>
#include <dracosha/validator/operators/and.hpp>
#include <dracosha/validator/detail/aggregate_all.hpp>
#include <dracosha/validator/filter_member.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

/**
 * @brief String descriptions helper for operator ALL.
 */
struct string_all_t : public aggregate_op<string_all_t>,
                      public enable_to_string<string_all_t>
{
    constexpr static const aggregation_id id=aggregation_id::ALL;
    constexpr static const char* open_token="";
    constexpr static const char* close_token="";
    constexpr static const char* conjunction_token="";

    constexpr static const char* description="each element";
};

/**
  @brief Instance of string description helper for operator ALL.
*/
constexpr string_all_t string_all{};

//-------------------------------------------------------------

/**
 * @brief Definition of aggregation pseudo operator ALL for checking if all elements of a container satisfy condition.
 * @param op Validator to apply to each element of the container.
 * @return Success if all elements of the container passed validator.
 */
struct all_t : public element_aggregation
{
    template <typename ... Ops>
    constexpr auto operator() (Ops&&... ops) const
    {
        return (*this)(AND(std::forward<Ops>(ops)...));
    }

    template <typename OpT>
    constexpr auto operator() (OpT&& op) const
    {
        return make_validator(
                    hana::reverse_partial(
                        detail::aggregate_all,
                        std::forward<OpT>(op)
                    )
               );
    }

    /**
     * @brief Create validator form operator and operand.
     * @param op Operator.
     * @param b Operand.
     * @return Validator.
     */
    template <typename OpT, typename T>
    constexpr auto operator () (OpT&& op,
                                T&& b,
                                std::enable_if_t<hana::is_a<operator_tag,OpT>,void*> =nullptr
            ) const
    {
        return (*this)(value(std::forward<OpT>(op),std::forward<T>(b)));
    }
};

template <typename KeyT>
struct generate_paths_t<KeyT,hana::when<std::is_same<all_t,std::decay_t<KeyT>>::value>>
{
    template <typename PathT, typename AdapterT, typename HandlerT>
    status operator () (PathT&& path, AdapterT&& adapter, HandlerT&& handler) const
    {
        return element_aggregation::invoke(
            [](status ret)
            {
                return ret==true;
            },
            [](bool empty)
            {
                std::ignore=empty;
                return status(status::code::success);
            },
            string_all,
            std::forward<PathT>(path),
            std::forward<AdapterT>(adapter),
            std::forward<HandlerT>(handler)
        );
    }
};

/**
  @brief Aggregation operator ALL that requires for all container elements to satisfy a condition.
*/
constexpr all_t ALL{};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_ALL_HPP
