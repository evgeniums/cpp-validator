/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/reporting/flag_presets.hpp
*
*  Defines some presets of flag descriptions.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_FLAG_PRESETS_HPP
#define DRACOSHA_VALIDATOR_FLAG_PRESETS_HPP

#include <dracosha/validator/config.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

struct flag_preset_tag;

/**
 * @brief Base template for preset flag descriptions.
 *
 * This template is a functor that defines operator for getting description depending on the flag value.
 */
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

    constexpr static const char* str()
    {
        return DerivedT::description;
    }

    constexpr static const char* n_str()
    {
        return DerivedT::n_description;
    }
};

/**
 * @brief Preset flag descriptions with true/false words.
 *
 * This is a default helper for flag descriptions.
 */
struct flag_true_false_t : public flag_preset<flag_true_false_t>
{
    constexpr static const char* description="must be true";
    constexpr static const char* n_description="must be false";
};
/**
 * @brief Instance of preset flag descriptions with on/off words.
 */
constexpr flag_true_false_t flag_true_false{};

/**
 * @brief Default flag descriptions that uses true/false words.
 */
constexpr flag_true_false_t default_flag_preset{};

/**
 * @brief Preset flag descriptions with on/off words.
 */
struct flag_on_off_t : public flag_preset<flag_on_off_t>
{
    constexpr static const char* description="must be on";
    constexpr static const char* n_description="must be off";
};

/**
 * @brief Instance of preset flag descriptions with on/off words.
 */
constexpr flag_on_off_t flag_on_off{};

/**
 * @brief Preset flag descriptions with checked/unchecked words.
 */
struct flag_checked_unchecked_t : public flag_preset<flag_checked_unchecked_t>
{
    constexpr static const char* description="must be checked";
    constexpr static const char* n_description="must be unchecked";
};

/**
 * @brief Instance of preset flag descriptions with checked/unchecked words.
 */
constexpr flag_checked_unchecked_t flag_checked_unchecked{};

/**
 * @brief Preset flag descriptions with set/unset words.
 */
struct flag_set_unset_t : public flag_preset<flag_set_unset_t>
{
    constexpr static const char* description="must be set";
    constexpr static const char* n_description="must be unset";
};

/**
 * @brief Instance of preset flag descriptions with set/unset words.
 */
constexpr flag_set_unset_t flag_set_unset{};

/**
 * @brief Preset flag descriptions with enabled/disabled words.
 */
struct flag_enable_disable_t : public flag_preset<flag_enable_disable_t>
{
    constexpr static const char* description="must be enabled";
    constexpr static const char* n_description="must be disabled";
};

/**
 * @brief Instance of preset flag descriptions with enabled/disabled words.
 */
constexpr flag_enable_disable_t flag_enable_disable{};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_FLAG_PRESETS_HPP
