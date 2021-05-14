#undef NDEBUG

#include <map>
#include <set>

#include <dracosha/validator/validator.hpp>
using namespace DRACOSHA_VALIDATOR_NAMESPACE;

int main()
{
    // plain validator
    auto validator_of_sets=validator(
        _["level2"](exists,true)
    );
    // nested validator
    auto validator_of_maps_of_sets=validator(
        _["level1"](validator_of_sets)
    );

    // apply validator to nested container that satisfies validation conditions
    std::map<std::string,std::set<std::string>> map_of_sets1{
        {"level1",{"level2"}}
    };
    assert(validator_of_maps_of_sets.apply(map_of_sets1));

    // apply validator to nested container that does not satisfy validation conditions
    std::map<std::string,std::set<std::string>> map_of_sets2{
        {"level1",{"level2_1"}}
    };
    assert(!validator_of_maps_of_sets.apply(map_of_sets2));

    std::cout << "Example 6 done" << std::endl;
    return 0;
}
