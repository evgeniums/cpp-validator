#undef NDEBUG

#include <hatn/validator/property.hpp>
#include <hatn/validator/validator.hpp>
using namespace HATN_VALIDATOR_NAMESPACE;

// structure with two properties
struct Foo
{
    // member variable
    std::string var1;

    // getter method
    uint32_t get_var2() const
    {
        return _var2;
    }

    private:

        uint32_t _var2=1000;
};

// define property corresponding to member variable
HATN_VALIDATOR_PROPERTY(var1);

// define property corresponding to getter method
HATN_VALIDATOR_PROPERTY(get_var2);

int main()
{
    // validator with custom properties
    auto v=validator(
        var1(ne,"unknown"),
        get_var2(gte,100)
    );

    // valiate object
    Foo foo_instance;
    assert(v.apply(foo_instance));

    std::cout << "Example 9 done" << std::endl;
    return 0;
}
