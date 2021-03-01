/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/detail/has_method.hpp
*
*  Defines helpers to check if class has a specific method.
*  Supports methods at(key), has(key), contains(key), find(key), isSet(key) and brackets operator [key].
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_HAS_METHOD_HPP
#define DRACOSHA_VALIDATOR_HAS_METHOD_HPP

#include <dracosha/validator/config.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

namespace detail
{

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_INLINE_LAMBDA auto has_at = hana::is_valid([](auto&& v, auto&& x) -> decltype((void)v.at(std::forward<decltype(x)>(x))){});
DRACOSHA_VALIDATOR_INLINE_LAMBDA auto has_has = hana::is_valid([](auto&& v, auto&& x) -> decltype((void)v.has(std::forward<decltype(x)>(x))){});
DRACOSHA_VALIDATOR_INLINE_LAMBDA auto has_contains = hana::is_valid([](auto&& v, auto&& x) -> decltype((void)v.contains(std::forward<decltype(x)>(x))){});
DRACOSHA_VALIDATOR_INLINE_LAMBDA auto has_find_it = hana::is_valid([](auto&& v, auto&& x) -> decltype((void)*v.find(std::forward<decltype(x)>(x))){});
DRACOSHA_VALIDATOR_INLINE_LAMBDA auto has_isSet = hana::is_valid([](auto&& v, auto&& x) -> decltype((void)v.isSet(std::forward<decltype(x)>(x))){});
DRACOSHA_VALIDATOR_INLINE_LAMBDA auto has_brackets = hana::is_valid([](auto&& v, auto&& x) -> decltype((void)v[std::forward<decltype(x)>(x)]){});
DRACOSHA_VALIDATOR_INLINE_LAMBDA auto has_size = hana::is_valid([](auto&& v) -> decltype((void)v.size()){});

DRACOSHA_VALIDATOR_INLINE_LAMBDA auto has_at_c = hana::is_valid([](auto&& v, auto&& x) -> decltype(
                                                            (void)hana::traits::declval(v).at(hana::traits::declval(x))
                                                           )
                                                        {});
DRACOSHA_VALIDATOR_INLINE_LAMBDA auto has_brackets_c = hana::is_valid([](auto&& v, auto&& x) -> decltype(
                                                            (void)hana::traits::declval(v)[hana::traits::declval(x)]
                                                           )
                                                        {});

DRACOSHA_VALIDATOR_INLINE_LAMBDA auto has_find_it_c = hana::is_valid([](auto v, auto x) -> decltype(
                                                                    (void)*hana::traits::declval(v).find(hana::traits::declval(x))
                                                                )
                                                            {});
DRACOSHA_VALIDATOR_INLINE_LAMBDA auto has_has_c = hana::is_valid([](auto v, auto x) -> decltype(
                                                                    (void)hana::traits::declval(v).has(hana::traits::declval(x))
                                                                )
                                                            {});
DRACOSHA_VALIDATOR_INLINE_LAMBDA auto has_contains_c = hana::is_valid([](auto v, auto x) -> decltype(
                                                                    (void)hana::traits::declval(v).contains(hana::traits::declval(x))
                                                                )
                                                            {});
DRACOSHA_VALIDATOR_INLINE_LAMBDA auto has_isSet_c = hana::is_valid([](auto v, auto x) -> decltype(
                                                                    (void)hana::traits::declval(v).isSet(hana::traits::declval(x))
                                                                )
                                                            {});

DRACOSHA_VALIDATOR_INLINE_LAMBDA auto has_size_c = hana::is_valid([](auto v) -> decltype(
                                                                    (void)hana::traits::declval(v).size()
                                                                )
                                                            {});

//-------------------------------------------------------------

}

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_HAS_METHOD_HPP
