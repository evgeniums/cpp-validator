/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** \file validator/operators/invert_op.hpp
*
*  Defines invertion operator
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_INVERT_OP_HPP
#define DRACOSHA_VALIDATOR_INVERT_OP_HPP

#include <string>

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/utils/enable_to_string.hpp>
#include <dracosha/validator/operators/operator.hpp>
#include <dracosha/validator/operators/exists.hpp>
#include <dracosha/validator/operators/flag.hpp>
#include <dracosha/validator/reporting/backend_formatter.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

struct string_invert_op_t : public enable_to_string<string_invert_op_t>
{
    constexpr static const char* description="NOT";
};
constexpr string_invert_op_t string_invert_op{};

/**
 * @brief Invert operator
 */
template <typename T>
class invert_op : public object_wrapper<T>
{
    public:

        using hana_tag=operator_tag;

        static_assert(!std::is_same<exists_t,std::decay_t<T>>::value && !std::is_same<exists_op_with_string_t,std::decay_t<T>>::value,
                      "Invertion _n() can not be applied to operator exists");
        static_assert(!std::is_base_of<flag_t,std::decay_t<T>>::value,
                      "Invertion _n() can not be applied to operator flag");

        /**
         * @brief Constructor
         * @param val Operator
         */
        invert_op(
                T&& val
            ) : object_wrapper<T>(std::forward<T>(val))
        {}

        /**
         * @brief Operator of conversion to std::string
         */
        operator std::string() const
        {
            std::string str;
            backend_formatter.append(str,std::string(string_invert_op)," ",std::string(this->get()));
            return str;
        }

        /**
         * @brief Apply and invert operator
         * @param Value to validate
         * @param Sample vaue
         * @return Validation status
         */
        template <typename T1, typename T2>
        bool operator() (const T1& a, const T2& b) const
        {
            return !this->get()(a,b);
        }
};

/**
 * @brief Invert operator with explicit description
 */
template <typename T>
class invert_op_with_string : public invert_op<T>
{
    public:

        /**
         * @brief Constructor
         * @param val Operator
         * @param description String to use in report formatting to represent the operator
         */
        invert_op_with_string(
                T&& val,
                std::string description
            ) : invert_op<T>(std::forward<T>(val)),
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

/**
 * @brief Create inverted operator
 * @param v Operator
 * @return Inverted perator
 */
template <typename T>
constexpr auto _n(T&& v)
{
    return invert_op<T>(std::forward<T>(v));
}

/**
 * @brief Create inverted operator with custom description
 * @param v Operator
 * @param description Custom description
 * @return Inverted perator
 */
template <typename T>
auto _n(T&& v, std::string description)
{
    return invert_op_with_string<T>(std::forward<T>(v),std::move(description));
}

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_INVERT_OP_HPP
