#undef NDEBUG

#include <dracosha/validator/validator.hpp>
#include <dracosha/validator/validate.hpp>
#include <dracosha/validator/operators/operator.hpp>
using namespace DRACOSHA_VALIDATOR_NAMESPACE;

// new operator implementation
struct simple_eq_t : public op<simple_eq_t>
{
    // comparison operator with two templated arguments
    template <typename T1, typename T2>
    constexpr bool operator() (const T1& a, const T2& b) const
    {
        return a==b;
    }

    // direct error description
    constexpr static const char* description="must be simply equal to";
    // negated error description
    constexpr static const char* n_description="must be not simply equal to";
};
// callable object to be used in validators
constexpr simple_eq_t simple_eq{};

int main()
{
    // define validator with custom operator
    auto v1=validator(simple_eq,100);

    error_report err;

    // validate good value
    validate(100,v1,err);
    assert(!err);

    // validate bad value
    validate(90,v1,err);
    assert(err);
    std::cerr << err.message() << std::endl;
    assert(err.message()==std::string("must be simply equal to 100"));

    std::cout << "Example 17 done" << std::endl;
    return 0;
}
