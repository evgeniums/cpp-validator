/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** \file validator/member.hpp
*
*  Defines generic descriptor of a member to be validated in objects.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_MEMBER_HPP
#define DRACOSHA_VALIDATOR_MEMBER_HPP

#include <type_traits>

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/utils/adjust_storable_type.hpp>
#include <dracosha/validator/utils/make_types_tuple.hpp>
#include <dracosha/validator/apply.hpp>
#include <dracosha/validator/dispatcher.hpp>
#include <dracosha/validator/properties.hpp>
#include <dracosha/validator/make_validator.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

struct member_tag;

/**
 *  @brief Generic descriptor of a member to be validated.
 *
 * Member descriptor defines a path for extracting specific member/subobject from the object under validation.
 */
template <typename T, typename ...ParentPathT>
struct member
{
    using hana_tag=member_tag;
    using type=typename adjust_storable_type<T>::type;

    hana::tuple<ParentPathT...,type> path;

    /**
     * @brief Ctor of nested member
     * @param key Key of current member
     * @param parent_path Path to parent member which is of previous (upper) level
     */
    template <typename T1, typename ParentPathTs>
    member(T1&& key, ParentPathTs&& parent_path)
         : path(hana::append(std::forward<ParentPathTs>(parent_path),std::forward<T1>(key)))
    {}

    /**
     * @brief Ctor of nested member
     * @param key Key of current member
     * @param parent_path Path to parent member which is of previous (upper) level
     */
    template <typename ParentPathTs>
    member(type key, ParentPathTs&& parent_path)
         : path(hana::append(std::forward<ParentPathTs>(parent_path),std::move(key)))
    {}

    /**
     * @brief Ctor
     * @param key Key of current member
     */
    template <typename T1>
    member(T1&& key)
         : path(hana::make_tuple(std::forward<T1>(key)))
    {}

    /**
     * @brief Ctor with key of string type
     * @param str Key of current member
     */
    member(std::string str)
         : path(hana::make_tuple(std::move(str)))
    {}

    /**
     * @brief Check if path of this member is equal to path of other member
     * @param other Other member
     */
    template <typename T1>
    bool isEqual(const T1& other) const
    {
        return hana::equal(path,other.path);
    }

    /**
     * @brief Bind compound validator to current member
     * @param v Prepared partial validator
     * @return Prepared partial validator bound to current member
     */
    template <typename T1>
    constexpr auto operator () (T1&& v) const -> decltype(auto)
    {
        return make_validator(hana::reverse_partial(apply_member,std::forward<T1>(v),*this));
    }

    /**
     * @brief Bind plain operator to current member
     * @param op Operator
     * @param b Argument to forward to operator
     * @return Prepared partial validator of "value" property bound to current member
     */
    template <typename OpT, typename T1>
    constexpr auto operator () (OpT&& op, T1&& b) const -> decltype(auto)
    {
        return (*this)(value(std::forward<OpT>(op),std::forward<T1>(b)));
    }

    /**
     * @brief Get key of the member at current level
     * @return Key of current member
     */
    constexpr const type& key() const
    {
        return hana::back(path);
    }

    /**
     * @brief Append next level to member
     * @param key Member key
     */
    template <typename T1>
    constexpr auto operator [] (T1&& key) const -> decltype(auto)
    {
        auto path_types=hana::transform(path,hana::make_type);
        auto key_and_path_types=hana::prepend(path_types,hana::type_c<typename adjust_storable_type<T1>::type>);
        auto next_member_tmpl=hana::unpack(key_and_path_types,hana::template_<member>);
        return typename decltype(next_member_tmpl)::type(std::forward<T1>(key),path);
    }
};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_MEMBER_HPP
