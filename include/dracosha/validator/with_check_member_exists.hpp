/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** \file validator/with_check_member_exists.hpp
*
*  Defines helper class for checking if member exists.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_WITH_CHECK_MEMBER_EXISTS_HPP
#define DRACOSHA_VALIDATOR_WITH_CHECK_MEMBER_EXISTS_HPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/extract.hpp>
#include <dracosha/validator/check_member.hpp>
#include <dracosha/validator/check_member_path.hpp>
#include <dracosha/validator/status.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

/**
 * @brief Mode of processing of not found members
 */
enum class if_member_not_found : int
{
    ignore, //!< Ignore not found members
    abort //!< Abort validating with false report if member is not found
};

template <typename AdapterT>
struct with_check_member_exists
{
    const AdapterT& _adapter;
    if_member_not_found _unknown_member_mode;
    bool _check_member_exists;

    with_check_member_exists(const AdapterT& adpt):
        _adapter(adpt),
        _unknown_member_mode(if_member_not_found::ignore),
        _check_member_exists(false)
    {}

    /**
     * @brief Enable/disable checking if a member exists before validating the member
     * @param enabled Flag
     *
     * Disabling checking of member existance improves validation performance but can sometimes cause
     * exceptions or other undefined errors.
     * Some basic checking if a member can be found for given object type is performed statically at compile time
     * regardless of this flag.
     */
    void set_check_member_exists_before_validation(bool enable) noexcept
    {
        _check_member_exists=enable;
    }
    /**
     * @brief Get flag of checking if member exists befor validation
     * @return Flag
     */
    bool is_check_member_exists_before_validation() const noexcept
    {
        return _check_member_exists;
    }

    /**
     * @brief Set mode to use if member is not found
     * @param mode Mode
     */
    void set_unknown_member_mode(if_member_not_found mode) noexcept
    {
        _unknown_member_mode=mode;
    }
    /**
     * @brief Get mode used if a member is not found
     * @return Mode to use if a member not found
     */
    if_member_not_found unknown_member_mode() const noexcept
    {
        return _unknown_member_mode;
    }

    /**
     * @brief Check if member exists
     * @param member Member to check
     * @return Status of checking
     */
    template <typename MemberT>
    bool check_member_exists(MemberT&& member) const
    {
        if (!check_member_path(extract(_adapter.traits().get()),member.path))
        {
            return false;
        }
        if (is_check_member_exists_before_validation())
        {
            return _adapter.validate_exists(member,true,true);
        }
        return true;
    }

    /**
     * @brief Get status to return if member is not found
     * @return Resulting status
     */
    status not_found_status() const
    {
        return (_unknown_member_mode==if_member_not_found::ignore)?status::code::ignore:status::code::fail;
    }
};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_WITH_CHECK_MEMBER_EXISTS_HPP