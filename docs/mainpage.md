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
		* [Reports customization](#reports-customization)
		* [Reporting hints](#reporting-hints)
		* [Localization](#localization)
			* [Concrete phrases and grammatical categories](#concrete-phrases-and-grammatical-categories)
			* [Translator](#translator)
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

Modern C++ header-only library for validation of variables, objects and containers. 

Cpp-validator can be used to validate:
- plain variables;
- properties of objects, where a property can be accessed either as object's variable or object's method;
- contents and properties of containers;
- nested containers and objects.

Basic usage of the library includes a few steps:

- first, define a [validator](#validator) using almost declarative syntax;
- then, apply the [validator](#validator) to [object](#object) that must be validated;
- finally, check the results and print [report](#report) if applicable.

# Definitions

##### *Adapter*
Customizable validation implementer. Adapter performs actual validation when the [validator](#validator) is applied to an [object](#object).

##### *Aggregation*
Combination of validating [operators](#operator) or other [validators](#validator) that make up a compound [validator](#validator). There are logical aggregations such as _AND_, _OR_, _NOT_ and [element](#element) aggregations such as _ANY_, _ALL_.

##### *Backend formatter*
String formatter used for [reports](#report) building. One of the following backend formatters can be used:
* *(preferred)* [fmt](https://github.com/fmtlib/fmt) based backend formatter;
* std::stringstream based backend formatter.

##### *Concrete phrase*
Immutable string that will be used in final [report](#report).

##### *Element*
A special type of [member](#member) that points to an element of container when the [object](#object) is a container.

##### *Formatter*
Customizable implementer of [reports](#report) formatting.

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
Customizable [report](#report) constructor.

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

Data validation is performed by [adapters](#adapter). When a [validator](#validator) is applied to an [adapter](#adapter) the [adapter](#adapter) *reads* validation conditions from the [validator](#validator) and processes them depending on [adapter](#adapter) implementation. See more about adapters in [Adapters](#adapters) section.
```cpp
#include <dracosha/validator/validator.hpp>

using namespace DRACOSHA_VALIDATOR_NAMESPACE;

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
```

A [validator](#validator) can be applied directly to a variable that must be validated. In this case a [default adapter](#default-adapter) will be used implicitly.
```cpp
#include <dracosha/validator/validator.hpp>

using namespace DRACOSHA_VALIDATOR_NAMESPACE;

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
```
## Members

Members are used to define what parts of [objects](#object) must be validated. A member can point to one of the following:
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

A [member](#member) name must be placed within square brackets that follow an underscore symbol `_` which is defined in cpp-validator library namespace.
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

To validate members of nested objects or containers a compound member notation must be used.
where name of member at each level is placed within square brackets and appended to the upper member. See examples below.

```cpp
#include <dracosha/validator/validator.hpp>
using namespace DRACOSHA_VALIDATOR_NAMESPACE;

// 3 levels
auto member_3_levels=_[level1][level2][level3];

// nested container elements
auto member_nested_container=_["element1"]["element1_1"]["element1_1_1"];

```

### Member existence

Special operator [exists](#exists) can be used to check explicitly if an [object](#object) contains some [member](#member). 

There is also a [contains](#contains) operator added for convenience to validate container members.

A [member](#member) existence can also be checked implicitly before applying validation conditions to the [member](#member). Such checking is performed by an [adapter](#adapter) if the [adapter](#adapter) supports that. [Default adapter](#default-adapter) and [reporting adapter](#reporting-adapter) provide such feature that can be configured with  `set_check_member_exists_before_validation` and `set_unknown_member_mode` adapter methods. 

Method `set_check_member_exists_before_validation` enables/disables implicit checking of member existence. By default this option is disabled which improves validation performance but can sometimes cause exceptions or other undefined errors. Besides, some basic checking if a member can be found for given object type is performed statically at compile time regardless of this flag.

Method `set_unknown_member_mode` instructs adapter what to do if a member is not found. There are two options:
- ignore not found members and continue validation process;
- abort validation process with error.

See examples below.

```cpp
#include <dracosha/validator/validator.hpp>
using namespace DRACOSHA_VALIDATOR_NAMESPACE;

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

```

## Properties

[Properties](#property) are used to validate either contents or characteristics of [objects](#object) and [members](#member). A [property](#property) can correspond either to a member variable or to a getter method of C++ `class` or `struct`.

### Property notations

A [property](#property) can be used in a [validator](#validator) in one of the following forms:
* `property notation` when property's name is used as part of a validation condition;
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

If a [property](#property) is boolean that can be used with [flag](#flag) operator then `DRACOSHA_VALIDATOR_PROPERTY_FLAG` macro must be used which has three arguments:
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

```

All the rest [properties](#property) must be defined using `DRACOSHA_VALIDATOR_PROPERTY` macro which has only one argument for name of the [property](#property).

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

// validator with custom properties
auto v=validator(
    var1(ne,"unknown"),
    get_var2(gte,100)
);

// valiate object
Foo foo_instance;
assert(v.apply(foo_instance));

```

## Operators

[Operators](#operator) define atomic validation conditions in [validators](validator). 

[Operator](#operator) is a callable object that returns `true` if validation condition is satisfied, `false` otherwise. In default [adapter](#adapter) implementations [operator](#operator) is called with two arguments when [validator](#validator) is applied to the [adapter](#adapter):
* the first argument is a corresponding [property](#property) of the [object](#object) or object's [member](#member) that must be validated;
* the second argument is an [operand](#operand) that must be used as a sample for validation.

### Special operators

#### *exists*

Operator `exists` is used to check explicitly if an [object](#object) contains some [member](#member). See example below.

```cpp
#include <dracosha/validator/validator.hpp>
using namespace DRACOSHA_VALIDATOR_NAMESPACE;

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

```

#### *contains*

Operator `contains` is used to check corresponding argument that is under validation contains value of corresponding [operand](#operand). See example below.

```cpp
#include <dracosha/validator/validator.hpp>
#include <dracosha/validator/operator/contains.hpp>
using namespace DRACOSHA_VALIDATOR_NAMESPACE;

// define validator
auto v1=validator(contains,"field1");

// m1 satisfies validation condition
std::map<std::string,size_t> m1={{"field1",1}};
assert(v1.apply(m1));

// m2 does not satisfy validation condition
std::map<std::string,size_t> m2={{"field2",2}};
assert(!v1.apply(m2));

```

#### *flag*

Operator `flag` is a special case of equality operator for boolean arguments. The main purpose of defining separate operator `flag` in addition to operator [eq](builtin_operators.md#eq) is more flexible [reports](#report) construction. With operator `eq` report would always use "must be equal to" string. With operator `flag` report strings can be customized depending on the [property](#property) or explicitly preset strings, e.g. in some places "must be checked", in other places "must be set" and so on can be used.

There are three ways of string customization for `flag` operator.

1. Define [custom property](#adding-new-property) using `DRACOSHA_VALIDATOR_PROPERTY_FLAG` macro where the second argument is a reporting string for `<custom property>(flag,true)` condition and the third argument is a reporting string for `<custom property>(flag,false)` condition. See [example](#adding-new-property).

2. Use `flag` operator with one of string presets, e.g. `value(flag(flag_on_off),true)` will result in reporting string "must be on" and `value(flag(flag_on_off),false)` will result in reporting string "must be off". See below list of presets for `flag` operator.
*Preset flag strings are defined in `validator/reporting/flag_presets.hpp` header file. The following presets are defined:

    * `flag_true_false` (default flag string): "must be true" | "must be false";
    * `flag_on_off`: "must be on" | "must be off";
    * `flag_checked_unchecked`: "must be checked" | "must be unchecked";
    * `flag_set_unset`: "must be set" | "must be unset";
    * `flag_enable_disable`: "must be enabled" | "must be disabled".

3. Use `flag` operator with explicit string description, e.g. `value(flag("custom description"),true)` will result in reporting string "custom description".

```cpp
#include <vector>
#include <iostream>
#include <dracosha/validator/validator.hpp>
#include <dracosha/validator/operator/flag.hpp>
#include <dracosha/validator/reporting/flag_presets.hpp>
#include <dracosha/validator/adapters/reporting_adapter.hpp>
using namespace DRACOSHA_VALIDATOR_NAMESPACE;

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

```

#### *in*

Operator `in` is used to check if corresponding argument fits into [range](#range) or [interval](#interval). See examples below.

```cpp
#include <dracosha/validator/validator.hpp>
#include <dracosha/validator/operators.hpp>
#include <dracosha/validator/range.hpp>
#include <dracosha/validator/interval.hpp>
using namespace DRACOSHA_VALIDATOR_NAMESPACE;

// define value for validation
size_t val=90;

// check if the value is in range
auto v1=validator(in,range({70,80,90,100}));
assert(v1.apply(val));

// check if the value is in interval
auto v2=validator(in,interval(80,100));
assert(v2.apply(val));
    
```

#### *_n* (negation)

`_n` is an [operator](#operator) wrapper that negates wrapped [operator](#operator). See example below.

```cpp
#include <dracosha/validator/validator.hpp>
#include <dracosha/validator/operators.hpp>
using namespace DRACOSHA_VALIDATOR_NAMESPACE;

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

```

### Built-in operators

See [Built-in validation operators](builtin_operators.md).

### Adding new operator

Adding a new [operator](#operator) consists of the following steps.

1. Define `struct` that inherits from template `op` with the name of the defined struct as template argument.
2. Define callable `operator ()` in the struct with two templated arguments.
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

### Variables

### Lazy operands

### Other members

### Sample objects

### Intervals

### Ranges

## Aggregations

### Logical aggregations

#### AND

#### OR

#### NOT

### Element aggregations

#### ANY

#### ALL

## Adapters

### Default adapter

### Single member adapter

### Adding new adapter

## Reporting

### Reporting adapter

### Reports customization

### Reporting hints

### Localization

#### Concrete phrases and grammatical categories

#### Translator

#### Translator repository

#### Adding new locale

# Building and installation

## Supported platforms and compilers

## Dependencies

## CMake configuration

## Building and running tests

# License

&copy; Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
See accompanying file [LICENSE.md](../LICENSE.md) or copy at http://boost.org/LICENSE_1_0.txt.

# Contributing

See accompanying file [CONTRIBUTING.md](../CONTRIBUTING.md).