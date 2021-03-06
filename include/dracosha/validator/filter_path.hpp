/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/filer_path.hpp
*
*  Defines "filter_path".
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_FILTER_PATH_HPP
#define DRACOSHA_VALIDATOR_FILTER_PATH_HPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/utils/conditional_fold.hpp>
#include <dracosha/validator/check_member_path.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

struct filter_path_tag{};

struct path_starts_with_t
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
                    return result{check_paths_equal(path,path2),true};
                }
                return state;
            }
        );
    }
};
constexpr path_starts_with_t path_starts_with{};

struct has_path_t
{
    template <typename T1, typename T2>
    constexpr bool operator() (const T1& path_ts, const T2& path) const
    {
        return hana::fold(
            path_ts,
            false,
            [](bool has, auto&& current_path)
            {
                return has || path_starts_with(path,current_path);
            }
        );
    }
};
constexpr has_path_t has_path{};


template <typename BaseTraitsT,
          typename IncludePathsT,
          typename ExcludePathsT
          >
class filter_path_traits : public BaseTraitsT,
                           public filter_path_tag
{
    public:

        template <typename BaseTraitsT>
        filter_path_traits(
                BaseTraitsT&& traits,
                IncludePathsT include_paths,
                ExcludePathsT exclude_paths
            )
            : BaseTraitsT(std::forward<BaseTraitsT1>(traits)),
              _include_paths(std::move(include_paths)),
              _exclude_paths(std::move(exclude_paths))
        {
        }

        template <typename BaseTraitsT>
        filter_path_traits(
                BaseTraitsT&& traits,
                IncludePathsT include_paths
            )
            : filter_path_traits(
                  std::forward<BaseTraitsT1>(traits),
                  std::move(include_paths),
                  ExcludePathsT()
              )
        {
        }

        template <typename BaseTraitsT>
        filter_path_traits(
                BaseTraitsT&& traits,
                ExcludePathsT exlude_paths
            )
            : filter_path_traits(
                  std::forward<BaseTraitsT1>(traits),
                  IncludePathsT(),
                  _exclude_paths(std::move(exclude_paths))
              )
        {
        }

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

struct filter_path_t
{
    template <typename AdapterT, typename PathT>
    bool operator () (const AdapterT& adapter, const PathT& path) const noexcept
    {
        using type=typename AdapterT::type;
        return hana::eval_if(
            std::is_base_of<filter_path_tag,std::decay_t<type>>{},
            [&](auto&& _)
            {
                return _(adapter).traits().filter(_(path));
            },
            [&](auto&&)
            {
                return false;
            }
        );
    }
};
constexpr filter_path_t filter_path{};

template <typename AdapterT, typename IncludePathsT>
auto include_paths(AdapterT&& adapter, IncludePathsT&& include_paths)
{
    auto create=[&](auto&& traits)
    {
        return filter_path_traits<std::decay_t<decltype(traits)>,std::decay_t<IncludePathsT>,std::tuple<>>{
            std::move(traits),
            std::forward<IncludePathsT>(include_paths)
        };
    };
    return adapter.derive(create);
}

template <typename AdapterT, typename ExcludePathsT>
auto exclude_paths(AdapterT&& adapter, ExcludePathsT&& exclude_paths)
{
    auto create=[&](auto&& traits)
    {
        return filter_path_traits<std::decay_t<AdapterT>,std::tuple<>,std::decay_t<ExcludePathsT>>{
            std::move(traits),
            std::forward<ExcludePathsT>(exclude_paths)
        };
    };
    return adapter.derive(create);
}

template <typename AdapterT, typename IncludePathsT, typename ExcludePathsT>
auto include_and_exclude_paths(AdapterT&& adapter, ExcludePathsT&& exclude_paths)
{
    auto create=[&](auto&& traits)
    {
        return filter_path_traits<std::decay_t<AdapterT>,std::decay_t<IncludePathsT>,std::decay_t<ExcludePathsT>>{
            std::move(traits),
            std::forward<IncludePathsT>(include_paths),
            std::forward<ExcludePathsT>(exclude_paths)
        };
    };
    return adapter.derive(create);
}

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_FILTER_PATH_HPP
