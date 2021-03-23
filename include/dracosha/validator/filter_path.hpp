/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/filer_path.hpp
*
*  Defines "filter_path" and related helpers.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_FILTER_PATH_HPP
#define DRACOSHA_VALIDATOR_FILTER_PATH_HPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/utils/conditional_fold.hpp>
#include <dracosha/validator/check_member_path.hpp>
#include <dracosha/validator/member_path.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

struct filter_path_tag{};

//-------------------------------------------------------------

/**
 * @brief Implementer of path_starts_with().
 */
struct path_starts_with_impl
{
    struct result
    {
        bool status;
        bool done;

        operator bool() const noexcept
        {
            return status;
        }
    };

    template <typename T1, typename T2>
    constexpr bool operator() (const T1& path1, const T2& path2) const
    {
        return while_prefix(
            path1,
            [](const result& res)
            {
                return !res.status && !res.done;
            },
            result{false,false},
            [&](auto&& state, auto&& path)
            {
                if (hana::equal(hana::size(path),hana::size(path2)))
                {
                    return result{paths_equal(path,path2),true};
                }
                return state;
            }
        );
    }
};
/**
 * @brief Check if path 1 starts with path 2.
 * @param path1
 * @param path2
 * @return Boolean result.
 */
constexpr path_starts_with_impl path_starts_with{};

//-------------------------------------------------------------

/**
 * @brief Implementer of has_path().
 */
struct has_path_impl
{
    template <typename T1, typename T2>
    constexpr bool operator() (const T1& path_ts, const T2& path) const
    {
        return while_each(
            path_ts,
            predicate_or,
            false,
            [&](auto&& current_path)
            {
                return path_starts_with(path,current_path);
            }
        );
    }
};
/**
 * @brief Check if foldable container of paths contains certain path.
 * @param path_ts COntainer of paths.
 * @param path Path to test.
 * @return Boolean result.
 */
constexpr has_path_impl has_path{};

//-------------------------------------------------------------

/**
 * @brief Adapter traits for member filtering adapter.
 *
 * This adapter traits inherits original adapter traits adding member filtering feature.
 * Adapter will allow only included paths except for excluded paths.
 * By default all paths are included and none of paths is excluded.
 */
template <typename BaseTraitsT,
          typename IncludePathsT,
          typename ExcludePathsT
          >
class filter_path_traits : public BaseTraitsT,
                           public filter_path_tag
{
    public:

        /**
         * @brief Constructor.
         * @param Original adapter traits.
         * @param include_paths Included paths.
         * @param exclude_paths Excluded paths.
         */
        template <typename BaseTraitsT1>
        filter_path_traits(
                BaseTraitsT1&& traits,
                IncludePathsT include_paths,
                ExcludePathsT exclude_paths
            )
            : BaseTraitsT(std::forward<BaseTraitsT1>(traits)),
              _include_paths(std::move(include_paths)),
              _exclude_paths(std::move(exclude_paths))
        {}

        /**
         * @brief Constructor.
         * @param Original adapter traits.
         * @param include_paths Included paths.
         */
        template <typename BaseTraitsT1>
        filter_path_traits(
                BaseTraitsT1&& traits,
                IncludePathsT include_paths
            )
            : filter_path_traits(
                  std::forward<BaseTraitsT1>(traits),
                  std::move(include_paths),
                  ExcludePathsT()
              )
        {}

        /**
         * @brief Constructor.
         * @param Original adapter traits.
         * @param exclude_paths Excluded paths.
         */
        template <typename BaseTraitsT1>
        filter_path_traits(
                BaseTraitsT1&& traits,
                ExcludePathsT exclude_paths
            )
            : filter_path_traits(
                  std::forward<BaseTraitsT1>(traits),
                  IncludePathsT(),
                  std::move(exclude_paths)
              )
        {}

        /**
         * @brief Filter a member's path.
         * @param path Member's path.
         * @return Trus if path is filtered off and must not be validated, false otherwise.
         */
        template <typename PathT>
        bool filter(const PathT& path) const noexcept
        {
            bool included=hana::is_empty(_include_paths)
                    || has_path(_include_paths,path);

            return !(included && !has_path(_exclude_paths,path));
        }

    private:

        IncludePathsT _include_paths;
        ExcludePathsT _exclude_paths;
};

//-------------------------------------------------------------

/**
 * @brief Implementer of filter_path().
 */
struct filter_path_impl
{
    template <typename AdapterT, typename PathT>
    bool operator () (const AdapterT& adapter, const PathT& path) const noexcept
    {
        using type=typename AdapterT::type;
        return hana::eval_if(
            std::is_base_of<filter_path_tag,std::decay_t<type>>{},
            [&](auto&& _)
            {
                return traits_of(_(adapter)).filter(_(path));
            },
            [&](auto&&)
            {
                return false;
            }
        );
    }
};
/**
 * @brief Filter a member's path.
 * @param adapter Valdation adapter.
 * @param path Member's path.
 * @return Trus if path is filtered off and must not be validated, false otherwise.
 *
 * Filtering occurs only if adapter traits inherit filter_path_tag.
 */
constexpr filter_path_impl filter_path{};

//-------------------------------------------------------------

/**
 * @brief Helper for building filtering adapter with explicit list of included paths.
 * @param adapter Original validation adapter.
 * @param paths Container with list of explicitly included paths.
 * @return Member filtering adapter.
 */
template <typename AdapterT, typename PathsT>
auto include_paths(AdapterT adapter, PathsT&& paths)
{
    auto create=[&](auto traits)
    {
        return filter_path_traits<std::decay_t<decltype(traits)>,std::decay_t<PathsT>,std::tuple<>>{
            std::move(traits),
            std::forward<PathsT>(paths)
        };
    };
    return adapter.clone(create);
}

//-------------------------------------------------------------

/**
 * @brief Helper for building filtering adapter with explicit list of excluded paths.
 * @param adapter Original validation adapter.
 * @param paths Container with list of explicitly excluded paths.
 * @return Member filtering adapter.
 */
template <typename AdapterT, typename PathsT>
auto exclude_paths(AdapterT adapter, PathsT&& paths)
{
    auto create=[&](auto traits)
    {
        return filter_path_traits<std::decay_t<decltype(traits)>,std::tuple<>,std::decay_t<PathsT>>{
            std::move(traits),
            std::forward<PathsT>(paths)
        };
    };
    return adapter.clone(create);
}

//-------------------------------------------------------------

/**
 * @brief Helper for building filtering adapter with explicit lists of included and excluded paths.
 * @param adapter Original validation adapter.
 * @param in_paths Container with list of explicitly included paths.
 * @param ex_paths Container with list of explicitly excluded paths.
 * @return Member filtering adapter.
 */
template <typename AdapterT, typename InPathsT, typename ExPathsT>
auto include_and_exclude_paths(AdapterT adapter, InPathsT&& in_paths, ExPathsT&& ex_paths)
{
    auto create=[&](auto traits)
    {
        return filter_path_traits<std::decay_t<decltype(traits)>,std::decay_t<InPathsT>,std::decay_t<ExPathsT>>{
            std::move(traits),
            std::forward<InPathsT>(in_paths),
            std::forward<ExPathsT>(ex_paths)
        };
    };
    return adapter.clone(create);
}

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_FILTER_PATH_HPP
