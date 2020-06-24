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
        return a>=b;
    }
};
constexpr gte_t gte{};

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

BOOST_HANA_CONSTEXPR_LAMBDA auto extract = [](auto&& v)
{
  return hana::if_(hana::is_a<invokable_tag,decltype(v)>,
    [](auto&& x) { return x(); },
    [](auto&& x) { return hana::id(std::forward<decltype(x)>(x)); }
  )(std::forward<decltype(v)>(v));
};

//-------------------------------------------------------------

BOOST_HANA_CONSTEXPR_LAMBDA auto property = [](auto&& val, auto&& prop)
{
    return std::decay<decltype(prop)>::type::get(std::forward<decltype(val)>(val));
};

//-------------------------------------------------------------

struct property_tag;

#define DRACOSHA_VALIDATOR_HAS_PROPERTY_FN(val,prop) hana::is_valid([](auto&& v) -> decltype((void)v.prop()){})(val)
#define DRACOSHA_VALIDATOR_HAS_PROPERTY(val,prop) hana::is_valid([](auto&& v) -> decltype((void)v.prop){})(val)

#define DRACOSHA_VALIDATOR_PROPERTY(prop) \
    auto try_get_##prop =[](auto&& v) \
    { \
      return hana::if_(DRACOSHA_VALIDATOR_HAS_PROPERTY_FN(v,prop), \
        [](auto&& x) { return x.prop(); }, \
        [](auto&& vv) { \
                return hana::if_(DRACOSHA_VALIDATOR_HAS_PROPERTY(vv,prop), \
                  [](auto&& x) { return x.prop; }, \
                  [](auto&& x) { return hana::id(std::forward<decltype(x)>(x)); } \
                )(std::forward<decltype(vv)>(vv)); \
            } \
      )(std::forward<decltype(v)>(v)); \
    }; \
    struct type_p_##prop \
    { \
        using hana_tag=property_tag; \
        template <typename T> \
        constexpr static size_t get(T&& v) \
        { \
            return try_get_##prop(std::forward<T>(v)); \
        } \
        template <typename OpT, typename T> \
        constexpr auto operator () (OpT op, T b) const; \
    }; \
    constexpr type_p_##prop prop{}; \
    template <typename OpT, typename T> \
    constexpr auto type_p_##prop::operator () (OpT op, T b) const \
    { \
        return prepare_validate(prop,std::move(op),std::move(b)); \
    }

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
        return op(
                    property(extract(std::forward<T1>(a)),std::forward<PropT>(prop)),
                    extract(std::forward<T2>(b))
                );
    }

    template <typename T1, typename T2, typename OpT, typename PropT>
    constexpr static bool invoke(T1&& a, PropT&& prop, OpT&& op, T2&& b)
    {
        return op(
                    property(extract(std::forward<T1>(a)),std::forward<PropT>(prop)),
                    extract(std::forward<T2>(b))
                );
    }
};
constexpr validate_t validate{};

BOOST_HANA_CONSTEXPR_LAMBDA auto validate_invoke = [](auto&& prop, auto&& a, auto&& op, auto&& b)
{
    return validate.invoke(std::forward<decltype(prop)>(prop),
                           std::forward<decltype(a)>(a),
                           std::forward<decltype(op)>(op),
                           std::forward<decltype(b)>(b)
                           );
};

BOOST_HANA_CONSTEXPR_LAMBDA auto prepare_validate = [](auto prop, auto op, auto b)
{
    return hana::reverse_partial(validate_invoke,std::move(prop),std::move(op),std::move(b));
};

//-------------------------------------------------------------

struct type_p_value
{
    using hana_tag=property_tag;
    template <typename T>
    constexpr static T get(T&& v)
    {
        return hana::id(std::forward<T>(v));
    }
    template <typename OpT, typename T> \
    constexpr auto operator () (OpT op, T b) const;
};
constexpr type_p_value value{};
template <typename OpT, typename T>
constexpr auto type_p_value::operator () (OpT op, T b) const
{
    return prepare_validate(value,std::move(op),std::move(b));
}

//-------------------------------------------------------------

BOOST_HANA_CONSTEXPR_LAMBDA auto get =[](auto&& v, const auto& k)
{
    return hana::if_(hana::is_a<property_tag,decltype(k)>,
      [&v](auto&& x) { return property(v,x); },
      [&v](auto&& x) { return v[x]; }
    )(std::forward<decltype(k)>(k));
};

//-------------------------------------------------------------
struct validator_tag;

BOOST_HANA_CONSTEXPR_LAMBDA auto apply = [](auto&& a,auto&& v)
{
    return hana::if_(hana::is_a<validator_tag,decltype(v)>,
      [&a](auto&& x) { return x.apply(a); },
      [&a](auto&& x) { return x(a); }
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
    auto apply(Args&&... args) const
    {
        return fn(std::forward<Args>(args)...);
    }
};
BOOST_HANA_CONSTEXPR_LAMBDA auto make_validator = [](auto fn)
{
    return validator<decltype(fn)>(std::move(fn));
};

struct single_validator_tag;
template <typename T>
struct compose_single_validator
{
    using hana_tag=single_validator_tag;

    T key;

    template <typename T1>
    compose_single_validator(T1&& key):key(std::forward<T1>(key))
    {}

    compose_single_validator(std::string str):key(std::move(str))
    {}

    template <typename OpT, typename T1>
    auto operator () (OpT&& op, T1&& b) const
    {
        return make_validator(hana::compose(
                        value(std::forward<OpT>(op),std::forward<T1>(b)),
                        hana::reverse_partial(get,key)
                    ));
    }

    template <typename T1>
    auto operator () (T1&& v) const
    {
        return make_validator(hana::compose(
                        hana::reverse_partial(apply,std::forward<T1>(v)),
                        hana::reverse_partial(get,key)
                    ));
    }
};

}

struct _t
{
    template <typename T>
    constexpr auto operator [] (T&& key) const
    {
        return detail::compose_single_validator<typename std::decay<T>::type>(std::forward<T>(key));
    }
};
constexpr _t _{};

//-------------------------------------------------------------

BOOST_HANA_CONSTEXPR_LAMBDA auto aggregate_and = [](auto&& a,auto&& ops)
{
    return hana::back(
        hana::scan_left(ops,true,
            [&a](bool prevResult, auto&& op)
            {
                return prevResult && apply(a,op);
            }
        )
    );
};

BOOST_HANA_CONSTEXPR_LAMBDA auto aggregate_or = [](auto&& a,auto&& ops)
{
    return hana::value(hana::length(ops))==0
            ||
           hana::back(
             hana::scan_left(ops,false,
                [&a](bool prevResult, auto&& op)
                {
                    return prevResult || apply(a,std::forward<decltype(op)>(op));
                }
            )
    );
};

BOOST_HANA_CONSTEXPR_LAMBDA auto AND=hana::infix([](auto&& ...xs)
{
    return detail::make_validator(
                hana::reverse_partial(
                    aggregate_and,
                    hana::make_tuple(std::forward<decltype(xs)>(xs)...)
                )
           );
});

BOOST_HANA_CONSTEXPR_LAMBDA auto OR=hana::infix([](auto&& ...xs)
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
    constexpr auto operator () (Args&& ...args) const
    {
        return AND(std::forward<Args>(args)...);
    }

    template <typename T>
    constexpr auto operator () (T&& v) const
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
