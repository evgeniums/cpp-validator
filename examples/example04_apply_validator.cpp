#undef NDEBUG

#include <dracosha/validator/validator.hpp>
using namespace DRACOSHA_VALIDATOR_NAMESPACE;

int main()
{
    // define validator
    auto v=validator(gt,100);

    // validate variable that satisfies validator
    int value1=200;
    auto a1=make_default_adapter(value1);
    assert(v.apply(a1)); // apply to adapter
    assert(v.apply(value1)); // apply to value

    // validate variable that doesn't satisfy validator
    int value2=90;
    auto a2=make_default_adapter(value2);
    assert(!v.apply(a2)); // apply to adapter
    assert(!v.apply(value2)); // apply to value

    std::cout << "Example 4 done" << std::endl;
    return 0;
}
