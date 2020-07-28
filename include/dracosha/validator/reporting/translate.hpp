/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** \file validator/reporting/translate.hpp
*
* Defines helper for translating using object that can optionally have or not have translators
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_TRANSLATE_HPP
#define DRACOSHA_VALIDATOR_TRANSLATE_HPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/reporting/translator.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

/**
 * @brief Helper to work with object that can optionally have or not have translator
 *
 * Version for object that does not have translator
 */
template <typename T, typename Arg, typename =void>
struct translate_t
{
    template <typename T1, typename Arg1>
    constexpr auto operator() (T1&&, Arg1&& arg) const -> decltype(auto)
    {
        return hana::id(std::forward<Arg1>(arg));
    }
};
/**
 * @brief Helper to work with object that can optionally have or not have translator
 *
 * Version for object that has translator
 */
template <typename T, typename Arg>
struct translate_t<T,Arg,
        decltype(
            (void)std::declval<std::decay_t<T>>().translator(std::declval<std::decay_t<Arg>>())
        )>
{
    template <typename T1, typename Arg1>
    auto operator() (T1&& obj, Arg1&& arg) const -> decltype(auto)
    {
        return obj.translator(std::forward<Arg1>(arg));
    }
};

/**
 * @brief Helper to work with object that can optionally have or not have translator
 */
template <typename T, typename Arg>
constexpr translate_t<T,Arg> translate_inst{};

template <typename T, typename Arg>
constexpr auto translate(T&& obj, Arg&& arg) -> decltype(auto)
{
    return translate_inst<T,Arg>(std::forward<T>(obj),std::forward<Arg>(arg));
}

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_TRANSLATE_HPP
