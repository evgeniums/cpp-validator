#undef NDEBUG

#include <map>
#include <set>

#include <dracosha/validator/validator.hpp>
#include <dracosha/validator/adapters/default_adapter.hpp>
#include <dracosha/validator/filter_path.hpp>

using namespace DRACOSHA_VALIDATOR_NAMESPACE;

int main()
{
    // original validator
    auto v1=validator(
        _["level1"]["field1"](exists,true),
        _["level1"]["field2"](exists,true),
        _["level2"]["field3"](exists,true)
    );

    // container to validate
    std::map<std::string,std::set<std::string>> m1{
        {"level1",{"field1","field2"}},
        {"level2",{"field4"}},
    };
    // wrap container into default adapter
    auto a1=make_default_adapter(m1);

    // validation fails with original validator
    assert(!v1.apply(a1));

    // limit validation paths to a single path
    auto a2=include_paths(a1,
                member_path_list(_["level1"])
            );
    // validation succeeded with partial validation
    assert(v1.apply(a2));

    // exclude failing path from validation
    auto a3=exclude_paths(a1,
                member_path_list(_["level2"]["field3"])
            );
    // validation succeeded
    assert(v1.apply(a3));

    // include paths for validation and exclude failing sub-path
    auto a4=include_and_exclude_paths(a1,
                member_path_list(_["level1"],_["level2"]),
                member_path_list(_["level2"]["field3"])
            );
    // validation succeeded
    assert(v1.apply(a4));

    std::cout << "Example 24 done" << std::endl;
    return 0;
}
