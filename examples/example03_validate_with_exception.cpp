#undef NDEBUG

#include <dracosha/validator/validator.hpp>
#include <dracosha/validator/validate.hpp>
using namespace DRACOSHA_VALIDATOR_NAMESPACE;

int main()
{
    // define validator
    auto v=validator(gt,100);

    // validate good value
    try
    {
        validate(200,v);
        std::cout << "Validation for 200 succeeded" << std::endl;
    }
    catch (const validation_error&)
    {
    }

    // validate bad value
    try
    {
        validate(90,v);
        std::cout << "Never reaches this point" << std::endl;
        assert(false);
    }
    catch (const validation_error& err)
    {
        std::cerr << "Validation for 90 failed: " << err.what() << std::endl;
    }

    std::cout << "Example 3 done" << std::endl;
    return 0;
}
