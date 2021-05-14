#undef NDEBUG

#include <map>
#include <dracosha/validator/validator.hpp>
#include <dracosha/validator/adapters/reporting_adapter.hpp>
using namespace DRACOSHA_VALIDATOR_NAMESPACE;

int main()
{
    auto v=validator(
                    _["field1"][1](in,range({10,20,30,40,50})),
                    _["field1"][2](lt,100),
                    _["field2"](exists,false),
                    _["field3"](empty(flag,true))
                );

    std::string report;

    std::map<std::string,std::map<size_t,size_t>> nested_map={
                {"field1",{{1,5},{2,50}}},
                {"field3",{}}
            };
    auto ra=make_reporting_adapter(nested_map,report);

    if (!v.apply(ra))
    {
        std::cerr << report << std::endl;
        /* prints:
        "element #1 of field1 must be in range [10, 20, 30, 40, 50]"
        */
    }
    assert(!v.apply(ra));

    std::cout << "Example 7 done" << std::endl;
    return 0;
}
