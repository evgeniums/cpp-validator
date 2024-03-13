/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/reporting/prepare_property_formatting.hpp
*
*  Defines "prepare_property_formatting" helper.
*  Currently not used.
*
*/

/****************************************************************************/

#ifndef HATN_VALIDATOR_PREPARE_PROPERTY_FORMATTING_HPP
#define HATN_VALIDATOR_PREPARE_PROPERTY_FORMATTING_HPP

#include <hatn/validator/config.hpp>
#include <hatn/validator/variadic_property.hpp>
#include <hatn/validator/reporting/backend_formatter.hpp>
#include <hatn/validator/reporting/format_join_grammar_cats.hpp>

HATN_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

/**
 * @brief Currently not used.
 */
#ifdef HATN_VALIDATOR_PREPARE_PROPERTY_FORMATTING
struct prepare_property_formatting_impl
{
    template <typename StringsFmtT, typename MemberNamesFmtT, typename PropT>
    auto operator() (StringsFmtT&& strings, MemberNamesFmtT&& mn, PropT&& prop) const
    {
        return hana::eval_if(
            std::is_base_of<variadic_property_tag,unwrap_object_t<PropT>>{},
            [&](auto&& _)
            {
                return prepare_variadic_property_formatting(_(strings),_(mn),_(prop));
            },
            [&](auto&& _)
            {
                return fmt_pair(_(mn),_(prop));
            }
        );
    }
};
constexpr prepare_property_formatting_impl prepare_property_formatting{};
#endif

//-------------------------------------------------------------
HATN_VALIDATOR_NAMESPACE_END

#endif // HATN_VALIDATOR_PREPARE_PROPERTY_FORMATTING_HPP
