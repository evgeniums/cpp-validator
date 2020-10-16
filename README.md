# *cpp-validator* (C++ Data Validation Library)

## Motivation

It is quite a common task to check if data satisfies specific constraints. For example, server backend must check validity of commands came from the clients. Another example is a requirement to validate input data in application user interface and show the messages describing validation errors.

A typical approach for implementing complex data validation is writing nested *if-conditions* or chained invocations of partial validation methods. Declarations of the constraints can become intermixed with their implementations so that the constraints are spread over the code which makes it hard to watch and support them. Requirement to construct and show messages describing validation errors makes everything still more complicated.

`cpp-validator` library allows one to declare data constraints with clean statements in certain points of code and apply them in other points of code on demand. If needed, the validation error messages are automatically constructed with taking into account the user's locale.

## Introduction

*cpp-validator* is a modern C++ header-only library for validation of variables, objects and containers. 

The library can be used to validate:
- plain variables;
- properties of objects, where a property can be accessed either as object's variable or object's getter method;
- contents and properties of containers;
- nested containers and objects.

Basic usage of the library includes two steps:

- first, define a validator using almost declarative syntax;
- then, apply the validator to data that must be validated and check the results.

The library is suitable for both *post-validation* and *pre-validation*. *Post-validation* stands for validating the object that is already populated with the data. *Pre-validation* stands for validating the data before writing it to the object. The same validator declaration can be used in both cases.

There are a lot of options for validator extension and customization. During validation a text report describing an error can be constructed. Reports can be widely customised and translated to supported languages.

The library was tested with *Clang*, *GCC* and *MSVC* compilers that support *C\++14* or *C\++17* standards. Tested operating systems include *Linux*, *Windows*, *macOS*, *iOS* and *Android* platforms.

For more details see [Documentation](docs/index.md).

## Some examples of basic usage

### Check if value is greater than constant

```cpp
// define validator
auto v=validator(gt,100);

// apply validator to variables

int value1=90;
if (!v.apply(value1))
{
  // validation failed
}

int value2=200;
if (v.apply(value2))
{
  // validation succeeded
}

```

### Check if string is greater than or equal to other string and size of the string is less than constant

```cpp
// define validator
auto v=validator(
  value(gte,"sample string"),
  size(lt,15)
);

// apply validator to variables

std::string str1="sample";
if (!v.apply(str1))
{
  // validation failed, string is less than sample string
}

std::string str2="sample string+";
if (v.apply(str2))
{
  // validation succeeded
}

std::string str3="too long sample string";
if (!v.apply(str3))
{
  // validation failed, string is too long
}

```
### Check if value is in interval and print report

```cpp
// define validator
auto v=validator(in,interval(95,100));

// apply validator to variable and construct validation error message

std::string report;
size_t val=90;
auto ra=make_reporting_adapter(val,report);
if (!v.apply(ra))
{
    std::cerr << report << std::endl; 
    /* prints:
    
    "must be in interval [95,100]"
    
    */
}

```
### Check container element and print report

```cpp
// define compound validator of container elements
auto v=validator(
                _["field1"](gte,"xxxxxx")
                 ^OR^
                _["field1"](size(gte,100) ^OR^ value(gte,"zzzzzzzzzzzz"))
            );

// apply validator to container and construct validation error message

std::string report;
std::map<std::string,std::string> test_map={{"field1","value1"}};
auto ra=make_reporting_adapter(test_map,report);

if (!v.apply(ra))
{
    std::cerr << report << std::endl;
    /* prints:
    
    "field1 must be greater than or equal to xxxxxx OR size of field1 must be greater than or equal to 100 OR field1 must be greater than or equal to zzzzzzzzzzzz"
    
    */    
}
    
```

### Check nested container elements and print report

```cpp
// define compound validator of nested container elements
auto v=validator(
                _["field1"][1](in,range({10,20,30,40,50})),
                _["field1"][2](lt,100),
                _["field2"](exists,false),
                _["field3"](empty(flag,true))
            );
                
std::string report;

// apply validator to container and construct validation error message

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
    
```

### Check custom object property and print report

```cpp
// define structure with getter method
struct Foo
{
    bool red_color() const
    {
        return true;
    }
};

// define custom property
DRACOSHA_VALIDATOR_PROPERTY_FLAG(red_color,"Must be red","Must be not red");

// define validator of custom property
auto v=validator(
    _[red_color](flag,false)
);

// apply validator to object with custom property and construct validation error message

std::string report;
Foo foo_instance;
auto ra=make_reporting_adapter(foo_instance,report);

if (!v.apply(ra))
{
    std::cerr << report << std::endl;
    /* prints:
    
    "Must be not red"
    
    */
}

```

### Pre-validate data before updating object's member

```cpp
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

// define validator of custom properties
auto v=validator(
    _[bar_value](ilex_ne,"UNKNOWN"), // case insensitive lexicographical not equal
    _[other_value](gte,1000)
); // define validator

// object setter that performs data pre-validation and prints validation error
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

// call object setter with valid data
auto ok=bar_value_setter("Hello world");
// ok == true

// call object setter with invalid data
ok=bar_value_setter("unknown");
// ok == false
/* prints:
 
 "bar_value must be not equal to UNKNOWN"
 
 */
```
