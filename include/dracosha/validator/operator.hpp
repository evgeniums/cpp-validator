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

#include <boost/hana.hpp>

#include <dracosha/validator/config.hpp>

namespace hana=boost::hana;
using namespace hana::literals;

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

template <typename LeftT, typename RightT, typename=void>
struct scalar_compare
{
    constexpr static bool less(const LeftT& left, const RightT& right) noexcept
    {
        return left < right;
    }
    constexpr static bool equal(const LeftT& left, const RightT& right) noexcept
    {
        return left == right;
    }
    constexpr static bool less_equal(const LeftT& left, const RightT& right) noexcept
    {
        return left <= right;
    }
};
template <typename LeftT, typename RightT>
struct scalar_compare<LeftT,RightT,
                        std::enable_if_t<
                            std::is_signed<LeftT>::value &&
                            !std::is_floating_point<LeftT>::value &&
                            std::is_unsigned<RightT>::value &&
                            !std::is_same<RightT, bool>::value
                        >
                    >
{
    constexpr static bool less(const LeftT& left, const RightT& right) noexcept
    {
        if (left < 0)
        {
            return true;
        }
        return static_cast<std::make_unsigned_t<LeftT>>(left) < right;
    }
    constexpr static bool equal(const LeftT& left, const RightT& right) noexcept
    {
        return static_cast<std::make_unsigned_t<LeftT>>(left) == right;
    }
    constexpr static bool less_equal(const LeftT& left, const RightT& right) noexcept
    {
        if (left < 0)
        {
            return true;
        }
        return static_cast<std::make_unsigned_t<LeftT>>(left) <= right;
    }
};

template <typename LeftT, typename RightT>
struct scalar_compare<LeftT, RightT,
        std::enable_if_t<
            std::is_unsigned<LeftT>::value &&
            std::is_signed<RightT>::value &&
            !std::is_floating_point<RightT>::value &&
            !std::is_same<LeftT, bool>::value>
    >
{
    constexpr static bool less(const LeftT& left, const RightT& right) noexcept
    {
        if (right < 0)
        {
            return false;
        }
        return left < static_cast<std::make_unsigned_t<RightT>>(right);
    }
    constexpr static bool equal(const LeftT& left, const RightT& right) noexcept
    {
        return left == static_cast<std::make_unsigned_t<RightT>>(right);
    }
    constexpr static bool less_equal(const LeftT& left, const RightT& right) noexcept
    {
        if (right < 0)
        {
            return false;
        }
        return left <= static_cast<std::make_unsigned_t<RightT>>(right);
    }
};

template <typename LeftT,typename RightT>
struct scalar_compare<LeftT,RightT,
                    std::enable_if_t<!std::is_same<LeftT, bool>::value && std::is_same<RightT, bool>::value>
                >
{
    constexpr static bool less(const LeftT& left, const RightT& right) noexcept
    {
        return static_cast<bool>(left) < right;
    }
    constexpr static bool equal(const LeftT& left, const RightT& right) noexcept
    {
        return static_cast<bool>(left)==right;
    }
    constexpr static bool less_equal(const LeftT& left, const RightT& right) noexcept
    {
        return static_cast<bool>(left) <= right;
    }
};
template <typename LeftT, typename RightT>
struct scalar_compare<LeftT, RightT,
        std::enable_if_t<std::is_same<LeftT, bool>::value && !std::is_same<RightT, bool>::value>
    >
{
    constexpr static bool less(const LeftT& left, const RightT& right) noexcept
    {
        return left < static_cast<bool>(right);
    }
    constexpr static bool equal(const LeftT& left, const RightT& right) noexcept
    {
        return left==static_cast<bool>(right);
    }
    constexpr static bool less_equal(const LeftT& left, const RightT& right) noexcept
    {
        return left <= static_cast<bool>(right);
    }
};

template <typename LeftT, typename RightT, typename=void>
struct compare
{
};

template <typename LeftT, typename RightT>
struct compare<LeftT, RightT,
        std::enable_if_t<!std::is_scalar<LeftT>::value && !std::is_scalar<RightT>::value>>
{
    constexpr static bool less(const LeftT& left, const RightT& right) noexcept
    {
        return left < right;
    }
    constexpr static bool equal(const LeftT& left, const RightT& right) noexcept
    {
        return left==right;
    }
    constexpr static bool less_equal(const LeftT& left, const RightT& right) noexcept
    {
        return left <= right;
    }
};

