#undef NDEBUG

#include <dracosha/validator/validator.hpp>
#include <dracosha/validator/prevalidation/set_validated.hpp>

using namespace DRACOSHA_VALIDATOR_NAMESPACE;

DRACOSHA_VALIDATOR_PROPERTY(field1)

int main()
{
    // define validator
    auto v=validator(
        _["field1"](gte,100)
    );

    std::map<std::string,size_t> m1;
    error_report err;

    // set valid value
    set_validated(m1,_["field1"],1000,v,err);
    assert(!err); // success
    assert(m1["field1"]==1000); // was set

    // try to set invalid value
    set_validated(m1,_["field1"],10,v,err);
    assert(err); // fail
    assert(m1["field1"]==1000); // not changed

    std::cout << "Example 26 done" << std::endl;
    return 0;
}
