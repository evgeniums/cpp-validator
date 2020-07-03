/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** \file validator/master_sampler.hpp
*
*  Defines wrapper of master sample object that can be used in validation operator
*  as argument substitution. If such master sample is used as argument then
*  the same member from master sample object will be forwarded to validation operator.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_MASTER_SAMPLE_HPP
#define DRACOSHA_VALIDATOR_MASTER_SAMPLE_HPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/dispatcher.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

struct master_sample_tag;

/**
 * @brief Wrapper of master sample object
 */
template <typename T>
struct master_sample
{
    using hana_tag=master_sample_tag;

    /**
     * @brief Ctor
     * @param Reference object
     */
    master_sample(const T& obj) : ref(obj)
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

struct string_master_sample_t : public op<string_master_sample_t>
{
    constexpr static const char* description="sample";
};
constexpr string_master_sample_t string_master_sample{};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_MASTER_SAMPLE_HPP
