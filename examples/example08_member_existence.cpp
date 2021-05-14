#undef NDEBUG

#include <map>
#include <dracosha/validator/validator.hpp>
using namespace DRACOSHA_VALIDATOR_NAMESPACE;

int main()
{
    std::map<std::string,int> m1={
            {"field1",10},
            {"field3",100}
        };

    // adapter 1 aborts validation if some member is not found
    auto a1=make_default_adapter(m1);
    a1.set_check_member_exists_before_validation(true);
    a1.set_unknown_member_mode(if_member_not_found::abort);

    // adapter 2 ignores not found members
    auto a2=make_default_adapter(m1);
    a2.set_check_member_exists_before_validation(true);
    a2.set_unknown_member_mode(if_member_not_found::ignore);

    // both members exist in the map
    auto v1=validator(
                _["field1"](gte,9),
                _["field3"](eq,100)
            );
    // validation succeeded with both adapters
    assert(v1.apply(a1));
    assert(v1.apply(a2));

    // member "field2" doesn't exist in the map
    auto v2=validator(
                _["field1"](eq,10),
                _["field2"](lt,1000)
            );
    // validation failed with adapter 1
    assert(!v2.apply(a1));
    // validation succeeded with adapter 2
    assert(v2.apply(a2));

    std::cout << "Example 8 done" << std::endl;
    return 0;
}
