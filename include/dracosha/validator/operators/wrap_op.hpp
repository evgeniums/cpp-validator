/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/operators/wrap_op.hpp
*
*  Defines operator wrapper.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_WRAP_OP_HPP
#define DRACOSHA_VALIDATOR_WRAP_OP_HPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/operators/operator.hpp>
#include <dracosha/validator/utils/object_wrapper.hpp>
#include <dracosha/validator/utils/adjust_storable_type.hpp>
#include <dracosha/validator/reporting/concrete_phrase.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

/**
 * @brief Wrap operator.
 */
template <typename T>
class wrap_op : public object_wrapper<T>
{
    public:

        using hana_tag=operator_tag;

        /**
         * @brief Constructor.
         * @param val Operator.
         */
        wrap_op(
                T&& val
            ) : object_wrapper<T>(std::forward<T>(val))
        {}

        /**
         * @brief Operator of conversion to std::string.
         */
        operator std::string() const
        {
            return str();
        }

        /**
         * @brief Get operator description for reporting.
         * @return Description of embedded operator.
         */
        constexpr auto str() const
        {
            return this->get().str();
        }

        /**
         * @brief Get negative operator description for reporting.
         * @return Negative description of embedded operator.
         */
        constexpr auto n_str() const
        {
            return this->get().n_str();
        }

        /**
         * @brief Apply operator.
         * @param Value to validate.
         * @param Sample vaue.
         * @return Validation status.
         */
        template <typename T1, typename T2>
        constexpr bool operator() (const T1& a, const T2& b) const
        {
            return this->get()(a,b);
        }
};

struct wrap_op_with_string_tag
{
};

/**
 * @brief Wrap operator with description.
 */
template <typename T1, typename T2>
class wrap_op_with_string : public wrap_op<T1>,
                            public wrap_op_with_string_tag
{
    public:

        /**
         * @brief Constructor.
         * @param val Operator to wrap.
         * @param description String to use in report formatting to represent the operator.
         */
        template <typename Tt1, typename Tt2>
        wrap_op_with_string(
                Tt1&& val,
                Tt2&& description
            ) : wrap_op<T1>(std::forward<Tt1>(val)),
                _description(std::forward<Tt2>(description))
        {}

        /**
         * @brief Operator of conversion to std::string.
         */
        operator std::string() const
        {
            return _description;
        }

        /**
         * @brief Get explicit operator description.
         */
        auto str() const
        {
            return _description;
        }

        /**
         * @brief Get explicit operator description.
         */
        auto n_str() const
        {
            return _description;
        }

    private:

        std::decay_t<T2> _description;
};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_WRAP_OP_HPP
