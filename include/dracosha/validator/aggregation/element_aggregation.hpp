/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/aggregation/element_aggregation.hpp
*
*  Defines helpers used for element aggregations.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_ELEMENT_AGGREGATION_HPP
#define DRACOSHA_VALIDATOR_ELEMENT_AGGREGATION_HPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/utils/adjust_storable_ignore.hpp>
#include <dracosha/validator/aggregation/aggregation.hpp>
#include <dracosha/validator/filter_member.hpp>
#include <dracosha/validator/variadic_arg_tag.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

/**
 * @brief Base tag of ANY/ALL element aggregations.
 */
struct element_aggregation_tag
{
    template <typename T>
    constexpr bool operator == (T) const noexcept
    {
        return true;
    }
    template <typename T>
    constexpr bool operator != (T) const noexcept
    {
        return false;
    }

    template <typename T>
    constexpr friend bool operator == (T, const element_aggregation_tag&) noexcept
    {
        return true;
    }
    template <typename T>
    constexpr friend bool operator != (T, const element_aggregation_tag&) noexcept
    {
        return true;
    }
};

//-------------------------------------------------------------
/**
 * @brief Base struct of ANY/ALL element aggregations.
 */
struct element_aggregation : public adjust_storable_ignore
{
    using hana_tag=element_aggregation_tag;

    /**
     * @brief Invoke element aggregation operation for ALL/ANY aggregation.
     * @param pred Logical predicate to use for aggregation (and/or for ALL/ANY).
     * @param empt Handler of empty list of elements.
     * @param aggr Aggregation.
     * @param used_path_size Length of member's path prefix already used for extracting intermediate member values.
     * @param path Member's path prefix before this aggregation in the path including the aggregation itself.
     * @param adapter Validation adapter.
     * @param handler Handler to invoke on the member.
     */
    template <typename PredicateT, typename EmptyFnT, typename AggregationT,
              typename UsedPathSizeT, typename PathT, typename AdapterT, typename HandlerT>
    static status invoke(PredicateT&& pred, EmptyFnT&& empt, AggregationT&& aggr,
                         UsedPathSizeT&& used_path_size, PathT&& path, AdapterT&& adapter, HandlerT&& handler);

    /**
     * @brief Invoke element aggregation operation for ALL/ANY aggregation of variadic property argument aka varg(ALL/ANY,max_arg).
     * @param pred Logical predicate to use for aggregation (and/or for ALL/ANY).
     * @param empt Handler of empty list of elements.
     * @param aggr Aggregation.
     * @param used_path_size Length of member's path prefix already used for extracting intermediate member values.
     * @param path Member's path prefix before this aggregation in the path including the aggregation itself.
     * @param adapter Validation adapter.
     * @param handler Handler to invoke on the member.
     */
    template <typename PredicateT, typename EmptyFnT, typename AggregationT,
              typename UsedPathSizeT, typename PathT, typename AdapterT, typename HandlerT>
    static status invoke_variadic(PredicateT&& pred, EmptyFnT&& empt, AggregationT&& aggr,
                         UsedPathSizeT&& used_path_size, PathT&& path, AdapterT&& adapter, HandlerT&& handler);
};

//-------------------------------------------------------------

struct tree_tag;

/**
 * @brief Implementer of is_element_aggregation.
 */
struct is_element_aggregation_impl
{
    template <typename PrevResultT, typename T>
    constexpr auto operator () (PrevResultT prev, T) const
    {
        using type=typename T::type;
        return hana::if_(
            prev,
            prev,
            hana::bool_
            <
                hana::is_a<element_aggregation_tag,type>
                ||
                hana::is_a<tree_tag,type>
                ||
                std::is_base_of<variadic_arg_aggregation_tag,std::decay_t<type>>::value
            >{}
        );
    }
};
/**
 * @brief Helper to figure out out if member path contains element aggregation.
 */
constexpr is_element_aggregation_impl is_element_aggregation{};

//-------------------------------------------------------------

/**
 * @brief Base struct of element aggregations with modifiers.
 */
template <typename ModifierT>
struct element_aggregation_with_modifier : public element_aggregation
{
    constexpr static const ModifierT& modifier=ModifierT::instance();
};

//-------------------------------------------------------------

struct element_aggregation_modifier_tag;

/**
 * @brief Base struct of element aggregation modifiers.
 */
template <typename DerivedT>
struct element_aggregation_modifier
{
    using hana_tag=element_aggregation_modifier_tag;
};

/**
 * @brief Element aggregation modifier for keys.
 */
struct keys_t : public element_aggregation_modifier<keys_t>
{
    constexpr static const keys_t& instance();
};
constexpr keys_t keys{};
constexpr const keys_t& keys_t::instance()
{
    return keys;
}

/**
 * @brief Element aggregation modifier for values.
 */
struct values_t : public element_aggregation_modifier<values_t>
{
    constexpr static const values_t& instance();
};
constexpr values_t values{};
constexpr const values_t& values_t::instance()
{
    return values;
}

/**
 * @brief Element aggregation modifier for iterators.
 */
struct iterators_t : public element_aggregation_modifier<iterators_t>
{
    constexpr static const iterators_t& instance();
};
constexpr iterators_t iterators{};
constexpr const iterators_t& iterators_t::instance()
{
    return iterators;
}

//-------------------------------------------------------------

/**
 * @brief Template specialization of generate_paths_t to generate paths from element aggregation.
 */
template <typename KeyT>
struct generate_paths_t<KeyT,hana::when<std::is_base_of<element_aggregation,std::decay_t<KeyT>>::value>>
{
    template <typename UsedPathSizeT, typename PathT, typename AdapterT, typename HandlerT>
    status operator () (UsedPathSizeT&& used_path_size, PathT&& path, AdapterT&& adapter, HandlerT&& handler) const
    {
        const auto& aggregation=hana::back(path);
        return element_aggregation::invoke(
            hana::partial(aggregation.predicate(),std::forward<AdapterT>(adapter)),
            aggregation.post_empty_handler(),
            aggregation.string(),
            std::forward<UsedPathSizeT>(used_path_size),
            path,
            std::forward<AdapterT>(adapter),
            std::forward<HandlerT>(handler)
        );
    }
};

//-------------------------------------------------------------

/**
 * @brief Template specialization of generate_paths_t to generate paths from element aggregation of variadic property arguments.
 */
template <typename KeyT>
struct generate_paths_t<KeyT,hana::when<std::is_base_of<variadic_arg_aggregation_tag,KeyT>::value>>
{
    template <typename UsedPathSizeT, typename PathT, typename AdapterT, typename HandlerT>
    status operator () (UsedPathSizeT&& used_path_size, PathT&& path, AdapterT&& adapter, HandlerT&& handler) const
    {
        const auto& aggregation=hana::back(path).get().aggregation;
        return element_aggregation::invoke_variadic(
            hana::partial(aggregation.predicate(),std::forward<AdapterT>(adapter)),
            aggregation.post_empty_handler(),
            aggregation.string(),
            std::forward<UsedPathSizeT>(used_path_size),
            path,
            std::forward<AdapterT>(adapter),
            std::forward<HandlerT>(handler)
        );
    }
};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_ELEMENT_AGGREGATION_HPP
