/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/utils/conditional_fold.hpp
*
*  Defines helpers to sequantially call each callable in a tuple with some logical predicate.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_CONDITIONAL_FOLD_HPP
#define DRACOSHA_VALIDATOR_CONDITIONAL_FOLD_HPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/ignore_compiler_warnings.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//---------------------------------------------------------------

namespace detail {

/**
 * @brief Implementations of while_each() and while_prefix().
 */
template <typename FoldableT>
struct conditional_fold_t
{
    template <typename HandlerT, typename PredicateT>
    static auto each(const FoldableT& foldable, const PredicateT& pred, const HandlerT& fn)
    {
        auto res=fn(hana::front(foldable));
        if (!pred(res))
        {
            return res;
        }
        auto next=hana::drop_front(foldable);
        return hana::eval_if(
            hana::is_empty(next),
            [&](auto&&)
            {
                return res;
            },
            [&](auto&& _)
            {
                return conditional_fold_t<decltype(_(next))>::each(_(next),pred,fn);
            }
        );
    }

    template <typename PredicateT, typename StateT, typename HandlerT>
    static auto each_with_state(const FoldableT& foldable, const PredicateT& pred, StateT&& state, const HandlerT& fn)
    {
        auto res=fn(state,hana::front(foldable));
        if (!pred(res))
        {
            return res;
        }
        auto next=hana::drop_front(foldable);
        return hana::eval_if(
            hana::is_empty(next),
            [&](auto&& _)
            {
                return _(res);
            },
            [&](auto&& _)
            {
                return conditional_fold_t<decltype(_(next))>::each_with_state(_(next),pred,_(res),fn);
            }
        );
    }

DCS_IGNORE_MAYBE_UNINITIALIZED_BEGIN
    template <typename PredicateT, typename StateT, typename RetT, typename HandlerT>
    static auto each_with_state_and_ret(const FoldableT& foldable, const PredicateT& pred, StateT&& state, RetT&& ret, const HandlerT& fn)
    {
        auto res=fn(state,hana::front(foldable));
        if (!pred(res))
        {
            return ret;
        }
        auto next=hana::drop_front(foldable);
        return hana::eval_if(
            hana::is_empty(next),
            [&](auto&& _)
            {
                return _(res);
            },
            [&](auto&& _)
            {
                return conditional_fold_t<decltype(_(next))>::each_with_state_and_ret(_(next),pred,_(res),_(ret),fn);
            }
        );
    }
DCS_IGNORE_MAYBE_UNINITIALIZED_END

    template <typename PrefixT, typename NextT, typename StateT, typename HandlerT, typename PredicateT>
    static auto prefix(PrefixT&& pfx, const NextT& foldable, const PredicateT& pred, StateT&& state, const HandlerT& fn)
    {
        auto front=hana::front(foldable);
        auto new_prefix=hana::append(std::forward<PrefixT>(pfx),front);
        std::decay_t<StateT> res=fn(state,new_prefix);
        if (!pred(res))
        {
            return res;
        }
        auto next=hana::drop_front(foldable);
        return hana::eval_if(
            hana::is_empty(next),
            [&](auto&&)
            {
                return res;
            },
            [&](auto&& _)
            {
                return conditional_fold_t<decltype(_(next))>::prefix(std::move(_(new_prefix)),_(next),pred,res,fn);
            }
        );
    }
};
}

//-------------------------------------------------------------

/**
 * @brief Implementer of while_each().
 */
struct while_each_impl
{
    /**
     * @brief Invoke a handler on each element of a foldable container while predicate is satisfied.
     * @param foldable Foldable container.
     * @param pred Predicate taking handler's return as input.
     * @param init Initial value to pass to predicate and return if predicate is not satisfied.
     * @param handler Handler to invoke. Handler signature must be: "RetT (auto&& element)".
     * @return Accumulated result of handler invocations.
     */
    template <typename FoldableT, typename PredicateT, typename InitT, typename HandlerT>
    auto operator () (const FoldableT& foldable, const PredicateT& pred, InitT&& init, const HandlerT& handler) const -> decltype(auto)
    {
        return hana::eval_if(
            hana::is_empty(foldable),
            [&](auto&&) -> decltype(auto)
            {
                return hana::id(std::forward<InitT>(init));
            },
            [&](auto&& _)
            {
                if (!pred(init))
                {
                    return init;
                }
                return detail::conditional_fold_t<FoldableT>::each(_(foldable),_(pred),_(handler));
            }
        );
    }

