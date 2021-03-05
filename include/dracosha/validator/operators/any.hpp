/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/operators/any.hpp
*
*  Defines aggregation pseudo operator ANY.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_ANY_HPP
#define DRACOSHA_VALIDATOR_ANY_HPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/make_validator.hpp>
#include <dracosha/validator/operators/and.hpp>
#include <dracosha/validator/prevalidation/strict_any.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

/**
 * @brief String descriptions helper for operator ANY.
 */
struct string_any_t : public aggregate_op<string_any_t>,
                      public enable_to_string<string_any_t>
{
    constexpr static const aggregation_id id=aggregation_id::ANY;
    constexpr static const char* open_token="";
    constexpr static const char* close_token="";
    constexpr static const char* conjunction_token="";

    constexpr static const char* description="at least one element";
};

/**
  @brief Instance of string description helper for operator ANY.
*/
constexpr string_any_t string_any{};

//-------------------------------------------------------------
struct all_t;

/**
 * @brief Definition of aggregation pseudo operator ANY to check if any element of a container satisfies condition.
 * @param op Validator to apply to elements of the container.
 * @return Success if any element of the container passed validator.
 */
struct any_t : public element_aggregation,
               public enable_to_string<string_any_t>
{
    using type=any_t;

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

    bool operator == (const all_t&) const noexcept
    {
        return true;
    }
    bool operator != (const all_t&) const noexcept
    {
        return false;
    }

    bool operator == (const any_t&) const noexcept
    {
        return true;
    }
    bool operator != (const any_t&) const noexcept
    {
        return false;
    }

    template <typename T>
    constexpr friend bool operator == (T, const any_t&) noexcept
    {
        return true;
    }
    template <typename T>
    constexpr friend bool operator != (T, const any_t&) noexcept
    {
        return false;
    }
};

/**
  @brief Aggregation operator ANY that requires for at least one of container elements to satisfy a condition.
*/
constexpr any_t ANY{};

//-------------------------------------------------------------

template <typename AdapterT, typename = hana::when<true>>
struct check_strict_any
{
    template <typename T>
    static constexpr bool skip(T&&) noexcept
    {
        return false;
    }
};
template <typename AdapterT>
struct check_strict_any<AdapterT,
        hana::when<
            std::is_base_of<strict_any_tag,typename std::decay_t<AdapterT>::type>::value
        >>
{
    template <typename T>
    static bool skip(T&& adapter) noexcept
    {
        return !adapter.traits().is_strict_any();
    }
};

template <typename KeyT>
struct generate_paths_t<KeyT,hana::when<std::is_same<any_t,std::decay_t<KeyT>>::value>>
{
    template <typename PathT, typename AdapterT, typename HandlerT>
    status operator () (PathT&& path, AdapterT&& adapter, HandlerT&& handler) const
    {
        return element_aggregation::invoke(
            [&adapter](status& ret)
            {
                if (check_strict_any<AdapterT>::skip(adapter))
                {
                    ret=status{status::code::ignore};
                    return false;
                }
                return ret.value()!=status::code::success;
            },
            [](bool empty)
            {
                if (empty)
                {
                    return status(status::code::ignore);
                }
                return status(status::code::fail);
            },
            string_any,
            std::forward<PathT>(path),
            std::forward<AdapterT>(adapter),
            std::forward<HandlerT>(handler)
        );
    }
};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_ANY_HPP
