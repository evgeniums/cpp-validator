#undef NDEBUG

#include <dracosha/validator/validator.hpp>
#include <dracosha/validator/validate.hpp>
#include <dracosha/validator/operators/lexicographical.hpp>
#include <dracosha/validator/prevalidation/set_validated.hpp>

// define structure with member variables and member setter method
struct Foo
{
    std::string bar_value;

    uint32_t other_value;
    size_t some_size;

    void set_bar_value(std::string val)
    {
        bar_value=std::move(val);
    }
};

// define custom properties
DRACOSHA_VALIDATOR_PROPERTY(bar_value);
DRACOSHA_VALIDATOR_PROPERTY(other_value);

// template specialization for setting bar_value member of Foo
DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

template <>
struct set_member_t<Foo,DRACOSHA_VALIDATOR_PROPERTY_TYPE(bar_value)>
{
    template <typename ObjectT, typename MemberT, typename ValueT>
    void operator() (
            ObjectT& obj,
            MemberT&&,
            ValueT&& val
        ) const
    {
        obj.set_bar_value(std::forward<ValueT>(val));
    }
};

DRACOSHA_VALIDATOR_NAMESPACE_END

using namespace DRACOSHA_VALIDATOR_NAMESPACE;

int main()
{
    // define validator of custom properties
    auto v=validator(
        _[bar_value](ilex_ne,"UNKNOWN"), // case insensitive lexicographical not equal
        _[other_value](gte,1000)
    );

    Foo foo_instance;

    error_report err;

    // call setter with valid data
    set_validated(foo_instance,_[bar_value],"Hello world",v,err);
    assert(!err);

    // call setter with invalid data
    set_validated(foo_instance,_[bar_value],"unknown",v,err);
    assert(err);
    if (err)
    {
        // object's member is not set
        std::cerr << err.message() << std::endl;
        assert(err.message()==std::string("bar_value must be not equal to UNKNOWN"));
    }

    std::cout << "Example 26 done" << std::endl;
    return 0;
}
