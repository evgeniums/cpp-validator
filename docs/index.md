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
	* [Using validator for data validation](#using-validator-for-data-validation)
		* [Post-validation](#post-validation)
			* [validate() without report and without exception](#validate-without-report-and-without-exception)
			* [validate() with report but without exception](#validate-with-report-but-without-exception)
			* [validate() with exception](#validate-with-exception)
			* [Apply validator to adapter](#apply-validator-to-adapter)
			* [Apply validator to object](#apply-validator-to-object)
		* [Pre-validation](#pre-validation)
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
		* [Adding new property](#adding-new-property)
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
	* [Adapters](#adapters)
		* [Default adapter](#default-adapter)
		* [Single member adapter](#single-member-adapter)
		* [Adding new adapter](#adding-new-adapter)
	* [Reporting](#reporting)
		* [Reporting adapter](#reporting-adapter)
		* [Customization of reports](#customization-of-reports)
			* [Report construction](#report-construction)
			* [Formatters](#formatters)
			* [Decorator](#decorator)
		* [Reporting hints](#reporting-hints)
			* [Override the whole report](#override-the-whole-report)
			* [Override description of a member validation condition](#override-description-of-a-member-validation-condition)
			* [Override member name](#override-member-name)
			* [Override operator description](#override-operator-description)
			* [Override operand formatting](#override-operand-formatting)
		* [Localization](#localization)
			* [Concrete phrases and grammatical categories](#concrete-phrases-and-grammatical-categories)
			* [Translators](#translators)
			* [Translator repository](#translator-repository)
			* [Adding new locale](#adding-new-locale)
* [Building and installation](#building-and-installation)
	* [Supported platforms and compilers](#supported-platforms-and-compilers)
	* [Dependencies](#dependencies)
	* [CMake configuration](#cmake-configuration)
	* [Building and running tests](#building-and-running-tests)
* [License](#license)
* [Contributing](#contributing)

[//]: # (TOC End)

----

# Introduction

`cpp-validator` is a modern **C++** header-only library for validation of variables, objects and containers.

`cpp-validator` can be used to validate:
- plain variables;
- properties of objects, where a property can be accessed either as object's variable or object's method;
- contents and properties of containers;
- nested containers and objects.

Basic usage of the library includes a few steps:

- first, define a [validator](#validator) using almost declarative syntax;
- then, apply the [validator](#validator) to [object](#object) that must be validated;
- finally, check the results and print [report](#report) if applicable.

The library is suitable for both *post-validation* and *pre-validation*. *Post-validation* stands for validating the object that is already populated with the data. [*Pre-validation*](#single-member-adapter) stands for validating the data before writing it to the object. The same validator declaration can be used in both cases.

# Definitions

##### *Adapter*
Customizable validation implementer. Adapter performs actual validation when the [validator](#validator) is applied to an [object](#object).

##### *Aggregation*
Combination of validating [operators](#operator) or other [validators](#validator) that make up a compound [validator](#validator). There are logical aggregations such as _AND_, _OR_, _NOT_ and [element](#element) aggregations such as _ANY_, _ALL_.

##### *Concrete phrase*
Immutable string that will be used in final [report](#report).

##### *Element*
A special type of [member](#member) that points to an element of container when the [object](#object) is a container.

##### *Formatter*
Customizable implementer of [reports](#report) formatting. See [formatters](#formatters) section.

##### *Grammatical category*
A grammatical category is a property of items within the grammar of a language (see https://en.wikipedia.org/wiki/Grammatical_category). In some languages different grammatical categories of some words can affect the forms of the successive words in a phrase. In this library grammatical categories are used for more natural [reports](#report) construction.

##### *Member*
An [element](#element) or a [property](#property) of [object](#object) the validation must be applied to. Members can be either direct (single level depth) or nested (multi level depth).

##### *Object* 
Variable that must be validated. It can be a scalar variable, C++ object or container.

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
    * whole object itself;
    * [member(s)](#member) of the object.

2. [Property](#property) of selected object's part that must be verified. By default a special pseudo property *value* is used which means that validation must be applied to the variable "as is". The library provides a few other predefined properties such as *size*, *length* and *empty*. Custom properties can also be defined, see [Properties](#properties).

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

## Using validator for data validation

### Post-validation

*Post-validation* here stands for validating the object that is already populated with the data. The simplest way to validate an [object](#object) is to use `validate()` helper. There are forms `validate` with *exceptions* and without *exceptions*, with [report](#report) and without [report](#report).

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

validate(200,v,err))
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

validate(200,v,err))
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

*Pre-validation* here stands for validating the data before writing it to the object. The simplest way to validate data before writing it to the object is to use `set_validated`. To customize data *pre-validation* use [single member adapter](#single-member-adapter).

To use `set_validated` with custom [properties](#property) a template specialization of property setter `set_member_t` in `DRACOSHA_VALIDATOR_NAMESPACE` must be defined first. `set_validated` can be used both with and without exceptions.

See examples below.

```cpp
#include <dracosha/validator/validator.hpp>
#include <dracosha/validator/validate.hpp>

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
set_validated(foo_instance,bar_value,"Hello world",v,err);
if (!err)
{
    // object's member is set
}

// call setter with invalid data
set_validated(foo_instance,bar_value,"unknown",v,err);
if (err)
{
    // object's member is not set
    std::cerr << err.message() << std::endl;
    /* prints:
     
     "bar_value must be not equal to UNKNOWN"
     
     */
}

return 0;
} 
```

## Members

Members are used to define what parts of [objects](#object) must be validated. A [member](#member) can point to one of the following:
- member variable of C++ object;
- member method of C++ object, where the method must be of getter type, i.e. it must return some value and have no arguments;
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

A [member](#member) name must be placed within square brackets that follow an underscore symbol `_` which is defined in `cpp-validator` library `namespace`.
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

To validate members of nested objects or containers a compound member notation must be used, where name of the member at each level is placed within square brackets and appended to the upper member resulting in a `member path`. See examples below.

```cpp
#include <dracosha/validator/validator.hpp>
using namespace DRACOSHA_VALIDATOR_NAMESPACE;

// 3 levels
auto member_path_3_levels=_[level1][level2][level3];

// nested container elements
auto member_path_nested_container=_["element1"]["element1_1"]["element1_1_1"];
```

```cpp
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

A [member](#member) existence can also be checked implicitly before applying validation conditions to the [member](#member). Such check is performed by an [adapter](#adapter) if the [adapter](#adapter) supports that. [Default adapter](#default-adapter) and [reporting adapter](#reporting-adapter) provide such feature that can be configured with  `set_check_member_exists_before_validation` and `set_unknown_member_mode` adapter methods. 

Method `set_check_member_exists_before_validation` enables/disables implicit checking of member existence. By default this option is disabled which improves validation performance but can sometimes cause exceptions or other undefined errors. Besides, some basic checking if a member can be found for given object type is performed statically at compile time regardless of this flag.

Method `set_unknown_member_mode` instructs adapter what to do if a member is not found. There are two options:
- ignore not found members and continue validation process;
- abort validation process with error.

See examples below.

```cpp
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

`value` property stands for the variable itself. This is a special pseudo property which means that validation must be applied to the variable "as is". See examples how the `value` property can be used in section [validator with aggregations](#validator-with-aggregations).

#### *size*

`size` property is used to validate result of `size()` method or `size` member variable of an [object](#object).

#### *length*

`length` property is used to validate result of `length()` method or `length` member variable of an [object](#object).

#### *empty*

`empty` property is used to validate result of `empty()` method or `empty` member variable of an [object](#object).

### Adding new property

A new [property](#property) can be added using special macros defined in `cpp-validator` library. 

A [property](#property) of non-boolean type must be defined using `DRACOSHA_VALIDATOR_PROPERTY` macro which has only one argument for name of the [property](#property).

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

If a [property](#property) is of boolean type and must be capable of being used with [flag](#flag) operator then the property must be defined with `DRACOSHA_VALIDATOR_PROPERTY_FLAG` macro which has three arguments:
1. name of the [property](#property);
2. description of positive [flag](#flag);
3. description of negative [flag](#flag).

```cpp
#include <iostream>
#include <dracosha/validator/property.hpp>
#include <dracosha/validator/validator.hpp>
#include <dracosha/validator/adapter/reporting_adapter.hpp>
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

int main()
{

// define flaggable red_color property
DRACOSHA_VALIDATOR_PROPERTY_FLAG(red_color,"Must be red","Must be not red");

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

## Operators

[Operators](#operator) define atomic validation conditions in [validators](validator). 

[Operator](#operator) is a callable object that returns `true` if validation condition is satisfied, `false` otherwise. In default [adapter](#adapter) implementations an [operator](#operator) is called with two arguments when [validator](#validator) is applied to the [adapter](#adapter):
* the first argument is a corresponding [property](#property) of the [object](#object) or object's [member](#member) that must be validated;
* the second argument is an [operand](#operand) that must be used as a sample for validation.

### Special operators

#### *exists*

Operator `exists` is used to check explicitly if an [object](#object) contains some [member](#member). See example below.

```cpp
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

Operator `flag` is a special case of equality operator for boolean arguments. The main purpose of defining separate operator `flag` in addition to operator [eq](builtin_operators.md#eq) is more flexible [reports](#report) construction. With operator [eq](builtin_operators.md#eq) a [report](#report) would always use "must be equal to" string. With operator `flag` report strings can be customized depending on the [property](#property) or explicitly preset strings, e.g. "must be checked" or "must be set" and so on can be used in different places.

There are three ways of string customization for `flag` operator.

1. Define [custom property](#adding-new-property) using `DRACOSHA_VALIDATOR_PROPERTY_FLAG` macro where the second argument is a reporting string for `<custom property>(flag,true)` condition and the third argument is a reporting string for `<custom property>(flag,false)` condition. See [example](#adding-new-property).

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
#include <dracosha/validator/validator.hpp>
#include <dracosha/validator/operators.hpp>
using namespace DRACOSHA_VALIDATOR_NAMESPACE;

int main()
{

// container to validate
std::map<std::string,uint32_t> m1={{"key1",100}};

// validator with original operator
auto v1=validator(
    ["key1"](eq,100)
);
assert(v1.apply(m1));

// validator with negated operator
auto v2=validator(
    ["key1"](_n(eq),100)
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
3. Define `description` and `n_description` as `constexpr static const char*` variables of the the struct that will be used as human readable error descriptions in [report](#report) if this operator returns `false` during validation: `description` is used for the operator itself and `n_description` is used for negation of this operator.
4. Define `constexpr` callable object with type of the new struct. This object will be used in [validators](#validator).

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

Value of an [operand](#operand) is given as the second argument to an [operator](#operator) and is meant to be used as a validation sample. An [operand](#operand) can be one of:
- constant or `lvalue` or `rvalue` [variable](#variables);
- [lazy](#lazy-operands) operand;
- [other member](#other-members) of the [object](#object) under validation;
- [the same member of sample object](#sample-objects);
- [interval](#intervals);
- [range](#ranges).

### Variables

The most common case is when an [operand](#operand) is either constant or `lvalue` or `rvalue` variable. See examples below.

```cpp
// operand is constant
auto v1=validator(gt,100);

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

Range [operand](#operand) is a searchable set of elements. Range [operand](#operand) can be used only with [in](#in) and [nin](#nin) operators. With [in](#in) operator it is used to check if a variable matches one of the elements in the `range`.

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
- construct range in-line, e.g. 
    ```cpp
    auto v1=validator(in,range({10,5,9,100}));
    ```
- construct sorted range in-line, e.g. 
    ```cpp
    auto v1=validator(in,range({1,2,3,4,5},sorted));
    ```
Sorted and unsorted ranges differ in processing: for sorted ranges `std::binary_search` is used while for unsorted ranges `std::find_if` is used which is slower than `std::binary_search`.

In [reporting](#report) a `range` is formatted as "range [x[0], x[1], ... , x[N]]", where x[i] denotes i-th element of the container. To limit a number of elements in a [report](#report) one should use `range` with additional integer argument that stands for `max_report_elements`. If  `max_report_elements` is set then at most `max_report_elements` will be used in [report](#report) formatting and ", ... " will be appended to the end of the list. See examples below.

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
- `inline notation` where validation conditions are joined using aggregation conjunction made up of aggregation keyword surrounded with `^`, e.g. `op1 ^AND^ op2 ^AND^ op3`.

### Logical aggregations

Logical aggregation is a combination of validating [operators](#operator) or other [validators](#validator) that make up a compound [validator](#validator). Logical aggregations can be used to combine validation conditions for both [objects](#object) and [members](#member). See examples of logical aggregations in [Validator with aggregations](#validator-with-aggregations) section.

#### AND

`AND` aggregation is used when all validation conditions must be satisfied. In addition to `functional notation` and `inline notation` the `AND` aggregation can be implicitly invoked when validation conditions in a [validator](#validator) for the whole object are listed separated with commas. See examples below.

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

// inline notation in validation condition of a member
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

// inline notation in validation condition of whole object
auto v2=validator(
        value(eq,1) ^OR^ value(eq,100)
    );

// validator is satisfied when "key1" element of variable is equal to 1 or is equal to 100

// functional notation in validation condition of a member
auto v3=validator(
        _["key1"](OR(value(eq,1),value(eq,100)))
    );

// inline notation in validation condition of a member
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

Element aggregations are used when the same validation conditions must be applied to multiple elements of container. Element aggregations can use only `functional notation` with single argument.

#### ANY

`ANY` aggregation is used when at least one element of container must satisfy a condition. See examples below.

```cpp
// at least one element of container must be equal to "unknown" 
// or must have size greater or equal to 5
auto v1=validator(
        ANY(value(eq,"unknown") ^OR^ size(gte,5))
    );

// at least one element of cntainer at "key1" element must be equal to "unknown" 
// or must have size greater or equal to 5
auto v2=validator(
        _["key1"](ANY(value(eq,"unknown") ^OR^ size(gte,5)))
    );
```

#### ALL

`ALL` aggregation is used when each element of container must satisfy a condition. See examples below.

```cpp
// each element of container must be equal to "unknown" 
// or must have size greater or equal to 5
auto v1=validator(
        ALL(value(eq,"unknown") ^OR^ size(gte,5))
    );

// each element of container at "key1" element must be equal to "unknown" 
// or must have size greater or equal to 5
auto v2=validator(
        _["key1"](ALL(value(eq,"unknown") ^OR^ size(gte,5)))
    );
```

## Adapters

[Adapters](#adapter) perform actual processing of validation conditions specified in [validators](#validator). To invoke validation with a specific adapter a [validator](#validator) must be applied to the adapter. Adapters implemented in the `cpp-validator` library use [operators](#operator) as callable objects to check validation conditions. However, adapters of other types can also be implemented, e.g. adapters that construct SQL queries equivalent to validation conditions specified in [validators](#validator).

There are three built-in adapter types implemented in `cpp-validator` library:
- [default adapter](#default-adapter) that applies validation to an [object](#object) by invoking [operators](#operator) one by one as specified in a [validator](#validator);
- [reporting adapter](#reporting-adapter) that does the same as [default adapter](#default-adapter) with addition of constructing a [report](#report) describing an error if validation fails;
- [single member adapter](single-member-adapter) that validates only one [member](#member) and constructs a [report](#report) if validation fails.

### Default adapter

*Default adapter* wraps *lvalue* reference to the [object](#object) to be validated and invokes [operators](#operator) one by one as specified in a [validator](#validator). To make a *default adapter* call `make_default_adapter(object_to_validate)` with the [object](#object) as an argument. If a [validator](#validator) is applied directly to the [object](#object) then *default adapter* is constructed implicitly. See examples in [Using validator for data validation](#using-validator-for-data-validation) section.

*Default adapter* supports implicit check of [member existence](#member-existence).

### Single member adapter

*Single member adapter* validates only a single member. This adapter is best suitable for *pre-validation*, i.e. validating the data before updating an object in the object's setters. *Single member adapter* is constructed by calling `make_single_member_adapter()` helper that can have one of the following signatures:
- `make_single_member_adapter(member_path,val,reporter)` where
    - `member_path` is a [member](#member) specified in [member notation](#member-notation);
    - `val` is a variable to validate;
    - `reporter` is a custom [reporter](#reporter) used for [report](#report) construction if validation fails;
- `make_single_member_adapter(member_path,val,dst)` where
    - `member_path` is a [member](#member) specified in [member notation](#member-notation);
    - `val` is a variable to validate;
    - `dst` destination object (e.g. string) where to put the validation [report](#report) to constructed with the default [reporter](#reporter) if validation fails.

If the [member](#member) used in a *single member adapter* is not found in a [validator](#validator) then the validation will be considered as successful.

See example of object setter with validation below.

```cpp
#include <iostream>
#include <dracosha/validator/validator.hpp>
#include <dracosha/validator/adapters/single_member_adapter.hpp>
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
    auto sa=make_single_member_adapter(_[bar_value],val,report);
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

### Adding new adapter

Base `adapter` template class is defined in `validator/adapters/adapter.hpp` header file. To implement a *custom adapter* the *custom adapter traits* must be implemented that will be used as a template argument in the base `adapter` template class. In addition, if the *custom adapter* supports implicit check of [member existence](#member-existence) then it also must inherit from `check_member_exists_traits_proxy` template class and the *custom adapter traits* must inherit from `with_check_member_exists` template class.

Examples of *custom adapter traits* implementation can be found in `validator/detail/default_adapter_impl.hpp`, `validator/detail/reporting_adapter_impl.hpp` and `validator/detail/single_member_adapter_impl.hpp`.

Examples of *custom adapter* implementation can be found in `validator/adapters/default_adapter.hpp`, `validator/adapters/reporting_adapter.hpp` and `validator/adapters/single_member_adapter.hpp`. 

## Reporting

`cpp-library` can construct text [reports](#report) describing validation errors. To enable construction of error [reports](#report) either [reporting adapter](#reporing-adapter) or [single member adapter](#single-member-adapter) should be used. [Reports](#report) can be customized with help of custom [reporters](#reporter) given to the corresponding adapters.

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

Typically, [report](#report) does not include a name of the object being validated. For example, `validator(gt,100)` will result in error message "must be greater than 100". If [report](#report) must include the object's name, e.g. "*the object under validation* must be greater than 100", then `reporter_with_object_name` must be used instead of [default reporter](#default-reporter). `reporter_with_object_name` template class is defined `validator/reporting/reporter_with_object_name.hpp` header file. A helper function `make_reporter_with_object_name()` can be used for convenience. See example below.

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

Default implementation of *member names formatter* joins member names in reverse order using *of* conjunction, e.g. `["field1"]["subfield1_1"]["subfield1_1_1"]` will be formatted as "*subfield1_1_1 of subfield1_1 of field1*". Default member names formatter can be obtained with `get_default_member_names()`.

There is also `dotted_member_names` formatter that displays member names similar to their declaration, e.g. `["field1"]["subfield1_1"]["subfield1_1_1"]`` will be formatted as "*[field1].[subfield1_1].[subfield1_1_1]*".

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

By default [operands](#operand) are formatted at the discretion of [backend formatter](#backend-formatter). Sometimes [operands](#operand) may require special formatting. The obvious example is a boolean value which can be displayed in different ways, e.g. as 1/0, true/false, yes/not, checked/unchecked, etc. Another example is when the [operands](#operand) must be decorated, e.g. with quotes or HTML tags.

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
        backend_formatter.append(dst,"<b>",detail::to_string(std::forward<T>(id)),"</b>");
        return dst;
    }
};
constexpr bold_decorator_t bold_decorator{};
```

### Reporting hints

To override strings constructed by [formatters](#formatters) one can use *hints*. *Hint* is an explicit string that must be used in a [report](#report) for certain part of the report instead of automatically generated string.

#### Override the whole report

To override the whole error message a reporting *hint* must be attached to the validator using one of the following: 
- call `hint(override_message)` method of validator with the desired error message as an argument;
- invoke validator as a callable object with the desired error message as an argument.

See examples below.

```cpp
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

#### Concrete phrases and grammatical categories

#### Translators

#### Translator repository

#### Adding new locale

# Building and installation

`cpp-validator` is a header-only library, so no special library building is required. Still, some extra configuration may be required when using the library.

For `CMake` build system there is `CMakeLists.txt` project file in the library's root folder. Add `cpp-library` folder as a subdirectory to your `CMake` project and configure build parameters, see [CMake configuration](#cmake-configuration).

For the rest build systems ensure that `include` subfolder of `cpp-library` is available for your compiler in the list of include paths. [Boost](http://boost.org) libraries must also be in the paths. To use [fmt](https://github.com/fmtlib/fmt) library as a [backend formatter](#backend-formatter) add definition of macro `DRACOSHA_VALIDATOR_FMT` to compiler command line and ensure that [fmt](https://github.com/fmtlib/fmt) library is available in compiler's paths. When compiling with `MSVC` don't forget to add `/Zc:ternary` compiler flag.

## Supported platforms and compilers

A compiler must support at least C\+\+14 or C\+\+17 standards to build `cpp-validator` library. The library was tested with the following platforms and compilers:

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

`cpp-validator` library strictly depends only on the [Boost](http://boost.org) libraries, version 1.65 and above, in particular on the following libraries:
- `Boost.Hana`;
- `Boost.Regex` only if [lexicographical operators](builtin_operators.md#lexicographical-operators) are used;
- `Boost.Test` only for testing.

Also `cpp-validator` library can depend on [fmt](https://github.com/fmtlib/fmt) library, version 7.0.0 and above, if [fmt](https://github.com/fmtlib/fmt) is used as a [backend formatter](#backend-formatter) which is recommended.

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
    - `VALIDATOR_WITH_FMT` - *On*|*Off* - enable [fmt](https://github.com/fmtlib/fmt) library for [report](#report) [formatting](#backend-formatter) - default is **On**;
    - `FMT_ROOT` - path to folder where [fmt](https://github.com/fmtlib/fmt) library is installed;
    - `FMT_INCLUDE_DIRECTORY` - path to headers of [fmt](https://github.com/fmtlib/fmt) library if `FMT_ROOT` is not set;
    - `FMT_HEADER_ONLY` - *Off*|*On* - mode of [fmt](https://github.com/fmtlib/fmt) library - default **Off**;
    - `FMT_LIB_DIR` - path to folder with built [fmt](https://github.com/fmtlib/fmt) library if `FMT_ROOT` is not set and `FMT_HEADER_ONLY` is off;
    - `VALIDATOR_WITH_TESTS` - *Off*|*On* - enable tests building - default is **Off**;

## Building and running tests

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

Run a script corresponding to your platform from a folder where source folder `cpp-validator` resides (i.e. go to folder `cpp-validator/../`).

# License

&copy; Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
See accompanying file [LICENSE.md](../LICENSE.md) or copy at http://boost.org/LICENSE_1_0.txt.

# Contributing

See accompanying file [CONTRIBUTING.md](../CONTRIBUTING.md).