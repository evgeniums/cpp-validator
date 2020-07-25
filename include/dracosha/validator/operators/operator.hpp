/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** \file validator/operators/operator.hpp
*
*  Defines base operator
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_OPERATOR_HPP
#define DRACOSHA_VALIDATOR_OPERATOR_HPP

#include <string>
#include <dracosha/validator/config.hpp>
#include <dracosha/validator/utils/object_wrapper.hpp>
#include <dracosha/validator/utils/enable_to_string.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

/**
 * @brief Tag for all operator clases
 */
struct operator_tag;

/**
 * @brief Base operator class
 */
template <typename DerivedT>
struct op : public enable_to_string<DerivedT>
{
    using hana_tag=operator_tag;
};

/**
 * @brief Wrap operator
 */
template <typename T>
class wrap_op : public object_wrapper<T>
{
    public:

        using hana_tag=operator_tag;

        /**
         * @brief Constructor
         * @param val Operator
         */
        wrap_op(
                T&& val
            ) : object_wrapper<T>(std::forward<T>(val))
        {}

        /**
         * @brief Operator of conversion to std::string
         */
        operator std::string() const
        {
            return std::string(this->get());
        }

        /**
         * @brief Apply operator
         * @param Value to validate
         * @param Sample vaue
         * @return Validation status
         */
        template <typename T1, typename T2>
        bool operator() (const T1& a, const T2& b) const
        {
            return this->get()(a,b);
        }
};

/**
 * @brief Wrap operator with description
 */
template <typename T>
class wrap_op_with_string : public wrap_op<T>
{
    public:

        /**
         * @brief Constructor
         * @param val Operator
         * @param description String to use in report formatting to represent the operator
         */
        wrap_op_with_string(
                T&& val,
                std::string description
            ) : wrap_op<T>(std::forward<T>(val)),
                _description(std::move(description))
        {}

        /**
         * @brief Operator of conversion to std::string
         */
        operator std::string() const
        {
            return _description;
        }

    private:

        std::string _description;
};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_OPERATOR_HPP
