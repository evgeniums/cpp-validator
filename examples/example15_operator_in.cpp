#undef NDEBUG

#include <dracosha/validator/validator.hpp>
#include <dracosha/validator/operators.hpp>
#include <dracosha/validator/range.hpp>
#include <dracosha/validator/interval.hpp>
using namespace DRACOSHA_VALIDATOR_NAMESPACE;

int main()
{
    // define value for validation
    size_t val=90;

    // check if the value is in range
    auto v1=validator(in,range({70,80,90,100}));
    assert(v1.apply(val));

    // check if the value is in interval
    auto v2=validator(in,interval(80,100));
    assert(v2.apply(val));

    std::cout << "Example 15 done" << std::endl;
    return 0;
}
