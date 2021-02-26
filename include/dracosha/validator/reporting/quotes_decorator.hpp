/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/reporting/decorator.hpp
*
* Defines quotes decorator.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_QUOTES_DECORATOR_HPP
#define DRACOSHA_VALIDATOR_QUOTES_DECORATOR_HPP

#include <string>

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/reporting/decorator.hpp>
#include <dracosha/validator/reporting/concrete_phrase.hpp>
#include <dracosha/validator/reporting/backend_formatter.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

namespace detail
{
/**
 * @brief Default helper for quotes decorator.
 */
template <typename T, typename = hana::when<true>>
struct quotes_decorator_helper
{
    static auto f(T&& val) -> decltype(auto)
    {
        return hana::id(std::forward<T>(val));
    }
};

/**
 * @brief Helper for quotes decorator when argument is a concrete_phrase.
 */
template <typename T>
struct quotes_decorator_helper<T,
        hana::when<std::is_same<concrete_phrase,std::decay_t<T>>::value>>
{
    static auto f(T&& val) -> decltype(auto)
    {
        std::string str;
        backend_formatter.append(str,"\"",std::string(std::forward<T>(val)),"\"");
        return concrete_phrase(std::move(str),val.grammar_cats());
    }
};

/**
 * @brief Helper for quotes decorator when argument can be used to construct std::string but is not a concrete_phrase.
 */
template <typename T>
struct quotes_decorator_helper<T,
        hana::when<std::is_constructible<std::string,T>::value
        &&
        !std::is_same<concrete_phrase,std::decay_t<T>>::value
        >>
{
    static auto f(T&& val) -> decltype(auto)
    {
        std::string str;
        backend_formatter.append(str,"\"",std::string(std::forward<T>(val)),"\"");
        return str;
    }
};
}

/**
 * @brief Quotes decorator that adds quotes around original string.
 */
struct quotes_decorator_t
{
    using hana_tag=decorator_tag;

    template <typename T>
    auto operator() (T&& val) const -> decltype(auto)
    {
        return detail::quotes_decorator_helper<T>::f(std::forward<T>(val));
    }
};
constexpr quotes_decorator_t quotes_decorator{};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_QUOTES_DECORATOR_HPP
