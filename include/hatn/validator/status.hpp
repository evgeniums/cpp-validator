/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/status.hpp
*
*  Defines status of validation operations.
*
*/

/****************************************************************************/

#ifndef HATN_VALIDATOR_STATUS_HPP
#define HATN_VALIDATOR_STATUS_HPP

#include <hatn/validator/config.hpp>

HATN_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

/**
 * @brief Status to be used as result of validation operation
 */
class status
{
    public:

        enum class code : int
        {
            success,
            fail,
            ignore
        };

        /**
         * @brief Default constructor.
         */
        status() noexcept : _code(code::success)
        {}

        /**
         * @brief Constructor from code.
         * @param c Status code.
         */
        status(code c) noexcept : _code(c)
        {}

        /**
         * @brief Constructor from boolean.
         * @param ok Boolean status.
         */
        status(bool ok) noexcept : _code(ok?code::success:code::fail)
        {}

        /**
         * @brief Convert to boolean.
         */
        operator bool () const noexcept
        {
            return _code!=code::fail;
        }

        /**
         * @brief Get code value.
         * @return Status code.
         */
        code value() const noexcept
        {
            return _code;
        }

        void reset(code value=code::success) noexcept
        {
            _code=value;
        }

        bool operator == (const code& val) const noexcept
        {
            return val==_code;
        }
        bool operator != (const code& val) const noexcept
        {
            return val!=_code;
        }

        bool success() const noexcept
        {
            return _code==code::success;
        }

        bool fail() const noexcept
        {
            return _code==code::fail;
        }

        bool ignore() const noexcept
        {
            return _code==code::ignore;
        }

    private:

        code _code;
};

struct status_predicate_or_t
{
    bool operator() (const status& v) const
    {
        return v.value()!=status::code::success;
    }
};
constexpr status_predicate_or_t status_predicate_or{};

//-------------------------------------------------------------

HATN_VALIDATOR_NAMESPACE_END

#endif // HATN_VALIDATOR_STATUS_HPP
