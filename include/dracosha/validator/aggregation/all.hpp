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
#include <dracosha/validator/aggregation/element_aggregation.hpp>
#include <dracosha/validator/utils/get_it.hpp>
#include <dracosha/validator/make_validator.hpp>
#include <dracosha/validator/aggregation/and.hpp>
#include <dracosha/validator/filter_member.hpp>
#include <dracosha/validator/variadic_arg.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

struct tree_base;

/**
 * @brief Base tag for ALL element aggregation.
 */
struct all_tag
{
    template <typename T>
    constexpr bool operator == (T) const noexcept
    {
        return true;
    }
    template <typename T>
    constexpr bool operator != (T) const noexcept
    {
        return false;
    }
};

/**
 * @brief String descriptions helper for operator ALL.
 */
struct string_all_t : public aggregate_op<string_all_t>,
                      public enable_to_string<string_all_t>
{
    using type=all_tag;

    constexpr static const aggregation_id id=aggregation_id::ALL;
    constexpr static const char* open_token="";
    constexpr static const char* close_token="";
    constexpr static const char* conjunction_token="";

    constexpr static const char* name="ALL";

    constexpr static const char* description="each element";
    constexpr static const char* iterator_description="each iterator";
    constexpr static const char* key_description="each key";
    constexpr static const char* tree_description="each tree node";

    constexpr static const char* base_phrase="each";

    constexpr static const char* name_str()
    {
        return name;
    }
    constexpr static const char* base_phrase_str()
    {
        return base_phrase;
    }
    constexpr static const char* iterator_description_str()
    {
        return iterator_description;
    }
    constexpr static const char* key_description_str()
    {
        return key_description;
    }
    constexpr static const char* tree_description_str()
    {
        return tree_description;
    }

    std::string operator() (const values_t&) const
    {
        return description;
    }
    std::string operator() (const iterators_t&) const
    {
        return iterator_description;
    }
    std::string operator() (const keys_t&) const
    {
        return key_description;
    }
    std::string operator() (const tree_base&) const
    {
        return tree_description;
    }
};

/**
  @brief Instance of string description helper for operator ALL.
*/
constexpr string_all_t string_all{};

//-------------------------------------------------------------
struct all_aggregation_t{};

/**
 * @brief Definition of aggregation pseudo operator ALL for checking if all elements of a container satisfy condition.
 * @param op Validator to apply to each element of the container.
 * @return Success if all elements of the container passed validator.
 */
template <typename ModifierT>
struct all_t : public element_aggregation_with_modifier<ModifierT>,
               public enable_to_string<string_all_t>,
               public all_tag,
               public all_aggregation_t
{
    using type=all_tag;

    template <typename ... Ops>
    constexpr auto operator() (Ops&&... ops) const
    {
        return (*this)(make_validator(std::forward<Ops>(ops)...));
    }

    template <typename OpT>
    constexpr auto operator() (OpT&& op) const;

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

    constexpr static auto string() -> decltype(auto)
    {
        return string_all;
    }

    static auto predicate()
    {
        return [](auto&& adapter, status& ret)
        {
            std::ignore=adapter;
            return ret==true;
        };
    }

    static auto post_empty_handler()
    {
        return [](bool empty)
        {
            std::ignore=empty;
            return status(status::code::success);
        };
    }

    template <typename T>
    constexpr friend bool operator == (const T&, const all_t<ModifierT>&) noexcept
    {
        return true;
    }
    template <typename T>
    constexpr friend bool operator != (const T&, const all_t<ModifierT>&) noexcept
    {
        return false;
    }
};

/**
  @brief Aggregation operator ALL that requires for all container elements to satisfy a condition.
*/
constexpr all_t<decltype(values)> ALL;

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_ALL_HPP
