#undef NDEBUG

#include <dracosha/validator/validator.hpp>
#include <dracosha/validator/validate.hpp>
using namespace DRACOSHA_VALIDATOR_NAMESPACE;

int main()
{
    // define validator
    auto v=validator(gt,100);

    // validate variables
    error_report err;

    // validate good value
    validate(90,v,err);
    if (err)
    {
        std::cerr << "Validation for 90 failed: " << err.message() << std::endl;
    }
    assert(err);

    // validate bad value
    validate(200,v,err);
    if (!err)
    {
       std::cout << "Validation for 200 succeeded" << std::endl;
    }
    assert(!err);

    std::cout << "Example 2 done" << std::endl;
    return 0;
}
