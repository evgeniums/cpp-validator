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

#include <string>

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/basic_property.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

namespace detail {

/**
 * @brief Helper to figure out if value transformer is invokable for given type.
 */
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

//-------------------------------------------------------------

/**
 * @brief Special property that transforms preceding value using some handler.
 */
template <typename HandlerT, typename HasFlagT=hana::false_>
struct value_transformer_t : public basic_property
{
    HandlerT _handler;
    std::string _name;
    std::string _flag_str;
    std::string _n_flag_str;

    /**
     * @brief Constructor.
     * @param handler Handler used to transform value.
     * @param name Name of this property.
     * @param flag_str Flag description of this property.
     * @param n_flag_str Negative flag description of this property.
     */
    template <typename HandlerT1>
    value_transformer_t(
            HandlerT1&& handler,
            std::string name,
            std::string flag_str,
            std::string n_flag_str
        ) : _handler(std::forward<HandlerT1>(handler)),
            _name(std::move(name)),
            _flag_str(std::move(flag_str)),
            _n_flag_str(std::move(n_flag_str))
    {}

    /**
     * @brief Get property from the value.
     * @param v Value.
     * @return Value transformed with handler.
     */
    template <typename T>
    auto get(T&& v) const -> decltype(auto)
    {
        return _handler(std::forward<T>(v));
    }

    /**
     * @brief Check if value of givent type can be transformed with handler of this property.
     */
    template <typename T>
    constexpr static bool has()
    {
        return detail::is_transformer_invokable<T,HandlerT>::value;
    }

    /**
     * @brief Create poperty validator with this property.
     * @param args Arguments to forward to property validator.
     * @return Property validator.
     */
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

//-------------------------------------------------------------

/**
 * @brief Implementer of make_value_transformer().
 */
struct make_value_transformer_impl
{
    /**
     * @brief Create property for value transformation.
     * @param handler Handler to use for value transformation.
     * @param name Name of the property.
     * @return Property of value_transformer_t type.
     */
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

    /**
     * @brief Create property for value transformation.
     * @param handler Handler to use for value transformation.
     * @param name Name of the property.
     * @param flag_str Flag description of this property.
     * @param n_flag_str Negative flag description of this property.
     * @return Property of value_transformer_t type.
     */
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
/**
 * @brief Create property for value transformation.
 *
 * See descriptions of operators above in make_value_transformer_impl.
*/
constexpr make_value_transformer_impl make_value_transformer{};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_VALUE_TRANSFORMER_HPP
