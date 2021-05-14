#undef NDEBUG

#include <map>
#include <dracosha/validator/validator.hpp>
using namespace DRACOSHA_VALIDATOR_NAMESPACE;

int main()
{
    // create raw pointer validator
    auto raw_pointer_validator=new_validator(
                    _["field1"](eq,1)
                );

    // create shared pointer validator
    auto shared_pointer_validator=shared_validator(
                    _["field1"](eq,1)
                );

    // validate check_var with validators
    std::map<std::string,size_t> check_var={{"field1",1}};

    assert(raw_pointer_validator->apply(check_var));
    assert(shared_pointer_validator->apply(check_var));

    // delete raw pointer validator
    delete raw_pointer_validator;

    std::cout << "Example 5 done" << std::endl;
    return 0;
}
