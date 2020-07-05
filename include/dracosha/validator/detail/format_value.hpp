/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** \file validator/detail/format_value.hpp
*
* Defines helper for value formatting.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_FORMAT_VALUE_HPP
#define DRACOSHA_VALIDATOR_FORMAT_VALUE_HPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/detail/if_bool.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

namespace detail
{

template <typename T, typename =hana::when<true>>
struct format_value_t
{
};

/**
 * @brief  Formatter of non-boolean values
 */
template <typename T>
struct format_value_t<T,hana::when<!std::is_same<bool,std::decay_t<T>>::value>>
{
    template <typename T1, typename TranslatorT>
    auto operator () (T1&& val, const TranslatorT&) const -> decltype(auto)
    {
        return hana::id(val);
    }
};

/**
 * @brief  Formatter of boolean values
 */
template <typename T>
struct format_value_t<T,hana::when<std::is_same<bool,std::decay_t<T>>::value>>
{
    template <typename T1, typename TranslatorT>
    auto operator () (T1&& val, const TranslatorT& translator) const -> decltype(auto)
    {
        return translator(val?std::string(string_true):std::string(string_false));
    }
};

/**
 * @brief Default formatter of values
 */
template <typename T>
constexpr format_value_t<T> format_value{};

}

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_FORMAT_VALUE_HPP
