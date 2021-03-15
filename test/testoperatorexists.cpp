#include <boost/test/unit_test.hpp>

#include <dracosha/validator/validator.hpp>
#include <dracosha/validator/base_validator.hpp>
#include <dracosha/validator/operators/exists.hpp>
#include <dracosha/validator/properties/size.hpp>
#include <dracosha/validator/adapters/reporting_adapter.hpp>

using namespace DRACOSHA_VALIDATOR_NAMESPACE;

BOOST_AUTO_TEST_SUITE(TestOperatorExists)
#if 1
#if 1
BOOST_AUTO_TEST_CASE(CheckWithCheckExists)
{
    static_assert(std::is_base_of<exists_tag,decltype(exists)>::value,"");

    auto v1=value(exists,true);
    static_assert(decltype(v1)::with_check_exists::value,"");

    auto v2=value(eq,true);
    static_assert(!decltype(v2)::with_check_exists::value,"");

#ifdef _DRACOSHA_ALWAYS_UNDEF
    // static assertion
    auto v3_0=size(exists,true);
#endif
    auto v3=size(eq,100);
    static_assert(!decltype(v3)::with_check_exists::value,"");

    auto v4=validator(v1);
    static_assert(decltype(v4)::with_check_exists::value,"");

    static_assert(decltype(is_validator_with_check_exists(v1))::value,"");
    static_assert(!decltype(is_validator_with_check_exists(v2))::value,"");
    static_assert(!decltype(is_validator_with_check_exists(v3))::value,"");

    auto v5=make_aggregation_validator(detail::aggregate_and,v1);
    static_assert(decltype(v5)::with_check_exists::value,"");

    static_assert(decltype(is_validator_with_check_exists(hana::make_tuple(v3,v2,v1)))::value,"");
    static_assert(!decltype(is_validator_with_check_exists(hana::make_tuple(v3,v2)))::value,"");

    auto v6=AND(v1);
    static_assert(decltype(v6)::with_check_exists::value,"");

    auto v7=AND(v3,v2,v1);
    static_assert(decltype(v7)::with_check_exists::value,"");
    auto v8=AND(v3,v2);
    static_assert(!decltype(v8)::with_check_exists::value,"");
    auto v9=validator(v1 ^AND^ v2 ^AND^ v3);
    static_assert(decltype(v9)::with_check_exists::value,"");
    auto v10=validator(v2 ^AND^ v3);
    static_assert(!decltype(v10)::with_check_exists::value,"");

    auto v11=OR(v1);
    static_assert(decltype(v11)::with_check_exists::value,"");
    auto v12=OR(v3,v2,v1);
    static_assert(decltype(v12)::with_check_exists::value,"");
    auto v13=OR(v3,v2);
    static_assert(!decltype(v13)::with_check_exists::value,"");
    auto v14=validator(v1 ^OR^ v2 ^OR^ v3);
    static_assert(decltype(v14)::with_check_exists::value,"");
    auto v15=validator(v2 ^OR^ v3);
    static_assert(!decltype(v15)::with_check_exists::value,"");

    auto v16=NOT(v1);
    static_assert(decltype(v16)::with_check_exists::value,"");
    auto v17=validator(NOT(v1));
    static_assert(decltype(v17)::with_check_exists::value,"");
    auto v18=NOT(v2);
    static_assert(!decltype(v18)::with_check_exists::value,"");
    auto v19=validator(NOT(v2));
    static_assert(!decltype(v19)::with_check_exists::value,"");

    auto v20=(v1 ^AND^ v2) ^OR^ (v3 ^AND^ v2);
    static_assert(decltype(v20)::with_check_exists::value,"");

    // with_check_exists is not propagated across member validator
#ifdef _DRACOSHA_ALWAYS_UNDEF
    auto v21=_["hello"](v1);
    static_assert(decltype(v21)::with_check_exists::value,"");
    auto v22=_["hello"](v2);
    static_assert(!decltype(v22)::with_check_exists::value,"");
    auto v23=_["hello"](v3);
    static_assert(!decltype(v23)::with_check_exists::value,"");
    auto v24=_["hello"](exists,true);
    static_assert(decltype(v24)::with_check_exists::value,"");
    auto v25=_["hello"](eq,true);
    static_assert(!decltype(v25)::with_check_exists::value,"");
    auto v26=_["hello"](v20);
    static_assert(decltype(v26)::with_check_exists::value,"");
    auto v27=_["hello"](v15);
    static_assert(!decltype(v27)::with_check_exists::value,"");

    auto v28=validator(_["hello"](v20))("hi");
    static_assert(decltype(v28)::with_check_exists::value,"");
    auto v29=validator(_["hello"](v15))("hi");
    static_assert(!decltype(v29)::with_check_exists::value,"");
#else
    auto v21=_["hello"](v1);
    static_assert(!decltype(v21)::with_check_exists::value,"");
    auto v22=_["hello"](v2);
    static_assert(!decltype(v22)::with_check_exists::value,"");
    auto v23=_["hello"](v3);
    static_assert(!decltype(v23)::with_check_exists::value,"");
    auto v24=_["hello"](exists,true);
    static_assert(!decltype(v24)::with_check_exists::value,"");
    auto v25=_["hello"](eq,true);
    static_assert(!decltype(v25)::with_check_exists::value,"");
    auto v26=_["hello"](v20);
    static_assert(!decltype(v26)::with_check_exists::value,"");
    auto v27=_["hello"](v15);
    static_assert(!decltype(v27)::with_check_exists::value,"");

    auto v28=validator(_["hello"](v20))("hi");
    static_assert(!decltype(v28)::with_check_exists::value,"");
    auto v29=validator(_["hello"](v15))("hi");
    static_assert(!decltype(v29)::with_check_exists::value,"");
#endif
}

