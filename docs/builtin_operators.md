# Built-in validation operators

## Table of Contents
[//]: # (TOC Begin)
* [Comparison operators](#comparison-operators)
	* [eq](#eq)
	* [ne](#ne)
	* [lt](#lt)
	* [lte](#lte)
	* [gt](#gt)
	* [gte](#gte)
* [Lexicographical operators](#lexicographical-operators)
	* [lex_eq](#lex_eq)
	* [lex_ne](#lex_ne)
	* [lex_lt](#lex_lt)
	* [lex_lte](#lex_lte)
	* [lex_gt](#lex_gt)
	* [lex_gte](#lex_gte)
	* [ilex_eq](#ilex_eq)
	* [ilex_ne](#ilex_ne)
	* [ilex_lt](#ilex_lt)
	* [ilex_lte](#ilex_lte)
	* [ilex_gt](#ilex_gt)
	* [ilex_gte](#ilex_gte)
	* [lex_in](#lex_in)
	* [lex_nin](#lex_nin)
	* [ilex_in](#ilex_in)
	* [ilex_nin](#ilex_nin)
	* [lex_contains](#lex_contains)
	* [ilex_contains](#ilex_contains)
	* [lex_starts_with](#lex_starts_with)
	* [ilex_starts_with](#ilex_starts_with)
	* [lex_ends_with](#lex_ends_with)
	* [ilex_ends_with](#ilex_ends_with)
* [Regular expressions](#regular-expressions)
	* [regex_match](#regex_match)
	* [regex_nmatch](#regex_nmatch)
	* [regex_contains](#regex_contains)
	* [regex_ncontains](#regex_ncontains)
* [String patterns](#string-patterns)
	* [str_int](#str_int)
	* [str_float](#str_float)
	* [str_alpha](#str_alpha)
	* [str_hex](#str_hex)

[//]: # (TOC End)

## Comparison operators

Comparison operators are defined in `validator/operators/comparison.hpp` header file.

### eq

Equal to.

### ne

Not equal to.

### lt

Less than.

### lte

Less than or equal to.

### gt

Greater than.

### gte

Greater than or equal to.

## Lexicographical operators

Lexicographical operators are defined in `validator/operators/lexicographical.hpp` and `validator/operators/lex_in.hpp` header files.

### lex_eq

Lexicographically equal to.

### lex_ne

Lexicographically not equal to.

### lex_lt

Lexicographically less than.

### lex_lte

Lexicographically less than or equal to.

### lex_gt

Lexicographically greater than.

### lex_gte

Lexicographically greater than or equal to.

### ilex_eq

Lexicographically case insensitive equal to.

### ilex_ne

Lexicographically case insensitive not equal to.

### ilex_lt

Lexicographically case insensitive less than.

### ilex_lte

Lexicographically case insensitive less than or equal to.

### ilex_gt

Lexicographically case insensitive greater than.

### ilex_gte

Lexicographically case insensitive greater than or equal to.

### lex_in

Lexicographical [in](index.md#in) operator.

### lex_nin

Lexicographical [nin](index.md#nin) operator.

### ilex_in

Lexicographical case insensitive [in](index.md#in) operator.

### ilex_nin

Lexicographical case insensitive [nin](index.md#nin) operator.

### lex_contains

Lexicographically contains.

### ilex_contains

Lexicographically case insensitive contains.

### lex_starts_with

Lexicographically starts with.

### ilex_starts_with

Lexicographically case insensitive starts with.

### lex_ends_with

Lexicographically ends with.

### ilex_ends_with

Lexicographically case insensitive ends with.

## Regular expressions

Regular expression operators are defined in `validator/operators/regex.hpp` header file.

### regex_match

Match regular expression.
Operands can be either of boost::regex or to-string-convertible types.

### regex_nmatch

Do not match regular expression.
Operands can be either of boost::regex or to-string-convertible types.

### regex_contains

Contains regular expression.
Operands can be either of boost::regex or to-string-convertible types.

### regex_ncontains

Does not contain regular expression.
Operands can be either of boost::regex or to-string-convertible types.

## String patterns

Operators for checking some common string patterns are defined in `validator/operators/number_patterns.hpp` and `validator/operators/string_patterns.hpp` header files.

### str_int

String can be converted to integer.

### str_float

String can be converted to float.

### str_alpha

String contains only alpha-symbols, i.e. digits, letters and underscore _.

### str_hex

String is a hexadecimal number.