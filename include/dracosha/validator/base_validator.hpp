/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/base_validator.hpp
*
*  Defines base validator.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_BASE_VALIDATOR_HPP
#define DRACOSHA_VALIDATOR_BASE_VALIDATOR_HPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/utils/conditional_fold.hpp>
#include <dracosha/validator/can_get.hpp>
#include <dracosha/validator/operators/exists.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

struct exists_t;

template <typename HandlerT, typename WithCheckExistsT=hana::false_, typename ExistsOperatorT=exists_t>
struct base_validator
{
    using with_check_exists=WithCheckExistsT;

    base_validator(
            HandlerT fn,
            bool must_exist=false,
            ExistsOperatorT exists_op=exists
        ) : fn(std::move(fn)),
            check_exists_operand(must_exist),
            exists_operator(std::move(exists_op))
    {}

    template <typename ... Args>
    auto operator () (Args&&... args) const -> decltype(auto)
    {
        return fn(std::forward<Args>(args)...);
    }

    HandlerT fn;
    const bool check_exists_operand;
    const ExistsOperatorT exists_operator;
};

struct is_validator_with_check_exists_single_impl
{
    template <typename T>
    constexpr auto operator () (T&&) const
    {
        return typename std::decay_t<T>::with_check_exists{};
    }
};
constexpr is_validator_with_check_exists_single_impl is_validator_with_check_exists_single{};

struct is_validator_with_check_exists_type_c_impl
{
    template <typename T>
    constexpr auto operator () (T) const
    {
        return typename std::decay_t<typename T::type>::with_check_exists{};
    }
};
constexpr is_validator_with_check_exists_type_c_impl is_validator_with_check_exists_type_c{};

struct is_validator_with_check_exists_tuple_impl
{
    template <typename T>
    constexpr auto operator () (T&& ts) const
    {
        return hana::not_equal(
                    hana::find_if(
                        hana::transform(ts,hana::make_type),
                        is_validator_with_check_exists_type_c
                    ),
                    hana::nothing
                );
    }
};
constexpr is_validator_with_check_exists_tuple_impl is_validator_with_check_exists_tuple{};

struct is_validator_with_check_exists_impl
{
    template <typename T>
    constexpr auto operator () (T&& xs) const
    {
        return hana::if_(
            hana::is_a<hana::tuple_tag,T>,
            [](auto&& xs)
            {
                return is_validator_with_check_exists_tuple(xs);
            },
            [](auto&& xs)
            {
                return is_validator_with_check_exists_single(xs);
            }
        )(std::forward<T>(xs));
    }
};
constexpr is_validator_with_check_exists_impl is_validator_with_check_exists{};

struct content_of_check_exists_impl
{
    template <typename T>
    auto operator () (T&& xs) const
    {
        return hana::if_(
            hana::is_a<hana::tuple_tag,T>,
            [](auto&& xs)
            {
                auto first_v=hana::find_if(
                    xs,
                    [](auto&& v)
                    {
                        return typename std::decay_t<decltype(v)>::with_check_exists{};
                    }
                );
                return hana::eval_if(
                    hana::not_equal(first_v,hana::nothing),
                    [&](auto&& _)
                    {
                        const auto& val=_(first_v).value();
                        return std::make_pair(val.check_exists_operand,val.exists_operator);
                    },
                    [](auto&&)
                    {
                        return std::make_pair(false,exists);
                    }
                );
            },
            [](auto&& xs)
            {
                return std::make_pair(xs.check_exists_operand,xs.exists_operator);
            }
        )(std::forward<T>(xs));
    }
};
constexpr content_of_check_exists_impl content_of_check_exists{};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_BASE_VALIDATOR_HPP
