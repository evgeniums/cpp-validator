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
struct all_t
{
    using hana_tag=element_aggregation_tag;

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
        const auto& obj=extract(adapter.traits().get());
        const auto parent_path=hana::drop_back(path);

        auto&& parent=make_member(parent_path);
        if (!adapter.traits().check_member_exists(parent))
        {
            return adapter.traits().not_found_status();
        }

        const auto& parent_element=get_member(obj,parent_path);
        return hana::eval_if(
            is_container_t<std::decay_t<decltype(parent_element)>>{},
            [&](auto&& _)
            {
                for (auto it=_(parent_element).begin();it!=_(parent_element).end();++it)
                {
                    status ret=_(handler)(hana::append(parent_path,wrap_it(it,string_all)));
                    if (ret.value()==status::code::fail)
                    {
                        return ret;
                    }
                }
                return status(status::code::success);
            },
            [&](auto&& _)
            {
                return _(handler)(std::forward<decltype(path)>(path));
            }
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
