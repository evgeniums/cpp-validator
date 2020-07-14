/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** \file validator/check_member_exists_traits_proxy.hpp
*
*  Defines proxy class for adapters using traits that inherits with_check_member_exists.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_CHECK_MEMBER_EXISTS_PROXY_HPP
#define DRACOSHA_VALIDATOR_CHECK_MEMBER_EXISTS_PROXY_HPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/status.hpp>
#include <dracosha/validator/with_check_member_exists.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

/**
 * @brief Proxy class for adapters using traits that inherits with_check_member_exists
 */
template <typename AdapterT>
class check_member_exists_traits_proxy
{
    public:

        using adapter_type=AdapterT;

        /**
         * @brief Constructor
         * @param adapter Adapter object
         */
        check_member_exists_traits_proxy(
            AdapterT& adapter
        ) : _adapter(adapter)
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
            _adapter.traits().set_check_member_exists_before_validation(enable);
        }
        /**
         * @brief Get flag of checking if member exists befor validation
         * @return Flag
         */
        bool is_check_member_exists_before_validation() const noexcept
        {
            return _adapter.traits().is_check_member_exists_before_validation();
        }

        /**
         * @brief Set mode to use if member is not found
         * @param mode Mode
         */
        void set_unknown_member_mode(if_member_not_found mode) noexcept
        {
            _adapter.traits().set_unknown_member_mode(mode);
        }

        /**
         * @brief Get mode used if a member is not found
         * @return Mode to use if a member not found
         */
        if_member_not_found unknown_member_mode() const noexcept
        {
            return _adapter.traits().unknown_member_mode();
        }

        /**
         * @brief Check if member exists
         * @param member Member to check
         * @return Status of checking
         */
        template <typename MemberT>
        bool check_member_exists(MemberT&& member) const
        {
            return _adapter.traits().check_member_exists(std::forward<MemberT>(member));
        }

        /**
         * @brief Get status to return if member is not found
         * @return Resulting status
         */
        status not_found_status() const
        {
            return _adapter.traits().not_found_status();
        }

    private:

        AdapterT& _adapter;
};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_CHECK_MEMBER_EXISTS_PROXY_HPP
