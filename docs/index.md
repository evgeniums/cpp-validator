# C++ Data Validation Library (*cpp-validator*)
----
# Table of Contents
[//]: # (TOC Begin)
* [Introduction](#introduction)
* [Definitions](#definitions)
* [Usage](#usage)
	* [Validator construction](#validator-construction)
		* [Components of validator](#components-of-validator)
		* [Basic validator](#basic-validator)
			* [Whole object](#whole-object)
			* [Object's member](#objects-member)
		* [Validator with properties](#validator-with-properties)
			* [Whole object](#whole-object)
			* [Object's member](#objects-member)
		* [Validator with aggregations](#validator-with-aggregations)
			* [Whole object](#whole-object)
			* [Object's member](#objects-member)
			* [Property of object's member](#property-of-objects-member)
			* [Mixed](#mixed)
		* [Dynamically allocated validator](#dynamically-allocated-validator)
		* [Nested validators](#nested-validators)
	* [Using validator for data validation](#using-validator-for-data-validation)
		* [Post-validation](#post-validation)
			* [validate() without report and without exception](#validate-without-report-and-without-exception)
			* [validate() with report but without exception](#validate-with-report-but-without-exception)
			* [validate() with exception](#validate-with-exception)
			* [Apply validator to adapter](#apply-validator-to-adapter)
			* [Apply validator to object](#apply-validator-to-object)
		* [Pre-validation](#pre-validation)
			* [set_validated](#set_validated)
			* [unset_validated](#unset_validated)
			* [resize_validated](#resize_validated)
			* [clear_validated](#clear_validated)
	* [Members](#members)
		* [Member notation](#member-notation)
			* [Single level members](#single-level-members)
			* [Nested members](#nested-members)
		* [Member existence](#member-existence)
	* [Properties](#properties)
		* [Property notations](#property-notations)
		* [Built-in properties](#built-in-properties)
			* [*value*](#value)
			* [*size*](#size)
			* [*length*](#length)
			* [*empty*](#empty)
			* [*h_size*](#h_size)
			* [*first* and *second*](#first-and-second)
		* [Adding new property](#adding-new-property)
		* [Properties of heterogeneous containers](#properties-of-heterogeneous-containers)
			* [Implicit heterogeneous property](#implicit-heterogeneous-property)
			* [Explicit heterogeneous property](#explicit-heterogeneous-property)
		* [Variadic properties](#variadic-properties)
			* [Properties with arguments](#properties-with-arguments)
			* [Variadic properties with aggregations](#variadic-properties-with-aggregations)
	* [Operators](#operators)
		* [Special operators](#special-operators)
			* [*exists*](#exists)
			* [*contains*](#contains)
			* [*flag*](#flag)
			* [*in*](#in)
			* [*nin*](#nin)
			* [*_n* (negation)](#n-negation)
		* [Built-in operators](#built-in-operators)
		* [Adding new operator](#adding-new-operator)
	* [Operands](#operands)
		* [Variables](#variables)
		* [Lazy operands](#lazy-operands)
		* [Other members](#other-members)
		* [Sample objects](#sample-objects)
		* [Intervals](#intervals)
		* [Ranges](#ranges)
	* [Aggregations](#aggregations)
		* [Logical aggregations](#logical-aggregations)
			* [AND](#and)
			* [OR](#or)
			* [NOT](#not)
		* [Element aggregations](#element-aggregations)
			* [ANY](#any)
			* [ALL](#all)
			* [Aggregation modifiers](#aggregation-modifiers)
		* [Validation of trees](#validation-of-trees)
	* [Adapters](#adapters)
		* [Default adapter](#default-adapter)
		* [Prevalidation adapter](#prevalidation-adapter)
			* [Adapter creation and usage](#adapter-creation-and-usage)
			* [Element aggregations with prevalidation adapter](#element-aggregations-with-prevalidation-adapter)
			* [Bulk prevalidation](#bulk-prevalidation)
		* [Adding new adapter](#adding-new-adapter)
	* [Validation of pointers](#validation-of-pointers)
	* [Partial validation](#partial-validation)
	* [Validation of transformed or evaluated values](#validation-of-transformed-or-evaluated-values)
	* [Reporting](#reporting)
		* [Reporting adapter](#reporting-adapter)
		* [Customization of reports](#customization-of-reports)
			* [Report construction](#report-construction)
			* [Formatters](#formatters)
			* [Decorator](#decorator)
		* [Reporting hints](#reporting-hints)
			* [Override entire report](#override-entire-report)
			* [Override description of a member validation condition](#override-description-of-a-member-validation-condition)
			* [Override member name](#override-member-name)
			* [Override operator description](#override-operator-description)
			* [Override operand formatting](#override-operand-formatting)
		* [Localization](#localization)
			* [Grammatical categories and concrete phrases](#grammatical-categories-and-concrete-phrases)
			* [Translators](#translators)
			* [Repository of translators](#repository-of-translators)
			* [Adding new locale](#adding-new-locale)
			* [Localization example](#localization-example)
* [Performance considerations](#performance-considerations)
	* [Validation overhead](#validation-overhead)
	* [Zero copy](#zero-copy)
	* [Checking member existence before validation](#checking-member-existence-before-validation)
	* [Validation with text reports](#validation-with-text-reports)
* [Building and installation](#building-and-installation)
	* [Supported platforms and compilers](#supported-platforms-and-compilers)
	* [Dependencies](#dependencies)
	* [CMake configuration](#cmake-configuration)
	* [Building and running tests and examples](#building-and-running-tests-and-examples)
* [License](#license)
* [Contributing](#contributing)

[//]: # (TOC End)

----

# Introduction

`cpp-validator` is a modern C++ header-only library for validation of variables, objects and containers.

`cpp-validator` can be used to validate:
- plain variables;
- properties of objects, where a property can be accessed either as object's variable or object's method;
- contents and properties of containers;
- nested containers and objects;
- heterogeneous containers such as pairs and tuples;
- trees;
- transformed values or results of evaluations.

Basic usage of the library includes a few steps:

- first, define a [validator](#validator) using almost declarative syntax;
- then, apply the [validator](#validator) to [object](#object) that must be validated;
- finally, check the results and print [report](#report) if applicable.

The library is suitable for both *post-validation* and *pre-validation*. *Post-validation* stands for validating the object that is already populated with the data. *Pre-validation* stands for validating the data before writing it to the object. The same validator declaration can be used in both cases.

# Definitions

##### *Adapter*
Customizable validation implementer. Adapter performs actual validation when the [validator](#validator) is applied to an [object](#object).

##### *Aggregation*
Combination of validating [operators](#operator) or other [validators](#validator) that make up a compound [validator](#validator). There are logical aggregations such as _AND_, _OR_, _NOT_ and [element](#element) aggregations such as _ANY_, _ALL_.

##### *Element*
A special type of [member](#member) that points to an element of container when the [object](#object) is a container.

##### *Formatter*
Customizable implementer of [reports](#report) formatting. See [formatters](#formatters) section.

##### *Member*
An [element](#element) or a [property](#property) of [object](#object) the validation must be applied to. Members can be either direct (single level depth) or nested (multi level depth).

##### *Object* 
Variable that must be validated. It can be either scalar variable, C++ object or container.

##### *Sample object* 
Sample variable whose [member](#member) must be used as [operand](#operand).

##### *Operand*
Variable or value used as a validation sample in [operator](#operator).

##### *Operator*
Atomic validation condition.

##### *Property*
Method or member variable of C++ class or structure that must be validated, see [Properties](#properties).

##### *Report*
String description of validation error.

##### *Reporter*
Customizable [constructor](#report-construction) of [reports](#report).

##### *Translator*
Translates strings of a [report](#report) to certain language.

##### *Translator repository*
Repository of [translators](#translator) for various languages and locales.

##### *Validator*
Bundle of validation conditions described with [operators](#operator) and [aggregations](#aggregation). See [Validator construction](#validator-construction) for details.

# Usage

## Validator construction

### Components of validator

To construct a validator one should describe validation conditions using five groups of components.

1. Part(s) of an [object](#object) the validation will be applied to, which can be:
    * whole object;
    * [member(s)](#member) of the object.

2. [Property](#property) of selected object's part that must be verified. By default a special pseudo property *value* is used which means that validation must be applied to the variable itself. The library provides a few other predefined properties such as *size*, *length* and *empty*. Custom properties can also be defined, see [Properties](#properties).

3. [Operator(s)](#operator) that must be used for validation.

4. [Operand(s)](#operand) used as validation sample(s) in [operator(s)](#operator).

5. Modes of aggregation:
    * plain validation without any aggregation when only single [operator](#operator) is used;
    * compound validation with [aggregation(s)](#aggregation).

### Basic validator

#### Whole object

```cpp
#include <dracosha/validator/validator.hpp>
using namespace DRACOSHA_VALIDATOR_NAMESPACE;

auto v=validator(gt,100);
```
The example above defines validation condition "variable must be greater than 100" where *gt* is an [operator](#operator) and *100* is an [operand](#operand).

#### Object's member

```cpp
#include <dracosha/validator/validator.hpp>
using namespace DRACOSHA_VALIDATOR_NAMESPACE;

auto v=validator(
        _["field1"](gt,100)
    );
```
The example above defines validation condition "field1 of variable must be greater than 100" where *"field1"* is object's [member](#member), *gt* is an [operator](#operator) and *100* is an [operand](#operand).

### Validator with properties

#### Whole object

```cpp
#include <dracosha/validator/validator.hpp>
using namespace DRACOSHA_VALIDATOR_NAMESPACE;

// property notation
auto v1=validator(
        size(gt,100)
    );

// member notation
auto v2=validator(
        _[size](gt,100)
    );
```
Validators *v1* and *v2* in the example above both define validation condition "size of variable must be greater than 100" where *size* is a [property](#property), *gt* is an [operator](#operator) and *100* is an [operand](#operand). The first validator is described using property notation and the second validator is described using member notation.

#### Object's member

```cpp
#include <dracosha/validator/validator.hpp>
using namespace DRACOSHA_VALIDATOR_NAMESPACE;

// property notation
auto v1=validator(
        _["field1"](size(gt,100))
    );

// member notation
auto v2=validator(
        _["field1"][size](gt,100)
    );
```
Validators *v1* and *v2* in the example above both define validation condition "size of field1 of variable must be greater than 100" where *field1* is a member of variable, *size* is a [property](#property) of *field1*, *gt* is an [operator](#operator) and *100* is an [operand](#operand). The first validator is described using property notation and the second validator is described using member notation.

### Validator with aggregations

#### Whole object

```cpp
#include <dracosha/validator/validator.hpp>
using namespace DRACOSHA_VALIDATOR_NAMESPACE;

// implicit AND
auto v1=validator(
        value(gt,100),
        value(lt,200)
    );

// explicit AND
auto v2=validator(
        value(gt,100) ^AND^ value(lt,200)
    );
```
The example above defines validation condition "variable must be greater than 100 AND less than 200" where *value* is a pseudo property meaning variable itself, *gt* and *lt* are [operators](#operator), *100* and *200* are [operands](#operand), and *\^AND\^* is [aggregation](#aggregation).

#### Object's member

```cpp
#include <dracosha/validator/validator.hpp>
using namespace DRACOSHA_VALIDATOR_NAMESPACE;

auto v=validator(
        _["field1"](
                value(gt,100) ^AND^ value(lt,200)
            )
    );
```
The example above defines validation condition "field1 of variable must be greater than 100 AND less than 200" where *field1* is a member of variable, *value* is a pseudo [property](#property) standing for member *field1* of variable, *gt* and *lt* are [operators](#operator), *100* and *200* are [operands](#operand), and *\^AND\^* is [aggregation](#aggregation).

#### Property of object's member

```cpp
#include <dracosha/validator/validator.hpp>
using namespace DRACOSHA_VALIDATOR_NAMESPACE;

auto v=validator(
        _["field1"](
                value(eq,"value1") ^OR^ size(lt,3)
            )
    );
```
The example above defines validation condition "field1 of variable must be equal to "value1" OR size of field1 of variable is less than 3" where *field1* is a member of variable, *value* is a pseudo [property](#property) standing for member *field1* of variable, *size* is a [property](#property) of member *field1*, *eq* and *lt* are [operators](#operator), *"value1"* and *3* are [operands](#operand), and *\^OR\^* is [aggregation](#aggregation).

#### Mixed

```cpp
#include <dracosha/validator/validator.hpp>
using namespace DRACOSHA_VALIDATOR_NAMESPACE;

// implicit AND
auto v1=validator(
        _[size](ne,10),
        _["field1"](
                value(eq,"value1") ^OR^ size(lt,3)
            )
    );

// explicit AND    
auto v2=validator(
        _[size](ne,10)
        ^AND^
        _["field1"](
                value(eq,"value1") ^OR^ size(lt,3)
            )
    );
```
The examples above define validation condition "size of variable is not equal to 3 AND field1 of variable must be equal to "value1" OR size of field1 of variable is less than 3" where *field1* is a member of variable, *value* is a pseudo [property](#property) standing for member *field1* of variable, *size* is a [property](#property), *ne*, *eq* and *lt* are [operators](#operator), *3*, *10*, *"value1"* are [operands](#operand), *\^OR\^* and *\^AND\^* are [aggregations](#aggregation).

### Dynamically allocated validator

Validators can be dynamically allocated on the memory heap. There are two forms of dynamically allocated validators:
- raw pointer variables created with `new_validator()` helper;
- shared pointer variables created with `shared_validator()` helper.

See examples below.

```cpp
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
    
    return 0;
}
```

### Nested validators

Once defined validator can be reused by other validators. For example, if there is already a validator that validates objects of certain type this validator can be used within other validators for validation of containers of objects of that type.

```cpp
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

    return 0;    
}
```

## Using validator for data validation

### Post-validation

*Post-validation* here stands for validating the object that is already populated with the data. The simplest way to validate an [object](#object) is to use `validate()` helper. There are forms of `validate()` with *exceptions* and without *exceptions*, with [report](#report) and without [report](#report).

Also validation can be performed by calling `apply()` method of [validator](#validator), see the next sections.

#### validate() without report and without exception

```cpp
#include <dracosha/validator/validator.hpp>
#include <dracosha/validator/validate.hpp>
using namespace DRACOSHA_VALIDATOR_NAMESPACE;

int main()
{

// define validator
auto v=validator(gt,100);

// validate variables
error err;

validate(90,v,err);
if (err)
{
  // validation failed
}

validate(200,v,err);
if (!err)
{
  // validation succeeded
}

return 0;
}
```

#### validate() with report but without exception

```cpp
#include <dracosha/validator/validator.hpp>
#include <dracosha/validator/validate.hpp>
using namespace DRACOSHA_VALIDATOR_NAMESPACE;

int main()
{

// define validator
auto v=validator(gt,100);

// validate variables
error_report err;

validate(90,v,err);
if (err)
{
  // validation failed
    std::cerr << err.message() << std::endl;
    /* prints:
    "must be greater than 100"
    */
}

validate(200,v,err);
if (!err)
{
  // validation succeeded
}

return 0;
}
```

#### validate() with exception

```cpp
#include <dracosha/validator/validator.hpp>
#include <dracosha/validator/validate.hpp>
using namespace DRACOSHA_VALIDATOR_NAMESPACE;

int main()
{

// define validator
auto v=validator(gt,100);

// validate variables

try
{
    validate(200,v); // succeed
    validate(90,v); // throw
}
catch (const validation_error& err)
{
    std::cerr << err.what() << std::endl;
    /* prints:
    
    "must be greater than 100"
    
    */
}

return 0;
}
```

#### Apply validator to adapter

Data validation is performed by [adapters](#adapter). When a [validator](#validator) is applied to an [adapter](#adapter) the [adapter](#adapter) *reads* validation conditions from the [validator](#validator) and processes them depending on [adapter](#adapter) implementation. See more about adapters in [Adapters](#adapters) section.
```cpp
#include <dracosha/validator/validator.hpp>
using namespace DRACOSHA_VALIDATOR_NAMESPACE;

int main()
{

// define validator
auto v=validator(gt,100);

// validate variable that satisfies validator
int value1=200;
auto a1=make_default_adapter(value1);
if (v.apply(a1))
{
  // validation succeeded
}

// validate variable that doesn't satisfy validator
int value2=90;
auto a2=make_default_adapter(value2);
if (!v.apply(a2))
{
  // validation failed
}

return 0;
}
```

#### Apply validator to object

A [validator](#validator) can be applied directly to a variable that must be validated. In this case a [default adapter](#default-adapter) will be used implicitly.
```cpp
#include <dracosha/validator/validator.hpp>
using namespace DRACOSHA_VALIDATOR_NAMESPACE;

int main()
{

// define validator
auto v=validator(gt,100);

// validate variable that satisfies validator
int value1=200;
if (v.apply(value1))
{
  // validation succeeded
}

// validate variable that doesn't satisfy validator
int value2=90;
if (!v.apply(value2))
{
  // validation failed
}

return 0;
}
```

### Pre-validation

*Pre-validation* here stands for validating data before updating the target object. To customize data *pre-validation* use [prevalidation adapter](#prevalidation-adapter). The library already implements a few pre-validation helpers:

- `set_validated` - validate variable and write it to a member of the target object;
- `unset_validated` - validate if a member can be unset and remove it from the target object;
- `resize_validated` - validate a member's size and resize the member in the target object;
- `clear_validated` - validate if a member can be cleared and clear it in the target object.

#### set_validated

Default implementation of `set_validated` uses square brackets operator to set a member as an element of a container.

```cpp
#include <map>
#include <dracosha/validator/validator.hpp>
#include <dracosha/validator/prevalidation/set_validated.hpp>

DRACOSHA_VALIDATOR_PROPERTY(field1)

int main()
{
    // define validator
    auto v=validator(
        _["field1"](gte,100)
    );

    std::map<std::string,size_t> m1;
    error_report err;
    
    // set valid value
    set_validated(m1,_["field1"],1000,v,err);
    assert(!err); // success
    assert(m1["field1"]==1000); // was set

    // try to set invalid value
    set_validated(m1,_["field1"],10,v,err);
    assert(err); // fail
    assert(m1["field1"]==1000); // not changed
    
    return 0;
}    
```

To use `set_validated` with custom [properties](#property) a template specialization of property setter `set_member_t` in `DRACOSHA_VALIDATOR_NAMESPACE` must be defined first. `set_validated` can be used both with and without exceptions.

See examples below.

```cpp
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

```

#### unset_validated

Default implementation of `unset_validated` uses `erase()` method of container. Before unsetting a member the validator checks [exists](#exists) and [contains](#contains) operators.
To use `unset_validator` with custom types a template specialization of `unset_member_t` must be defined. See example in `validator/prevalidation/unset_validated.hpp` header.

```cpp
#include <map>
#include <dracosha/validator/validator.hpp>
#include <dracosha/validator/prevalidation/unset_validated.hpp>

using namespace DRACOSHA_VALIDATOR_NAMESPACE;

int main()
{
    error_report err;

    // define validator
    auto v1=validator(
                _["field1"](exists,true),
                _["field1"](eq,100)
            );
            
    // define container for validation
    std::map<std::string,std::string> m1{
        {"field1","value1"},
        {"field2","value2"},
        {"field3","value3"}
    };

    // try to unset element
    unset_validated(m1,_["field1"],v1,err);
    assert(err); // fail
    assert(err.message()==std::string("field1 must exist"));
    assert(m1.find("field1")!=m1.end()); // element was not unset
    
    return 0;
}
```

#### resize_validated

Default implementation of `resize_validated` uses `resize()` method of a member. To use `resize_validated` with custom types a template specialization of `resize_member_t` must be defined. See example in `validator/prevalidation/resize_validated.hpp` header.

Note that only [size](#size), [length](#length) and [empty](#empty) properties as well as [comparison](builtin_operators.md#comparison-operators) and [lexicographical](builtin_operators.md#lexicographical-operators) operators are validated before this operation. If other operators are used to validate content, then they are not checked. For example, resizing string with `validator(_[string_field](regex_match,"Hello world!"))` will not emit error even in case the validation condition is not met. 

```cpp
#include <map>
#include <dracosha/validator/validator.hpp>
#include <dracosha/validator/prevalidation/resize_validated.hpp>

using namespace DRACOSHA_VALIDATOR_NAMESPACE;

int main()
{
    error_report err;
    
    // define validator
    auto v1=validator(
                _["field1"](empty(flag,false)),
                _["field2"](size(gte,5) ^AND^ value(ne,"Invalid value")),
                _["field5"](length(gte,4)),
                _["field6"](NOT(length(lt,4)))
            );

    // define container for validation
    std::map<std::string,std::string> m1{
        {"field1","value1"},
        {"field2","value2"},
        {"field3","value3"},
        {"field4","value4"},
        {"field5","value5"},
        {"field6","value6"}
    };

    // try to resize element
    resize_validated(m1,_["field1"],0,v1,err);
    assert(err); // fail
    assert(err.message()==std::string("field1 must be not empty"));
    assert(m1.find("field1")->second.size()==6); // size was not changed

    return 0;
}
```

#### clear_validated

Default implementation of `clear_validated` uses `clear()` method of a member. To use `resize_validated` with custom types a template specialization of `clear_member_t` must be defined. See example in `validator/prevalidation/clear_validated.hpp` header.

Note that only [size](#size), [length](#length) and [empty](#empty) properties as well as [comparison](builtin_operators.md#comparison-operators) and [lexicographical](builtin_operators.md#lexicographical-operators) operators are validated before this operation. If other operators are used to validate content, then they are not checked. For example, resizing string with `validator(_[string_field](regex_match,"Hello world!"))` will not emit error even in case the validation condition is not met. 

```cpp
#include <dracosha/validator/validator.hpp>
#include <dracosha/validator/prevalidation/clear_validated.hpp>

using namespace DRACOSHA_VALIDATOR_NAMESPACE;

int main()
{
    error_report err;
    
    // define validator
    auto v1=validator(
                _["field1"](empty(flag,false)),
                _["field2"](size(gte,5) ^AND^ value(ne,"Invalid value")),
                _["field5"](length(gte,4)),
                _["field6"](NOT(length(lt,4)))
            );

    // define container for validation
    std::map<std::string,std::string> m1{
        {"field1","value1"},
        {"field2","value2"},
        {"field3","value3"},
        {"field4","value4"},
        {"field5","value5"},
        {"field6","value6"}
    };

    // try to clear element
    clear_validated(m1,_["field1"],v1,err);
    assert(err); // fail
    assert(err.message()==std::string("field1 must be not empty"));
    assert(!m1.find("field1")->second.empty()); // element was not cleared

    return 0;
}
```

## Members

Members are used to define what parts of [objects](#object) must be validated. A [member](#member) can point to one of the following:
- member variable of C++ object;
- member method of C++ object, where the method must be of getter type, i.e. it must be without arguments and must return a value;
- element of container.

### Member notation

Member notation is used to define validation conditions for a certain [member](#member). With member notation a temporary callable object is constructed which will define the validation conditions for that member when invoked with validation operators and operands.

```cpp
#include <dracosha/validator/validator.hpp>
using namespace DRACOSHA_VALIDATOR_NAMESPACE;

// define member with member notation
auto member_callable=_[member_id];

/* 
   define validator of the member by invoking the member callable 
   with validation operators and operands
*/
auto member_validator1=member_callable(gte,100);

// the same as above but in one line
auto member_validator2=_[member_id](gte,100);
```

#### Single level members

A [member](#member) name must be placed within square brackets that follow an underscore symbol `_` defined in the namespace of `cpp-validator` library.
If a member name is a literal key of an element in container then it must be surrounded with quotes. See examples below.

```cpp
#include <dracosha/validator/validator.hpp>
using namespace DRACOSHA_VALIDATOR_NAMESPACE;

// member is a property
auto member_property=_[size];

// member is an integer key of container element
auto member_integer_key=_[100];

// member is a literal key of container element
auto member_string_key=_["some_member"];
```
#### Nested members

To validate members of nested objects or containers a hierarchical member notation must be used, where name of the member at each level is placed within square brackets and appended to the upper member resulting in a `member path`. See examples below.

```cpp
#include <dracosha/validator/validator.hpp>
using namespace DRACOSHA_VALIDATOR_NAMESPACE;

// 3 levels
auto member_path_3_levels=_[level1][level2][level3];

// nested container elements
auto member_path_nested_container=_["element1"]["element1_1"]["element1_1_1"];
```

```cpp
#include <map>
#include <dracosha/validator/validator.hpp>
#include <dracosha/validator/adapters/reporting_adapter.hpp>
using namespace DRACOSHA_VALIDATOR_NAMESPACE;

int main()
{

auto v=validator(
                _["field1"][1](in,range({10,20,30,40,50})),
                _["field1"][2](lt,100),
                _["field2"](exists,false),
                _["field3"](empty(flag,true))
            );
                
std::string report;

std::map<std::string,std::map<size_t,size_t>> nested_map={
            {"field1",{{1,5},{2,50}}},
            {"field3",{}}
        };
auto ra=make_reporting_adapter(nested_map,report);

if (!v.apply(ra))
{
    std::cerr << report << std::endl;
    /* prints:
    "element #1 of field1 must be in range [10, 20, 30, 40, 50]"
    */
}

return 0;
}
```

### Member existence

Special operator [exists](#exists) can be used to check explicitly if an [object](#object) contains some [member](#member). 

There is also a [contains](#contains) operator added to the library for convenience to validate members of container types.

A [member](#member) existence can also be checked implicitly before applying validation to the [member](#member). Check of member existence is performed by an [adapter](#adapter) if the [adapter](#adapter) supports that. [Default adapter](#default-adapter) and [reporting adapter](#reporting-adapter) provide this feature which can be configured with  `set_check_member_exists_before_validation` and `set_unknown_member_mode` adapter methods.

Method `set_check_member_exists_before_validation` enables/disables implicit check of member existence. By default this option is disabled which improves validation performance but can sometimes cause exceptions or other undefined errors. Note that some basic check of property existence or type compatibility might be performed statically at compile time regardless of this flag.

Method `set_unknown_member_mode` instructs adapter what to do if a member is not found. There are two options:
- ignore missed members and continue validation process;
- abort validation process with error.

See examples below.

```cpp
#include <map>
#include <dracosha/validator/validator.hpp>
using namespace DRACOSHA_VALIDATOR_NAMESPACE;

int main()
{

std::map<std::string,int> m1={
        {"field1",10},
        {"field3",100}
    };

// adapter 1 aborts validation if some member is not found        
auto a1=make_default_adapter(m1);
a1.set_check_member_exists_before_validation(true);
a1.set_unknown_member_mode(if_member_not_found::abort);

// adapter 2 ignores not found members
auto a2=make_default_adapter(m1);
a2.set_check_member_exists_before_validation(true);
a2.set_unknown_member_mode(if_member_not_found::ignore);

// both members exist in the map
auto v1=validator(
            _["field1"](gte,9),
            _["field3"](eq,100)
        );
// validation succeeded with both adapters
assert(v1.apply(a1));
assert(v1.apply(a2));

// member "field2" doesn't exist in the map
auto v2=validator(
            _["field1"](eq,10),
            _["field2"](lt,1000)
        );
// validation failed with adapter 1
assert(!v2.apply(a1));
// validation succeeded with adapter 2
assert(v2.apply(a2));

return 0;
}
```

## Properties

[Properties](#property) are used to validate contents or characteristics of [objects](#object) and [members](#member). A [property](#property) can correspond either to a member variable or to a getter method of C++ `class` or `struct`.

### Property notations

A [property](#property) can be used in a [validator](#validator) in one of the following forms:
* `property notation` when property's name is used as part of a validation condition in the form of `property_name(operator_name,operand)`;
* [member notation](#member-notation) when property's name is used in [member](#member) section of a [validator](#validator).

See examples of different property notations in section [Validator with properties](#validator-with-properties).

### Built-in properties

#### *value*

`value` property stands for the variable itself. This is a special pseudo property which means that validation must be applied to the whole variable. See examples how the `value` property can be used in section [validator with aggregations](#validator-with-aggregations).

#### *size*

`size` property is used to validate result of `size()` method or `size` member variable of an [object](#object).

#### *length*

`length` property is used to validate result of `length()` method or `length` member variable of an [object](#object).

#### *empty*

`empty` property is used to validate result of `empty()` method or `empty` member variable of an [object](#object).

#### *h_size*

`h_size` property is used to validate size of heterogeneous container. If object is not a heterogeneous container then `h_size` is zero. The library can detect size of two type of heterogeneous containers: `std::tuple` and `hana::tuple`. A custom type can be added by defining specialization of `heterogeneous_size_t` template struct that must be of `size_t` integral constant type. See example below.

```cpp
#include <utility>
#include "dracosha/validator/utils/heterogeneous_size.hpp"

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

// template specialization for std:pair
template <typename T>
struct heterogeneous_size_t<std::pair<T>> : public hana::size_t<2>
{};

DRACOSHA_VALIDATOR_NAMESPACE_END
```
#### *first* and *second*

`first` and `second` properties are used to validate elements of `std::pair`.

### Adding new property

A new [property](#property) can be added using special macros defined in `cpp-validator` library. 

A [property](#property) of non-boolean type must be defined using `DRACOSHA_VALIDATOR_PROPERTY(property_name)` macro with one argument for name of the [property](#property).

```cpp
#include <dracosha/validator/property.hpp>
#include <dracosha/validator/validator.hpp>
using namespace DRACOSHA_VALIDATOR_NAMESPACE;

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
DRACOSHA_VALIDATOR_PROPERTY(var1);

// define property corresponding to getter method
DRACOSHA_VALIDATOR_PROPERTY(get_var2);

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

return 0;
}
```

If a [property](#property) is of boolean type and must be capable of being used with [flag](#flag) operator then the property must be defined with `DRACOSHA_VALIDATOR_PROPERTY_FLAG(property_name,positive_flag_descriprion,negative_flag_description)` macro that has three arguments:
1. name of the [property](#property);
2. description of positive [flag](#flag);
3. description of negative [flag](#flag).

```cpp
#include <iostream>
#include <dracosha/validator/property.hpp>
#include <dracosha/validator/validator.hpp>
#include <dracosha/validator/adapters/reporting_adapter.hpp>
using namespace DRACOSHA_VALIDATOR_NAMESPACE;

// structure with red_color() getter method
struct Foo
{
    // boolean getter method
    bool red_color() const
    {
        return true;
    }
};

// define flaggable red_color property
DRACOSHA_VALIDATOR_PROPERTY_FLAG(red_color,"Must be red","Must be not red")

int main()
{

// validator with red_color property
auto v=validator(
    _[red_color](flag,false)
);

// create reporting adapter
std::string report;
Foo foo_instance;
auto ra=make_reporting_adapter(foo_instance,report);

// apply validator and print report
if (!v.apply(ra))
{
    std::cerr << report << std::endl;
    /* prints:
    
    "Must be not red"
    
    */
}

return 0;
}
```

### Properties of heterogeneous containers

To validate elements of heterogeneous containers a special kind of properties should be used. A `heterogeneous property` wraps an index of a tuple element. [Element aggregations](#element-aggregations) can be used with heterogeneous properties in the same way as with ordinary properties. Note that element aggregation can be used with heterogeneous containers whose [h_size](h_size) is greater than zero.

A heterogeneous property can be defined either implicitly or explicitly.

#### Implicit heterogeneous property

Heterogeneous property is defined implicitly when an integral constant is used as one of the keys of [member's](#member) path.

See example below.

```cpp
#include <dracosha/validator/heterogeneous_property.hpp>
#include <dracosha/validator/validator.hpp>
#include <dracosha/validator/validate.hpp>

using namespace DRACOSHA_VALIDATOR_NAMESPACE;

int main()
{
    // implicit heterogeneous property from std::integral_constant<size_t,CONSTANT>
    auto v1=validator(
        _[std::integral_constant<size_t,1>{}](gt,100)
    );
    // implicit heterogeneous property from hana::size_c<CONSTANT>
    auto v2=validator(
        _[hana::size_c<1>](gt,100)
    );

    error_report err;

    // validate std::tuple
    auto t1=std::make_tuple(200,50,"hello");
    validate(t1,v1,err);
    assert(err);
    assert(err.message()==std::string("element #1 must be greater than 100"));

    // validate hana::tuple
    auto t2=hana::make_tuple(200,50,"hello");
    validate(t2,v2,err);
    assert(err);
    assert(err.message()==std::string("element #1 must be greater than 100"));

    return 0;
}
```

#### Explicit heterogeneous property

Explicit heterogeneous property can be used if the property must have distinct name and/or [flag](#flag) descriptions. To define explicit heterogeneous property use either macro `DRACOSHA_VALIDATOR_HETEROGENEOUS_PROPERTY(name,element_index)` or macro `DRACOSHA_VALIDATOR_HETEROGENEOUS_PROPERTY_FLAG(name,element_index,flag_description,negative_flag_description)`.

See example below.

```cpp
#include <dracosha/validator/heterogeneous_property.hpp>
#include <dracosha/validator/validator.hpp>
#include <dracosha/validator/validate.hpp>

using namespace DRACOSHA_VALIDATOR_NAMESPACE;

// define property "zero" with index 0
DRACOSHA_VALIDATOR_HETEROGENEOUS_PROPERTY(zero,0)
// define property "one" with index 1
DRACOSHA_VALIDATOR_HETEROGENEOUS_PROPERTY(one,1)

int main()
{
    // use explicit heterogeneous property "one"
    auto v1=validator(
        _[one](gt,100)
    );
    // use explicit heterogeneous property "zero"
    auto v2=validator(
        _[zero](lt,100)
    );

    error_report err;

    auto t1=std::make_tuple(200,50,"hello");
    validate(t1,v1,err);
    BOOST_CHECK(err);
    BOOST_CHECK_EQUAL(err.message(),std::string("one must be greater than 100"));

    auto t2=hana::make_tuple(200,50,"hello");
    validate(t2,v2,err);
    BOOST_CHECK(err);
    BOOST_CHECK_EQUAL(err.message(),std::string("zero must be less than 100"));

    return 0;
}
```

### Variadic properties

#### Properties with arguments

Ordinary [properties](#properties) do not have arguments. To validate [object](#object) methods with arguments one shoud use `variadic properties`. A variadic property is defined similar to an ordinary property but only using a different set of macros:
- `DRACOSHA_VALIDATOR_VARIADIC_PROPERTY(method)` defines variadic property corresponding to object's `method`;
- `DRACOSHA_VALIDATOR_VARIADIC_PROPERTY_HAS(method,has_method)` defines variadic property corresponding to object's `method` and `has_method` used to check if object has the variadic property with provided arguments;
- `DRACOSHA_VALIDATOR_VARIADIC_PROPERTY_FLAG(method,flag_descriprion,negative_flag_description)` defines variadic property corresponding to object's `method` that can be used with [flag](#flag) operator;
- `DRACOSHA_VALIDATOR_VARIADIC_PROPERTY_HF(method,has_method,flag_descriprion,negative_flag_description)` defines variadic property corresponding to object's `method` and `has_method` that can be used with [flag](#flag) operator.

A variadic property defined with one of the macros listed above can be used either in property notation or member notation:
- `auto property_notation=variadic_property(arg1,arg2);`
- `auto member_notation=_[variadic_property][arg1][arg2];`

When used with member notation the arguments passed to variadic property must follow the property's key in the member's path. In this case special intermediate closures will be created at each level of member's path until number of required arguments is reached thus using [currying](https://en.wikipedia.org/wiki/Currying) technique. 

The arguments following the variadic property in the member's path can be provided either "as is" or wrapped into `varg(argument)`. In the latter case the variadic property will be presented in more natural way in text reports:

- `auto member_notation=_[variadic_property][1][2];` will result in "*2 of 1 of variadic_property*" in the text report;
- `auto member_notation=_[variadic_property][varg(1)][varg(2)];` will result in "*variadic_property(1,2)*" in the text report;

See examples below.

```cpp
#include <dracosha/validator/variadic_property.hpp>
#include <dracosha/validator/validator.hpp>
#include <dracosha/validator/adapters/reporting_adapter.hpp>
using namespace DRACOSHA_VALIDATOR_NAMESPACE;

// define a structure with variadic properties
struct WithPlusProperties
{
    // method with one argument
    int plus_one(int val) const noexcept
    {
        return val+1;
    }

    // method with two arguments
    int val_plus_word_size(int val, const std::string& word) const noexcept
    {
        return val+static_cast<int>(word.size());
    }

    // method to check if val_plus_word_size can be used with provided arguments
    bool has_val_plus_word_size(int val, const std::string& word) const noexcept
    {
        return val>10 && word.size()>=5;
    }
};

// define variadic property
DRACOSHA_VALIDATOR_VARIADIC_PROPERTY(plus_one)
// define variadic property to be used as "exists" checker
DRACOSHA_VALIDATOR_VARIADIC_PROPERTY(has_val_plus_word_size)
// define variadic property with "exists" checker
DRACOSHA_VALIDATOR_VARIADIC_PROPERTY_HAS(val_plus_word_size,has_val_plus_word_size)

int main()
{
    // define object
    WithPlusProperties o1;
    
    // define validation adapter with text reports
    std::string rep;
    auto ra1=make_reporting_adapter(o1,rep);

    // variadic property with single argument and member notation
    auto v1=validator(
        _[plus_one][varg(1)](eq,30)
    );
    assert(!v1.apply(ra1));
    assert(rep==std::string("plus_one(1) must be equal to 30"));
    rep.clear();
    
    // variadic property with single argument and property notation
    auto v2=validator(
        plus_one(1)(eq,30)
    );
    assert(!v2.apply(ra1));
    assert(rep==std::string("plus_one(1) must be equal to 30"));
    rep.clear();

    // variadic property with two arguments and member notation
    auto v3=validator(
        _[val_plus_word_size][varg(20)][varg("hello")](eq,30)
    );
    assert(!v3.apply(ra1));
    assert(rep==std::string("val_plus_word_size(20,hello) must be equal to 30"));
    rep.clear();

    // variadic property with two arguments and property notation
    auto v4=validator(
        val_plus_word_size(20,"hello")(eq,30)
    );
    assert(!v4.apply(ra1));
    assert(rep==std::string("val_plus_word_size(20,hello) must be equal to 30"));
    rep.clear();
    
    // check if variadic property exists
    auto v5=validator(
        _[val_plus_word_size][20]["hello"](exists,true)
    );
    assert(v5.apply(o1));
    auto v6=validator(
        _[val_plus_word_size][5]["hello"](exists,true)
    );
    assert(!v6.apply(o1));
    
    return 0;
}
```

#### Variadic properties with aggregations

Variadic properties can be used with [element aggregations](#element-aggregations). Element aggregation can be applied to any argument of a variadic property. Element aggregations with variadic properties can be used only in `member notation` as `_[variadic_property][varg(aggregation,end_argument)]` where:

- `variadic_property` - name of the property (object's method);
- `aggregation` - name of element aggregation which can be either `ALL` or `ANY`;
- `end_argument` - end value of the argument passed to the variadic property; note that the end value will not be processed as it is used only as end condition in `for-loop`; end_argument can be either a variable or an ordinary [property](#property) of the object.

See example below.

```cpp
#include <dracosha/validator/variadic_property.hpp>
#include <dracosha/validator/validator.hpp>
using namespace DRACOSHA_VALIDATOR_NAMESPACE;

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
DRACOSHA_VALIDATOR_VARIADIC_PROPERTY(element)

// define "end" properties
DRACOSHA_VALIDATOR_PROPERTY(count_i)
DRACOSHA_VALIDATOR_PROPERTY(count_j)

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
    
    return 0;
}
```

## Operators

[Operators](#operator) define atomic validation conditions in [validators](validator). 

[Operator](#operator) is a callable object that returns `true` if validation condition is satisfied, `false` otherwise. In default [adapter](#adapter) implementations an [operator](#operator) is called with two arguments when [validator](#validator) is applied to the [adapter](#adapter):
* the first argument is a corresponding [property](#property) of the [object](#object) or object's [member](#member) that must be validated;
* the second argument is an [operand](#operand) that must be used as a sample for validation.

### Special operators

#### *exists*

Operator `exists` is used to check explicitly if an [object](#object) contains some [member](#member). See example below.

```cpp
#include <map>
#include <dracosha/validator/validator.hpp>
using namespace DRACOSHA_VALIDATOR_NAMESPACE;

int main()
{

// define validator
auto v1=validator(
    _["key1"](exists,true),
    _["key2"](exists,false)
);

// m1 satisfies validation conditions
std::map<std::string,std::string> 
    m1={{"key1","value1"}};
assert(v1.apply(m1));

// m2 does not satisfy validation conditions
std::map<std::string,std::string> 
    m2={{"key2","value2"}};
assert(!v1.apply(m2));

return 0;
}
```

#### *contains*

Operator `contains` is used to check if the variable that is under validation contains an element that matches the [operand](#operand). See example below.

```cpp
#include <map>
#include <dracosha/validator/validator.hpp>
#include <dracosha/validator/operator/contains.hpp>
using namespace DRACOSHA_VALIDATOR_NAMESPACE;

int main()
{

// define validator
auto v1=validator(contains,"field1");

// m1 satisfies validation condition
std::map<std::string,size_t> m1={{"field1",1}};
assert(v1.apply(m1));

// m2 does not satisfy validation condition
std::map<std::string,size_t> m2={{"field2",2}};
assert(!v1.apply(m2));

return 0;
}
```

#### *flag*

Operator `flag` is a special case of equality operator for boolean arguments. The main purpose of defining separate operator `flag` in addition to operator [eq](builtin_operators.md#eq) is more flexible [reports](#report) construction. With operator [eq](builtin_operators.md#eq) a [report](#report) would always use "must be equal to" string. With operator `flag` report strings can be customized depending on either the [property](#property) definition or explicitly preset strings, e.g. "must be checked" or "must be set" would be used in reports instead of "must be equal to true".

There are three ways of string customization for `flag` operator.

1. Define [custom property](#adding-new-property) using `DRACOSHA_VALIDATOR_PROPERTY_FLAG(property_name,flag_descriprion,nwgative_flag_description)` macro where the second argument is a reporting string for `<custom property>(flag,true)` condition and the third argument is a reporting string for `<custom property>(flag,false)` condition. See [example](#adding-new-property).

2. Use `flag` operator with one of string presets, e.g. `value(flag(flag_on_off),true)` will result in reporting string "must be on" and `value(flag(flag_on_off),false)` will result in reporting string "must be off". The following preset flag strings are defined in `validator/reporting/flag_presets.hpp` header file:

    * `flag_true_false` (default flag string): "must be true" | "must be false";
    * `flag_on_off`: "must be on" | "must be off";
    * `flag_checked_unchecked`: "must be checked" | "must be unchecked";
    * `flag_set_unset`: "must be set" | "must be unset";
    * `flag_enable_disable`: "must be enabled" | "must be disabled".

3. Use `flag` operator with explicit string description, e.g. `value(flag("custom description"),true)` will result in reporting string "custom description".

Examples of `flag` operator.
```cpp
#include <vector>
#include <iostream>
#include <dracosha/validator/validator.hpp>
#include <dracosha/validator/operator/flag.hpp>
#include <dracosha/validator/reporting/flag_presets.hpp>
#include <dracosha/validator/adapters/reporting_adapter.hpp>
using namespace DRACOSHA_VALIDATOR_NAMESPACE;

int main()
{

// prepare object and reporting adapter
std::vector<size_t> m1;
std::string report;
auto ra1=make_reporting_adapter(m1,report);

// validator with preset flag string
auto v1=validator(
    empty(flag(flag_true_false),false)
);
assert(!v1.apply(ra1));
std::cerr << report << std::endl;
/* prints:
"empty must be false"
*/
report.clear();

// validator with explicit flag string
auto v2=validator(
    empty(flag("why?"),false)
);
assert(!v2.apply(ra1));
std::cerr << report << std::endl;
/* prints:
"empty why?"
*/
report.clear();

return 0;
}
```

#### *in*

Operator `in` is used to check if corresponding argument fits into [range](#range) or [interval](#interval). Operator `nin` is a [negation](#_n-negation) of the `in` operator. See examples below.

```cpp
#include <dracosha/validator/validator.hpp>
#include <dracosha/validator/operators.hpp>
#include <dracosha/validator/range.hpp>
#include <dracosha/validator/interval.hpp>
using namespace DRACOSHA_VALIDATOR_NAMESPACE;

int main()
{

// define value for validation
size_t val=90;

// check if the value is in range
auto v1=validator(in,range({70,80,90,100}));
assert(v1.apply(val));

// check if the value is in interval
auto v2=validator(in,interval(80,100));
assert(v2.apply(val));

return 0;
}
```

#### *nin*

See [in](#in) operator.

#### *_n* (negation)

`_n` is an [operator](#operator) wrapper that negates wrapped [operator](#operator). See example below.

```cpp
#include <map>
#include <dracosha/validator/validator.hpp>
#include <dracosha/validator/operators.hpp>
using namespace DRACOSHA_VALIDATOR_NAMESPACE;

int main()
{

// container to validate
std::map<std::string,uint32_t> m1={{"key1",100}};

// validator with original operator
auto v1=validator(
    _["key1"](eq,100)
);
assert(v1.apply(m1));

// validator with negated operator
auto v2=validator(
    _["key1"](_n(eq),100)
);
assert(!v2.apply(m1));

return 0;
}
```

### Built-in operators

See [Built-in validation operators](builtin_operators.md).

### Adding new operator

Adding a new [operator](#operator) consists of the following steps.

1. Define `struct` that inherits from template class `op` with the name of the defined struct as a template argument.
2. Define callable `operator ()` in the struct with two template arguments.
3. Define `description` and `n_description` as `constexpr static const char*` variables of the struct that will be used as human readable error descriptions in [report](#report): `description` is used for the operator itself and `n_description` is used for negation of this operator.
4. Define `constexpr` callable object of this struct type. This object will be used in [validators](#validator).

See example below.

```cpp
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
```

## Operands

Value of an [operand](#operand) is given as the second argument to an [operator](#operator) and is meant to be used as a validation sample. An [operand](#operand) can be one of the following:
- constant or `lvalue` or `rvalue` [variable](#variables);
- [lazy](#lazy-operands) operand;
- [other member](#other-members) of the [object](#object) under validation;
- [the same member of sample object](#sample-objects);
- [interval](#intervals);
- [range](#ranges).

### Variables and constants

The most common case is when an [operand](#operand) is either constant or `lvalue` variable or `rvalue` variable. Note, that `lvalue` variable is used by reference and a user is responsible for the operand to stay valid during validator's life time.

See examples below.

```cpp
// operand is constant
constexpr const int constant_operand=100;
auto v1=validator(gt,constant_operand);
// or
auto v1_1=validator(gt,100);

// operand is lvalue
int lval_operand=100;
auto v2=validator(gt,lval_operand);

// operand is rvalue
int rval_operand=100;
auto v3=validator(gt,std::move(rval_operand));
```

### Lazy operands

If value of an [operand](#operand) must be evaluated at the moment of [validator](#validator) applying then `lazy` wrapper must be used. `lazy` wraps a callable object (e.g. lambda) that will be invoked later when the [validator](#validator) is applied. See example below.

```cpp
#include <dracosha/validator/validator.hpp>
#include <dracosha/validator/lazy.hpp>
using namespace DRACOSHA_VALIDATOR_NAMESPACE;

int main()
{

// define sample getter
int sample=100;
auto get_sample=[&sample]()
{
    return sample;
};

// define validator with lazy operand
auto v1=validator(
            eq,lazy(get_sample)
        );

// define variable
int val=100;

// validate variable with original sample
assert(v1.apply(val));

// update sample and validate variable again
sample=200;
assert(!v1.apply(val));

return 0;
}
```

### Other members

Other [member](#member) of the same [object](#object) can be used as an [operand](#operand). For example, one can check if two [members](#members) of the same object match. See example below.

```cpp
#include <map>
#include <dracosha/validator/validator.hpp>
using namespace DRACOSHA_VALIDATOR_NAMESPACE;

int main()
{

// validator to check if "password" is equal to "check_password"
auto v1=validator(
    _["password"](eq,_["check_password"])
);

// map to validate, just for example - NEVER use such passwords!
std::map<std::string,std::string> m1={
        {"password","12345678"},
        {"check_password","12345678"}
    };

// validate map
assert(v1.apply(m1));

return 0;
}
```

### Sample objects

[Members](#member) of other object can be used as [operands](#operand) for validation. A [member](#member) with the same [member path](#nested-members) of sample object is used as [operand](#operand) in validation conditions for a given [member](#member). To construct such [operand](#operand) a sample object must be placed within round brackets following underscore symbol, e.g. `_(sample_object)`. See example below.

```cpp
#include <map>
#include <dracosha/validator/validator.hpp>
using namespace DRACOSHA_VALIDATOR_NAMESPACE;

int main()
{

// sample container
std::map<std::string,std::string> sample={
        {"token","12345678"}
    };

// validator to check if "token" element of object under validation 
// is equal to "token" element of the sample
auto v1=validator(
    _["token"](eq,_(sample))
);

// map to validate
std::map<std::string,std::string> m1={
        {"token","12345678"}
    };

// validate map
assert(v1.apply(m1));

return 0;
}
```

### Intervals

An interval can be specified with two endpoints: `from` and `to`. Interval can be one of the following:
- `open` when its endpoints do not belong to the interval;
- `closed` when its endpoints belong to the interval;
- partially `open` or `closed` when one of its endpoints belongs to the interval while the other does not.

Interval [operand](#operand) can be used only with [in](#in) and [nin](#nin) operators. With [in](#in) operator it is used to check if a variable is greater than or equal to `from` endpoint and less than or equal to `to` endpoint. If an endpoint is `open` then condition "is greater than" or "is less than" will be checked respectively.

Notation of interval operand consists of keyword `interval` followed by `from` endpoint and `to` endpoint separated with comma and surrounded with round braces, e.g. `interval(1,100)`. By default an interval is `closed`. An interval modifier can be used as the third argument of `interval(from,to,modifier)` to change the type of interval's endpoint. There are four modifiers defined:
- `interval.closed()` (default modifier) corresponds to `[from,to]`;
- `interval.open()` corresponds to `(from,to)`;
- `interval.open_from()` corresponds to `(from,to]`;
- `interval.open_to()` corresponds to `[from,to)`.

See examples below.

```cpp
#include <dracosha/validator/validator.hpp>
#include <dracosha/validator/interval.hpp>
#include <dracosha/validator/operators/in.hpp>
using namespace DRACOSHA_VALIDATOR_NAMESPACE;

int main
{

size_t val=90;

auto v1=validator(in,interval(1,100));
assert(v1.apply(val));

auto v2=validator(in,interval(95,100));
assert(!v2.apply(val));

auto v3=validator(in,interval(90,100));
assert(v3.apply(val));

auto v4=validator(in,interval(90,100,interval.open()));
assert(!v4.apply(val));

auto v5=validator(in,interval(90,100,interval.open_from()));
assert(!v5.apply(val));

auto v6=validator(in,interval(90,100,interval.open_to()));
assert(v6.apply(val));

auto v7=validator(in,interval(80,90,interval.open_to()));
assert(!v7.apply(val));

return 0;
}
```

In [report](#report) construction an interval is formatted as "interval [from,two]" where forms of the braces depend on the interval's mode as follows:
- "interval [from,two]" for closed intervals;
- "interval (from,two)" for open intervals;
- "interval (from,two]" for left open intervals;
- "interval [from,two)" for right open intervals.

If [decorator](#decorator) is used then only part within braces including the braces is decorated.

### Ranges

Range [operand](#operand) is a searchable set of elements. Range [operand](#operand) can be used only with [in](#in) and [nin](#nin) operators. With [in](#in) operator it is used to check if a variable matches one of the elements in the `range`. With [nin](#nin) it is used to check if a variable does not match any element in the `range`.

Range can be specified using one of the following ways:

- wrap some existing container, e.g. 
    ```cpp
    std::vector<int> vec={10,5,9,100};
    auto v1=validator(in,range(vec));
    ```
- wrap some existing sorted container, e.g. 
    ```cpp
    std::vector<int> vec={1,2,3,4,5};
    auto v1=validator(in,range(vec,sorted));
    ```
- construct range inline, e.g. 
    ```cpp
    auto v1=validator(in,range({10,5,9,100}));
    ```
- construct sorted range inline, e.g. 
    ```cpp
    auto v1=validator(in,range({1,2,3,4,5},sorted));
    ```
Sorted and unsorted ranges differ in processing: for sorted ranges `std::binary_search` is used whereas `std::find_if` is used for unsorted ranges which is slower than `std::binary_search`.

In [reporting](#report) a `range` is formatted as "range [x[0], x[1], ... , x[N]]", where x[i] denotes i-th element of the container. To limit a number of elements in a [report](#report) one should use `range` with additional integer argument that stands for `max_report_elements`. If  `max_report_elements` is set then at most `max_report_elements` will be used in [report](#report) formatting and ellipsis ", ... " will be appended to the end of the list. See examples below.

```cpp
std::vector<size_t> vec{1,2,3,4,5,6,7,8,9,10};

// report will use string "range [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]"  
auto v1=validator(in,range(vec,5));

// report will use string "range [1, 2, 3, 4, 5, ... ]"  
auto v2=validator(in,range(vec,5));

// report will use string "range [1, 2, 3, 4, 5, ... ]"  
auto v3=validator(in,range(vec,sorted,5));

// report will use string "range [1, 2, 3, 4, 5, ... ]"  
auto v4=validator(in,range({1,2,3,4,5},5));

// report will use string "range [1, 2, 3, 4, 5, ... ]"  
auto v5=validator(in,range({1,2,3,4,5},sorted,5));
```

If [decorator](#decorator) is used then only the part within braces including the braces is decorated.

## Aggregations

[Aggregations](#aggregation) can be used in one of the following notations:
- `functional notation` where validation conditions are given as list of arguments in aggregation callable object, e.g. `AND(op1,op2,op3)`;
- `infix notation` where validation conditions are joined using aggregation conjunctive made up of aggregation keyword surrounded with `^`, e.g. `op1 ^AND^ op2 ^AND^ op3`.

### Logical aggregations

Logical aggregation is a combination of validating [operators](#operator) or other [validators](#validator) that make up a compound [validator](#validator). Logical aggregations can be used to combine validation conditions for both [objects](#object) and [members](#member). See examples of logical aggregations in [Validator with aggregations](#validator-with-aggregations) section.

#### AND

`AND` aggregation is used when all validation conditions must be satisfied. In addition to `functional notation` and `infix notation` the `AND` aggregation can be implicitly invoked when validation conditions in a [validator](#validator) for the whole object are separated with commas. See examples below.

```cpp
// validator is satisfied when variable is greater than 1 and less than 100

// functional notation in validation condition of whole object
auto v1=validator(
        AND(value(gt,1),value(lt,100))
    );

// inline notation in validation condition of whole object
auto v2=validator(
        value(gt,1) ^AND^ value(lt,100)
    );

// implicit AND in validation condition of whole object
auto v3=validator(
        value(gt,1),
        value(lt,100)
    );

// validator is satisfied when "key1" element of variable is greater than 1 and less than 100

// functional notation in validation condition of a member
auto v4=validator(
        _["key1"](AND(value(gt,1),value(lt,100)))
    );

// infix notation in validation condition of a member
auto v5=validator(
        _["key1"](value(gt,1) ^AND^ value(lt,100))
    );
```

#### OR

`OR` aggregation is used when at least one validation condition must be satisfied. See examples below.

```cpp
// validator is satisfied when variable is equal to 1 or is equal to 100

// functional notation in validation condition of whole object
auto v1=validator(
        OR(value(eq,1),value(eq,100))
    );

// infix notation in validation condition of whole object
auto v2=validator(
        value(eq,1) ^OR^ value(eq,100)
    );

// validator is satisfied when "key1" element of variable is equal to 1 or is equal to 100

// functional notation in validation condition of a member
auto v3=validator(
        _["key1"](OR(value(eq,1),value(eq,100)))
    );

// infix notation in validation condition of a member
auto v4=validator(
        _["key1"](value(eq,1) ^OR^ value(eq,100))
    );
```

#### NOT

`NOT` aggregation is used to negate the whole expression. `NOT` aggregation can use only `functional notation` with single argument. See examples below.

```cpp
// validator is satisfied when variable is not equal to 1 and not equal to 100
auto v1=validator(
        NOT(value(eq,1) ^OR^ value(eq,100))
    );

// validator is satisfied when "key1" element of variable is not equal to 1 and is not equal to 100
auto v4=validator(
        _["key1"](NOT(value(eq,1) ^OR^ value(eq,100)))
    );
```

### Element aggregations

Element aggregations are used when the same validation conditions must be applied to multiple elements of container. Element aggregations can be used either with `member notation` or with `functional notation` with single argument. In both cases element aggregations can be nested.

#### ANY

`ANY` aggregation is used when at least one element of container must satisfy a condition. See examples below.

```cpp
// at least one element of container must be equal to "unknown" 
// or must have size greater or equal to 5
// member notation
auto v1_1=validator(
        _[ANY](value(eq,"unknown") ^OR^ size(gte,5))
    );
// functional notation    
auto v1_2=validator(
        ANY(value(eq,"unknown") ^OR^ size(gte,5))
    );

// at least one element of container at "key1" element must be equal to "unknown" 
// or must have size greater or equal to 5
// member notation
auto v2_1=validator(
        _["key1"][ANY](value(eq,"unknown") ^OR^ size(gte,5))
    );
// functional notation    
auto v2_2=validator(
        _["key1"](ANY(value(eq,"unknown") ^OR^ size(gte,5)))
    );
    
// nested ANY
// member notation
auto v3_1=validator(
        _["key1"][ANY][ANY](value(eq,"unknown") ^OR^ size(gte,5))
    );
// functional notation    
auto v3_2=validator(
        _["key1"](ANY(ANY(value(eq,"unknown") ^OR^ size(gte,5))))
    );
```

#### ALL

`ALL` aggregation is used when each element of container must satisfy a condition. See examples below.

```cpp
// each element of container must be equal to "unknown" 
// or must have size greater or equal to 5
// member notation
auto v1_1=validator(
        _[ALL](value(eq,"unknown") ^OR^ size(gte,5))
    );
// functional notation
auto v1_2=validator(
        ALL(value(eq,"unknown") ^OR^ size(gte,5))
    );

// each element of container at "key1" element must be equal to "unknown" 
// or must have size greater or equal to 5
// member notation
auto v2_1=validator(
        _["key1"][ALL](value(eq,"unknown") ^OR^ size(gte,5))
    );
// functional notation    
auto v2_2=validator(
        _["key1"](ALL(value(eq,"unknown") ^OR^ size(gte,5)))
    );
    
// nested ALL
// member notation
auto v3_1=validator(
        _["key1"][ALL][ALL](value(eq,"unknown") ^OR^ size(gte,5))
    );
// functional notation    
auto v3_2=validator(
        _["key1"](ALL(ALL(value(eq,"unknown") ^OR^ size(gte,5))))
    );
```

#### Aggregation modifiers

By default `ALL` and `ANY` aggregations validate values of container elements. In order to validate iterators or keys the aggregation modifiers should be used. There are three kinds of aggregation modifiers:

- `values` (default) to validate a value of each element;
- `keys` to validate a key of each element;
- `iterators` to validate an iterator of each element.

To use aggregation with modifier the modifier must be provided as an argument to the aggregation, e.g. `ALL(keys)` or `ANY(iterators)`.

See examples below.

```cpp
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

    // validate values
    auto v2=validator(
        _[ALL(values)](gt,"value")
    );
    assert(v2.apply(m1));
    assert(!v2.apply(m2));

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
    
    return 0;
}
```

### Validation of trees

Validator can be used for validation of tree nodes. To validate trees a special keyword `tree` must be used as a key in [member's](#member) path. A `tree` key has three parameters `tree(aggregation,node_child_getter,node_children_count)`, where:
- `aggregation` is a mode of [element aggregation](#element-aggregations) - [ALL](#all) or [ANY](#any). In the former case each node must satisfy validation conditions, in the latter case at least one node must satisfy validation conditions.
- `node_child_getter` is a [variadic property](#variadic-properties) of a node to access the node's child by index. Variadic property must have only one argument of index type.
- `node_children_count` is a [property](#properties) of a node to figure out number of the node's children.

See example below.

```cpp
#include <dracosha/validator/validator.hpp>
#include <dracosha/validator/variadic_property.hpp>
#include <dracosha/validator/aggregation/tree.hpp>
#include <dracosha/validator/adapters/reporting_adapter.hpp>

using namespace DRACOSHA_VALIDATOR_NAMESPACE;

// define tree node
struct TreeNode
{
    TreeNode(std::string name) : _name(std::move(name))
    {}

    void add_child(std::shared_ptr<TreeNode> child)
    {
        _children.push_back(std::move(child));
    }

    const TreeNode& child(size_t index) const
    {
        return *_children.at(index);
    }

    auto mutable_child(size_t index)
    {
        return _children.at(index);
    }

    size_t child_count() const noexcept
    {
        return _children.size();
    }

    std::string name() const
    {
        return _name;
    }

    std::vector<std::shared_ptr<TreeNode>> _children;
    std::string _name;
};

// name property is used to access name of a node
DRACOSHA_VALIDATOR_PROPERTY(name)
// child_count property is used to get number of the node's children
DRACOSHA_VALIDATOR_PROPERTY(child_count)
// child is a variadic property for accessing a node's child by index
DRACOSHA_VALIDATOR_VARIADIC_PROPERTY(child)

int main()
{
    // define tree validator
    auto v1=validator(
            _[tree(ALL,child,child_count)][name](gte,"Node")
         );

    // tree with one top level node satisfying validation conditions
    TreeNode tr1("Node 0");
    assert(v1.apply(tr1));

    // tree with one top level node not satisfying validation conditions
    TreeNode tr2("0");
    assert(!v1.apply(tr2));

    // populate top node with children
    tr1.add_child(std::make_shared<TreeNode>("Node 0.0"));
    assert(v1.apply(tr1));
    tr1.add_child(std::make_shared<TreeNode>("Node 0.1"));
    assert(v1.apply(tr1));
    tr1.add_child(std::make_shared<TreeNode>("Node 0.2"));
    assert(v1.apply(tr1));

    // populate nested nodes
    tr1.mutable_child(0)->add_child(std::make_shared<TreeNode>("Node 0.0.0"));
    assert(v1.apply(tr1));
    tr1.mutable_child(0)->add_child(std::make_shared<TreeNode>("Node 0.0.1"));
    assert(v1.apply(tr1));
    tr1.mutable_child(1)->add_child(std::make_shared<TreeNode>("Node 0.1.0"));
    assert(v1.apply(tr1));
    tr1.mutable_child(2)->add_child(std::make_shared<TreeNode>("Node 0.2.0"));
    assert(v1.apply(tr1));

    // validate nested node that doesn't satisfy validation conditions and construct report
    std::string rep;
    auto ra1=make_reporting_adapter(tr1,rep);
    tr1.mutable_child(2)->add_child(std::make_shared<TreeNode>("0.2.1"));
    assert(!v1.apply(ra1));
    assert(rep==std::string("name of each tree node must be greater than or equal to Node"));
    
    return 0;
}
```

## Adapters

[Adapters](#adapter) perform actual processing of validation conditions specified in [validators](#validator). To invoke validation with a specific adapter a [validator](#validator) must be applied to the adapter. Adapters implemented in the `cpp-validator` library use [operators](#operator) as callable objects to check validation conditions. However, adapters of other types can also be implemented, e.g. one can implement an adapter that constructs SQL queries that are equivalent to validation conditions specified in [validators](#validator).

There are a few built-in adapter types implemented in `cpp-validator` library:
- [default adapter](#default-adapter) that applies validation to an [object](#object) by invoking [operators](#operator) one by one as specified in a [validator](#validator);
- [reporting adapter](#reporting-adapter) that does the same as [default adapter](#default-adapter) with addition of constructing a [report](#report) describing an error if validation fails;
- [prevalidation adapter](#prevalidation-adapter) that validates only one [member](#member) and constructs a [report](#report) if validation fails;
- [filtering adapter](#partial-validation) used to filter member paths before validation.

### Default adapter

*Default adapter* wraps *lvalue* reference to the [object](#object) to be validated and invokes [operators](#operator) one by one as specified in a [validator](#validator). To create a *default adapter* call `make_default_adapter(object_to_validate)` with the [object](#object) as an argument. If a [validator](#validator) is applied directly to the [object](#object) then *default adapter* is constructed implicitly. See examples in [Using validator for data validation](#using-validator-for-data-validation) section.

*Default adapter* supports implicit check of [member existence](#member-existence).

### Prevalidation adapter

#### Adapter creation and usage

*Prevalidation adapter* validates only a single member before updating. *Prevalidation adapter* is constructed by calling `make_prevalidation_adapter()` helper that can have one of the following signatures:
- `make_prevalidation_adapter(member_path,val,reporter)` where
    - `member_path` is a [member](#member) specified in [member notation](#member-notation);
    - `val` is a variable to validate;
    - `reporter` is a custom [reporter](#reporter) used for [report](#report) construction if validation fails;
- `make_prevalidation_adapter(member_path,val,dst)` where
    - `member_path` is a [member](#member) specified in [member notation](#member-notation);
    - `val` is a variable to validate;
    - `dst` destination object (e.g. string) where to put the validation [report](#report) to, the report will be constructed with the default [reporter](#reporter) if validation fails.

If the [member](#member) used in a *prevalidation adapter* is not found in a [validator](#validator) then the validation will be considered successful.

See example of object setter with validation below.

```cpp
#include <iostream>
#include <dracosha/validator/validator.hpp>
#include <dracosha/validator/adapters/prevalidation_adapter.hpp>
using namespace DRACOSHA_VALIDATOR_NAMESPACE;

// define Foo type
struct Foo
{
    std::string bar_value;
    uint32_t other_value=0;

    void set_bar_value(std::string val)
    {
        bar_value=std::move(val);
    }
};

// define bar_value property
DRACOSHA_VALIDATOR_PROPERTY(bar_value);

// define other_value property
DRACOSHA_VALIDATOR_PROPERTY(other_value);

int main
{

// define validator
auto v=validator(
    _[bar_value](ilex_ne,"UNKNOWN"), // case insensitive lexicographical not equal
    _[other_value](gte,1000)
);

// define object and setter
Foo foo_instance;
auto bar_value_setter = [&v,&foo_instance] (std::string val)
{
    std::string report;
    auto sa=make_prevalidation_adapter(_[bar_value],val,report);
    if (!v.apply(sa))
    {
        std::cerr << report << std::endl;
        return false;
    }

    foo_instance.set_bar_value(std::move(val));
    return true;
};

// validate and set value
auto ok=bar_value_setter("Hello world");
// ok == true

// validate and print error report
ok=bar_value_setter("unknown");
// ok == false
/* prints:
 "bar_value must be not equal to UNKNOWN"
 */

return 0;
}
```

#### Element aggregations with prevalidation adapter

If validator contains [ANY](#any) aggregation there can be some ambiguity how to handle it when pre-validating a single element: should this element satisfy the condition or some other element already satisfied the condition and this element can be skipped? By default, prevalidation adapter interprets `ANY` as always satisfied, i.e. it is assumed that some other element might satisfy the condition. To change the behaviour to strict checking use `strict_any()` wrapper around the value to be validated, i.e. instead of 
- `make_prevalidation_adapter(member_to_set,value_to_set,report);`

use

- `make_prevalidation_adapter(member_to_set,strict_any(value_to_set),report);`.

For [ALL](#all) aggregation the condition is always strictly checked.

#### Bulk prevalidation

Prevalidation adapter can be used to prevalidate a number of values at once. To check multiple values they must be wrapped with [range](#range) helper, see example below. In this case `strict_any` is applied to the entire set of values, i.e. validation succeeds if at least one element satisfies the conditions.

```cpp
#include <dracosha/validator/validator.hpp>
#include <dracosha/validator/adapters/prevalidation_adapter.hpp>
using namespace DRACOSHA_VALIDATOR_NAMESPACE;

int main()
{
    // vector of values to validate
    auto vec1=std::vector<int>{1,2,3,10};
    std::string rep1;
    // prevalidation adapter to validate a number of values with strict_any
    auto pa1=make_prevalidation_adapter(_["field1"]["field1_1"],strict_any(range(vec1)),rep1);
    
    // validate success
    auto v1=validator(
                _["field1"]["field1_1"](ANY(value(gte,9))),
                _["field2"](size(gte,100))
            );
    assert(v1.apply(pa1)); // succeeds because one element is ok
    
    // validate failure
    auto v2=validator(
                _["field1"]["field1_1"](ANY(value(gte,50))),
                _["field2"](size(gte,100))
            );
    assert(!v2.apply(pa1)); // fails because no elements satisfied condition
    
    return 0;
}
```

### Adding new adapter

Base `adapter` template class is defined in `validator/adapters/adapter.hpp` header file. To implement a *custom adapter* the *custom adapter traits* must be implemented that will be used as a template argument in the base `adapter` template class. In addition, if the *custom adapter* supports implicit check of [member existence](#member-existence) then it also must inherit from `check_member_exists_traits_proxy` template class and the *custom adapter traits* must inherit from `with_check_member_exists` template class.

Examples of *custom adapter traits* implementation can be found in `validator/adapters/impl/default_adapter_impl.hpp`, `validator/reporting/reporting_adapter_impl.hpp` and `validator/prevalidation/prevalidation_adapter_impl.hpp`.

Examples of *adapter* implementations can be found in `validator/adapters/default_adapter.hpp`, `validator/adapters/reporting_adapter.hpp` and `validator/adapters/prevalidation_adapter.hpp`. 

## Validation of pointers

Normally, validator is used to validate references to variables. Even when validating [nested members](#nested-members) it is expected that there is a reference to a value at each level of the member's path. 

In addition, pointered values are also supported. If validator sees a pointer then it dereferences the pointer and uses that dereferenced value for validation. Null pointers can be checked with [member existence](#member-existence) - null pointers are assumed to be not existing members.

Validator internally supports plain pointers and `std::shared_ptr` pointers. For custom smart pointers define specialization of `pointer_as_reference_t` template struct. Note that to support null pointer detection a custom smart pointer must be capable of casting to `bool`.

```cpp
#include <QSharedPointer>
#include <dracosha/validator/utils/pointer_as_reference.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

/**
 * Dereference QSharedPointer pointer.
 */
template <typename T>
struct pointer_as_reference_t<QSharedPointer<T>>
{
    using is_pointer=hana::true_;

    template <typename T1>
    auto operator () (T1&& v) const -> decltype(auto)
    {
       return *v;
    }
};

DRACOSHA_VALIDATOR_NAMESPACE_END
```

## Partial validation

Sometimes a validator can be too strict and only a part of its rules needs to be checked on certain object. In this case a filtering validation adapter can be used to check only specific [members](#member) ignoring other paths. There are three forms of defining a filter for such validation:
- use `include_paths()` to list explicitly only paths that must be validated;
- use `exclude_paths()` to list explicitly paths that must be ignored;
- use `include_and_exclude_paths()` to list explicitly paths that must be validated and specify sub-paths that must be excluded from validaton.

See examples below.

```cpp
#include <map>
#include <set>

#include <dracosha/validator/validator.hpp>
#include <dracosha/validator/adapters/default_adapter.hpp>
#include <dracosha/validator/filter_path.hpp>

using namespace DRACOSHA_VALIDATOR_NAMESPACE;

int main()
{
    // original validator
    auto v1=validator(
        _["level1"]["field1"](exists,true),
        _["level1"]["field2"](exists,true),
        _["level2"]["field3"](exists,true)
    );

    // container to validate
    std::map<std::string,std::set<std::string>> m1{
        {"level1",{"field1","field2"}},
        {"level2",{"field4"}},
    };
    // wrap container into default adapter
    auto a1=make_default_adapter(m1);
    
    // validation fails with original validator
    assert(!v1.apply(a1));

    // limit validation paths to a single path
    auto a2=include_paths(a1,
                member_path_list(_["level1"])
            );
    // validation succeeded with partial validation
    assert(v1.apply(a2));

    // exclude failing path from validation
    auto a3=exclude_paths(a1,
                member_path_list(_["level2"]["field3"])
            );
    // validation succeeded
    assert(v1.apply(a3));

    // include paths for validation and exclude failing sub-path
    auto a4=include_and_exclude_paths(a1,
                member_path_list(_["level1"],_["level2"]),
                member_path_list(_["level2"]["field3"])
            );
    // validation succeeded
    assert(v1.apply(a4));

    return 0;
}
```

## Validation of transformed or evaluated values

A value can be passed to some function before validation and the result of that function will be validated instead of the original value. That function could perform any transformations or evaluations on the value. 

To transform a value before validation a special [property](#properties) of value transformer type must be used. Call `make_value_transformer(handler,name)` to create such property, where:
- `handler` is a function taking a value as a single argument and returning result of value transformation or result of some evaluations on the value;
- `name` is a string to be used as a key's name in text reports.

There is also an extended version of this helper to construct value transforming properties that can be used with [flag](#flag) operator: `make_value_transformer(handler,name,flag_string,negative_flag_string)`.

See example below.

```cpp
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
 
    return 0;   
}
```

## Reporting

`cpp-library` can construct text [reports](#report) describing validation errors. To enable construction of error [reports](#report) either [reporting adapter](#reporing-adapter) or [prevalidation adapter](#prevalidation-adapter) should be used. [Reports](#report) can be customized with help of custom [reporters](#reporter) given to the corresponding adapters.

### Reporting adapter

*Reporting adapter* does the same validation as [default adapter](#default-adapter) with addition of constructing a text [report](#report) describing the validation error. *Reporting adapter* is constructed by calling `make_reporting_adapter()` that can have one of the following signatures:
- `make_reporting_adapter(object,reporter)` where
    - `object` is an [object](#object) to validate;
    - `reporter` is a custom [reporter](#reporter) used for [report](#report) constructing if validation fails;
- `make_reporting_adapter(object,dst)` where
    - `object` is an [object](#object) to validate;
    - `dst` destination object (e.g. string) where to put the validation [report](#report) to constructed with the default [reporter](#reporter) if validation fails.

See example below.

```cpp
#include <dracosha/validator/validator.hpp>
#include <dracosha/validator/adapters/reporting_adapter.hpp>
using namespace DRACOSHA_VALIDATOR_NAMESPACE;

int main()
{

auto v=validator(gt,100);
                
std::string report;
size_t val=90;
auto ra=make_reporting_adapter(val,report);

if (!v.apply(ra))
{
    std::cerr << report << std::endl;
    /* prints:
    "must be greater than 100"
    */
}

return 0;
}
```

*Reporting adapter* supports implicit check of [member existence](#member-existence).

### Customization of reports

[Reports](#report) can be customized with help of custom [reporters](#reporter) that are given to adapters supporting [reports](#reports) construction.

#### Report construction

##### Default reporter

A [report](#report) is constructed by a [reporter](#reporter) given to an adapter that supports [reports](#reports) construction. *Default [reporter](#reporter)* is implemented by `reporter` template class defined in `validator/reporting/reporter.hpp` header file. There are two ways to customize report construction:
- implement a custom reporter *from scratch* and give it to the corresponding adapter;
- use default `reporter` template class with custom [formatter](#formatter), a helper function `make_reporter(report_destination,custom_formatter)` can be used for convenience, e.g.:
    ```cpp
    
    // object_for_validation must be defined elsewhere
    // custom_formatter must be defined elsewhere
    
    std::string report;
    
    // create reporting adapter with custom report formatter
    auto ra=make_reporting_adapter(
            object_for_validation,
            make_reporter(report,custom_formatter)
        );
    ```
##### Report with object's name

Typically, [report](#report) does not include a name of the object being validated. For example, `validator(gt,100)` will result in error message "must be greater than 100". If [report](#report) must include the object's name, e.g. "*the object under validation* must be greater than 100", then `reporter_with_object_name` must be used instead of [default reporter](#default-reporter). `reporter_with_object_name` template class is defined in `validator/reporting/reporter_with_object_name.hpp` header file. A helper function `make_reporter_with_object_name()` can be used for convenience. See example below.

```cpp
// object_for_validation must be defined elsewhere

std::string report;

// create reporting adapter with custom report formatter
auto ra=make_reporting_adapter(
        object_for_validation,
        make_reporter_with_object_name(report,get_default_formatter(),"the object under validation")
    );
```

#### Formatters

[Formatter](#formatter) of [reports](#report) uses four components that can be customized:
- [formatter of member names](#members-formatter);
- [formatter of miscellaneous strings](#miscellaneous-strings-formatter) such as descriptions of [special](#special-operators) and [built-in](#built-in-operators) operators as well as keywords used in the library;
- [formatter of operands](#operands-formatter);
- implementation of [final ordering and presentation](strings-order-and-presentation) of strings prepared with the partial formatters listed above.

Actual strings formatting is performed by a [backend formatter](backend-formatter).

Base `formatter` template class is defined in `validator/reporting/formatter.hpp` header file. Default *formatter* implementation can be obtained with `get_default_formatter()`.

Use `make_formatter()` helper to construct *formatters* with custom components and [translators](#translator). There is a number of `make_formatter()` signatures defined in `validator/reporting/formatter.hpp` header file, choose the most suitable for certain cases.

Strings prepared with either top-level *formatter* or partial formatters can be overwritten with [reporting hints](#reporting-hints).

##### Backend formatter

*Backend formatter* is a *variable-to-string* formatter. One of the following string formatters can be used:
- *(preferred)* [fmt](https://github.com/fmtlib/fmt) based backend formatter;
- `std::stringstream` based backend formatter.

To use [fmt](https://github.com/fmtlib/fmt) for strings formatting define `DRACOSHA_VALIDATOR_FMT` macro, see [Building and installation](#building-and-installation) for details of formatter configuration.

##### Members formatter

[Reports](#report) must display human readable names of [members](#members). Member names formatting is performed by a *member names formatter* with base template class `member_names` defined in `validator/reporting/member_names.hpp` header file. For custom *member names formatting* the custom traits must be implemented and used as a template argument of `member_names`. There is `make_member_names()` helper to construct *member names formatter* from the custom traits.

Default implementation of *member names formatter* joins member names in reverse order using *of* conjunctive, e.g. `["field1"]["subfield1_1"]["subfield1_1_1"]` will be formatted as "*subfield1_1_1 of subfield1_1 of field1*". Default member names formatter can be obtained with `get_default_member_names()`.

There is also `dotted_member_names` formatter that displays member names similar to their declaration, e.g. `["field1"]["subfield1_1"]["subfield1_1_1"]` will be formatted as "*[field1].[subfield1_1].[subfield1_1_1]*".

If [localization](#localization) of member names must be supported then original *member names formatter* must be wrapped with *locale-aware member names formatter* using one of `make_translated_member_names()` helpers.

To [decorate](#decorator) member names the original *member names formatter* must be wrapped with *decorating member names formatter* using  `make_decorated_member_names()` helper. See example below.

```cpp
// define validator
auto v=validator(
        ["field1"]["subfield1_1"]["subfield1_1_1"](gt,100)
    );

// wrap default member names formatter with quoted member names decorator
auto decorated_mn=make_decorated_member_names(get_default_member_names(),quotes_decorator);

// object_for_validation must be defined elsewhere

std::string report;

// create reporting adapter with quoted member names
auto ra_decorated=make_reporting_adapter(
        object_for_validation,
        make_reporter(report,make_formatter(decorated_mn))
    );

if (!v.apply(ra))
{
    std::cerr << report << std::endl;
    /* prints:
    '"subfield1_1_1" of "subfield1_1" of "field1" must be greater than 100'
    */
}
```

##### Operands formatter

By default [operands](#operand) are formatted at the discretion of [backend formatter](#backend-formatter). Sometimes [operands](#operand) may require special formatting. The straightforward example is a boolean value that can be displayed in different ways, e.g. as 1/0, true/false, yes/not, checked/unchecked, etc. Another possible example is when the [operands](#operand) must be decorated, e.g. with quotes or HTML tags.

[Operands](#operand) formatting is performed by the *operands formatter* with base template class `operand_formatter` defined in `validator/reporting/operand_formatter.hpp` header file. For custom *operands formatting* the custom traits must be implemented and used as a template argument of `operand_formatter`. There is also `default_operand_formatter` which forwards operands to [backend formatter](#backend-formatter) "as is".

If [localization](#localization) of operands must be supported then *locale-aware operands formatter* must be used that can be constructed with one of `make_translated_operand_formatter()` helpers.

To [decorate](#decorator) operands the *decorating member names formatter* can be constructed with `make_operand_formatter()` or `make_translated_operand_formatter()` helpers. See example below.

```cpp
// define validator
auto v=validator(
        [size](gt,100)
    );

// operands formatter with quoted decorator
auto decorated_operands=make_operand_formatter(quotes_decorator);

// object_for_validation must be defined elsewhere

std::string report;

// create reporting adapter with quoted member names
auto ra_decorated=make_reporting_adapter(
        object_for_validation,
        make_reporter(report,make_formatter(get_default_member_names(),decorated_operands))
    );

if (!v.apply(ra))
{
    std::cerr << report << std::endl;
    /* prints:
    'size must be greater than "100"'
    */
}
```

##### Miscellaneous strings formatter

This formatter is used to format [operators](#operator) and some other strings.

Conversion of a generic *ID* to the string is performed as follows.
- If string *ID* is a [property](#property) then `name()` of the [property](#property) is used.
- If string *ID* is convertible to `std::string` then that conversion is used, e.g. all validation [operators](#operator) are formatted using the *to string conversion operator*.
- If string *ID* is of some scalar type that can be forwarded to `std::to_string(id)` then `std::to_string()` is used.
- In the rest cases "*\<?????\>*" string is used as a result.

After *ID* is converted to the string the conversion result goes to [translator](#translator). Translator returns either translated or original string.

Base template class `strings` for *miscellaneous strings formatting* is defined in `validator/reporting/strings.hpp` header file. Customization of *miscellaneous strings formatting* can be done with use of custom [translators](#translator) or *[aggregation](#aggregation) strings* which are used as template arguments in `strings` template class. Default *miscellaneous strings formatting* is implemented with `default_strings` formatter that uses default `aggregation_strings` formatter and no [translators](#translator).

If [localization](#localization) of miscellaneous strings must be supported then *locale-aware miscellaneous strings formatter* must be used that can be constructed with one of `make_translated_strings()` helpers.

##### Strings order and presentation

Resulting strings from the partial [formatters](#formatters) must be ordered and joined to construct the final [report](#report). By default that is performed by `default_order_and_presentation` helper defined in `validator/reporting/order_and_presentation.hpp` header file.

For custom strings ordering and presentation a custom implementation of the *strings order and presentation* must be used in `formatter` that can be constructed with corresponding `make_formatter()` helper.

#### Decorator

Decorators can be used to decorate [members](#members-formatter) and [operands](#operands-formatter). For example, if one wants to mark [operands](#operands-formatter) with bold text using HTML tags to highlight them in user interface then a `decorator` that wraps operands with `<b>...</b>` might be implemented and used in [operands formatter](#operands-formatter). See examples of `decorator` use in [members formatter](#members-formatter) and [operands formatter](#operands-formatter) sections.

There are two built-in decorators in `cpp-validator` library:
- `brackets_decorator` defined in `validator/reporting/decorator.hpp` header file that decorates string with square brackets, e.g. `text` will be decorated as `[text]`;
- `quotes_decorator` defined in `validator/reporting/quotes_decorator.hpp` header file that decorates string with quotes, e.g. `text` will be decorated as `"text"`.

Adding a new `decorator` consists of the following steps.

1. Define `struct` that will implement *custom decorator*.
2. Add `using hana_tag=decorator_tag;` in the struct.
3. Define in the struct a callable `operator ()` with value to be decorated as an argument. The operator must return decorated input argument.
4. Define `constexpr` callable object with type of the new struct. This object will be used in [members formatters](#members-formatter) and [operands formatters](#operands-formatter).

See example below.

```cpp
#include <dracosha/validator/reporting/decorator.hpp>
using namespace DRACOSHA_VALIDATOR_NAMESPACE;

// define decorator for marking text bold with <b>...</b> tag
struct bold_decorator_t
{
    using hana_tag=decorator_tag;

    template <typename T>
    auto operator () (T&& id) const
    {
        std::string dst;
        backend_formatter.append(dst,"<b>",to_string(std::forward<T>(id)),"</b>");
        return dst;
    }
};
constexpr bold_decorator_t bold_decorator{};
```

### Reporting hints

To override strings constructed by [formatters](#formatters) one can use *hints*. *Hint* is an explicit string that must be used in a [report](#report) for certain part of the report instead of automatically generated string.

#### Override entire report

To override entire error message a reporting *hint* must be attached to the validator using one of the following: 
- call `hint(override_message)` method of validator with the desired error message as an argument;
- invoke validator as a callable object with the desired error message as an argument.

See examples below.

```cpp
#include <map>
#include <dracosha/validator/validator.hpp>
#include <dracosha/validator/adapters/reporting_adapter.hpp>
using namespace DRACOSHA_VALIDATOR_NAMESPACE;

int main()
{

// define reporting adapter
std::map<std::string,size_t> m1={{"field1",1}};
std::string report;
auto ra=make_reporting_adapter(m1,report);

// override error message by invoking hint method of validator
auto v1=validator(
            _["field1"](gte,10)
        ).hint("Invoking hint method of validator object");
if (!v1.apply(ra))
{
    std::cerr << report << std::endl;
    /* prints:
    "Invoking hint method of validator object"
    */
}
report.clear();

// override error message by invoking validator as a callable
auto v2=validator(
            _["field1"](gte,10)
        )("Invoking callable validator object");
if (!v2.apply(ra))
{
    std::cerr << report << std::endl;
    /* prints:
    "Invoking callable validator object"
    */
}
report.clear();

return 0;
}
```

#### Override description of a member validation condition

To override description of a member validation condition a temporary member validator object must be called with string *hint* as an argument, e.g. `_["key1"](gt,100)("reporting hint")`. See examples below.

```cpp
#include <map>
#include <dracosha/validator/validator.hpp>
#include <dracosha/validator/adapters/reporting_adapter.hpp>
using namespace DRACOSHA_VALIDATOR_NAMESPACE;

int main()
{

// define reporting adapter
std::map<std::string,size_t> m1={{"field1",1}};
std::string report;
auto ra=make_reporting_adapter(m1,report);

// define validator overriding description of member validation condition
auto v1=validator(
            _["field1"](gte,10)("Explicit description")
        );
if (!v1.apply(ra))
{
    std::cerr << report << std::endl;
    /* prints:
    "Explicit description"
    */
}
report.clear();

/* define aggregate validator 
overriding description of one of the member validation conditions
*/
auto v2=validator(
            _["field1"](gte,10)
            ^OR^
            _["field1"](eq,100)("Explicit description")
        );
if (!v2.apply(ra))
{
    std::cerr << report << std::endl;
    /* prints:
    '"field1" must be greater or equal to 10 OR Explicit description'
    */
}
report.clear();

return 0;
}
```

#### Override member name

To override [member name](#members-formatter) a temporary [member notation](#member-notation) object must be called with string *hint* as an argument, e.g. `_["key1"]("reporting hint")(gt,100)`. See example below.

```cpp
#include <dracosha/validator/validator.hpp>
#include <dracosha/validator/adapters/reporting_adapter.hpp>
using namespace DRACOSHA_VALIDATOR_NAMESPACE;

int main()
{

// reporting adapter
std::string report;
std::vector<size_t> vec={10,20,30,40,50};
auto ra=make_reporting_adapter(vec,report);

// define validator with member name hint
auto v1=validator(
            _[1]("first element")(gt,100)
        );
if (!v1.apply(ra))
{
    std::cerr << report << std::endl;
    /* prints:
    "first element must be greater than 100"
    */
}
report.clear();

return 0;
}
```

#### Override operator description

To override description of an [operator](#operator) the operator must be wrapped as `_(operator,custom_description)`. To override description of [negated](#_n-negation) operator the `_n(operator,custom_description)` must be used with two arguments. See examples below.

```cpp
#include <dracosha/validator/validator.hpp>
#include <dracosha/validator/adapters/reporting_adapter.hpp>
using namespace DRACOSHA_VALIDATOR_NAMESPACE;

int main()
{

// reporting adapter
std::string report;
std::vector<size_t> vec={10,20,30,40,50};
auto ra=make_reporting_adapter(vec,report);

// define validator with custom descriptions of operators
auto v1=validator(
            _[1](
                value(_(gte,"must be not less than"),100)
                ^OR^
                value(_(eq,"must be the same as"),1)
            )
        );
if (!v1.apply(ra))
{
    std::cerr << report << std::endl;
    /* prints:
    "element #1 must be not less than 100 OR element #1 must be the same as 1"
    */
}
report.clear();

// define validator with custom descriptions of negated operators
auto v2=validator(
            _[1](
                value(_n(gte,"must be less than"),5)
                ^OR^
                value(_n(eq,"must not be the same as"),20)
            )
        );
if (!v2.apply(ra))
{
    std::cerr << report << std::endl;
    /* prints:
    "element #1 must be less than 5 OR element #1 must not be the same as 1"
    */
}
report.clear();

return 0;
}
```

#### Override operand formatting

To override [operand formatting](#operands-formatting) the operand must be wrapped as `_(operand,custom_description)`. See example below.

```cpp
#include <dracosha/validator/validator.hpp>
#include <dracosha/validator/adapters/reporting_adapter.hpp>
using namespace DRACOSHA_VALIDATOR_NAMESPACE;

int main()
{

// reporting adapter
std::string report;
std::vector<size_t> vec={10,20,30,40,50};
auto ra=make_reporting_adapter(vec,report);

// define validator with custom operand string
auto v1=validator(
            _[1](gte,_(100,"one hundred"))
        );
if (!v1.apply(ra))
{
    std::cerr << report << std::endl;
    /* prints:
    "element #1 must be greater or equal to one hundred"
    */
}
report.clear();

return 0;
}
```

### Localization

[Reports](#report) can be translated either with the external translation tools or with the localization tools of `cpp-validator` library.

External translation tools can be used only for translation of the whole final [report](#report). In order to translate the whole [report](#report) with the external translation tools one must give desired translation of the [report](#report) as a [hint to validator](#Override-the-whole-report).

In `cpp-validator` a [report](#report) is usually constructed by a [formatter](#formatter) dynamically *on-the-fly* from the parts prepared with [partial formatters](#formatters). Thus, with `cpp-validator` library the final [report](#report) can not be translated in advance. Instead, only parts from [partial formatters](#formatters) can be translated in advance that will be dynamically [joined](#strings-order-and-presentation) into the final [report](#report) later.

#### Grammatical categories and concrete phrases

A *grammatical category* is a property of items within the grammar of a language (see https://en.wikipedia.org/wiki/Grammatical_category). In some languages different *grammatical categories* of some words can affect the forms of the successive words in a phrase. For example the forms of *go* word are different in phrases *I go*, *they go*, *she **goes***.

In `cpp-validator` library *grammatical categories* are used for more natural [reports](#report) construction. When results from the [partial formatters](#formatters) are being [joined](#strings-order-and-presentation) the results of the successive [partial formatters](#formatters) can depend on *grammatical categories* of the previous results. 

`concrete_phrase` class is used to combine the string result of a [partial formatter](#formatters) and *grammatical categories* of that string actual for the current locale.

*Grammatical categories* are used by [translators](#translator) to select which translations must be used in certain cases. Result of a single translation is returned as `concrete_phrase`. String value of the `concrete_phrase` will be used in the final [report](#report). *Grammatical categories* of the `concrete_phrase` will be used by [translator](#translator) of the next [partial formatter](#formatters) to select corresponding translation.

#### Translators

Translators are used by [partial formatters](#formatters) to translate partial string results. Base `translator` class is defined in `validator/reporting/translator.hpp` header file. 

To construct a [formatter](#formatter) with the same translator for all [partial formatters](#formatters) use `make_formatter(translator_instance)` or `make_formatter(translator_instance,std::true_type())` helpers. Besides, different translators can be used in different [partial formatters](#formatters). See corresponding `make_translated_member_names()`, `make_translated_strings()` and `make_translated_operand_formatter()` helpers.

##### Translator with grammatical categories

`phrase_translator` defined in `validator/reporting/phrase_translator.hpp` can be used as a base class for *grammatical categories* aware translators. Each string in `phrase_translator` can be bound to two types of grammatical categories:
- *grammatical categories* of *preceding phrase* that should be use to select *current phrase*;
- *grammatical categories* of *current phrase* that should be used for translation of the *successive phrase*.
 
*Grammatical categories* of the latter type are stored within current `concrete_phrase`. *Grammatical categories* of the former type are used as selectors of the most suitable phrase translation of given string in the `phrase_translator`. Translator will select the phrase with the maximum number of matching grammatical categories of the former type. See examples in [Adding new locale](#adding-new-locale).

#### Repository of translators

*Translator repository* is a repository of [translators](#translator) mapped to names of locales. `translator_repository` is defined in `validator/reporting/translator_repository.hpp` header file.

To construct a [formatter](#formatter) in which all [partial formatters](#formatters) select translator for locale from the same *translator repository* use `make_formatter(translator_repository_instance,current_locale)` or `make_formatter(translator_repository_instance,current_locale,std::true_type())` helpers. Besides, different *translator repositories* and locales can be used in different [partial formatters](#formatters). See corresponding `make_translated_member_names()`, `make_translated_strings()` and `make_translated_operand_formatter()` helpers.

A name of locale can be either in more generic form like "en" or in more specific form like "en_US.UTF-8". When looking for a [translator](#translator) the *repository* will first try to use the most specific name of locale, and if that is not found then the *repository* will try to use further less specific names down to the name of language only. If still no [translator](#translator) is found then the default translator will be used. A [translator](#translator) should be added to the repository with the full list of all forms of locale names this [translator](#translator) is suitable for. See example below.

```cpp
const auto& translator_en_us=validator_translator_sample();
phrase_translator translator_en_gb;

translator_repository rep;

// translator_en_us will be used for all "en_US" locales as well as for generic "en" language
rep.add_translator(translator_en_us,{"en","en_US","en_US.UTF-8"});
// translator_en_gb will be used for all "en_GB" locales but for the generic "en" language the translator_en_us will be used
rep.add_translator(translator_en_gb,{"en_GB","en_GB.UTF-8"});

auto locale1="en_US";
auto formatter_for_locale1=make_formatter(rep,locale1); 
// formatter_for_locale1 will use translator_en_us

auto locale2="en_GB";
auto formatter_for_locale2=make_formatter(rep,locale2); 
// formatter_for_locale2 will use translator_en_gb

auto locale3="en";
auto formatter_for_locale3=make_formatter(rep,locale3); 
// formatter_for_locale3 will use translator_en_us
```

#### Adding new locale

To add a new locale the `phrase_translator` for that locale must be populated.

As a sample of translator of phrases defined in `cpp-validator` library the `validator_translator_sample()` helper can be used which is defined in `validator/reporting/locale/sample_locale.hpp`. To add new language or locale copy that file, rename `validator_translator_sample()` to something like `translator_of_<locale_name>` (e.g. `translator_of_de`) and replace its phrases with the translation of the phrases for target locale.

If translation of a phrase depends on *grammatical categories* of the previous phrase then add translation with taking into account that dependency as follows:
```cpp
m[original]={
               {"default translation"},
               {"translation for grammatical category 1", grammatical_category1},
               {"translation for combination of grammatical category 1 and grammatical category 2", grammatical_category1, grammatical_category2}
          };
```
See example for value and plural values:
```cpp
m[value.name()]={
                    {"value"},
                    {"values",grammar::plural} // use "values" translation if preceding grammatical category is grammar::plural
                };
```

If translation of successive phrase can depend on *grammatical categories* of the current translated phrase then translation must be added as follows:
```cpp
m[original]={
               {{"default translation",grammatical_category_to_use_for_next_phrase}},
               {{"translation for grammatical_category_to_select_current_translation", grammatical_category_to_use_for_next_phrase}, grammatical_category_to_select_current_translation},
               {
                   {
                       "translation for combination of grammatical_category_to_select_current_translation1 and grammatical_category_to_select_current_translation2",
                       grammatical_category_to_use_for_next_phrase3, grammatical_category_to_use_for_next_phrase4
                   },
                   grammatical_category_to_select_current_translation1,
                   grammatical_category_to_select_current_translation2
               }
          };
```

See example for plural values:
```cpp
m["values"]={
               {{"values",grammar::plural}} // use this grammatical category grammar::plural for next phrase
            };
```

See example for value and plural values:
```cpp
m["value"]={
               {"value"}
               {
                   {"values",grammar::plural}, // use grammatical category grammar::plural for next phrase
                   grammar::plural // use "values" translation if preceding grammatical category is grammar::plural
               }
            };
```

Another example of custom locale can be found in `validator/reporting/locale/ru.hpp` that implements translations for Russian locale.

#### Localization example

Below is an example of custom localization for Russian language. This example uses some grammatical categories of Russian language listed in `grammar_ru` enumeration.

```cpp
#include <map>
#include <iostream>
#include <dracosha/validator/validator.hpp>
#include <dracosha/validator/reporting/extend_translator.hpp>
#include <dracosha/validator/reporting/locale/ru.hpp>

using namespace DRACOSHA_VALIDATOR_NAMESPACE;

// define custom trasnlator of container keys for Russian language taking into account some Russian grammatical categories
phrase_translator custom_key_translator;
custom_key_translator["password"]={
                    {"пароль"},
                    {"пароля",grammar_ru::roditelny_padezh}
               };
custom_key_translator["hyperlink"]={
                    {{"гиперссылка",grammar_ru::zhensky_rod}},
                    {{"гиперссылки",grammar_ru::zhensky_rod},grammar_ru::roditelny_padezh}
                };
custom_key_translator["words"]={
                {{"слова",grammar_ru::mn_chislo}}
            };

/*
final translator merges predefined Russian translator
validator_translator_ru() and custom translator for element names defined above
*/
auto final_key_translator=extend_translator(validator_translator_ru(),custom_key_translator);

// container to validate
std::map<std::string,std::string> m1={
    {"password","123456"},
    {"hyperlink","zzzzzzzzz"}
};

// define reporting adapter that will generate error messages in Russian language
std::string rep;
auto ra1=make_reporting_adapter(m1,make_reporter(rep,make_formatter(final_key_translator)));

// examples of error generation in Russian language with the reporting adapter defined above

auto v1=validator(
    _["words"](exists,true)
 );
if (!v1.apply(ra1))
{
    std::cerr<<rep<<std::endl;
    /*
    prints:
    "слова должны существовать"
    */
}
rep.clear();

auto v2=validator(
    _["hyperlink"](eq,"https://www.boost.org")
 );
if (!v2.apply(ra1))
{
    std::cerr<<rep<<std::endl;
    /*
    prints:
    "гиперссылка должна быть равна https://www.boost.org"
    */
}
rep.clear();

auto v3=validator(
    _["password"](length(gt,7))
 );
if (!v3.apply(ra1))
{
    std::cerr<<rep<<std::endl;
    /*
    prints:
    "длина пароля должна быть больше 7"
    */
}
rep.clear();

auto v4=validator(
    _["hyperlink"](length(lte,7))
 );
if (!v4.apply(ra1))
{
    std::cerr<<rep<<std::endl;
    /*
    prints:
    "длина гиперссылки должна быть меньше или равна 7"
    */
}
rep.clear();
```

# Performance considerations

## Validation overhead

Normally, validator should add very little overhead compared to manual coding of the same checks because significant part of preparations is made and optimized at compilation time.

Still, there are some considerations that should be taken into account.
For example, have a look at four validators below. All of them logically do the same verification: a value of nested member must be greater or equal to 100 and less or equal to 1000. Nevertheless, the first validator has the worst performance and the fourth validator has the best performance. The second and the third validators are equivalent.

```cpp
    auto v1=validator(
        _[key1][key2][key3](gte,100),
        _[key1][key2][key3](lte,1000)
    );
    
    auto v2=validator(
        _[key1][key2][key3](value(gte,100) ^AND^ value(lte,1000))
    );

    auto v3_1=validator(
        _[key3](value(gte,100) ^AND^ value(lte,1000))
    );
    auto v3=validator(
        _[key1][key2](v3_1)
    );
    
    auto v4=validator(
        _[key1][key2][key3](in, interval(100,1000))
    );
```

- The first validator extracts a value of the nested member twice because each member in a validator is evaluated independently. If object is a nested container with huge number of elements then that extraction overhead can be noticeable.
- The second validator extracts a value only once and then invokes two operators one by one using logical aggregation which is quite fast but can be a little bit more expensive than the fourth variant.
- The third validator pre-extracts intermediate value of a partial member's path and then invokes nested validator that finishes value extraction and applies final validation. As a result, full actual validation procedure and its speed are the same as with the second validator.
- The fourth validator extracts a value once and invokes a single "atomic" operator which is the most effective way to solve this sample task. 

Though, it is a rare case when two operators can be narrowed down to a single operator like in the forth case.
In general, a rule of thumb is to use [logical aggregations](#logical-aggregations) and/or [nested validators](#nested-validators) at a member level to avoid repetitive value extractions at the same member path.

## Zero copy

A `validator` tries to do as little data copying as possible. All variables provided to validators are used by references. Thus, a user is responsible for the variables to stay valid during life time of a validator. If it is not possible then a variable must be explicitly moved or copied to a validator. Note that moved/copied values owned by the validator can be implicitly copied or moved multiple times during some validator operations - for example, when a nested member is constructed the keys of parent member path are moved or copied to the elements of the child member path.

```cpp
#include <dracosha/validator/validator.hpp>
#include <dracosha/validator/utils/copy.hpp>

using namespace DRACOSHA_VALIDATOR_NAMESPACE;

// ....

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
```

## Checking member existence before validation

By default only types compatibility is checked before validation. This can lead to exceptions or other errors or undefined behaviour in case the validation operation is requsted on the member that doesn't exist. To avoid such problems explicit or implicit checks of member existence can be used, see [Member existence](#member-existence). Though, those checks can add some overhead because a value might be looked up twice - the first one is to check existence and the second one is to extract a value for validation. So, it depends on the use case whether to enable or not enable pre-checking of member existence.

## Validation with text reports

Building text reports sometimes can add meaningful overhead because construction of the reports can be rather complicated at certain cases. Therefore, in some scenarios it is reasonable to use double run of validation: first, validate data without text report, and then use validation with text reports only on already failed data just to construct a report.

# Building and installation

`cpp-validator` is a header-only library, so no special library building is required. Still, some extra configuration may be required when using the library.

For `CMake` build system there is `CMakeLists.txt` project file in the library's root folder. Add `cpp-library` folder as a subdirectory to your `CMake` project and configure build parameters, see [CMake configuration](#cmake-configuration).

For the rest build systems ensure that `include` subfolder of `cpp-library` is available for your compiler in the list of include paths. [Boost](http://boost.org) libraries must also be in the paths. To use [fmt](https://github.com/fmtlib/fmt) library as a [backend formatter](#backend-formatter) add definition of macro `DRACOSHA_VALIDATOR_FMT` to compiler command line and ensure that [fmt](https://github.com/fmtlib/fmt) library is available in compiler's paths. When compiling with `MSVC` compiler flags `/Zc:ternary` and `/Zc:__cplusplus` are required.

## Supported platforms and compilers

A compiler must support at least C\+\+14 or C\+\+17 standards to build `cpp-validator` library. The library was tested with the following platforms and minimal compiler versions:

- `Windows`:
    - `MSVC` 14.2;
    - `MinGW` 9.2.0;
- `Linux`:
    - `Clang` 10.0.0;
    - `GCC` 9.3.0;
- `macOS`:
    - `Apple Clang` 10.0.1;
- `iOS`:
    - `Apple Clang` 10.0.1;
- `Android`:
    - `Android Clang` 8.0.7.

## Dependencies

`cpp-validator` library strictly depends only on the [Boost](http://boost.org) libraries, version 1.65 or above, in particular on the following libraries:
- `Boost.Hana`;
- `Boost.Regex` only if [regular expression](builtin_operators.md#regular-expressions) or [string pattern](#string-patterns) operators are used;
- `Boost.Test` only for testing.

Also `cpp-validator` library can depend on [fmt](https://github.com/fmtlib/fmt) library, version 7.0.0 or above, if [fmt](https://github.com/fmtlib/fmt) is used as a [backend formatter](#backend-formatter) which is recommended.

## CMake configuration

To use `cpp-validator` library with `CMake` build system do the following:

- ensure that [Boost](http://boost.org) libraries can be found by `CMake`;
- add `cpp-library` folder as a subdirectory to your `CMake` project:
    ```
    ADD_SUBDIRECTORY(cpp-validator)
    ```
- add `dracoshavalidator` library dependency to your project:
    ```
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} dracoshavalidator ${Boost_LIBRARIES})
    ```
- configure the following optional `CMake` parameters if needed:
    - `VALIDATOR_WITH_FMT` - *ON*|*OFF* - enable [fmt](https://github.com/fmtlib/fmt) library for [report](#report) [formatting](#backend-formatter) - default is *ON*;
    - `FMT_ROOT` - path to folder where [fmt](https://github.com/fmtlib/fmt) library is installed;
    - `FMT_INCLUDE_DIRECTORY` - path to headers of [fmt](https://github.com/fmtlib/fmt) library if `FMT_ROOT` is not set;
    - `FMT_HEADER_ONLY` - *OFF*|*ON* - mode of [fmt](https://github.com/fmtlib/fmt) library - default is *OFF*;
    - `FMT_LIB_DIR` - path to folder with built [fmt](https://github.com/fmtlib/fmt) library if `FMT_ROOT` is not set and `FMT_HEADER_ONLY` is off;
    - `VALIDATOR_WITH_TESTS` - *OFF*|*ON* - build with tests - default is *OFF*;
    - `VALIDATOR_WITH_EXAMPLES` - *OFF*|*ON* - build with examples - default is *OFF*.

## Building and running tests and examples

Sample scripts for tests building and running are located in `sample-build` folder:
- `win-msvc.bat` to build and run tests on Windows with MSVC compiler;
- `linux-clang.sh` to build and run tests on Linux with Clang compiler.

Ensure that variables in a script fit your environment and edit the following parameters if required:

- `BOOST_ROOT` - root folder where  [Boost](http://boost.org) is installed;
- `FMT_ROOT` - root folder where [fmt](https://github.com/fmtlib/fmt) is installed;
- `BUILD_TYPE` - build type which can be one of *Release*|*Debug*;

- `CXX` (Linux only) - name of C++ compiler (e.g. *clang++*);

- `CMAKE_PATH` (Windows only) - path where CMake is installed on Windows (e.g. *C:\Program Files\CMake\bin*);
- `MSVC_ROOT` (Windows only) - path where Microsoft Visual C++ is installed (e.g. *C:\Program Files (x86)\Microsoft Visual Studio\2019\Community*);
- `MSVC_COMPILER` (Windows only) - string representing version of MSVC compiler (e.g. *v142*);
- `COMPILER_VERSION` (Windows only) - version of MSVC compiler (e.g. *14.2*).

Run a script corresponding to your platform from a folder where source folder `cpp-validator` resides, for example go to folder `cpp-validator/../` and run `cpp-validator/sample-build/linux-clang.sh`.

# License

&copy; Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
See accompanying file [LICENSE.md](../LICENSE.md) or copy at http://boost.org/LICENSE_1_0.txt.

# Contributing

See accompanying file [CONTRIBUTING.md](../CONTRIBUTING.md).