/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** \file validator/master_referencer.hpp
*
*  Defines wrapper of master reference object that can be used in validation operator
*  as argument substitution. If such master reference is used as argument then
*  the same member from master reference object will be forwarded to validation operator.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_MASTER_REFERENCE_HPP
#define DRACOSHA_VALIDATOR_MASTER_REFERENCE_HPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/validate.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

/**
 * @brief Wrapper of master reference object
 */
template <typename T>
struct master_reference
{
    using hana_tag=master_reference_tag;

    /**
     * @brief Ctor
     * @param Reference object
     */
    master_reference(const T& obj) : ref(obj)
    {}

    /**
     * @brief Get reference object
     * @return Reference object
     */
    const T& operator() () const
    {
        return ref;
    }

    const T& ref;
};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_MASTER_REFERENCE_HPP
