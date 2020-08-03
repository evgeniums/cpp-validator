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
#include <dracosha/validator/reporting/concrete_phrase.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

struct member_tag;

namespace detail
{

template <typename T1, typename = hana::when<true>>
struct member_helper_1arg_t
{
};

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
     * @brief Bind compound validator to current member
     * @param member Current member
     * @param v Prepared partial validator
     * @return Prepared partial validator bound to current member
     */
    template <typename MemberT>
    auto operator() (MemberT&& member, T1&& v) const
    {
        return make_validator(hana::reverse_partial(apply_member,std::forward<T1>(v),std::forward<MemberT>(member)));
    }
};

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
     * @brief Construct a member from the current member with explicit name
     * @param member Current member
     * @param name Explicit name
     * @return Member with explicit name
     */
    template <typename MemberT>
    auto operator() (MemberT&& member, T1&& name) const;
};

template <typename T1, typename T2, typename = hana::when<true>>
struct member_helper_2args_t
{
};

template <typename T1, typename T2>
struct member_helper_2args_t<T1,T2,
            hana::when<
                std::is_enum<std::decay_t<T2>>::value
            >
        >
{
    /**
     * @brief Construct a member from the current member with explicit name and grammatical category
     * @param member Current member
     * @param name Name and grammatical catrgories
     * @param grammar_category Grammatical categoty of the name
     * @return Member with explicit name
     */
    template <typename MemberT>
    auto operator() (MemberT&& member, T1&& name, T2&& grammar_category) const;
};

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
     * @brief Bind plain operator to current member
     * @param member Current member
     * @param op Operator
     * @param b Argument to forward to operator
     * @return Prepared partial validator of "value" property bound to current member
     */
    template <typename MemberT>
    auto operator() (MemberT&& member, OpT&& op, T1&& b) const
    {
        return member(value(std::forward<OpT>(op),std::forward<T1>(b)));
    }
};

template <typename OpT, typename T1>
struct member_helper_2args_t<OpT,T1,
            hana::when<
                std::is_base_of<flag_t,std::decay_t<OpT>>::value
            >
        >
{
    /**
     * @brief Rebind plain operator to the property validator if the last key in the path is a property and operator is a flag
     * @param member Current member
     * @param op Operator of flag type
     * @param b Argument to forward to operator
     * @return Prepared partial validator of the property corresponding to the last property key in the path
     */
    template <typename MemberT>
    auto operator() (MemberT&& member, OpT&& op, T1&& b,
                                std::enable_if_t<!MemberT::is_nested,void*> =nullptr
                            ) const
    {
        return make_validator(member.key()(std::forward<OpT>(op),std::forward<T1>(b)));
    }

    /**
     * @brief Rebind plain operator to the property validator of parent member if the last key in the path is a property and operator is a flag
     * @param member Current member
     * @param op Operator of flag type
     * @param b Argument to forward to operator
     * @return Prepared partial validator of the property of parent member corresponding to the last property key in the path
     */
    template <typename MemberT>
    auto operator () (MemberT&& member, OpT&& op, T1&& b,
                                std::enable_if_t<MemberT::is_nested,void*> =nullptr
                            ) const
    {
        auto&& fn=[&member,&op,&b](auto&& key, auto&&... rpath)
        {
            return member.make_parent(hana::reverse(hana::make_tuple(std::forward<decltype(rpath)>(rpath)...)))(
                            key(std::forward<OpT>(op),std::forward<T1>(b))
                        );
        };
        return hana::unpack(hana::reverse(std::move(member.path)),std::move(fn));
    }
};

template <typename ... Args>
struct member_helper_t
{
    /**
     * @brief Construct a member from the current member with explicit name and grammatical categories.
     * @param member Current member
     * @param args Name and grammatical catrgories
     * @return Member with explicit name
     */
    template <typename MemberT>
    auto operator() (MemberT&& member, Args&&... args) const;
};

template <typename T1>
struct member_helper_t<T1> : public member_helper_1arg_t<T1>
{
};

template <typename T1, typename T2>
struct member_helper_t<T1,T2> : public member_helper_2args_t<T1,T2>
{
};

template <typename ... Args>
constexpr member_helper_t<Args...> member_helper{};

}

/**
 *  @brief Generic descriptor of a member to be validated.
 *
 * Member descriptor defines a path for extracting specific member/subobject from the object under validation.
 */
template <typename T, typename ...ParentPathT>
struct member
{
    using hana_tag=member_tag;

    using type=T;
    using path_type=hana::tuple<ParentPathT...,type>;

    constexpr static const bool is_nested=sizeof...(ParentPathT)!=0;

    path_type path;

    template <template <typename ...> class T1,
              typename T2, typename PathT>
    static auto create_derived(T2&& name, PathT&& path)
    {
        return T1<T2,T,ParentPathT...>(std::forward<PathT>(path),std::forward<T2>(name));
    }

