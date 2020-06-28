/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** \file validator/operator.hpp
*
*  Defines base operator classes
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_OPERATOR_HPP
#define DRACOSHA_VALIDATOR_OPERATOR_HPP

#include <string>
#include <type_traits>

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/scalar_compare.hpp>
#include <dracosha/validator/adjust_storable_type.hpp>
#include <dracosha/validator/detail/has_method.hpp>
#include <dracosha/validator/property.hpp>
#include <dracosha/validator/can_get.hpp>
#include <dracosha/validator/get.hpp>
#include <dracosha/validator/can_check_contains.hpp>
#include <dracosha/validator/contains.hpp>
#include <dracosha/validator/check_member.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

struct operator_tag;

struct op
{
    using hana_tag=operator_tag;
};

//-------------------------------------------------------------

struct gte_t : public op
{
    template <typename T1, typename T2>
    constexpr bool operator() (const T1& a, const T2& b) const
    {
        return compare<T2,T1>::less_equal(b,a);
    }
};
constexpr gte_t gte{};

//-------------------------------------------------------------

namespace detail
{

template <typename T>
constexpr auto take_address_of(T&& t) -> decltype(&t)
{
    return &t;
}

BOOST_HANA_CONSTEXPR_LAMBDA auto iterate_exists =[](auto&& obj,auto&& key)
{
    if (obj && contains(*obj,key))
    {
        return take_address_of(get(std::forward<decltype(*obj)>(*obj),std::forward<decltype(key)>(key)));
    }
    return decltype(take_address_of(get(std::forward<decltype(*obj)>(*obj),std::forward<decltype(key)>(key))))(nullptr);
};
}

BOOST_HANA_CONSTEXPR_LAMBDA auto check_exists =[](auto&& obj,auto&& path)
{
    return hana::fold(std::forward<decltype(path)>(path),&obj,detail::iterate_exists)!=nullptr;
};

struct exists_t : public op
{
    template <typename T1, typename T2>
    constexpr bool operator() (const T1& a, const T2& b) const
    {
        return check_exists(a,b);
    }
};
constexpr exists_t exists{};

//-------------------------------------------------------------

struct invokable_tag;
template <typename T>
struct invokable_t
{
    using hana_tag=invokable_tag;
    T fn;

    auto operator()() const -> decltype(fn())
    {
        return fn();
    }
};

BOOST_HANA_CONSTEXPR_LAMBDA auto invokable = [](auto&& fn) -> invokable_t<decltype(fn)>
{
    return invokable_t<decltype(fn)>{std::forward<decltype(fn)>(fn)};
};

//-------------------------------------------------------------

BOOST_HANA_CONSTEXPR_LAMBDA auto extract = [](auto&& v) ->decltype(auto)
{
  return hana::if_(hana::is_a<invokable_tag,decltype(v)>,
    [](auto&& x) -> decltype(auto) { return x(); },
    [](auto&& x) -> decltype(auto) { return hana::id(std::forward<decltype(x)>(x)); }
  )(std::forward<decltype(v)>(v));
};

//-------------------------------------------------------------

struct member_tag;
struct master_reference_tag;

//-------------------------------------------------------------
/**
  @brief Extract last member's object in the path
  @param Top level object
  @param Path to the member to extract
  @return Extracted member

    In most cases a reference will be returned except for the properties that return rvalues, e.g. size() or empty().
*/
BOOST_HANA_CONSTEXPR_LAMBDA auto extract_back= [](auto&& v, auto&& path) -> decltype(auto)
{
    return hana::fold(std::forward<decltype(path)>(path),std::forward<decltype(v)>(v),
            [](auto&& field, auto&& key) -> decltype(auto)
            {
                return get(std::forward<decltype(field)>(field),std::forward<decltype(key)>(key));
            }
        );
};

//-------------------------------------------------------------

