#undef NDEBUG

#include <map>
#include <dracosha/validator/validator.hpp>
#include <dracosha/validator/properties/pair.hpp>

using namespace DRACOSHA_VALIDATOR_NAMESPACE;

int main()
{
    std::map<std::string,std::string> m1{
        {"key1","value1"},
        {"key2","value2"},
        {"key3","value3"}
    };
    std::map<std::string,std::string> m2{
        {"key1","val1"},
        {"key2","val2"},
        {"key","val3"}
    };

    // validate keys
    auto v1=validator(
        _[ALL(keys)](gt,"key")
    );
    assert(v1.apply(m1));
    assert(!v1.apply(m2));

    // validate values explicitly
    auto v2=validator(
        _[ALL(values)](gt,"value")
    );
    assert(v2.apply(m1));
    assert(!v2.apply(m2));

    // va;idate values implicitly
    auto v2_1=validator(
        _[ALL](gt,"value")
    );
    assert(v2_1.apply(m1));
    assert(!v2_1.apply(m2));

    // validate first elements of iterators
    auto v3=validator(
        _[ALL(iterators)](first(gt,"key"))
    );
    assert(v3.apply(m1));
    assert(!v3.apply(m2));

    // validate second elements of iterators
    auto v4=validator(
        _[ALL(iterators)](second(gt,"value"))
    );
    assert(v4.apply(m1));
    assert(!v4.apply(m2));

    std::cout << "Example 22 done" << std::endl;
    return 0;
}
