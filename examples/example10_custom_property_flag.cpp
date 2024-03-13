#undef NDEBUG

#include <iostream>
#include <hatn/validator/property.hpp>
#include <hatn/validator/validator.hpp>
#include <hatn/validator/adapters/reporting_adapter.hpp>
using namespace HATN_VALIDATOR_NAMESPACE;

// structure with red_color() getter method
struct Foo
{
    // boolean getter method
    bool red_color() const
    {
        return true;
    }
};

// define flaggable red_color property
HATN_VALIDATOR_PROPERTY_FLAG(red_color,"Must be red","Must be not red")

int main()
{
    // validator with red_color property
    auto v=validator(
        _[red_color](flag,false)
    );

    // create reporting adapter
    std::string report;
    Foo foo_instance;
    auto ra=make_reporting_adapter(foo_instance,report);

    // apply validator and print report
    if (!v.apply(ra))
    {
        std::cerr << report << std::endl;
        /* prints:

        "Must be not red"

        */
    }
    assert(!v.apply(ra));

    std::cout << "Example 10 done" << std::endl;
    return 0;
}
