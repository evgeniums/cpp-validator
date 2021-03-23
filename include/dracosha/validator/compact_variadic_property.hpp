/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/compact_variadic_property.hpp
*
* Defines "compact_variadic_property".
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_COMPACT_VARIADIC_PROPERTY_HPP
#define DRACOSHA_VALIDATOR_COMPACT_VARIADIC_PROPERTY_HPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/variadic_property.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

/**
 * @brief Implementer of compact_variadic_property().
 */
struct compact_variadic_property_impl
{
    template <typename PathT>
    auto operator () (PathT&& path) const -> decltype(auto)
    {
        return hana::fold(
            path,
            hana::tuple<>{},
            [](auto&& accumulated, auto&& current_key)
            {
                // check if current key is variadic property
                using type=std::decay_t<decltype(current_key)>;
                return hana::eval_if(
                    hana::and_(
                        std::is_base_of<variadic_arg_tag,type>{},
                        hana::not_(hana::is_empty(accumulated))
                    ),
                    [&](auto&& _)
                    {
                        // modify last element in path if current key is variadic property
                        const auto& prev=hana::back(_(accumulated));
                        using prev_type=unwrap_object_t<decltype(prev)>;
                        return hana::eval_if(
                            std::is_base_of<variadic_property_base_tag,prev_type>{},
                            [&](auto&& _)
                            {
                                const auto& p1=_(prev);
                                using prev_type1=unwrap_object_t<decltype(p1)>;
                                return hana::eval_if(
                                    std::is_base_of<variadic_property_tag,prev_type1>{},
                                    [&](auto&& _)
                                    {
                                        // create next property notation appending current key
                                        auto new_prev=_(prev).next(_(current_key));
                                        return hana::append(
                                                        hana::drop_back(_(accumulated)),
                                                        new_prev
                                                    );
                                    },
                                    [&](auto&& _)
                                    {
                                        // create property notation with current key as the first argument
                                        auto new_prev=_(prev)(_(current_key));
                                        return hana::append(
                                                        hana::drop_back(_(accumulated)),
                                                        new_prev
                                                    );
                                    }
                                );
                            },
                            [&](auto&& _)
                            {
                                // varg must follow variadic property
                                // if not then leave the key as is
                                return hana::append(_(accumulated),_(current_key));
                            }
                        );
                    },
                    [&](auto&& _)
                    {
                        // leave the key as is if current key is not variadic property
                        // or varg is the first key in the path
                        return hana::append(_(accumulated),_(current_key));
                    }
                );
            }
        );
    }
};
/**
 * @brief Compact path with variadic property for reporting.
 */
constexpr compact_variadic_property_impl compact_variadic_property{};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_COMPACT_VARIADIC_PROPERTY_HPP