template <typename LeftT, typename RightT>
struct compare<LeftT, RightT,
        std::enable_if_t<std::is_scalar<LeftT>::value || std::is_scalar<RightT>::value>
    > : public scalar_compare<LeftT,RightT>
{
};

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

BOOST_HANA_CONSTEXPR_LAMBDA auto has_at = hana::is_valid([](auto&& v, auto&& x) -> decltype((void)v.at(std::forward<decltype(x)>(x))){});
BOOST_HANA_CONSTEXPR_LAMBDA auto has_has = hana::is_valid([](auto&& v, auto&& x) -> decltype((void)v.has(std::forward<decltype(x)>(x))){});
BOOST_HANA_CONSTEXPR_LAMBDA auto has_contains = hana::is_valid([](auto&& v, auto&& x) -> decltype((void)v.contains(std::forward<decltype(x)>(x))){});
BOOST_HANA_CONSTEXPR_LAMBDA auto has_find = hana::is_valid([](auto&& v, auto&& x) -> decltype((void)v.find(std::forward<decltype(x)>(x))){});
BOOST_HANA_CONSTEXPR_LAMBDA auto has_isSet = hana::is_valid([](auto&& v, auto&& x) -> decltype((void)v.isSet(std::forward<decltype(x)>(x))){});

BOOST_HANA_CONSTEXPR_LAMBDA auto has_at_c = hana::is_valid([](auto&& v, auto&& x) -> decltype(
                                                            (void)hana::traits::declval(v).at(hana::traits::declval(x))
                                                           )
                                                        {});
BOOST_HANA_CONSTEXPR_LAMBDA auto has_brackets_c = hana::is_valid([](auto&& v, auto&& x) -> decltype(
                                                            (void)hana::traits::declval(v)[hana::traits::declval(x)]
                                                           )
                                                        {});

BOOST_HANA_CONSTEXPR_LAMBDA auto has_find_c = hana::is_valid([](auto v, auto x) -> decltype(
                                                                    (void)hana::traits::declval(v).find(hana::traits::declval(x))
                                                                )
                                                            {});
BOOST_HANA_CONSTEXPR_LAMBDA auto has_has_c = hana::is_valid([](auto v, auto x) -> decltype(
                                                                    (void)hana::traits::declval(v).has(hana::traits::declval(x))
                                                                )
                                                            {});
BOOST_HANA_CONSTEXPR_LAMBDA auto has_contains_c = hana::is_valid([](auto v, auto x) -> decltype(
                                                                    (void)hana::traits::declval(v).contains(hana::traits::declval(x))
                                                                )
                                                            {});
BOOST_HANA_CONSTEXPR_LAMBDA auto has_isSet_c = hana::is_valid([](auto v, auto x) -> decltype(
                                                                    (void)hana::traits::declval(v).isSet(hana::traits::declval(x))
                                                                )
                                                            {});

//-------------------------------------------------------------

template <typename T, typename=hana::when<true>>
struct adjust_type
{
};
template <typename T>
struct adjust_type<T,
                    hana::when<std::is_constructible<std::string,T>::value>
                    >
{
    using type=std::string;
};
template <typename T>
struct adjust_type<T,
                    hana::when<!std::is_constructible<std::string,T>::value>
                    >
{
    using type=typename std::decay<T>::type;
};

//-------------------------------------------------------------

}

//-------------------------------------------------------------

namespace detail
{
BOOST_HANA_CONSTEXPR_LAMBDA auto has_property_c = hana::is_valid([](auto val, auto prop) -> decltype(
                                                                (void)hana::traits::declval(prop).get(hana::traits::declval(val))
                                                           )
                                                        {});
}

struct property_tag;
BOOST_HANA_CONSTEXPR_LAMBDA auto property = [](auto&& val, auto&& prop) -> decltype(auto)
{
    return std::decay<decltype(prop)>::type::get(std::forward<decltype(val)>(val));
};

//-------------------------------------------------------------

BOOST_HANA_CONSTEXPR_LAMBDA auto get =[](auto&& v, auto&& k) -> decltype(auto)
{
    return hana::if_(hana::is_a<property_tag,decltype(k)>,
      [&v](auto&& x) -> decltype (auto) { return property(v,x); },
      [&v](auto&& x) -> decltype (auto) {
            return hana::if_(detail::has_at(v,x),
                [&v](auto&& j) -> decltype (auto) { return v.at(j); },
                [&v](auto&& j) -> decltype (auto) { return v[j]; }
            )(std::forward<decltype(x)>(x));
      }
    )(std::forward<decltype(k)>(k));
};

