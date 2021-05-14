#undef NDEBUG

#include <dracosha/validator/variadic_property.hpp>
#include <dracosha/validator/validator.hpp>
#include <dracosha/validator/adapters/reporting_adapter.hpp>
using namespace DRACOSHA_VALIDATOR_NAMESPACE;

// define a structure with variadic properties
struct WithChild
{
    // method with one argument
    int child(int val) const noexcept
    {
        return val+1;
    }

    // method with two arguments
    int child_word(int val, const std::string& word) const noexcept
    {
        return val+static_cast<int>(word.size());
    }

    // method to check if object "has" child_world with provided arguments
    bool has_child_word(int val, const std::string& word) const noexcept
    {
        return val>10 && word.size()>=5;
    }
};

// define variadic property
DRACOSHA_VALIDATOR_VARIADIC_PROPERTY(child)
// define variadic property to be used as "exists" checker
DRACOSHA_VALIDATOR_VARIADIC_PROPERTY(has_child_word)
// define variadic property with "exists" checker
DRACOSHA_VALIDATOR_VARIADIC_PROPERTY_HAS(child_word,has_child_word)

int main()
{
    // define object
    WithChild o1;

    // define validation adapter with text reports
    std::string rep;
    auto ra1=make_reporting_adapter(o1,rep);

    // variadic property with single argument and member notation
    auto v1=validator(
        _[child][varg(1)](eq,30)
    );
    assert(!v1.apply(ra1));
    assert(rep==std::string("child(1) must be equal to 30"));
    rep.clear();

    // variadic property with single argument and property notation
    auto v2=validator(
        child(1)(eq,30)
    );
    assert(!v2.apply(ra1));
    assert(rep==std::string("child(1) must be equal to 30"));
    rep.clear();

    // variadic property with two arguments and member notation
    auto v3=validator(
        _[child_word][varg(20)][varg("hello")](eq,30)
    );
    assert(!v3.apply(ra1));
    assert(rep==std::string("child_word(20,hello) must be equal to 30"));
    rep.clear();

    // variadic property with two arguments and property notation
    auto v4=validator(
        child_word(20,"hello")(eq,30)
    );
    assert(!v4.apply(ra1));
    assert(rep==std::string("child_word(20,hello) must be equal to 30"));
    rep.clear();

    // check if variadic property exists
    auto v5=validator(
        _[child_word][20]["hello"](exists,true)
    );
    assert(v5.apply(o1));
    auto v6=validator(
        _[child_word][5]["hello"](exists,true)
    );
    assert(!v6.apply(o1));

    std::cout << "Example 13 done" << std::endl;
    return 0;
}