    member()=default;

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
     * @brief Callable operator
     */
    template <typename ... Args>
    auto operator() (Args&&... args)
    {
        return detail::member_helper<Args...>(std::move(*this),std::forward<Args>(args)...);
    }

    /**
     * @brief Make member of parent type
     * @param path Path of new member
     * @return Member of parent type
     */
    template <typename PathT>
    static auto make_parent(PathT&& path)
    {
        return member<ParentPathT...>(std::forward<PathT>(path));
    }

    /**
     * @brief Make super member prepending new key to the path
     * @param key First key of super member
     * @return Member of super type
     */
    template <typename KeyT>
    auto make_super(KeyT&& first_key) const
    {
        using stype=typename adjust_storable_type<std::decay_t<KeyT>>::type;
        return member<T,stype,ParentPathT...>(hana::prepend(path,stype(std::forward<KeyT>(first_key))));
    }

    /**
     * @brief Get the last key in the path corresponding to the member at current level
     * @return Key of current member
     */
    const type& key() const
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
     * @brief member does not have explicit name
     */
    constexpr static bool has_name = false;
};

/**
 * @brief Member with excplicit name
 */
template <typename T1, typename T2, typename ...ParentPathT>
struct member_with_name : public member<T2,ParentPathT...>
{
    using base_type=member<T2,ParentPathT...>;

    /**
     * @brief Constructor with name
     * @param path Member's path
     * @name name member's name
     */
    template <typename Tt1>
    member_with_name(typename base_type::path_type path,
           Tt1&& name)
         : base_type(std::move(path)),
           _name(std::forward<Tt1>(name))
    {}

    /**
     * @brief Get member's name
     * @return Name
     */
    auto name() const
    {
        return _name;
    }

    /**
     * @brief member_with_name has explicit name
     */
    constexpr static bool has_name = true;

    /**
     * @brief Bind compound validator to current member
     * @param v Prepared partial validator
     * @return Prepared partial validator bound to current member
     */
    template <typename Tt1>
    constexpr auto operator () (Tt1&& v) const -> decltype(auto)
    {
        return make_validator(hana::reverse_partial(apply_member,std::forward<Tt1>(v),*this));
    }

    /**
     * @brief Bind plain operator to current member
     * @param op Operator
     * @param b Argument to forward to operator
     * @return Prepared partial validator of "value" property bound to current member
     */
    template <typename OpT, typename Tt1>
    constexpr auto operator () (OpT&& op, Tt1&& b) const -> decltype(auto)
    {
        return (*this)(value(std::forward<OpT>(op),std::forward<Tt1>(b)));
    }

    std::decay_t<T1> _name;
};

template <typename MemberT, typename T>
auto make_member_with_name(MemberT&& member, T&& name)
{
    return member.template create_derived<member_with_name>(std::forward<T>(name),std::move(member.path));
}

template <typename T>
constexpr auto make_plain_member(T&& key)
{
    return member<
                typename adjust_storable_type<std::decay_t<T>>::type
            >(std::forward<T>(key));
}

namespace detail
{

template <typename T1>
template <typename MemberT>
auto member_helper_1arg_t<T1,hana::when<
        (
            std::is_constructible<concrete_phrase,T1>::value
            &&
            !hana::is_a<operator_tag,T1>
        )
    >>::operator ()
        (
            MemberT&& member,
            T1&& name
        ) const
{
    return make_member_with_name(std::forward<MemberT>(member),std::forward<T1>(name));
}

template <typename T1, typename T2>
template <typename MemberT>
auto member_helper_2args_t<T1,T2,hana::when<std::is_enum<std::decay_t<T2>>::value>>::operator ()
        (
             MemberT&& member,
             T1&& name,
             T2&& grammar_category
         ) const
{
    return make_member_with_name(std::forward<MemberT>(member),concrete_phrase(std::forward<T1>(name),std::forward<T2>(grammar_category)));
}

template <typename ... Args>
template <typename MemberT>
auto member_helper_t<Args...>::operator ()
      (
           MemberT&& member,
           Args&&... args
       ) const
{
  return make_member_with_name(std::forward<MemberT>(member),concrete_phrase(std::forward<Args>(args)...));
}

}

/**
* @brief Extract path from a member
* @param member Member
* @return Member's path
*/
template <typename T>
auto member_path(T&& member,
                 std::enable_if_t<hana::is_a<member_tag,T>,void*> =nullptr
                 ) -> decltype(auto)
{
    return member.path;
}

/**
* @brief Stub to emulate extracting path fro a member for non-members
* @param v Value
* @return Value wrapped into hana::tuple
*/
template <typename T>
auto member_path(T&& v,
                 std::enable_if_t<!hana::is_a<member_tag,T>,void*> =nullptr
                 )
{
    return hana::make_tuple(std::forward<T>(v));
}

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_MEMBER_HPP
