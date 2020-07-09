/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** \file validator/reporting/decorator.hpp
*
* Defines default decorator.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_DECORATOR_HPP
#define DRACOSHA_VALIDATOR_DECORATOR_HPP

#include <string>

#include <dracosha/validator/config.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

struct decorator_tag;

/**
 * @brief Default decorator class that bypasses string as is.
 */
struct no_decorator_t
{
    using hana_tag=decorator_tag;

    template <typename T>
    auto operator () (T&& val) const -> decltype(auto)
    {
        return hana::id(std::forward<T>(val));
    }
};
constexpr no_decorator_t no_decorator{};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_DECORATOR_HPP
