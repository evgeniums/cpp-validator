#undef NDEBUG

#include <hatn/validator/variadic_property.hpp>
#include <hatn/validator/validator.hpp>
using namespace HATN_VALIDATOR_NAMESPACE;

// sample struct
struct Matrix
{
    size_t element(size_t i, size_t j) const noexcept
    {
        return i+j;
    }

    size_t count_i() const noexcept
    {
        return 1000;
    }

    size_t count_j() const noexcept
    {
        return 100;
    }
};

// define variadic property
HATN_VALIDATOR_VARIADIC_PROPERTY(element)

// define "end" properties
HATN_VALIDATOR_PROPERTY(count_i)
HATN_VALIDATOR_PROPERTY(count_j)

int main()
{
    Matrix m1;

    // check if each element is greater than 100 - fails
    auto v1=validator(
        _[element][varg(ALL,count_i)][varg(ALL,count_j)](gt,100)
    );
    assert(!v1.apply(m1));

    // check if at least one element is greater than 100 - succeeds
    auto v2=validator(
        _[element][varg(ANY,count_i)][varg(ANY,count_j)](gt,100)
    );
    assert(v2.apply(m1));

    std::cout << "Example 14 done" << std::endl;
    return 0;
}