BOOST_AUTO_TEST_CASE(CheckOperandOfExist)
{
    auto v1=value(exists,true);
    BOOST_CHECK(v1.check_exists_operand);

    auto v2=value(exists,false);
    BOOST_CHECK(!v2.check_exists_operand);

#ifdef _DRACOSHA_ALWAYS_UNDEF
    // static assert because std::string is not convertible to bool
    auto v3_0=value(exists,std::string("hello"));

    // static assert because lazy operand is not allowed for operator exists
    auto v4_0=value(exists,lazy([]{return true;}));
#endif

    auto v3=value(exists,true) ^AND^ size(gte,10);
    BOOST_CHECK(v3.check_exists_operand);
    auto v4=size(gte,10) ^AND^ value(exists,true);
    BOOST_CHECK(v4.check_exists_operand);

    auto v5=value(exists,false) ^AND^ size(gte,10);
    BOOST_CHECK(!v5.check_exists_operand);
    auto v6=size(gte,10) ^AND^ value(exists,false);
    BOOST_CHECK(!v6.check_exists_operand);

    // only first "exists" is used
    auto v7=(value(exists,true) ^AND^ size(gte,10)) ^OR^ (value(exists,false));
    BOOST_CHECK(v7.check_exists_operand);
    auto v8=size(gte,10) ^AND^ value(exists,true) ^AND^ value(exists,false);
    BOOST_CHECK(v8.check_exists_operand);
    auto v9=(value(exists,false) ^AND^ size(gte,10)) ^OR^ (value(exists,true));
    BOOST_CHECK(!v9.check_exists_operand);
    auto v10=size(gte,10) ^AND^ value(exists,false) ^AND^ value(exists,true);
    BOOST_CHECK(!v10.check_exists_operand);

    auto v11=validator(v7);
    BOOST_CHECK(v11.check_exists_operand);
    auto v12=validator(v8);
    BOOST_CHECK(v12.check_exists_operand);
    auto v13=validator(v9);
    BOOST_CHECK(!v13.check_exists_operand);
    auto v14=validator(v10);
    BOOST_CHECK(!v14.check_exists_operand);

    auto v15=validator(v7)("v7");
    BOOST_CHECK(v15.check_exists_operand);
    auto v16=validator(v8)("v8");
    BOOST_CHECK(v16.check_exists_operand);
    auto v17=validator(v9)("v9");
    BOOST_CHECK(!v17.check_exists_operand);
    auto v18=validator(v10)("v10");
    BOOST_CHECK(!v18.check_exists_operand);

    // check_exists_operand is not propagated across member validator
#ifdef _DRACOSHA_ALWAYS_UNDEF
    auto v19=validator(_["key"](v7));
    BOOST_CHECK(v19.check_exists_operand);
    auto v20=validator(_["key"](v8));
    BOOST_CHECK(v20.check_exists_operand);
    auto v21=validator(_["key"](v9));
    BOOST_CHECK(!v21.check_exists_operand);
    auto v22=validator(_["key"](v10));
    BOOST_CHECK(!v22.check_exists_operand);

    auto v23=validator(_["key"](v7))("v23");
    BOOST_CHECK(v23.check_exists_operand);
    auto v24=validator(_["key"](v8))("v24");
    BOOST_CHECK(v24.check_exists_operand);
    auto v25=validator(_["key"](v9))("v25");
    BOOST_CHECK(!v25.check_exists_operand);
    auto v26=validator(_["key"](v10))("v26");
    BOOST_CHECK(!v26.check_exists_operand);
#else
    auto v19=validator(_["key"](v7));
    BOOST_CHECK(!v19.check_exists_operand);
    auto v20=validator(_["key"](v8));
    BOOST_CHECK(!v20.check_exists_operand);
    auto v21=validator(_["key"](v9));
    BOOST_CHECK(!v21.check_exists_operand);
    auto v22=validator(_["key"](v10));
    BOOST_CHECK(!v22.check_exists_operand);

    auto v23=validator(_["key"](v7))("v23");
    BOOST_CHECK(!v23.check_exists_operand);
    auto v24=validator(_["key"](v8))("v24");
    BOOST_CHECK(!v24.check_exists_operand);
    auto v25=validator(_["key"](v9))("v25");
    BOOST_CHECK(!v25.check_exists_operand);
    auto v26=validator(_["key"](v10))("v26");
    BOOST_CHECK(!v26.check_exists_operand);
#endif
}

