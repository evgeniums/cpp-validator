/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/utils/conditional_fold.hpp
*
*  Defines helpers to sequantially call each callable in a tupple with AND conjunction.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_CONDITIONAL_FOLD_HPP
#define DRACOSHA_VALIDATOR_CONDITIONAL_FOLD_HPP

#include <dracosha/validator/config.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//---------------------------------------------------------------

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

struct predicate_and_t
{
    template <typename T>
    constexpr bool operator() (const T& v) const
    {
        return v==true;
    }
};
constexpr predicate_and_t predicate_and{};

struct predicate_or_t
{
    template <typename T>
    constexpr bool operator() (const T& v) const
    {
        return v==false;
    }
};
constexpr predicate_or_t predicate_or{};

struct while_each_t
{
    template <typename FoldableT, typename PredicateT, typename InitT, typename HandlerT>
    auto operator () (const FoldableT& foldable, const PredicateT& pred, InitT&& init, const HandlerT& fn) const -> decltype(auto)
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
                return conditional_fold_t<FoldableT>::each(_(foldable),_(pred),_(fn));
            }
        );
    }

    template <typename FoldableT, typename PredicateT, typename InitT, typename StateT, typename HandlerT>
    auto operator () (const FoldableT& foldable, const PredicateT& pred, InitT&& init, StateT&& state, const HandlerT& fn) const -> decltype(auto)
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
                return conditional_fold_t<FoldableT>::each_with_state(_(foldable),_(pred),_(state),_(fn));
            }
        );
    }

    template <typename FoldableT, typename PredicateT, typename InitT, typename StateT, typename RetT, typename HandlerT>
    auto operator () (const FoldableT& foldable, const PredicateT& pred, InitT&& init, StateT&& state, RetT&& ret, const HandlerT& fn) const -> decltype(auto)
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
                return conditional_fold_t<FoldableT>::each_with_state_and_ret(_(foldable),_(pred),_(state),_(ret),_(fn));
            }
        );
    }
};
constexpr while_each_t while_each{};

struct while_prefix_t
{
    template <typename FoldableT, typename HandlerT, typename PredicateT, typename InitT>
    auto operator () (const FoldableT& foldable, const PredicateT& pred, InitT&& init, const HandlerT& fn) const -> decltype(auto)
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
                return conditional_fold_t<FoldableT>::prefix(hana::tuple<>{},_(foldable),_(pred),_(init),_(fn));
            }
        );
    }
};
constexpr while_prefix_t while_prefix{};

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_CONDITIONAL_FOLD_HPP
