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
#include <dracosha/validator/operators/flag.hpp>
#include <dracosha/validator/make_validator.hpp>
#include <dracosha/validator/reporting/word_attributtes.hpp>
#include <dracosha/validator/reporting/concrete_phrase.hpp>

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
    using path_type=hana::tuple<ParentPathT...,type>;

    path_type path;

    /**
     * @brief Ctor of nested member
     * @param key Key of current member
     * @param parent_path Path to parent member which is of previous (upper) level
     */
    template <typename T1, typename ParentPathTs>
    member(T1&& key, ParentPathTs&& parent_path,
           std::enable_if_t<!std::is_constructible<std::string,ParentPathTs>::value,void*> =nullptr)
         : path(hana::append(std::forward<ParentPathTs>(parent_path),std::forward<T1>(key)))
    {}

    /**
     * @brief Ctor of nested member
     * @param key Key of current member
     * @param parent_path Path to parent member which is of previous (upper) level
     */
    template <typename ParentPathTs>
    member(type key, ParentPathTs&& parent_path,
           std::enable_if_t<!std::is_constructible<std::string,ParentPathTs>::value,void*> =nullptr)
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
     * @brief Constructor with path
     * @param path Member's path
     */
    member(path_type path)
         : path(std::move(path))
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
    constexpr auto operator () (OpT&& op, T1&& b,
                                std::enable_if_t<
                                hana::is_a<operator_tag,OpT>
                                &&
                                !(
                                    hana::is_a<property_tag,type>
                                    &&
                                    std::is_base_of<flag_t,std::decay_t<OpT>>::value
                                 )
                                ,void*> =nullptr) const -> decltype(auto)
    {
        return (*this)(value(std::forward<OpT>(op),std::forward<T1>(b)));
    }

    /**
     * @brief Rebind plain operator to the property validator if the last key in the path is a property and operator is a flag
     * @param op Operator of flag type
     * @param b Argument to forward to operator
     * @return Prepared partial validator of the property corresponding to the last property key in the path
     */
    template <typename OpT, typename T1>
    constexpr auto operator () (OpT&& op, T1&& b,
                                std::enable_if_t<
                                (
                                    hana::is_a<property_tag,type>
                                    &&
                                    std::is_base_of<flag_t,std::decay_t<OpT>>::value
                                    &&
                                    sizeof...(ParentPathT)==0
                                 )
                                ,void*> =nullptr) const -> decltype(auto)
    {
        return make_validator(key()(std::forward<OpT>(op),std::forward<T1>(b)));
    }

    /**
     * @brief Rebind plain operator to the property validator of parent member if the last key in the path is a property and operator is a flag
     * @param op Operator of flag type
     * @param b Argument to forward to operator
     * @return Prepared partial validator of the property of parent member corresponding to the last property key in the path
     */
    template <typename OpT, typename T1>
    constexpr auto operator () (OpT&& op, T1&& b,
                                std::enable_if_t<
                                (
                                    hana::is_a<property_tag,type>
                                    &&
                                    std::is_base_of<flag_t,std::decay_t<OpT>>::value
                                    &&
                                    sizeof...(ParentPathT)!=0
                                 )
                                ,void*> =nullptr) const -> decltype(auto)
    {
        return member<ParentPathT...>(hana::drop_back(path,hana::size_c<1>))
                (key()(std::forward<OpT>(op),std::forward<T1>(b)));
    }

    /**
     * @brief Get the last key in the path corresponding to the member at current level
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

    /**
     * @brief Construct a member with explicit name from the current member.
     * @param name Explicit name
     * @return Member with explicit name
     */
    template <typename T1>
    auto operator () (T1&& name,
                      std::enable_if_t<
                            std::is_constructible<concrete_phrase,T1>::value
                            &&
                            !hana::is_a<operator_tag,T1>
                         ,void*> =nullptr);

    /**
     * @brief Construct a member with explicit name and lexical attributes from the current member.
     * @param name Explicit name
     * @param attribute First lexical attribute of the name
     * @param attributes The rest lexical attributes of the name
     * @return Member with explicit name
     */
    template <typename T1, typename ... Attributes>
    auto operator () (T1&& name,
                      word attribute,
                      Attributes&&... attributes);

    /**
     * @brief Stub for getting explicit name
     * @return Empty string
     */
    constexpr static const char* name()
    {
        return "";
    }

    /**
     * @brief Strub for has_name() property
     * @return Always false
     */
    constexpr static bool has_name()
    {
        return false;
    }
};

template <typename T, typename ...ParentPathT>
struct member_with_name : public member<T,ParentPathT...>
{
    using base_type=member<T,ParentPathT...>;

    /**
     * @brief Constructor with name
     * @param path Member's path
     * @name name member's name
     */
    template <typename T1>
    member_with_name(typename base_type::path_type path,
           T1&& name)
         : base_type(std::move(path)),
           _name(std::forward<T1>(name))
    {}

    /**
     * @brief Get member's name
     * @return Name
     */
    concrete_phrase name() const
    {
        return _name;
    }

    /**
     * @brief Check if member has a name
     * @return Always true
     */
    constexpr static bool has_name()
    {
        return true;
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

    concrete_phrase _name;
};

template <typename T, typename ...ParentPathT>
template <typename T1>
auto member<T,ParentPathT...>::operator ()
        (T1&& name,
          std::enable_if_t<
                std::is_constructible<concrete_phrase,T1>::value
                &&
                !hana::is_a<operator_tag,T1>
             ,void*>)
{
    return member_with_name<T,ParentPathT...>(path,std::forward<T1>(name));
}

template <typename T, typename ...ParentPathT>
template <typename T1, typename ... Attributes>
auto member<T,ParentPathT...>::operator ()
        (T1&& name,
         word attribute,
         Attributes&&... attributes
         )
{
    return member_with_name<T,ParentPathT...>(path,concrete_phrase(std::forward<T1>(name),attribute,std::forward<Attributes>(attributes)...));
}

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_MEMBER_HPP
