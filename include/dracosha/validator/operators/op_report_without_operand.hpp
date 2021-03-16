/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/operators/op_report_without_operand.hpp
*
* Defines base class for operators that swallow operands in reports.
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_OP_REPORT_WITHOUT_OPERAND_HPP
#define DRACOSHA_VALIDATOR_OP_REPORT_WITHOUT_OPERAND_HPP

#include <string>

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/operators/operator.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

/**
 * @brief Base class for operators that swallow operands in reports.
 */
struct op_report_without_operand_t : public adjust_storable_ignore
{
    using hana_tag=operator_tag;
};

/**
 * @brief Base template class for operators that swallow operands in reports.
 */
template <typename DerivedT>
struct op_report_without_operand : public op_report_without_operand_t,
                                   public enable_to_string<DerivedT>
{
    template <typename T>
    static auto str(const T& b,
                    std::enable_if_t<
                            std::is_same<T,bool>::value,
                            void*
                        > = nullptr)
    {
        return b ? DerivedT::description : DerivedT::n_description;
    }

    template <typename T>
    static auto n_str(const T& b,
                    std::enable_if_t<
                            std::is_same<T,bool>::value,
                            void*
                        > = nullptr)
    {
        return b ? DerivedT::n_description : DerivedT::description;
    }

    template <typename T>
    static auto str(const T& b,
                    std::enable_if_t<
                            !std::is_same<T,bool>::value,
                            void*
                        > = nullptr)
    {
        std::ignore=b;
        return DerivedT::description;
    }

    template <typename T>
    static auto n_str(const T& b,
                    std::enable_if_t<
                            !std::is_same<T,bool>::value,
                            void*
                        > = nullptr)
    {
        std::ignore=b;
        return DerivedT::n_description;
    }

    static auto str()
    {
        return DerivedT::description;
    }

    static auto n_str()
    {
        return DerivedT::n_description;
    }
};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_OP_REPORT_WITHOUT_OPERAND_HPP
