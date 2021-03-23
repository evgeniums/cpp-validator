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

//-------------------------------------------------------------

/**
 * @brief Base template struct for validators.
 */
template <typename HandlerT, typename WithCheckExistsT=hana::false_, typename ExistsOperatorT=exists_t>
struct base_validator
{
    using with_check_exists=WithCheckExistsT;

    /**
     * @brief Constructor.
     * @param fn Validation handler.
     * @param must_exist If "exists()" must be checked before validation.
     * @param exists_op Operand to be used to check if "exists()".
     */
    base_validator(
            HandlerT fn,
            bool must_exist=false,
            ExistsOperatorT exists_op=exists
        ) : fn(std::move(fn)),
            check_exists_operand(must_exist),
            exists_operator(std::move(exists_op))
    {}

    /**
     * @brief Invoke handler.
     * @param args Arguments.
     * @return Validation result.
     */
    template <typename ... Args>
    auto operator () (Args&&... args) const -> decltype(auto)
    {
        return fn(std::forward<Args>(args)...);
    }

    HandlerT fn;
    const bool check_exists_operand;
    const ExistsOperatorT exists_operator;
};

//-------------------------------------------------------------

namespace detail {
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
}

/**
 * @brief Implementer of is_validator_with_check_exists.
 */
struct is_validator_with_check_exists_impl
{
    template <typename T>
    constexpr auto operator () (T&& xs) const
    {
        return hana::if_(
            hana::is_a<hana::tuple_tag,T>,
            detail::is_validator_with_check_exists_tuple,
            detail::is_validator_with_check_exists_single
        )(std::forward<T>(xs));
    }
};
/**
 * @brief Figure if with_check_exists is set in validator(s).
 * @param Validator or container with validators.
 * @return True if any of validators have with_check_exists set.
 */
constexpr is_validator_with_check_exists_impl is_validator_with_check_exists{};

//-------------------------------------------------------------

/**
 * @brief Implementer of content_of_check_exists.
 */
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
/**
 * @brief Get content of "exists()" condition of validator where with_check_exists is set.
 * @param Validator or container with validators.
 * @return Content of "exists()" condition as std::pair{check_exists_operand,exists_operator}.
 */
constexpr content_of_check_exists_impl content_of_check_exists{};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_BASE_VALIDATOR_HPP
