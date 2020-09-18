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
		* [Member levels](#member-levels)
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
			* [*_n* (negation)](#n-negation)
			* [*flag*](#flag)
			* [*in*](#in)
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

### Member notation

### Member levels

### Member existence

## Properties

### Property notations

### Built-in properties

#### *value*

#### *size*

#### *length*

#### *empty*

### Adding new property

## Operators

### Special operators

#### *exists*

#### *_n* (negation)

#### *flag*

#### *in*

### Built-in operators

See [Built-in validation operators](builtin_operators.md).

### Adding new operator

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