template <typename T1, typename T2>
struct can_get_t
{
    constexpr bool operator () () const
    {
        return detail::has_at_c(hana::type_c<T1>,hana::type_c<T2>)
               ||
               detail::has_brackets_c(hana::type_c<T1>,hana::type_c<T2>)
               ||
               (hana::is_a<property_tag,T2> && detail::has_property_c(hana::type_c<T1>,hana::type_c<T2>));
    }
};
template <typename T1, typename T2>
constexpr can_get_t<T1,T2> can_get{};

//-------------------------------------------------------------

template <typename T1, typename T2>
struct can_check_contains_t
{
    constexpr bool operator () () const
    {
        return detail::has_find_c(hana::type_c<T1>,hana::type_c<T2>)
                ||
               detail::has_has_c(hana::type_c<T1>,hana::type_c<T2>)
                ||
               detail::has_contains_c(hana::type_c<T1>,hana::type_c<T2>)
                ||
               detail::has_isSet_c(hana::type_c<T1>,hana::type_c<T2>)
                ||
               (hana::is_a<property_tag,T2> && detail::has_property_c(hana::type_c<T1>,hana::type_c<T2>));
    }
};
template <typename T1, typename T2>
constexpr can_check_contains_t<T1,T2> can_check_contains{};

namespace detail
{
    template <typename T1, typename T2, typename=hana::when<true>>
    struct contains_c
    {
    };
    template <typename T1, typename T2>
    struct contains_c<T1,T2,hana::when<can_check_contains<T1,T2>() && can_get<T1,T2>()>>
    {
        using type=typename std::decay<decltype(get(std::declval<T1>(),std::declval<T2>()))>::type;
    };
}

using contains_c_t = hana::metafunction_t<detail::contains_c>;
contains_c_t contains_c{};

BOOST_HANA_CONSTEXPR_LAMBDA auto safe_contains = [](auto&& a, auto&& b) -> decltype(auto)
{
    auto fn=hana::if_((hana::is_a<property_tag,decltype(b)> && detail::has_property_c(hana::type_c<decltype(a)>,hana::type_c<decltype(b)>)),
                [](const auto&, const auto&) { return true; },
                hana::if_(detail::has_has(a,b),
                    [](const auto& a1, const auto& b1) { return a1.has(b1); },
                    hana::if_(detail::has_contains(a,b),
                        [](const auto& a2, const auto& b2) { return a2.contains(b2); },
                        hana::if_(detail::has_find(a,b),
                            [](const auto& a3, const auto& b3) { return a3.find(b3)!=a3.end(); },
                            hana::if_(detail::has_isSet(a,b),
                                [](const auto& a4, const auto& b4) { return a4.isSet(b4); },
                                hana::nothing
                            )
                        )
                    )
            )
        );

    auto bn=[](bool ok){return ok;};
    return hana::chain(hana::sfinae(fn)(a,b),hana::sfinae(bn));
};

struct contains_t
{
    template <typename T1, typename T2>
    constexpr bool operator () (
                                const T1& a,
                                const T2& b,
                                std::enable_if_t<can_check_contains<T1,T2>(),
                                                                    void*> =nullptr
                             ) const
    {
        return safe_contains(a,b).value();
    }

    template <typename T1, typename T2>
    constexpr bool operator () (
                                const T1&,
                                const T2&,
                                std::enable_if_t<!can_check_contains<T1,T2>(),
                                                                    void*> =nullptr
                             ) const
    {
        return false;
    }
};
constexpr contains_t contains{};

//-------------------------------------------------------------

namespace detail
{
BOOST_HANA_CONSTEXPR_LAMBDA auto iterate_exists =[](auto&& obj,auto&& key)
{
    if (obj && contains(*obj,key))
    {
        return &get(std::forward<decltype(*obj)>(*obj),std::forward<decltype(key)>(key));
    }
    return decltype(&get(std::forward<decltype(*obj)>(*obj),std::forward<decltype(key)>(key)))(nullptr);
};
}

