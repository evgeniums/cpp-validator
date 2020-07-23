/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** \file validator/operators/wrap_op.hpp
*
*  Defines operator wrapper
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_WRAP_OP_HPP
#define DRACOSHA_VALIDATOR_WRAP_OP_HPP

#include <string>
#include <dracosha/validator/config.hpp>
#include <dracosha/validator/utils/object_wrapper.hpp>
#include <dracosha/validator/operators/operator.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

template <typename T>
class wrap_op : public object_wrapper<T>
{
    public:

        using hana_tag=operator_tag;

        /**
         * @brief Constructor
         * @param val Operator
         * @param description String to use in report formatting to represent the operator
         */
        wrap_op(
                T&& val,
                std::string description=std::string()
            ) : object_wrapper<T>(std::forward<T>(val)),
                _description(std::move(description))
        {}

        /**
         * @brief Operator of conversion to std::string
         */
        operator std::string() const
        {
            return _description;
        }

        template <typename T1, typename T2>
        constexpr bool operator() (const T1& a, const T2& b) const
        {
            return get()(b,a);
        }

    private:

        std::string _description;
};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_WRAP_OP_HPP
