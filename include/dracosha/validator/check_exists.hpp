/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/check_exists.hpp
*
*  Defines helpers for checking existance of member in an object.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_CHECK_EXISTS_HPP
#define DRACOSHA_VALIDATOR_CHECK_EXISTS_HPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/ignore_compiler_warnings.hpp>
#include <dracosha/validator/get.hpp>
#include <dracosha/validator/get_member.hpp>
#include <dracosha/validator/check_member_path.hpp>
#include <dracosha/validator/check_contains.hpp>
#include <dracosha/validator/utils/hana_to_std_tuple.hpp>
#include <dracosha/validator/utils/optional.hpp>
#include <dracosha/validator/utils/reference_wrapper.hpp>
#include <dracosha/validator/utils/conditional_fold.hpp>
#include <dracosha/validator/utils/unwrap_object.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------
DCS_IGNORE_MAYBE_UNINITIALIZED_BEGIN
struct try_get_member_t
{
    template <typename Tobj, typename Tkey>
    constexpr auto operator () (Tobj&& obj_wrapper, Tkey&& key) const
    {
        using next_type=decltype(get(extract_ref(*obj_wrapper),std::forward<decltype(key)>(key)));

        // optional references must be wrapped with cref()
        return hana::if_(
            std::is_lvalue_reference<next_type>{},
            [](auto&& obj_wrapper, auto&& key)
            {
                using return_type=optional<decltype(cref(get(extract_ref(*obj_wrapper),std::forward<decltype(key)>(key))))>;
                if (obj_wrapper.has_value())
                {
                    auto&& obj=extract_ref(*obj_wrapper);
                    if (check_contains(obj,key))
                    {
                        auto ret=return_type{cref(get(obj,std::forward<decltype(key)>(key)))};
                        return ret;
                    }
                }
                return return_type{};
            },
            [](auto&& obj_wrapper, auto&& key)
            {
                using return_type=optional<decltype(get(extract_ref(*obj_wrapper),std::forward<decltype(key)>(key)))>;
                if (obj_wrapper.has_value())
                {
                    auto&& obj=extract_ref(*obj_wrapper);
                    if (check_contains(obj,key))
                    {
                        auto ret=return_type{get(obj,std::forward<decltype(key)>(key))};
                        return ret;
                    }
                }
                return return_type{};
            }
        )(std::forward<Tobj>(obj_wrapper),std::forward<Tkey>(key));
    }
};
DCS_IGNORE_MAYBE_UNINITIALIZED_END

constexpr try_get_member_t try_get_member{};

/**
  @brief Check if member at a given path exists in the object.
  @param object Object under validation.
  @param path Member path as a tuple.
  @return Validation status.

  This operation is performed at runtime.
*/
template <typename Tobj, typename Tpath>
bool check_exists(Tobj&& object, Tpath&& path)
{
    auto&& obj=unwrap_object(object);
    auto ret=hana::if_(
         hana_tuple_empty<Tpath>{},
         [](auto&&, auto&&)
         {
            // empty path means object itself
            return optional<bool>{true};
         },
         [](auto&& obj, auto&& path)
         {
            return hana::if_(
                is_member_path_valid(obj,path),
                [](auto&& obj, auto&& path)
                {
                    auto handler=[&](auto&& default_ret)
                    {
                        // iterate over each level in the path
                        auto wrap_obj=cref(std::forward<decltype(obj)>(obj));
                        using wrap_type=optional<decltype(wrap_obj)>;
                        auto init=wrap_type{std::move(wrap_obj)};
                        return while_each(
                            path,
                            [](auto&& res)
                            {
                                return res.has_value();
                            },
                            init,
                            init,
                            default_ret,
                            try_get_member
                        );
                    };

                    // optional references must be wrapped with cref()
                    using value_type=decltype(get_member_inst(obj,path));
                    return hana::eval_if(
                        std::is_lvalue_reference<value_type>{},
                        [&](auto&& _)
                        {
                            return _(handler)(optional<decltype(cref(get_member_inst(_(obj),_(path))))>{});
                        },
                        [&](auto&& _)
                        {
                            return _(handler)(optional<decltype(get_member_inst(_(obj),_(path)))>{});
                        }
                    );
                },
                [](auto&&, auto&&)
                {
                    return optional<bool>{};
                }
            )(std::forward<decltype(obj)>(obj),std::forward<decltype(path)>(path));
         }
    )(obj,std::forward<decltype(path)>(path));

    // if not exists then return false
    if (!ret.has_value())
    {
        return false;
    }

    // if exists then check if it is a nullptr
    return hana::eval_if(
        is_pointer(extract_ref(ret.value())),
        [&](auto&& _)
        {
            return static_cast<bool>(extract_ref(_(ret).value()));
        },
        [](auto&&)
        {
            return true;
        }
    );
}

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_CHECK_EXISTS_HPP
