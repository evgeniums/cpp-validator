/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** \file validator/reporting/strings.hpp
*
*
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_STRINGS_HPP
#define DRACOSHA_VALIDATOR_STRINGS_HPP

#include <string>

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/property.hpp>
#include <dracosha/validator/operators/operator.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

struct strings_tag;

template <typename TranslatorT>
struct strings_t
{
    using hana_tag=strings_tag;

    const TranslatorT& _translator;

    template <typename T>
    std::string to_string(const T& id) const
    {

        return hana::if_(
                    hana::is_a<property_tag,T>,
                    std::string(T::name()),
                    hana::if_(
                        std::is_constructible<std::string,T>::value,
                        id,
                        hana::if_(
                                hana::is_valid([](auto&& v) -> decltype((void)std::to_string(v)){})(id),
                                std::to_string(id),
                                std::string("<?????>")
                            )
                    )
              );
    }

#if 0
    template <typename T>
    std::string to_string(const T& id) const
    {
        return hana::if_(
                    hana::is_a<property_tag,T>,
                    std::string(T::name()),
                    hana::if_(
                        hana::is_a<operator_tag,T>,
                        std::string(T::description),
                        hana::if_(
                            hana::is_valid([](auto&& v) -> decltype((void)std::to_string(v)){})(id),
                            std::to_string(id),
                            hana::if_(
                                std::is_constructible<std::string,T>::value,
                                std::string(id),
                                std::string("<?????>")
                            )
                        )
                    )
              );
    }
#endif

    template <typename T>
    std::string operator() (const T& id) const
    {
        return _translator(to_string(id));
    }
};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_STRINGS_HPP
