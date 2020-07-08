/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** \file validator/reporting/member_names.hpp
*
* Defines member names formatter.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_MEMBER_NAMES_HPP
#define DRACOSHA_VALIDATOR_MEMBER_NAMES_HPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/utils/reference_wrapper.hpp>
#include <dracosha/validator/reporting/strings.hpp>
#include <dracosha/validator/reporting/single_member_name.hpp>
#include <dracosha/validator/reporting/nested_member_name.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

struct member_names_tag;

/**
 * @brief Traits for member names formatter that do not perform any formatting and just bypass names "as is".
 */
struct bypass_member_names_traits_t
{
    template <typename T>
    std::string operator() (const T&) const
    {
        return std::string();
    }
};
constexpr bypass_member_names_traits_t bypass_member_names_traits{};


/**
 * @brief Member names formatter.
 *
 * Member names formatter forwards a name to traits that implement actual name formatting.
 *
 */
template <typename StringsT, typename TraitsT>
struct member_names
{
    using hana_tag=member_names_tag;

    using strings_type=StringsT;
    using traits_type=TraitsT;

    StringsT _strings;
    TraitsT _traits;

    template <typename T>
    std::string operator() (const T& name,
                            std::enable_if_t<hana::is_a<member_tag,T>,void*> =nullptr
                            ) const
    {
        return nested_member_name<T,TraitsT,StringsT>(name,_traits,_strings);
    }

    template <typename T>
    std::string operator() (const T& name,
                            std::enable_if_t<!hana::is_a<member_tag,T>,void*> =nullptr
                            ) const
    {
        return single_member_name<T,TraitsT>(name,_traits,_strings);
    }
};

/**
 * @brief Create member names formatter
 * @param traits Actual implementer of names formatting
 * @param strings Strings object the unprocessed names will be forwarded to
 * @return Formatter of member names
 */
template <typename TraitsT,typename StringsT>
auto make_member_names(TraitsT&& traits, StringsT&& strings,
                    std::enable_if_t<hana::is_a<strings_tag,StringsT>,void*> =nullptr
                )
{
    return member_names<
                StringsT,
                TraitsT
            >
            {std::forward<StringsT>(strings),std::forward<TraitsT>(traits)};
}

/**
 * @brief Create member names formatter that bypasses names processing
 * @param strings Strings object the names will be forwarded to
 * @return Formatter of member names
 */
template <typename StringsT>
auto make_member_names(StringsT&& strings,
                    std::enable_if_t<hana::is_a<strings_tag,StringsT>,void*> =nullptr
                )
{
    return member_names<
                StringsT,
                bypass_member_names_traits_t
            >
            {std::forward<StringsT>(strings),bypass_member_names_traits};
}

/**
 * @brief Create member names formatter with default strings object
 * @param traits Actual implementer of names formatting
 * @return Formatter of member names
 */
template <typename TraitsT>
auto make_member_names(TraitsT&& traits,
                std::enable_if_t<(!hana::is_a<strings_tag,TraitsT> && !hana::is_a<translator_tag,TraitsT>),void*> =nullptr
            )
{
    return make_member_names(std::forward<TraitsT>(traits),default_strings);
}

/**
 * @brief Get default member names formatter that just forwards names to default strings object
 * @return Default formatter of member names
 */
inline auto get_default_member_names()
    -> std::add_lvalue_reference_t<
            std::add_const_t<decltype(make_member_names(default_strings))>
        >
{
    static const auto default_member_names=make_member_names(default_strings);
    return default_member_names;
}

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_MEMBER_NAMES_HPP
