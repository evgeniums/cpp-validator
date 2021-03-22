/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/detail/member_helper.hpp
*
*  Defines helpers to invoke member methods.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_MEMBER_HELPER_HPP
#define DRACOSHA_VALIDATOR_MEMBER_HELPER_HPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/aggregation/element_aggregation.hpp>
#include <dracosha/validator/aggregation/any.hpp>
#include <dracosha/validator/operators/flag.hpp>
#include <dracosha/validator/make_validator.hpp>
#include <dracosha/validator/reporting/concrete_phrase.hpp>
#include <dracosha/validator/apply.hpp>
#include <dracosha/validator/dispatcher.hpp>
#include <dracosha/validator/properties.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

namespace detail
{

/**
 * @brief Helper for member's callable operator with 1 argument.
 */
template <typename T1, typename = hana::when<true>>
struct member_helper_1arg_t
{
};

/**
 * @brief Helper for member's callable operator with 1 argument when the argument is validator.
 */
template <typename T1>
struct member_helper_1arg_t<T1,
            hana::when<
                !(
                    std::is_constructible<concrete_phrase,T1>::value
                    &&
                    !hana::is_a<operator_tag,T1>
                )
            >
        >
{
    /**
     * @brief Bind compound validator to current member.
     * @param member Current member.
     * @param v Prepared partial validator.
     * @return Prepared partial validator bound to current member.
     */
    template <typename MemberT>
    auto operator() (MemberT&& member, T1&& v) const
    {
        return make_member_validator(std::forward<MemberT>(member),std::forward<T1>(v));
    }
};

/**
 * @brief Helper for member's callable operator with 1 argument when the argument is a concrete_phrase.
 */
template <typename T1>
struct member_helper_1arg_t<T1,
            hana::when<
                (
                    std::is_constructible<concrete_phrase,T1>::value
                    &&
                    !hana::is_a<operator_tag,T1>
                )
            >
        >
{
    /**
     * @brief Construct a member from the current member with explicit name.
     * @param member Current member.
     * @param name Explicit name.
     * @return Member with explicit name.
     */
    template <typename MemberT>
    auto operator() (MemberT&& member, T1&& name) const;
};

/**
 * @brief Helper for member's callable operator with 2 arguments.
 */
template <typename T1, typename T2, typename = hana::when<true>>
struct member_helper_2args_t
{
};

/**
 * @brief Helper for member's callable operator with 2 arguments when the second argument is enum of grammatical categories.
 */
template <typename T1, typename T2>
struct member_helper_2args_t<T1,T2,
            hana::when<
                std::is_enum<std::decay_t<T2>>::value
            >
        >
{
    /**
     * @brief Construct a member from the current member with explicit name and grammatical category.
     * @param member Current member.
     * @param name Name and grammatical catrgories.
     * @param grammar_category Grammatical categoty of the name.
     * @return Member with explicit name.
     */
    template <typename MemberT>
    auto operator() (MemberT&& member, T1&& name, T2&& grammar_category) const;
};

/**
 * @brief Helper for member's callable operator with 2 arguments when the first argument is operator.
 */
template <typename OpT, typename T1>
struct member_helper_2args_t<OpT,T1,
            hana::when<
                (
                    hana::is_a<operator_tag,OpT>
                    &&
                    !std::is_base_of<flag_t,std::decay_t<OpT>>::value
                )
            >
        >
{
    /**
     * @brief Bind plain operator to current member.
     * @param member Current member.
     * @param op Operator.
     * @param b Argument to forward to operator.
     * @return Prepared partial validator of "value" property bound to current member.
     */
    template <typename MemberT>
    auto operator() (MemberT&& member, OpT&& op, T1&& b) const
    {
        return member(value(std::forward<OpT>(op),std::forward<T1>(b)));
    }
};

/**
 * @brief Helper for member's callable operator with 2 arguments when the first argument is a flag.
 */
template <typename OpT, typename T1>
struct member_helper_2args_t<OpT,T1,
            hana::when<
                std::is_base_of<flag_t,std::decay_t<OpT>>::value
            >
        >
{
    /**
     * @brief Rebind plain operator to the property validator if the last key in the path is a property and operator is a flag.
     * @param member Current member.
     * @param op Operator of flag type.
     * @param b Argument to forward to operator.
     * @return Prepared partial validator of the property corresponding to the last property key in the path.
     */
    template <typename MemberT>
    auto operator() (MemberT&& member, OpT&& op, T1&& b,
                                std::enable_if_t<!MemberT::is_nested,void*> =nullptr
                            ) const
    {
        return make_validator(member.key()(std::forward<OpT>(op),std::forward<T1>(b)));
    }

    /**
     * @brief Rebind plain operator to the property validator of parent member if the last key in the path is a property and operator is a flag.
     * @param member Current member.
     * @param op Operator of flag type.
     * @param b Argument to forward to operator.
     * @return Prepared partial validator of the property of parent member corresponding to the last property key in the path.
     */
    template <typename MemberT>
    auto operator () (MemberT&& member, OpT&& op, T1&& b,
                                std::enable_if_t<MemberT::is_nested,void*> =nullptr
                            ) const
    {
        auto&& fn=[&member,&op,&b](auto&& key, auto&&... rpath)
        {
            return inherit_member(hana::reverse(hana::make_tuple(std::forward<decltype(rpath)>(rpath)...)),member)
                        (
                            key(std::forward<OpT>(op),std::forward<T1>(b))
                        );
        };
        return hana::unpack(hana::reverse(std::move(member.path())),std::move(fn));
    }
};

/**
 * @brief Helper for member's callable operator.
 */
template <typename ... Args>
struct member_helper_t
{
    /**
     * @brief Construct a member from the current member with explicit name and grammatical categories.
     * @param member Current member.
     * @param args Name and grammatical catrgories.
     * @return Member with explicit name.
     */
    template <typename MemberT>
    auto operator() (MemberT&& member, Args&&... args) const;
};

/**
 * @brief Helper for member's callable operator with 1 argument.
 */
template <typename T1>
struct member_helper_t<T1> : public member_helper_1arg_t<T1>
{
};

/**
 * @brief Helper for member's callable operator with 2 arguments.
 */
template <typename T1, typename T2>
struct member_helper_t<T1,T2> : public member_helper_2args_t<T1,T2>
{
};

/**
 * @brief Calable instance of helper for member's callable operator.
 */
template <typename ... Args>
constexpr member_helper_t<Args...> member_helper{};

/**
 * @brief Helper to figure out if member has aggregations in path.
 */
template <typename ... PathT>
struct is_member_aggregated
{
    constexpr static const auto value=hana::fold(
                hana::tuple_t<PathT...>,
                hana::false_{},
                is_element_aggregation
            );
};

/**
 * @brief Helper to figure out if member has ANY aggregations in path.
 */
template <typename ... PathT>
struct is_member_with_any
{
    constexpr static const auto value=hana::fold(
                hana::tuple_t<PathT...>,
                hana::false_{},
                is_aggregation_any
            );
};

/**
 * @brief Helper to figure out if member variadic arguments in path.
 */
template <typename ... PathT>
struct is_member_with_varg
{
    constexpr static const auto value=hana::fold(
                hana::tuple_t<PathT...>,
                hana::false_{},
                is_varg
            );
};

}

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_MEMBER_HELPER_HPP
