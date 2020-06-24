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

#include <type_traits>

#include <boost/hana.hpp>

#include <dracosha/validator/config.hpp>

namespace hana=boost::hana;

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

struct _t
{

};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_PROPERTY(size)
DRACOSHA_VALIDATOR_PROPERTY(empty)

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_OPERATOR_HPP
