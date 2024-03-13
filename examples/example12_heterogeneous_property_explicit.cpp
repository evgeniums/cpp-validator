#undef NDEBUG

#include <hatn/validator/heterogeneous_property.hpp>
#include <hatn/validator/validator.hpp>
#include <hatn/validator/validate.hpp>

using namespace HATN_VALIDATOR_NAMESPACE;

// define property "zero" with index 0
HATN_VALIDATOR_HETEROGENEOUS_PROPERTY(zero,0)
// define property "one" with index 1
HATN_VALIDATOR_HETEROGENEOUS_PROPERTY(one,1)

int main()
{
    // use explicit heterogeneous property "one"
    auto v1=validator(
        _[one](gt,100)
    );
    // use explicit heterogeneous property "zero"
    auto v2=validator(
        _[zero](lt,100)
    );

    error_report err;

    auto t1=std::make_tuple(200,50,"hello");
    validate(t1,v1,err);
    assert(err);
    std::cerr << err.message() << std::endl;
    assert(err.message()==std::string("one must be greater than 100"));

    auto t2=hana::make_tuple(200,50,"hello");
    validate(t2,v2,err);
    assert(err);
    std::cerr << err.message() << std::endl;
    assert(err.message()==std::string("zero must be less than 100"));

    std::cout << "Example 12 done" << std::endl;
    return 0;
}
