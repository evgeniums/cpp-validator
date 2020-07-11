/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** \file validator/adapters/chained_adapter.hpp
*
*  Defines base class for chained adapters.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_CHAINED_ADAPTER_HPP
#define DRACOSHA_VALIDATOR_CHAINED_ADAPTER_HPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/adapters/adapter.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

/**
 *  @brief Base class for chained adapters
 *
 */
template <typename AdapterT>
class chained_adapter
{
    public:

        using hana_tag=adapter_tag;

        using next_adapter_type=AdapterT;

        /**
         * @brief Constructor
         * @param next_adapter Next adapter in chain
         */
        chained_adapter(
            AdapterT&& next_adapter
        ) : _next_adapter(std::forward<AdapterT>(next_adapter))
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
            _next_adapter.set_check_member_exists_before_validation(enable);
        }
        /**
         * @brief Get flag of checking if member exists befor validation
         * @return Flag
         */
        bool is_check_member_exists_before_validation() const noexcept
        {
            return _next_adapter.is_check_member_exists_before_validation();
        }

        /**
         * @brief Set mode to use if member is not found
         * @param mode Mode
         */
        void set_unknown_member_mode(if_member_not_found mode) noexcept
        {
            _next_adapter.set_unknown_member_mode(mode);
        }

        /**
         * @brief Get mode used if a member is not found
         * @return Mode to use if a member not found
         */
        if_member_not_found unknown_member_mode() const noexcept
        {
            return _next_adapter.unknown_member_mode();
        }

        /**
         * @brief Get reference to wrapped object
         * @return Wrapped object under validation
         */
        auto object() const noexcept -> decltype(auto)
        {
            return _next_adapter.object();
        }

    protected:

        AdapterT& next_adapter() noexcept
        {
            return _next_adapter;
        }

        const AdapterT& next_adapter() const noexcept
        {
            return _next_adapter;
        }

    private:

        AdapterT _next_adapter;
};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_CHAINED_ADAPTER_HPP
