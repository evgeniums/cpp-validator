/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** \file validator/reporting/decorator.hpp
*
* Defines decorator.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_QUOTES_DECORATOR_HPP
#define DRACOSHA_VALIDATOR_QUOTES_DECORATOR_HPP

#include <string>

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/reporting/decorator.hpp>
#include <dracosha/validator/reporting/backend_formatter.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

namespace detail
{
template <typename T, typename = hana::when<true>>
struct quotes_decorator_traits
{
    static auto f(T&& val) -> decltype(auto)
    {
        return hana::id(std::forward<T>(val));
    }
};
template <typename T>
struct quotes_decorator_traits<T,
        hana::when<std::is_constructible<std::string,T>::value>>
{
    static auto f(T&& val) -> decltype(auto)
    {
        std::string str;
        backend_formatter.append(str,"\"",std::forward<T>(val),"\"");
        return str;
    }
};
}

/**
 * @brief Quotes decorator adds quotes around initial string
 */
struct quotes_decorator_t
{
    using hana_tag=decorator_tag;

    template <typename T>
    auto operator() (T&& val) const -> decltype(auto)
    {
        return detail::quotes_decorator_traits<T>::f(std::forward<T>(val));
    }
};
constexpr quotes_decorator_t quotes_decorator{};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_QUOTES_DECORATOR_HPP
