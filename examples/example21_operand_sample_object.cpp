#undef NDEBUG

#include <map>
#include <dracosha/validator/validator.hpp>
using namespace DRACOSHA_VALIDATOR_NAMESPACE;

int main()
{
    // sample container
    std::map<std::string,std::string> sample={
            {"token","12345678"}
        };

    // validator to check if "token" element of object under validation
    // is equal to "token" element of the sample
    auto v1=validator(
        _["token"](eq,_(sample))
    );

    // map to validate
    std::map<std::string,std::string> m1={
            {"token","12345678"}
        };

    // validate map
    assert(v1.apply(m1));

    std::cout << "Example 21 done" << std::endl;
    return 0;
}
