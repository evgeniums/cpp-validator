#undef NDEBUG

#include <map>
#include <dracosha/validator/validator.hpp>
#include <dracosha/validator/operators.hpp>
using namespace DRACOSHA_VALIDATOR_NAMESPACE;

int main()
{
    // container to validate
    std::map<std::string,uint32_t> m1={{"key1",100}};

    // validator with original operator
    auto v1=validator(
        _["key1"](eq,100)
    );
    assert(v1.apply(m1));

    // validator with negated operator
    auto v2=validator(
        _["key1"](_n(eq),100)
    );
    assert(!v2.apply(m1));

    std::cout << "Example 16 done" << std::endl;
    return 0;
}
