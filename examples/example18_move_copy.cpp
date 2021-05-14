#undef NDEBUG

#include <map>
#include <dracosha/validator/validator.hpp>
#include <dracosha/validator/utils/copy.hpp>

using namespace DRACOSHA_VALIDATOR_NAMESPACE;

int main ()
{
    std::string key1{"key1"};
    std::string value1{"value1"};
    // key1 and value1 are passed by reference and must stay valid duiring validator life time
    auto v1=validator(
        _[key1](eq,value1)
    );

    std::string key2{"key2"};
    std::string value2{"value2"};
    // key2 and value2 are explicitly moved to validator and validator becomes responsible for their contents
    auto v2=validator(
        _[std::move(key2)](eq,std::move(value2))
    );

    std::string key3{"key3"};
    std::string value3{"value3"};
    // key3 and value3 are explicitly copied to validator and validator becomes responsible for copies of their contents
    auto v3=validator(
        _[copy(key3)](eq,copy(value3))
    );

    // "key4" and "value4" are implicitly moved to validator and validator is responsible for their contents
    auto v4=validator(
        _["key4"](eq,"value4")
    );

    // apply validators to map
    std::map<std::string,std::string> m1{
        {"key1","value1"},
        {"key2","value2"},
        {"key3","value3"},
        {"key4","value4"}
    };
    assert(v1.apply(m1));
    assert(v2.apply(m1));
    assert(v3.apply(m1));
    assert(v4.apply(m1));

    std::cout << "Example 18 done" << std::endl;
    return 0;
}
