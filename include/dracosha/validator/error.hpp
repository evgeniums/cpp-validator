/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/error.hpp
*
*  Defines validation error classes.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_ERROR_HPP
#define DRACOSHA_VALIDATOR_ERROR_HPP

#include <stdexcept>

#include <dracosha/validator/status.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

/**
 * @brief Base error class.
 */
class error
{
    public:

        error() = default;

        /**
         * @brief Constructor.
         * @param st Validation status.
         */
        error(
                status st
            ) : _status(std::move(st))
        {
        }

        /**
         * @brief Set valud of valdation status.
         * @param st Value of validation status.
         */
        void set_value(status st) noexcept
        {
            _status=std::move(st);
        }

        /**
         * @brief Get value of validation status.
         * @return Vaidation status.
         */
        status value() const noexcept
        {
            return _status;
        }

        void reset()
        {
            _status.reset();
        }

        /**
         * @brief Convert error to bool.
         * @return true if validation failed, false if validation succeded.
         */
        operator bool () const noexcept
        {
            return !static_cast<bool>(_status);
        }

    private:

        status _status;
};

/**
 * @brief Error status with string description.
 */
class error_report : public error
{
    public:

        error_report()=default;

        /**
         * @brief Constructor
         * @param st Validation status.
         * @param message Description of validation error.
         */
        error_report(
                status st,
                std::string message
            ) : error(std::move(st)),
                _message(std::move(message))
        {
        }

        /**
         * @brief Get desctiption of validation error.
         * @return Description of validation error.
         */
        std::string message() const
        {
            return _message;
        }

        void reset()
        {
            _message.clear();
            error::reset();
        }

    private:

        std::string _message;
        friend struct validate_t;
};

/**
 * @brief Validation exception.
 */
class validation_error : public std::runtime_error
{
    public:

        /**
         * @brief Constructor.
         * @param err Validation error with string description.
         */
        validation_error(const error_report& err)
            : std::runtime_error(err.message())
        {}
};

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_ERROR_HPP