    /**
     * @brief Invoke a handler on each element of a foldable container while predicate is satisfied, with state value.
     * @param foldable Foldable container.
     * @param pred Predicate taking handler's return as input.
     * @param init Initial value to pass to predicate and return if predicate is not satisfied.
     * @param state State that can be changed after each invocation of the handler and passed to next invocation.
     * @param handler Handler to invoke. Handler signature must be: "RetT (auto&& state, auto&& element)".
     * @return Accumulated result of handler invocations.
     */
    template <typename FoldableT, typename PredicateT, typename InitT, typename StateT, typename HandlerT>
    auto operator () (const FoldableT& foldable, const PredicateT& pred, InitT&& init, StateT&& state, const HandlerT& handler) const -> decltype(auto)
    {
        return hana::eval_if(
            hana::is_empty(foldable),
            [&](auto&&) -> decltype(auto)
            {
                return hana::id(std::forward<InitT>(init));
            },
            [&](auto&& _)
            {
                if (!pred(init))
                {
                    return init;
                }
                return detail::conditional_fold_t<FoldableT>::each_with_state(_(foldable),_(pred),_(state),_(handler));
            }
        );
    }

    /**
     * @brief Invoke a handler on each element of a foldable container while predicate is satisfied, with state and return values.
     * @param foldable Foldable container.
     * @param pred Predicate taking handler's return as input.
     * @param init Initial value to pass to predicate.
     * @param state State that can be changed after each invocation of the handler and passed to next invocation.
     * @param ret Return value that must be returned if predicate is not satisfied.
     * @param handler Handler to invoke. Handler signature must be: "RetT (auto&& state, auto&& element)".
     * @return Accumulated result of handler invocations if predicate is satisfied, ret otherwise.
     */
    template <typename FoldableT, typename PredicateT, typename InitT, typename StateT, typename RetT, typename HandlerT>
    auto operator () (const FoldableT& foldable, const PredicateT& pred, InitT&& init,
                      StateT&& state, RetT&& ret, const HandlerT& handler) const -> decltype(auto)
    {
        return hana::eval_if(
            hana::is_empty(foldable),
            [&](auto&&) -> decltype(auto)
            {
                return hana::id(std::forward<InitT>(init));
            },
            [&](auto&& _)
            {
                if (!pred(_(init)))
                {
                    return _(ret);
                }
                return detail::conditional_fold_t<FoldableT>::each_with_state_and_ret(_(foldable),_(pred),_(state),_(ret),_(handler));
            }
        );
    }
};
/**
 * @brief Invoke a handler on each element of a foldable container while predicate is satisfied.
 *
 * Three versions are available:
 *  - with init;
 *  - with init and state;
 *  - with init, state and return.
 *
 *  See comments to respective operators above.
 */
constexpr while_each_impl while_each{};

//-------------------------------------------------------------

/**
 * @brief Implementer of while_prefix().
 */
struct while_prefix_impl
{
    template <typename FoldableT, typename HandlerT, typename PredicateT, typename InitT>
    auto operator () (const FoldableT& foldable, const PredicateT& pred, InitT&& init, const HandlerT& handler) const -> decltype(auto)
    {
        return hana::eval_if(
            hana::is_empty(foldable),
            [&](auto&&) -> decltype(auto)
            {
                return hana::id(std::forward<InitT>(init));
            },
            [&](auto&& _) -> InitT
            {
                if (!pred(init))
                {
                    return init;
                }
                return detail::conditional_fold_t<FoldableT>::prefix(hana::tuple<>{},_(foldable),_(pred),_(init),_(handler));
            }
        );
    }
};
/**
 * @brief Invoke a handler on each prefix of a foldable container while predicate is satisfied.
 * @param foldable Foldable container.
 * @param pred Predicate taking handler's return as input.
 * @param init Initial value to pass to predicate and return if predicate is not satisfied.
 * @param handler Handler to invoke. Handler signature must be: "RetT (auto&& state, auto&& foldable_prefix)".
 * @return Accumulated result of handler invocations.
 */
constexpr while_prefix_impl while_prefix{};

//-------------------------------------------------------------

/**
 * @brief Implementer of predicate_and.
 */
struct predicate_and_impl
{
    template <typename T>
    constexpr bool operator() (const T& v) const
    {
        return v==true;
    }
};
/**
 * @brief Default predicate for AND condition.
 */
constexpr predicate_and_impl predicate_and{};

//-------------------------------------------------------------

/**
 * @brief Implementer of predicate_or.
 */
struct predicate_or_impl
{
    template <typename T>
    constexpr bool operator() (const T& v) const
    {
        return v==false;
    }
};
/**
 * @brief Default predicate for OR condition.
 */
constexpr predicate_or_impl predicate_or{};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_CONDITIONAL_FOLD_HPP