/**
 * @brief Validator
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
        // first two arguments of invoke are flipped against the order fo those arguments in operator ()
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
        auto path_c=hana::transform(member.path,hana::make_type);
        auto a_c=hana::type_c<decltype(a)>;

        return hana::if_(hana::is_nothing(hana::monadic_fold_left<hana::optional_tag>(path_c,a_c,hana::sfinae(check_member))),
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
                    property(extract_back(ax,member.path),std::forward<PropT>(prop)),
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
                    property(extract_back(ax,member.path),prop),
                    property(extract_back(ax,b.path),prop)
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
                    property(extract_back(ax,member.path),prop),
                    property(extract_back(b(),member.path),prop)
                );
    }
};
constexpr validate_t validate{};

//-------------------------------------------------------------

BOOST_HANA_CONSTEXPR_LAMBDA auto validate_invoke = [](auto&&... args) -> decltype(auto)
{
    return validate.invoke(std::forward<decltype(args)>(args)...);
};

struct prepare_validate_t
{
    template <typename ... Args>
    constexpr auto operator() (Args&&... args) const -> decltype(auto)
    {
        return hana::reverse_partial(validate_invoke,std::forward<Args>(args)...);
    }
};
constexpr prepare_validate_t prepare_validate{};

//-------------------------------------------------------------

struct type_p_value
{
    using hana_tag=property_tag;
    template <typename T>
    constexpr static T get(T&& v)
    {
        return hana::id(std::forward<T>(v));
    }
    template <typename T>
    constexpr static T has()
    {
        return true;
    }
    template <typename ... Args> \
    constexpr auto operator () (Args&&... args) const;
};
constexpr type_p_value value{};
template <typename ... Args>
constexpr auto type_p_value::operator () (Args&&... args) const
{
    return prepare_validate(value,std::forward<Args>(args)...);
}

//-------------------------------------------------------------
struct validator_tag;

BOOST_HANA_CONSTEXPR_LAMBDA auto apply = [](auto&& a,auto&& v) -> decltype(auto)
{
    return hana::if_(hana::is_a<validator_tag,decltype(v)>,
      [&a](auto&& x) -> decltype(auto) { return x.apply(a); },
      [&a](auto&& x) -> decltype(auto) { return x(a); }
    )(std::forward<decltype(v)>(v));
};

BOOST_HANA_CONSTEXPR_LAMBDA auto apply_member = [](auto&& a,auto&& v,auto&& member) -> decltype(auto)
{
    return hana::if_(hana::is_a<validator_tag,decltype(v)>,
      [&a,&member](auto&& x) -> decltype(auto) { return x.apply(a,member); },
      [&a,&member](auto&& x) -> decltype(auto) { return x(a,member); }
    )(std::forward<decltype(v)>(v));
};

//-------------------------------------------------------------

namespace detail
{

template <typename Handler>
struct validator
{
    using hana_tag=validator_tag;

    Handler fn;

    validator(Handler fn):fn(std::move(fn))
    {
    }

    template <typename ... Args>
    auto apply(Args&&... args) const -> decltype(auto)
    {
        return fn(std::forward<Args>(args)...);
    }
};
BOOST_HANA_CONSTEXPR_LAMBDA auto make_validator = [](auto fn)
{
    return validator<decltype(fn)>(std::move(fn));
};

template <template <typename...> class V>
struct tuple_to_variadic
{
    template <typename T, typename ... Prefix>
    constexpr static auto to_template(T&& t, Prefix&&... prefix)
    {
        auto concat=hana::concat(hana::make_tuple(prefix...),t);
        auto types_tuple=hana::transform(concat,hana::make<hana::type_tag>);
        return hana::unpack(types_tuple,hana::template_<V>);
    }
};

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
     * @brief Bind compound validator with current member
     * @param v Prepared partial validator
     * @return Prepared partial validator bound to current member
     */
    template <typename T1>
    auto operator () (T1&& v) const -> decltype(auto)
    {
        return make_validator(hana::reverse_partial(apply_member,std::forward<T1>(v),*this));
    }

    /**
     * @brief Bind plain operator with current member
     * @param op Operator
     * @param b Argument to forward to operator
     * @return Prepared partial validator of "value" property bound to current member
     */
    template <typename OpT, typename T1>
    auto operator () (OpT&& op, T1&& b) const -> decltype(auto)
    {
        return (*this)(value(std::forward<OpT>(op),std::forward<T1>(b)));
    }

    /**
     * @brief Get key of the member at current level
     * @return Key of current member
     */
    const type& key() const
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
    auto operator [] (T1&& key) const -> decltype(auto)
    {
        auto tmpl=tuple_to_variadic<member>::to_template(path,typename adjust_storable_type<T1>::type(key));
        return typename decltype(tmpl)::type(std::forward<T1>(key),path);
    }
};

}

