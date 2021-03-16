/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/operators/operator.hpp
*
*  Defines base operator class.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_OPERATOR_HPP
#define DRACOSHA_VALIDATOR_OPERATOR_HPP

#include <string>
#include <dracosha/validator/config.hpp>
#include <dracosha/validator/utils/enable_to_string.hpp>
#include <dracosha/validator/utils/adjust_storable_type.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

/**
 * @brief Tag for all operator clases.
 */
struct operator_tag
{
    constexpr bool operator ==(const operator_tag&) const
    {
        return true;
    }
    constexpr bool operator !=(const operator_tag&) const
    {
        return false;
    }
};

/**
 * @brief Base operator class.
 */
template <typename DerivedT>
struct op : public enable_to_string<DerivedT>,
            public adjust_storable_ignore
{
    using hana_tag=operator_tag;

    /**
     * @brief Get description of the operator to be used in reports.
     * @return String description of the operator.
     */
    constexpr static const char* str()
    {
        return DerivedT::description;
    }

    /**
     * @brief Get description of inverted operator to be used in reports.
     * @return String description of inverted operator.
     */
    constexpr static const char* n_str()
    {
        return DerivedT::n_description;
    }
};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_OPERATOR_HPP
