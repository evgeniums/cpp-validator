#undef NDEBUG

#include <dracosha/validator/heterogeneous_property.hpp>
#include <dracosha/validator/validator.hpp>
#include <dracosha/validator/validate.hpp>

using namespace DRACOSHA_VALIDATOR_NAMESPACE;

int main()
{
    // implicit heterogeneous property from std::integral_constant<size_t,CONSTANT>
    auto v1=validator(
        _[std::integral_constant<size_t,1>{}](gt,100)
    );
    // implicit heterogeneous property from hana::size_c<CONSTANT>
    auto v2=validator(
        _[hana::size_c<1>](gt,100)
    );

    error_report err;

    // validate std::tuple
    auto t1=std::make_tuple(200,50,"hello");
    validate(t1,v1,err);
    assert(err);
    assert(err.message()==std::string("element #1 must be greater than 100"));

    // validate hana::tuple
    auto t2=hana::make_tuple(200,50,"hello");
    validate(t2,v2,err);
    assert(err);
    assert(err.message()==std::string("element #1 must be greater than 100"));

    std::cout << "Example 11 done" << std::endl;
    return 0;
}