BOOST_HANA_CONSTEXPR_LAMBDA auto check_exists =[](auto&& obj,auto&& chain)
{
    return hana::fold(std::forward<decltype(chain)>(chain),&obj,detail::iterate_exists)!=nullptr;
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

struct single_validator_tag;
struct master_reference_tag;

#define DRACOSHA_VALIDATOR_HAS_PROPERTY_FN(val,prop) hana::is_valid([](auto&& v) -> decltype((void)v.prop()){})(val)
#define DRACOSHA_VALIDATOR_HAS_PROPERTY(val,prop) hana::is_valid([](auto&& v) -> decltype((void)v.prop){})(val)

#define DRACOSHA_VALIDATOR_PROPERTY(prop) \
    auto try_get_##prop =[](auto&& v) -> decltype(auto) \
    { \
      return hana::if_(DRACOSHA_VALIDATOR_HAS_PROPERTY_FN(v,prop), \
        [](auto&& x) -> decltype(auto) { return x.prop(); }, \
        [](auto&& vv) -> decltype(auto)  { \
                return hana::if_(DRACOSHA_VALIDATOR_HAS_PROPERTY(vv,prop), \
                  [](auto&& x) -> decltype(auto) { return x.prop; }, \
                  [](auto&& x) -> decltype(auto) { return hana::id(std::forward<decltype(x)>(x)); } \
                )(std::forward<decltype(vv)>(vv)); \
            } \
      )(std::forward<decltype(v)>(v)); \
    }; \
    struct type_p_##prop \
    { \
        using hana_tag=property_tag; \
        template <typename T> \
        constexpr static auto get(T&& v) -> decltype(auto) \
        { \
            return try_get_##prop(std::forward<T>(v)); \
        } \
        template <typename ...Args> \
        constexpr auto operator () (Args&&... args) const -> decltype(auto); \
    }; \
    constexpr type_p_##prop prop{}; \
    template <typename ...Args> \
    constexpr auto type_p_##prop::operator () (Args&&... args) const -> decltype(auto)\
    { \
        return prepare_validate(prop,std::forward<Args>(args)...); \
    }

//-------------------------------------------------------------

BOOST_HANA_CONSTEXPR_LAMBDA auto extract_back= [](auto&& v, auto&& ch) -> decltype(auto)
{
    return hana::fold(std::forward<decltype(ch)>(ch),std::forward<decltype(v)>(v),
            [](auto&& field, auto&& key) -> decltype(auto)
            {
                return get(std::forward<decltype(field)>(field),std::forward<decltype(key)>(key));
            }
        );
};

//-------------------------------------------------------------

struct validate_t
{
    template <typename T1, typename T2, typename OpT>
    constexpr bool operator() (T1&& a, OpT&& op, T2&& b) const
    {
        return op(
                    extract(std::forward<T1>(a)),
                    extract(std::forward<T2>(b))
                );
    }

    template <typename T1, typename T2, typename OpT, typename PropT>
    constexpr bool operator() (PropT&& prop, T1&& a, OpT&& op, T2&& b) const
    {
        // first two arguments of invoke are flipped against the order fo those arguments in operator
        return invoke(std::forward<T1>(a),std::forward<PropT>(prop),std::forward<OpT>(op),std::forward<T2>(b));
    }

    template <typename T1, typename T2, typename OpT, typename PropT>
    constexpr static bool invoke(T1&& a, PropT&& prop, OpT&& op, T2&& b,
                                     std::enable_if_t<
                                       !hana::is_a<single_validator_tag,T2>,
                                       void*
                                     > =nullptr
                                 )
    {
        return op(
                    property(extract(std::forward<T1>(a)),std::forward<PropT>(prop)),
                    extract(std::forward<T2>(b))
                );
    }

    template <typename T1, typename T2, typename OpT, typename PropT, typename ChainT>
    constexpr static bool invoke(T1&& a, ChainT&& chain, PropT&&, OpT&&, T2&& b,
                                 std::enable_if_t<std::is_same<exists_t,typename std::decay<OpT>::type>::value,
                                   void*
                                 > =nullptr
                                )
    {
        auto&& ax=extract(std::forward<T1>(a));
        return exists(ax,std::forward<decltype(chain)>(chain))==b;
    }

    template <typename T1, typename T2, typename OpT, typename PropT, typename ChainT>
    constexpr static bool invoke(T1&& a, ChainT&& chain, PropT&& prop, OpT&& op, T2&& b,
                                 std::enable_if_t<
                                   (!hana::is_a<single_validator_tag,T2> && !hana::is_a<master_reference_tag,T2> && !std::is_same<exists_t,typename std::decay<OpT>::type>::value),
                                   void*
                                 > =nullptr
                                )
    {
        auto&& ax=extract(std::forward<T1>(a));
        return op(
                    property(extract_back(ax,chain),std::forward<PropT>(prop)),
                    extract(std::forward<T2>(b))
                );
    }

    template <typename T1, typename T2, typename OpT, typename PropT, typename ChainT>
    constexpr static bool invoke(T1&& a, ChainT&& chain, PropT&& prop, OpT&& op, T2&& b,
                                 std::enable_if_t<
                                   hana::is_a<single_validator_tag,T2>,
                                   void*
                                 > =nullptr
                                )
    {
        auto&& ax=extract(std::forward<T1>(a));
        return op(
                    property(extract_back(ax,chain),prop),
                    property(extract_back(ax,b.chain),prop)
                );
    }

    template <typename T1, typename T2, typename OpT, typename PropT, typename ChainT>
    constexpr static bool invoke(T1&& a, ChainT&& chain, PropT&& prop, OpT&& op, T2&& b,
                                 std::enable_if_t<
                                   hana::is_a<master_reference_tag,T2>,
                                   void*
                                 > =nullptr
                                )
    {
        auto&& ax=extract(std::forward<T1>(a));
        return op(
                    property(extract_back(ax,chain),prop),
                    property(extract_back(b(),chain),prop)
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

BOOST_HANA_CONSTEXPR_LAMBDA auto apply_chain = [](auto&& a,auto&& v,auto&& chain) -> decltype(auto)
{
    return hana::if_(hana::is_a<validator_tag,decltype(v)>,
      [&a,&chain](auto&& x) -> decltype(auto) { return x.apply(a,chain); },
      [&a,&chain](auto&& x) -> decltype(auto) { return x(a,chain); }
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

BOOST_HANA_CONSTEXPR_LAMBDA auto key_chain_str= [](auto&& ch) -> decltype(auto)
{
    return hana::fold(std::forward<decltype(ch)>(ch),std::string(),
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

template <typename T, typename ...Chain>
struct compose_single_validator
{
    using hana_tag=single_validator_tag;
    using type=typename adjust_type<T>::type;

    hana::tuple<Chain...,type> chain;

    template <typename T1, typename ChainT>
    compose_single_validator(T1&& key, ChainT&& ch)
         : chain(hana::append(std::forward<ChainT>(ch),std::forward<T1>(key)))
    {}

    template <typename ChainT>
    compose_single_validator(type k, ChainT&& ch)
         : chain(hana::append(std::forward<ChainT>(ch),std::move(k)))
    {}

    template <typename T1>
    compose_single_validator(T1&& key)
         : chain(hana::make_tuple(std::forward<T1>(key)))
    {}

    compose_single_validator(std::string str)
         : chain(hana::make_tuple(std::move(str)))
    {}

    template <typename T1>
    auto operator () (T1&& v) const -> decltype(auto)
    {
        return make_validator(hana::reverse_partial(apply_chain,std::forward<T1>(v),chain));
    }

    template <typename OpT, typename T1>
    auto operator () (OpT&& op, T1&& b) const -> decltype(auto)
    {
        return (*this)(value(std::forward<OpT>(op),std::forward<T1>(b)));
    }

    const type& key() const
    {
        return hana::back(chain);
    }

    std::string format_key_chain() const
    {
        return key_chain_str(chain);
    }

    template <typename T1>
    auto operator [] (T1&& key) const -> decltype(auto)
    {
        auto tmpl=tuple_to_variadic<compose_single_validator>::to_template(chain,typename adjust_type<T1>::type(key));
        return typename decltype(tmpl)::type(std::forward<T1>(key),chain);
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
        return detail::compose_single_validator<typename std::decay<T>::type>(std::forward<T>(key));
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

    template <typename T, typename OpsT, typename ChainT>
    constexpr bool operator () (T&& a,ChainT&& chain,OpsT&& ops) const
    {
        return hana::fold(std::forward<decltype(ops)>(ops),true,
                    [&a,&chain](bool prevResult, auto&& op)
                    {
                        if (!prevResult)
                        {
                            return false;
                        }
                        return apply_chain(std::forward<decltype(a)>(a),std::forward<decltype(op)>(op),std::forward<decltype(chain)>(chain));
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

    template <typename T, typename OpsT, typename ChainT>
    constexpr bool operator () (T&& a,ChainT&& chain,OpsT&& ops) const
    {
        return hana::value(hana::length(ops))==0
                ||
               hana::fold(std::forward<decltype(ops)>(ops),false,
                    [&a,&chain](bool prevResult, auto&& op)
                    {
                        if (prevResult)
                        {
                            return true;
                        }
                        return apply_chain(std::forward<decltype(a)>(a),std::forward<decltype(op)>(op),std::forward<decltype(chain)>(chain));
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
