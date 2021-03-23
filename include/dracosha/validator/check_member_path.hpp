/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/check_member_path.hpp
*
*  Defines helpers for checking existance of member path in type of an object.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_CHECK_MEMBER_PATH_HPP
#define DRACOSHA_VALIDATOR_CHECK_MEMBER_PATH_HPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/check_member.hpp>
#include <dracosha/validator/utils/unwrap_object.hpp>
#include <dracosha/validator/utils/safe_compare.hpp>
#include <dracosha/validator/utils/conditional_fold.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

template <typename Tobj, typename Tpath>
constexpr auto member_value_type(Tobj&& obj, Tpath&& path)
{
    auto path_c=hana::transform(path,unwrap_object_type_c);
    auto obj_c=hana::type_c<decltype(obj)>;

    return hana::monadic_fold_left<hana::optional_tag>(path_c,obj_c,hana::sfinae(check_member));
}

/**
  @brief Check if path of types in member descriptor can exist in the object.
  @param obj Object under validation.
  @param path Member path as a tuple.
  @return Validation status.

  This operation is expected to be static and assumed to be performed at compile time.
*/
template <typename Tobj, typename Tpath>
constexpr auto is_member_path_valid(Tobj&& obj, Tpath&& path)
{
    return hana::or_(
                        hana::is_empty(path),
                        hana::not_(hana::is_nothing(member_value_type(obj,path)))
                    );
}

struct element_aggregation_tag;
struct path_types_equal_t
{
    template <typename LeftT, typename RightT>
    constexpr auto operator () (LeftT, RightT) const noexcept
    {
        using T1=typename std::decay_t<LeftT>::type;
        using T2=typename std::decay_t<RightT>::type;
        return hana::or_(
                    typename safe_eq<T1,T2>::comparable{},
                    hana::is_a<element_aggregation_tag,T1>,
                    hana::is_a<element_aggregation_tag,T2>
                  );
    }
};
constexpr path_types_equal_t path_types_equal{};

template <typename T1, typename T2>
auto same_path_types(const T1& path1,const T2& path2)
{
    auto path1_c=hana::transform(path1,unwrap_object_type_c);
    auto path2_c=hana::transform(path2,unwrap_object_type_c);

    return hana::eval_if(
        hana::not_equal(hana::size(path1_c),hana::size(path2_c)),
        [](auto&&)
        {
            return hana::false_c;
        },
        [&](auto&& _)
        {
            return hana::fold(
                hana::zip(_(path1_c),_(path2_c)),
                hana::true_c,
                [](auto pred, auto&& v)
                {
                    return hana::if_(
                        hana::not_(pred),
                        pred,
                        path_types_equal(
                            hana::front(v),
                            hana::back(v)
                        )
                    );
                }
            );
        }
    );
}

/**
  * @brief Check if members have paths of the same types.
  * @param member1 First member.
  * @param Second member.
  * @return Result of check operation.
  */
template <typename Tm1, typename Tm2>
auto same_member_path_types(const Tm1& member1,const Tm2& member2)
{
    return same_path_types(member1.path(),member2.path());
}

//-------------------------------------------------------------

/**
 * @brief Check if two paths are equal.
 * @param path1 First path.
 * @param path2 Second path.
 * @return Result of comparison.
 */
template <typename T1, typename T2>
bool paths_equal(const T1& path1, const T2& path2)
{
    return hana::eval_if(
        same_path_types(path1,path2),
        [&](auto&& _)
        {
            auto pairs=hana::zip(_(path1),_(path2));
            return while_each(
                                  pairs,
                                  predicate_and,
                                  true,
                                  [](auto&& pair)
                                  {
                                    return safe_compare_equal(unwrap_object(hana::front(pair)),unwrap_object(hana::back(pair)));
                                  }
                              );
        },
        [&](auto&&)
        {
            return false;
        }
    );
}

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_CHECK_MEMBER_PATH_HPP
