/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** \file validator/validate.hpp
*
*  Defines class that performs actual validating
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_VALIDATE_HPP
#define DRACOSHA_VALIDATOR_VALIDATE_HPP

#include <type_traits>

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/property.hpp>
#include <dracosha/validator/extract.hpp>
#include <dracosha/validator/check_member.hpp>
#include <dracosha/validator/get_member.hpp>
#include <dracosha/validator/operators/exists.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

struct member_tag;
struct master_reference_tag;

//-------------------------------------------------------------

/**
 * @brief Actual validation executor
 */
struct validate_t
{
    /**
     *  @brief Perform validation of object at one level without member nesting
     *  @param a Object to validate
     *  @param op Operator for validation
     *  @param b Reference argument for validation
     *  @return Validation status
     */
    template <typename T1, typename T2, typename OpT>
    constexpr bool operator() (T1&& a, OpT&& op, T2&& b) const
    {
        return op(
                    extract(std::forward<T1>(a)),
                    extract(std::forward<T2>(b))
                );
    }

    /**
     *  @brief Perform validation of object's property at one level without member nesting
     *  @param prop Property to validate
     *  @param a Object whose property must be validated
     *  @param op Operator for validation
     *  @param b Reference argument for validation
     *  @return Validation status
     */
    template <typename T1, typename T2, typename OpT, typename PropT>
    constexpr bool operator() (PropT&& prop, T1&& a, OpT&& op, T2&& b) const
    {
        // first two arguments of invoke are flipped against the order fo those arguments in operator () above
        return invoke(std::forward<T1>(a),std::forward<PropT>(prop),std::forward<OpT>(op),std::forward<T2>(b));
    }

    /**
     *  @brief Perform validation of object's property at one level without member nesting
     *  @param prop Property to validate
     *  @param a Object to validate
     *  @param op Operator for validation
     *  @param b Reference argument for validation
     *  @return Validation status
     */
    template <typename T1, typename T2, typename OpT, typename PropT>
    constexpr static bool invoke(T1&& a, PropT&& prop, OpT&& op, T2&& b,
                                     std::enable_if_t<
                                       !hana::is_a<member_tag,T2>,
                                       void*
                                     > =nullptr
                                 )
    {
        return op(
                    property(extract(std::forward<T1>(a)),std::forward<PropT>(prop)),
                    extract(std::forward<T2>(b))
                );
    }

    /**
     *  @brief Validate existance of a member
     *  @param a Object to validate
     *  @param member Member descriptor
     *  @param b Boolean flag, when true check if member exists, when false check if member does not exist
     *  @return Validation status
     */
    template <typename T1, typename T2, typename OpT, typename PropT, typename MemberT>
    constexpr static bool invoke(T1&& a, MemberT&& member, PropT&&, OpT&&, T2&& b,
                                 std::enable_if_t<std::is_same<exists_t,typename std::decay<OpT>::type>::value,
                                   void*
                                 > =nullptr
                                )
    {
        return hana::if_(check_member_path(a,member.path),
            [&b](auto&&, auto&&)
            {
                return b==false;
            },
            [&b](auto&& a1, auto&& path)
            {
                auto&& ax=extract(std::forward<decltype(a1)>(a1));
                return exists(ax,std::forward<decltype(path)>(path))==b;
            }
        )(std::forward<T1>(a),member.path);
    }

    /**
     *  @brief Normal validation of a member
     *  @param a Object to validate
     *  @param member Member descriptor
     *  @param prop Property to validate
     *  @param op Operator for validation
     *  @param b Reference argument for validation
     *  @return Validation status
     */
    template <typename T1, typename T2, typename OpT, typename PropT, typename MemberT>
    constexpr static bool invoke(T1&& a, MemberT&& member, PropT&& prop, OpT&& op, T2&& b,
                                 std::enable_if_t<
                                   (!hana::is_a<member_tag,T2> &&
                                    !hana::is_a<master_reference_tag,T2> &&
                                    !std::is_same<exists_t,typename std::decay<OpT>::type>::value),
                                   void*
                                 > =nullptr
                                )
    {
        auto&& ax=extract(std::forward<T1>(a));
        return op(
                    property(get_member(ax,member.path),std::forward<PropT>(prop)),
                    extract(std::forward<T2>(b))
                );
    }

    /**
     *  @brief Validate using other member of the same object as a reference argument for validation
     *  @param a Object to validate
     *  @param member Member descriptor
     *  @param prop Property to validate
     *  @param op Operator for validation
     *  @param b Descriptor of reference member of the same object
     *  @return Validation status
     */
    template <typename T1, typename T2, typename OpT, typename PropT, typename MemberT>
    constexpr static bool invoke(T1&& a, MemberT&& member, PropT&& prop, OpT&& op, T2&& b,
                                 std::enable_if_t<
                                   hana::is_a<member_tag,T2>,
                                   void*
                                 > =nullptr
                                )
    {
        auto&& ax=extract(std::forward<T1>(a));
        return op(
                    property(get_member(ax,member.path),prop),
                    property(get_member(ax,b.path),prop)
                );
    }

    /**
     *  @brief Validate using the same member of a reference object
     *  @param a Object to validate
     *  @param member Member descriptor
     *  @param prop Property to validate
     *  @param op Operator for validation
     *  @param b Reference object whose member to use as argument passed to validation operator
     *  @return Validation status
     */
    template <typename T1, typename T2, typename OpT, typename PropT, typename MemberT>
    constexpr static bool invoke(T1&& a, MemberT&& member, PropT&& prop, OpT&& op, T2&& b,
                                 std::enable_if_t<
                                   hana::is_a<master_reference_tag,T2>,
                                   void*
                                 > =nullptr
                                )
    {
        auto&& ax=extract(std::forward<T1>(a));
        return op(
                    property(get_member(ax,member.path),prop),
                    property(get_member(b(),member.path),prop)
                );
    }
};
constexpr validate_t validate{};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_VALIDATE_HPP
