# C++ validator

## Introduction

Modern C++ header-only library for validation of variables, objects and containers. 

Basic usage of the library includes two steps:

- first, define a validator using almost declarative syntax;
- then, apply the validator to objects that must be validated and check the results.

There are a lot of options for validator extension and customization. During validation a text report describing an error can be constructed. Reports can be widely customised and translated to supported languages.

Cpp-validator can be used to validate:
- plain variables;
- properties of objects, where a property can be accessed either as object's variable or object's method;
- contents and properties of containers;
- nested containers and objects.

The library was tested with clang, gcc and msvc compilers that support C++14 and/or C++17 standards. Tested operating systems include Linux, Windows, macOS, iOS and Android platforms.

_The README and reference documentation are under construction yet._

Currently only doxygen-style comments in sources are available. Tests in test folder can be used as a kind of examples. Scripts for tests budiling are in sample-build folder.

## Some examples of basic usage

### Check if value is greater than constant

```cpp

auto v=validator(gt,100); // define validator

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

auto v=validator(
  value(gte,"sample string"),
  size(lt,15)
); // define validator

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

auto v=validator(in,interval(95,100));

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

auto v=validator(
                _["field1"](gte,"xxxxxx")
                 ^OR^
                _["field1"](size(gte,100) ^OR^ value(gte,"zzzzzzzzzzzz"))
            );

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
    
```

### Check custom object property and print report

```cpp

struct Foo
{
    bool red_color() const
    {
        return true;
    }
};

DRACOSHA_VALIDATOR_PROPERTY_FLAG(red_color,"Must be red","Must be not red");

auto v=validator(
    _[red_color](flag,false)
);

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

### Validate value before updating object's member

```cpp

struct Foo
{
    std::string bar_value;
    
    uint32_t other_value;
    size_t some_size;
    
    int set_bar_value(std::string val)
    {
        bar_value=std::move(val);
    }
};

DRACOSHA_VALIDATOR_PROPERTY(bar_value);
DRACOSHA_VALIDATOR_PROPERTY(other_value);

auto v=validator(
    _[bar_value](ilex_ne,"UNKNOWN"), // case insensitive lexicographical not equal
    _[other_value](gte,1000)
); // define validator

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

auto ok=bar_value_setter("Hello world");
// ok == true

 ok=bar_value_setter("unknown");
 // ok == false
 /* prints:
 
 "bar_value must be not equal to UNKNOWN"
 
 */
```
