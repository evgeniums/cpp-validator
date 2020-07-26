/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** \file validator/detail/validator_impl.hpp
*
*  Defines implementation of validator
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_VALIDATOR_IMPL_HPP
#define DRACOSHA_VALIDATOR_VALIDATOR_IMPL_HPP

#include <string>

#include <dracosha/validator/config.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

struct validator_tag;

/**
 * @brief Implementation of validator with explicit string description
 *
 * Validator holds the validating executer and invokes apply() method to perform validation
 * using embedded executor. Reporting description in reporting adapter will be overriden with explicit string description.
 */
template <typename Handler>
class validator_with_string_t
{
    public:

        using hana_tag=validator_tag;

        /**
         * @brief Ctor
         * @param fn Validation handler that will be called to perform validating
         */
        validator_with_string_t(
                Handler fn,
                std::string description
            ) : _fn(std::move(fn)),
                _description(std::move(description))
        {
        }

        /**
         * @brief Invoke validating with supplied args and override reporting description with ecplicit description.
         * @param adpt Reporting adapter
         * @param args Arguments to forward to embedded executor
         * @return Validation status
         */
        template <typename AdapterT, typename ... Args>
        auto apply(AdapterT&& adpt, Args&&... args) const
        {
            adpt.begin_explicit_report();
            auto ret=_fn(std::forward<AdapterT>(adpt),std::forward<Args>(args)...);
            adpt.end_explicit_report(_description);
            return ret;
        }

    private:

        Handler _fn;
        std::string _description;
};

/**
 * @brief Implementation of validator
 *
 * Validator holds the validating executer and invokes apply() method to perform validation
 * using embedded executor.
 */
template <typename Handler>
class validator_t
{
    public:

        using hana_tag=validator_tag;

        /**
         * @brief Ctor
         * @param fn Validation handler that will be called to perform validating
         */
        validator_t(Handler fn):_fn(std::move(fn))
        {
        }

        /**
         * @brief Invoke validating with supplied args
         * @param args Arguments to forward to embedded executor
         * @return Validation status
         */
        template <typename ... Args>
        auto apply(Args&&... args) const
        {
            return _fn(std::forward<Args>(args)...);
        }

        /**
         * @brief Create validator with explicit description for reporting.
         * @param str Explicit description
         * @return Validator with explicit description
         */
        template <typename T>
        auto operator () (T&& str)
        {
            return validator_with_string_t<Handler>(std::move(_fn),std::forward<T>(str));
        }

    private:

        Handler _fn;
};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_VALIDATOR_IMPL_HPP