BOOST_AUTO_TEST_CASE(CheckValidatePlainExists)
{
    std::vector<int> vec1{1,2,3,4,5};
    std::vector<int> vec2{1,2};

    auto v1=validator(
                _[3](exists,true)
            );
    auto v2=validator(
                _[3](exists,false)
            );
    auto v3=validator(
                _[3](value(exists,true))
            );
    auto v4=validator(
                _[3](value(exists,false))
            );

    BOOST_CHECK(v1.apply(vec1));
    BOOST_CHECK(!v1.apply(vec2));
    BOOST_CHECK(!v2.apply(vec1));
    BOOST_CHECK(v2.apply(vec2));

    BOOST_CHECK(v3.apply(vec1));
    BOOST_CHECK(!v3.apply(vec2));
    BOOST_CHECK(!v4.apply(vec1));
    BOOST_CHECK(v4.apply(vec2));
}
#endif
BOOST_AUTO_TEST_CASE(CheckValidateAggregateExists)
{
    std::vector<int> vec1{1,2,3,4,5};
    std::vector<int> vec2{1,2};

    auto v0=value(exists,true)
            ^AND^
            value(eq,4);
    BOOST_CHECK(v0.check_exists_operand);

    auto v1=validator(
                _[3](
                    value(exists,true)
                    ^AND^
                    value(eq,4)
                )
            );
    auto v2=validator(
                _[3](
                    value(exists,true)
                    ^AND^
                    value(lte,3)
                )
            );

    BOOST_CHECK(v1.apply(vec1));
    BOOST_CHECK(!v1.apply(vec2));
    BOOST_CHECK(!v2.apply(vec1));
    BOOST_CHECK(!v2.apply(vec2));

    auto a2=make_default_adapter(vec2);
    a2.set_check_member_exists_before_validation(true);
    a2.set_unknown_member_mode(if_member_not_found::abort);
    BOOST_CHECK(!v1.apply(a2));
    BOOST_CHECK(!v2.apply(a2));
    auto v3=validator(
                _[3](
                    value(eq,4)
                )
            );
    auto v4=validator(
                _[3](
                    value(lte,3)
                )
            );
    BOOST_CHECK(!v3.apply(a2));
    BOOST_CHECK(!v4.apply(a2));
    a2.set_unknown_member_mode(if_member_not_found::ignore);
    BOOST_CHECK(v3.apply(a2));
    BOOST_CHECK(v4.apply(a2));
}
#endif
BOOST_AUTO_TEST_CASE(CheckReportHintOfExists)
{
    std::string rep;
    auto v1=validator(
        _[5](_(exists,"is expected to be present"), true)
    );
    std::vector<int> vec1{1,2,3,4};
    auto ra1=make_reporting_adapter(vec1,rep);
    BOOST_CHECK(!v1.apply(ra1));
    BOOST_CHECK_EQUAL(rep,std::string("element #5 is expected to be present"));
}

BOOST_AUTO_TEST_SUITE_END()
