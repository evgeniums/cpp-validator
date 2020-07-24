/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** \file validator/flag_presets.hpp
*
*  Defines some presets of flag descriptions
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_FLAG_PRESETS_HPP
#define DRACOSHA_VALIDATOR_FLAG_PRESETS_HPP

#include <dracosha/validator/config.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

struct flag_preset_tag;

template <typename DerivedT>
struct flag_preset
{
    using hana_tag=flag_preset_tag;
    constexpr const char* operator() (const bool& b) const
    {
        if (b)
        {
            return DerivedT::description;
        }
        return DerivedT::n_description;
    }
};

struct flag_true_false_t : public flag_preset<flag_true_false_t>
{
    constexpr static const char* description="must be true";
    constexpr static const char* n_description="must be false";
};
constexpr flag_true_false_t flag_true_false{};
constexpr flag_true_false_t default_flag_preset{};

struct flag_on_off_t : public flag_preset<flag_on_off_t>
{
    constexpr static const char* description="must be on";
    constexpr static const char* n_description="must be off";
};
constexpr flag_on_off_t flag_on_off{};

struct flag_checked_unchecked_t : public flag_preset<flag_checked_unchecked_t>
{
    constexpr static const char* description="must be checked";
    constexpr static const char* n_description="must be unchecked";
};
constexpr flag_checked_unchecked_t flag_checked_unchecked{};

struct flag_set_unset_t : public flag_preset<flag_set_unset_t>
{
    constexpr static const char* description="must be set";
    constexpr static const char* n_description="must be unset";
};
constexpr flag_set_unset_t flag_set_unset{};

struct flag_enable_disable_t : public flag_preset<flag_enable_disable_t>
{
    constexpr static const char* description="must be enabled";
    constexpr static const char* n_description="must be disabled";
};
constexpr flag_enable_disable_t flag_enable_disable{};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_FLAG_PRESETS_HPP
