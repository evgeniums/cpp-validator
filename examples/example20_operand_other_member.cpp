#undef NDEBUG

#include <map>
#include <dracosha/validator/validator.hpp>
using namespace DRACOSHA_VALIDATOR_NAMESPACE;

int main()
{
    // validator to check if "password" is equal to "check_password"
    auto v1=validator(
        _["password"](eq,_["check_password"])
    );

    // map to validate, just for example - NEVER use such passwords!
    std::map<std::string,std::string> m1={
            {"password","12345678"},
            {"check_password","12345678"}
        };

    // validate map
    assert(v1.apply(m1));

    std::cout << "Example 20 done" << std::endl;
    return 0;
}
