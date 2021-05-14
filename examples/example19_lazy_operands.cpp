#undef NDEBUG

#include <dracosha/validator/validator.hpp>
#include <dracosha/validator/lazy.hpp>
using namespace DRACOSHA_VALIDATOR_NAMESPACE;

int main()
{
    // define sample getter
    int sample=100;
    auto get_sample=[&sample]()
    {
        return sample;
    };

    // define validator with lazy operand
    auto v1=validator(
                eq,lazy(get_sample)
            );

    // define variable
    int val=100;

    // validate variable with original sample
    assert(v1.apply(val));

    // update sample and validate variable again
    sample=200;
    assert(!v1.apply(val));

    std::cout << "Example 19 done" << std::endl;
    return 0;
}
