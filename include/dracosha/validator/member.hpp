/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** \file validator/member.hpp
*
*  Defines generic descriptor of a member that must be validated in objects
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_MEMBER_HPP
#define DRACOSHA_VALIDATOR_MEMBER_HPP

#include <type_traits>

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/utils/adjust_storable_type.hpp>
#include <dracosha/validator/utils/tuple_to_variadic.hpp>
#include <dracosha/validator/apply.hpp>
#include <dracosha/validator/dispatcher.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

struct member_tag;

/**
 * @brief Format a path as string with dot separated levels
 * @param path Path as hana::tuple object
 * @return Formatted path
 *
 * @note Only string keys can be formatted using this method
 */
BOOST_HANA_CONSTEXPR_LAMBDA auto format_path_str= [](auto&& path) -> decltype(auto)
{
    return hana::fold(std::forward<decltype(path)>(path),std::string(),
            [](auto&& str, auto&& key) -> decltype(auto)
            {
                if (!str.empty())
                {
                    str+=".";
                }
                return str+key;
            }
        );
};

/**
 *  @brief Generic descriptor of a member that must be validated.
 *
 * Descriptor defines the path for extracting specific member/subobject from the object under validation.
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
     * @param parent Parent member which is of previous (upper) level
     */
    template <typename T1, typename MemberT>
    member(T1&& key, MemberT&& parent)
         : path(hana::append(std::forward<MemberT>(parent),std::forward<T1>(key)))
    {}

    /**
     * @brief Ctor of nested member
     * @param key Key of current member
     * @param parent Parent member which is of previous (upper) level
     */
    template <typename MemberT>
    member(type key, MemberT&& parent)
         : path(hana::append(std::forward<MemberT>(parent),std::move(key)))
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
     * @brief Format a path as string with dot separated levels
     * @return Formatted path
     *
     * @note Only string keys can be formatted using this method
     */
    std::string path_str() const
    {
        return format_path_str(path);
    }

    /**
     * @brief Append next level to member
     * @param key Member key
     */
    template <typename T1>
    constexpr auto operator [] (T1&& key) const -> decltype(auto)
    {
        auto tmpl=tuple_to_variadic<member>::to_template(path,typename adjust_storable_type<T1>::type(key));
        return typename decltype(tmpl)::type(std::forward<T1>(key),path);
    }
};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_MEMBER_HPP
