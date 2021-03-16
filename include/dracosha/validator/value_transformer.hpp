/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/properties/value_transformer.hpp
*
*  Defines value transformers to be used as properties.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_VALUE_TRANSFORMER_HPP
#define DRACOSHA_VALIDATOR_VALUE_TRANSFORMER_HPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/property.hpp>
#include <dracosha/validator/property_validator.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

namespace detail {

template <typename T, typename HandlerT, typename=hana::when<true>>
struct is_transformer_invokable
{
    constexpr static const bool value=false;
};

template <typename T, typename HandlerT>
struct is_transformer_invokable<T,HandlerT,
            hana::when<
                std::is_same<
                    decltype((void)std::declval<HandlerT>()(std::declval<std::decay_t<T>>()))
                    ,
                    void
                >::value
            >
        >
{
    constexpr static const bool value=true;
};

}

template <typename HandlerT, typename HasFlagT=hana::false_>
struct value_transformer_t
{
    using hana_tag=property_tag;

    HandlerT _handler;
    std::string _name;
    std::string _flag_str;
    std::string _n_flag_str;

    template <typename T>
    auto get(T&& v) const -> decltype(auto)
    {
        return _handler(std::forward<T>(v));
    }

    template <typename T>
    constexpr static bool has()
    {
        return detail::is_transformer_invokable<T,HandlerT>::value;
    }

    template <typename ... Args>
    auto operator () (Args&&... args) const
    {
        return make_property_validator(*this,std::forward<Args>(args)...);
    }

    auto name() const
    {
        return _name;
    }

    template <typename FormatterT>
    auto flag_str(bool b, const FormatterT&, bool =false) const
    {
        return b?_flag_str:_n_flag_str;
    }

    constexpr static bool has_flag_str()
    {
        return HasFlagT::value;
    }

    template <typename T> constexpr bool operator == (const T&&) const
    {
        return false;
    }
    template <typename T> constexpr bool operator != (const T&&) const
    {
        return true;
    }
    constexpr bool operator == (const value_transformer_t&) const
    {
        return true;
    }
    constexpr bool operator != (const value_transformer_t&) const
    {
        return false;
    }
};

struct make_value_transformer_impl
{
    template <typename HandlerT>
    auto operator() (HandlerT&& handler, std::string name) const
    {
        return value_transformer_t<std::decay_t<HandlerT>>{
            std::forward<HandlerT>(handler),
            std::move(name),
            std::string(),
            std::string()
        };
    }

    template <typename HandlerT>
    auto operator() (HandlerT&& handler, std::string name, std::string flag_str, std::string n_flag_str) const
    {
        return value_transformer_t<std::decay_t<HandlerT>,hana::true_>{
            std::forward<HandlerT>(handler),
            std::move(name),
            flag_str,
            n_flag_str
        };
    }
};
constexpr make_value_transformer_impl make_value_transformer{};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_VALUE_TRANSFORMER_HPP
