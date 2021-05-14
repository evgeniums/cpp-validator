#undef NDEBUG

#include <dracosha/validator/value_transformer.hpp>
#include <dracosha/validator/validator.hpp>

using namespace DRACOSHA_VALIDATOR_NAMESPACE;

namespace {
// define handler that returns a size of provided string
size_t string_size(const std::string& val) noexcept
{
    return val.size();
}
}

int main()
{
    // create value transforming property
    auto transformer=make_value_transformer(string_size,"string size");

    // value transformer with member notation
    auto v1=validator(
            _[transformer](gte,5)
        );
    // value transformer with property notation
    auto v2=validator(
            transformer(gte,5)
        );

    // validate string that satisfies validation conditions
    std::string s1("Hello world");
    assert(v1.apply(s1));
    assert(v2.apply(s1));

    // validate string that does not satisfy validation conditions
    std::string s2("Hi");
    assert(!v1.apply(s2));
    assert(!v2.apply(s2));

    std::cout << "Example 25 done" << std::endl;
    return 0;
}
