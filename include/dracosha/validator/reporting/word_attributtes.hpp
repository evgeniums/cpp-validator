/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** \file validator/reporting/word_attributes.hpp
*
* Defines lexical attributes of words and phrases
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_WORD_ATTRIBUTES_HPP
#define DRACOSHA_VALIDATOR_WORD_ATTRIBUTES_HPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/utils/make_types_tuple.hpp>
#include <dracosha/validator/utils/feature_bitmask.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

/**
 * @brief Lexical attributes of a word or phrase
 */
enum class word : uint8_t
{
    neuter, //!< neuter gender
    feminine, //!< feminine gender
    masculine, //!< masculine gender

    // reserved attributes from 4 to 16

    plural=17, //!< plural form

    END_=32 //!< special end tag
};
constexpr feature_bitmask_t<word> word_bitmask{};

using word_attributes=typename feature_bitmask_t<word>::bitmask;

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_WORD_ATTRIBUTES_HPP
