/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/detail/validator_impl.hpp
*
*  Defines implementation of validator.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_VALIDATOR_IMPL_HPP
#define DRACOSHA_VALIDATOR_VALIDATOR_IMPL_HPP

#include <string>

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/utils/adjust_storable_type.hpp>
#include <dracosha/validator/adapters/default_adapter.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

struct validator_tag;

/**
 * @brief Validator with hint.
 *
 * Validator holds the validating executer and invokes apply() method to perform validation
 * using embedded executor.
 * Before and after validation validator calls hint_before() and hint_after() methods of adapter respectively.
 */
template <typename Handler, typename HintT>
class validator_with_hint_t
{
    public:

        using hana_tag=validator_tag;
        using hint_type=typename adjust_storable_type<HintT>::type;

        /**
         * @brief Constructor.
         * @param fn Validation handler that will be called to perform validating.
         */
        template <typename HintT1>
        validator_with_hint_t(
                Handler fn,
                HintT1&& hint
            ) : _fn(std::move(fn)),
                _hint(std::forward<HintT1>(hint))
        {
        }

        /**
         * @brief Invoke validating with supplied args and override reporting description with ecplicit description.
         * @param adpt Adapter.
         * @param args Arguments to forward to embedded executor.
         * @return Validation status.
         */
        template <typename AdapterT, typename ... Args>
        auto apply(AdapterT&& adpt, Args&&... args) const
        {
            auto ret=adpt.hint_before(_hint);
            if (!ret)
            {
                return adpt.hint_after(ret,_hint);
            }
            ret=_fn(ensure_adapter(std::forward<AdapterT>(adpt)),std::forward<Args>(args)...);
            return adpt.hint_after(ret,_hint);
        }

    private:

        Handler _fn;
        hint_type _hint;
};

/**
 * @brief Validator.
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
         * @brief Construtor
         * @param fn Validation handler that will be called to perform validating.
         */
        validator_t(Handler fn):_fn(std::move(fn))
        {
        }

        /**
         * @brief Invoke validating with supplied args.
         * @param adpt Adapter.
         * @param args Arguments to forward to embedded executor.
         * @return Validation status.
         */
        template <typename AdapterT, typename ... Args>
        auto apply(AdapterT&& adpt, Args&&... args) const
        {
            return _fn(ensure_adapter(std::forward<AdapterT>(adpt)),std::forward<Args>(args)...);
        }

        /**
         * @brief Create validator with hint.
         * @param h Hint.
         * @return Validator with hint.
         */
        template <typename T>
        auto hint(T&& h)
        {
            return validator_with_hint_t<Handler,T>(std::move(_fn),std::forward<T>(h));
        }

        /**
         * @brief Create validator with hint.
         * @param h Hint.
         * @return Validator with hint.
         */
        template <typename T>
        auto operator () (T&& h)
        {
            return hint(std::forward<T>(h));
        }

    private:

        Handler _fn;
};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_VALIDATOR_IMPL_HPP
