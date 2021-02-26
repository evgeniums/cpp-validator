/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/master_sampler.hpp
*
*  Defines wrapper of master sample object.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_MASTER_SAMPLE_HPP
#define DRACOSHA_VALIDATOR_MASTER_SAMPLE_HPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/operand.hpp>
#include <dracosha/validator/utils/enable_to_string.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

struct master_sample_tag;

/**
 * @brief Wrapper of master sample object
 *
 *  Wrapped master sample object can be used in validation operator
 *  as argument substitution. If such master sample is used as argument then
 *  the same member from master sample object will be forwarded to validation operator.
 */
template <typename T>
struct master_sample
{
    using hana_tag=master_sample_tag;

    /**
     * @brief Ctor
     * @param Sample object
     */
    master_sample(const T& obj) : ref(obj)
    {}

    /**
     * @brief Get Sample object
     * @return Sample object
     */
    const T& operator() () const
    {
        return ref;
    }

    const T& ref;
};

/**
 * @brief Check if type is a master sample.
 */
template <typename T, typename =hana::when<true>>
struct is_master_sample
{
    constexpr static const bool value=false;
};

/**
 * @brief Check if type is a master sample.
 *
 * Direct master sample object.
 */
template <typename T>
struct is_master_sample<
            T,
            hana::when<hana::is_a<master_sample_tag,T>>
        >
{
    constexpr static const bool value=true;
};

/**
 * @brief Check if type is a master sample.
 *
 * Master sample object embedded to operand.
 */
template <typename T>
struct is_master_sample<
            T,
            hana::when<
                (
                    hana::is_a<operand_tag,T>
                    &&
                    std::decay_t<T>::is_master_sample
                )
            >
        >
{
    constexpr static const bool value=true;
};

//-------------------------------------------------------------

/**
 * @brief String description for operator maste sample.
 */
struct string_master_sample_t : public enable_to_string<string_master_sample_t>
{
    constexpr static const char* description="sample";
};

/**
 * @brief Instance of string description for operator maste sample.
 */
constexpr string_master_sample_t string_master_sample{};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_MASTER_SAMPLE_HPP