//-------------------------------------------------------------
namespace detail
{
    template <typename T>
    struct master_reference
    {
        using hana_tag=master_reference_tag;

        master_reference(const T& obj) : ref(obj)
        {}

        const T& operator() () const
        {
            return ref;
        }

        const T& ref;
    };
}

//-------------------------------------------------------------

struct _t
{
    template <typename T>
    constexpr auto operator [] (T&& key) const -> decltype(auto)
    {
        return detail::member<typename std::decay<T>::type>(std::forward<T>(key));
    }

    template <typename T>
    constexpr auto operator () (const T& masterRefObj) const -> decltype(auto)
    {
        return detail::master_reference<T>(masterRefObj);
    }
};
constexpr _t _{};

//-------------------------------------------------------------

struct aggregate_and_t
{
    template <typename T, typename OpsT>
    constexpr bool operator ()(T&& a,OpsT&& ops) const
    {
        return hana::fold(std::forward<decltype(ops)>(ops),true,
                    [&a](bool prevResult, auto&& op)
                    {
                        if (!prevResult)
                        {
                            return false;
                        }
                        return apply(std::forward<decltype(a)>(a),std::forward<decltype(op)>(op));
                    }
                );
    }

    template <typename T, typename OpsT, typename MemberT>
    constexpr bool operator () (T&& a,MemberT&& member,OpsT&& ops) const
    {
        return hana::fold(std::forward<decltype(ops)>(ops),true,
                    [&a,&member](bool prevResult, auto&& op)
                    {
                        if (!prevResult)
                        {
                            return false;
                        }
                        return apply_member(std::forward<decltype(a)>(a),std::forward<decltype(op)>(op),std::forward<decltype(member)>(member));
                    }
                );
    }
};
aggregate_and_t aggregate_and{};

struct aggregate_or_t
{
    template <typename T, typename OpsT>
    constexpr bool operator ()(T&& a,OpsT&& ops) const
    {
        return hana::value(hana::length(ops))==0
                ||
               hana::fold(std::forward<decltype(ops)>(ops),false,
                    [&a](bool prevResult, auto&& op)
                    {
                        if (prevResult)
                        {
                            return true;
                        }
                        return apply(std::forward<decltype(a)>(a),std::forward<decltype(op)>(op));
                    }
                );
    }

    template <typename T, typename OpsT, typename MemberT>
    constexpr bool operator () (T&& a,MemberT&& member,OpsT&& ops) const
    {
        return hana::value(hana::length(ops))==0
                ||
               hana::fold(std::forward<decltype(ops)>(ops),false,
                    [&a,&member](bool prevResult, auto&& op)
                    {
                        if (prevResult)
                        {
                            return true;
                        }
                        return apply_member(std::forward<decltype(a)>(a),std::forward<decltype(op)>(op),std::forward<decltype(member)>(member));
                    }
                );
    }
};
aggregate_or_t aggregate_or{};

BOOST_HANA_CONSTEXPR_LAMBDA auto AND=hana::infix([](auto&& ...xs) -> decltype(auto)
{
    return detail::make_validator(
                hana::reverse_partial(
                    aggregate_and,
                    hana::make_tuple(std::forward<decltype(xs)>(xs)...)
                )
           );
});

BOOST_HANA_CONSTEXPR_LAMBDA auto OR=hana::infix([](auto&& ...xs) -> decltype(auto)
{
    return detail::make_validator(
                hana::reverse_partial(
                    aggregate_or,
                    hana::make_tuple(std::forward<decltype(xs)>(xs)...)
                )
           );
});

//-------------------------------------------------------------

struct validator_t
{
    template <typename ... Args>
    constexpr auto operator () (Args&& ...args) const -> decltype(auto)
    {
        return AND(std::forward<Args>(args)...);
    }

    template <typename T>
    constexpr auto operator () (T&& v) const -> decltype(auto)
    {
        return hana::id(std::forward<T>(v));
    }
};
constexpr validator_t validator{};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_PROPERTY(size)
DRACOSHA_VALIDATOR_PROPERTY(empty)

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_OPERATOR_HPP
