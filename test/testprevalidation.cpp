#include <set>
#include <iterator>

#include <boost/test/unit_test.hpp>

#include <dracosha/validator/validator.hpp>
#include <dracosha/validator/adapters/prevalidation_adapter.hpp>
#include <dracosha/validator/prevalidation/set_validated.hpp>

namespace hana=boost::hana;

namespace
{

DRACOSHA_VALIDATOR_PROPERTY(val1)
DRACOSHA_VALIDATOR_PROPERTY(val2)

struct NonCopyable
{
    NonCopyable()=default;
    ~NonCopyable()=default;
    NonCopyable(const NonCopyable&)=delete;
    NonCopyable(NonCopyable&&)=default;
    NonCopyable& operator= (const NonCopyable&)=delete;
    NonCopyable& operator= (NonCopyable&&)=default;

    std::string val1;
    int val2=0;
};
}

using namespace DRACOSHA_VALIDATOR_NAMESPACE;

BOOST_AUTO_TEST_SUITE(TestPrevalidation)

#if 1
BOOST_AUTO_TEST_CASE(CheckPrevalidationReport)
{
    std::string rep1;

    auto v1=validator(
                _[size](ne,100),
                _["field1"](gte,"1"),
                _["field10"](size(lt,3)),
                _[10](gte,1000),
                _["field100"]["subfield100"](contains,"value10")
            );

    auto sa1=make_prevalidation_adapter(_["field1"],"value10",rep1);
    auto&& val1=sa1.traits().get();
    BOOST_CHECK_EQUAL(val1,"value10");
    BOOST_CHECK(v1.apply(sa1));
    rep1.clear();

    auto sa2=make_prevalidation_adapter(_["field10"],std::string("value10"),rep1);
    BOOST_CHECK(!v1.apply(sa2));
    BOOST_CHECK_EQUAL(rep1,"size of field10 must be less than 3");
    rep1.clear();

    auto sa3=make_prevalidation_adapter(_["field1"],"0",rep1);
    BOOST_CHECK(!v1.apply(sa3));
    BOOST_CHECK_EQUAL(rep1,"field1 must be greater than or equal to 1");
    rep1.clear();

    std::string str4="0";
    auto sa4=make_prevalidation_adapter(_["field100"],str4,rep1);
    BOOST_CHECK(v1.apply(sa4));
    rep1.clear();

    auto sa5=make_prevalidation_adapter(_["field200"],100,rep1);
    BOOST_CHECK(v1.apply(sa5));
    rep1.clear();

    auto sa6=make_prevalidation_adapter(_[100],NonCopyable(),rep1);
    BOOST_CHECK(v1.apply(sa6));
    rep1.clear();

    NonCopyable nc;
    auto sa7=make_prevalidation_adapter(_["field300"],nc,rep1);
    BOOST_CHECK(v1.apply(sa7));
    rep1.clear();

    auto sa8=make_prevalidation_adapter(_[10],5000,rep1);
    BOOST_CHECK(v1.apply(sa8));
    rep1.clear();

    auto sa9=make_prevalidation_adapter(_[10],100,rep1);
    BOOST_CHECK(!v1.apply(sa9));
    BOOST_CHECK_EQUAL(rep1,"element #10 must be greater than or equal to 1000");
    rep1.clear();

    BOOST_CHECK(check_contains(value_as_container("value10"),"value10"));

    auto sa10=make_prevalidation_adapter(_["field100"]["subfield100"],value_as_container("value1"),rep1);
    BOOST_CHECK(!v1.apply(sa10));
    BOOST_CHECK_EQUAL(rep1,"subfield100 of field100 must contain value10");
    auto sa11=make_prevalidation_adapter(_["field100"]["subfield100"],value_as_container("value10"),rep1);
    BOOST_CHECK(v1.apply(sa11));
    rep1.clear();

    auto v2=validator(
        _["field1"](contains,10)
    );
    auto sa21=make_prevalidation_adapter(_["field1"],20,rep1);
    BOOST_CHECK(!check_contains(20,10));
    BOOST_CHECK(!v2.apply(sa21));
}
#endif
BOOST_AUTO_TEST_CASE(CheckAggregationPrevalidationReport)
{
    auto v1=validator(
                _[size](eq,1),
                _["field1"](gte,"value1"),
                _["field10"](value(gte,"10") ^AND^ size(lt,3)),
                _["field100"](value(gt,"how are you") ^OR^ size(lte,5))
            );
    std::string rep1;

    auto sa1=make_prevalidation_adapter(_["field10"],"hi",rep1);
    BOOST_CHECK(v1.apply(sa1));
    rep1.clear();

    auto sa2=make_prevalidation_adapter(_["field10"],"01",rep1);
    BOOST_CHECK(!v1.apply(sa2));
    BOOST_CHECK_EQUAL(rep1,"field10 must be greater than or equal to 10");
    rep1.clear();

#if 1

    auto sa3=make_prevalidation_adapter(_["field10"],"1000",rep1);
    BOOST_CHECK(!v1.apply(sa3));
    BOOST_CHECK_EQUAL(rep1,"size of field10 must be less than 3");
    rep1.clear();

    auto sa4=make_prevalidation_adapter(_[10],5000,rep1);
    BOOST_CHECK(v1.apply(sa4));
    rep1.clear();

    auto sa5=make_prevalidation_adapter(_["field100"],"zzzzzzzzz",rep1);
    BOOST_CHECK(v1.apply(sa5));
    rep1.clear();

    auto sa6=make_prevalidation_adapter(_["field100"],"0123",rep1);
    BOOST_CHECK(v1.apply(sa6));
    rep1.clear();

    auto sa7=make_prevalidation_adapter(_["field100"],"01234567890",rep1);
    BOOST_CHECK(!v1.apply(sa7));
    BOOST_CHECK_EQUAL(rep1,"field100 must be greater than how are you OR size of field100 must be less than or equal to 5");
    rep1.clear();

    auto v2=validator(
                _["field10"](gte,"value1")
                ^OR^
                _["field10"](eq,"10")
            );

    auto sa8=make_prevalidation_adapter(_["field10"],"zzzzzzz",rep1);
    BOOST_CHECK(v2.apply(sa8));
    rep1.clear();

    auto sa9=make_prevalidation_adapter(_["field10"],"10",rep1);
    BOOST_CHECK(v2.apply(sa9));
    rep1.clear();

    auto sa10=make_prevalidation_adapter(_["field10"],"100",rep1);
    BOOST_CHECK(!v2.apply(sa10));
    BOOST_CHECK_EQUAL(rep1,"field10 must be greater than or equal to value1 OR field10 must be equal to 10");
    rep1.clear();

    auto v3=validator(
                _["field10"](NOT(value(gte,"value1"))),
                _["field100"](value(gt,"how are you") ^OR^ size(lte,5))
            );
    auto sa11=make_prevalidation_adapter(_["field10"],"10",rep1);
    BOOST_CHECK(v3.apply(sa11));
    rep1.clear();
    auto sa12=make_prevalidation_adapter(_["field10"],"value100",rep1);
    BOOST_CHECK(!v3.apply(sa12));
    BOOST_CHECK_EQUAL(rep1,"NOT field10 must be greater than or equal to value1");
    rep1.clear();

    auto v4=validator(
                NOT(_["field10"](gte,"value1"))
            );
    BOOST_CHECK(v4.apply(sa11));
    rep1.clear();
    BOOST_CHECK(!v4.apply(sa12));
    BOOST_CHECK_EQUAL(rep1,"NOT field10 must be greater than or equal to value1");
    rep1.clear();
#endif
}
#if 1
BOOST_AUTO_TEST_CASE(CheckNestedPrevalidationReport)
{
    std::vector<float> vec1;
    auto v1=validator(
                _[size](eq,1),
                _["field1"][vec1](gte,"value1"),
                _["field10"]["field20"](value(gte,"10") ^AND^ size(lt,3))
            );
    std::string rep1;

    auto sa1=make_prevalidation_adapter(_["field1000"][100],"hi",rep1);
    BOOST_CHECK(v1.apply(sa1));
    rep1.clear();

    auto sa2=make_prevalidation_adapter(_["field10"][100],"hello",rep1);
    BOOST_CHECK(v1.apply(sa2));
    rep1.clear();

    auto sa3=make_prevalidation_adapter(_["field10"]["field200"],"hello",rep1);
    BOOST_CHECK(v1.apply(sa3));
    rep1.clear();

    auto sa4=make_prevalidation_adapter(_["field10"]["field20"],"99",rep1);
    BOOST_CHECK(v1.apply(sa4));
    rep1.clear();

    auto sa5=make_prevalidation_adapter(_["field10"]["field20"],"0",rep1);
    BOOST_CHECK(!v1.apply(sa5));
    BOOST_CHECK_EQUAL(rep1,"field20 of field10 must be greater than or equal to 10");
    rep1.clear();

    auto sa6=make_prevalidation_adapter(_["field10"]["field20"],"999",rep1);
    BOOST_CHECK(!v1.apply(sa6));
    BOOST_CHECK_EQUAL(rep1,"size of field20 of field10 must be less than 3");
    rep1.clear();
}

BOOST_AUTO_TEST_CASE(CheckSampleObjectPrevalidationReport)
{
    std::map<size_t,size_t> sample={
        {1,10},
        {2,20},
        {3,30}
    };

    auto v1=validator(
                _[size](eq,1),
                _[1](gte,_(sample)),
                _[10](eq,_(sample)),
                _["hello"](eq,_(sample)),
                _[100](eq,_(sample))
            );
    std::string rep1;

    auto sa1=make_prevalidation_adapter(_[1],100,rep1);
    BOOST_CHECK(v1.apply(sa1));
    rep1.clear();

    auto sa2=make_prevalidation_adapter(_[20],1000,rep1);
    BOOST_CHECK(v1.apply(sa1));
    rep1.clear();

    auto sa3=make_prevalidation_adapter(_[1],5,rep1);
    BOOST_CHECK(!v1.apply(sa3));
    BOOST_CHECK_EQUAL(rep1,"element #1 must be greater than or equal to element #1 of sample");
    rep1.clear();

    auto sa4=make_prevalidation_adapter(_["hello"],5,rep1);
    BOOST_CHECK(v1.apply(sa4));

    auto sa5=make_prevalidation_adapter(_[100],5,rep1);
    BOOST_CHECK(v1.apply(sa5));

    auto v2=validator(
                _[size](eq,1),
                _[1](gte,_(_(sample),"sample object")),
                _[10](eq,_(_(sample),"sample object"))
            );
    BOOST_CHECK(!v2.apply(sa3));
    BOOST_CHECK_EQUAL(rep1,"element #1 must be greater than or equal to element #1 of sample object");
    rep1.clear();
}

BOOST_AUTO_TEST_CASE(CheckPropertyPrevalidation)
{
    auto v=validator(
        _[val1](eq,"value"),
        _[val2](lt,100)
    );

    std::string rep;
    auto sa1=make_prevalidation_adapter(_[val1],"hello",rep);
    BOOST_CHECK(!v.apply(sa1));
    rep.clear();

    auto sa2=make_prevalidation_adapter(_[val1],"value",rep);
    BOOST_CHECK(v.apply(sa2));
    rep.clear();
}

BOOST_AUTO_TEST_CASE(CheckSingleMemberAnyAllReport)
{
    std::string rep1;

    auto v0=validator(
                _['a'](ALL(value(gte,9)))
            );
    auto v0_1=validator(
                _['a'][ALL](gte,9)
            );
    auto pa0=make_prevalidation_adapter(_['a'],10,rep1);
    BOOST_CHECK(v0.apply(pa0));
    rep1.clear();
    BOOST_CHECK(v0_1.apply(pa0));
    rep1.clear();

    auto v1=validator(
                _["field1"](ALL(value(gte,9))),
                _["field2"](size(gte,100))
            );
    auto v1_1=validator(
                _["field1"][ALL](gte,9),
                _["field2"](size(gte,100))
            );
    auto pa1=make_prevalidation_adapter(_["field1"],10,rep1);
    BOOST_CHECK(v1.apply(pa1));
    rep1.clear();
    BOOST_CHECK(v1_1.apply(pa1));
    rep1.clear();

    auto v2=validator(
                _["field1"]["field1_1"](ALL(value(gte,9))),
                _["field2"](size(gte,100))
            );
    auto v2_1=validator(
                _["field1"]["field1_1"][ALL](gte,9),
                _["field2"](size(gte,100))
            );
    auto pa2=make_prevalidation_adapter(_["field1"]["field1_1"][0],10,rep1);
    BOOST_CHECK(v2.apply(pa2));
    rep1.clear();
    BOOST_CHECK(v2_1.apply(pa2));
    rep1.clear();

    auto pa3=make_prevalidation_adapter(_["field1"]["field1_1"][0],5,rep1);
    BOOST_CHECK(!v2.apply(pa3));
    BOOST_CHECK_EQUAL(rep1,std::string("each element of field1_1 of field1 must be greater than or equal to 9"));
    rep1.clear();
    BOOST_CHECK(!v2_1.apply(pa3));
    BOOST_CHECK_EQUAL(rep1,std::string("each element of field1_1 of field1 must be greater than or equal to 9"));
    rep1.clear();

    static_assert(is_container_t<std::vector<int>>::value,"");
    auto member1=_["field1"]["field1_1"];
    auto member2=_["field1"]["field1_1"][value];

    static_assert(hana::not_(hana::is_a<element_aggregation_tag,std::string>),"");
    static_assert(hana::not_(hana::is_a<element_aggregation_tag,decltype(size)>),"");

    static_assert(
            !decltype(same_member_path_types(_[std::string("A")],_[size]))::value,
        "");
    static_assert(
            decltype(same_member_path_types(member1[size],member2))::value,
        "");

    auto pa4=make_prevalidation_adapter(_["field1"]["field1_1"],range({10,11,12}),rep1);
    BOOST_CHECK(v2.apply(pa4));
    rep1.clear();
    BOOST_CHECK(v2_1.apply(pa4));
    rep1.clear();

    BOOST_CHECK(safe_compare_equal(ALL,ALL));
    BOOST_CHECK(safe_compare_equal(ANY,ANY));
    BOOST_CHECK(safe_compare_equal(ALL,ANY));
    BOOST_CHECK(safe_compare_equal(ANY,ALL));

    BOOST_CHECK(ALL==ALL);
    BOOST_CHECK(ANY==ANY);
    BOOST_CHECK(ALL==ANY);
    BOOST_CHECK(ANY==ALL);
    BOOST_CHECK(10==ALL);
    BOOST_CHECK(ALL==10);
    BOOST_CHECK(10==ANY);
    BOOST_CHECK(ANY==10);

    auto pa5=make_prevalidation_adapter(_["field1"]["field1_1"],range({1}),rep1);

    BOOST_CHECK(!v2.apply(pa5));
    BOOST_CHECK_EQUAL(rep1,std::string("each element of field1_1 of field1 must be greater than or equal to 9"));
    rep1.clear();
    BOOST_CHECK(!v2_1.apply(pa5));
    BOOST_CHECK_EQUAL(rep1,std::string("each element of field1_1 of field1 must be greater than or equal to 9"));
    rep1.clear();

    auto pa6=make_prevalidation_adapter(_["field1"]["field1_1"],range({10,11,12,1}),rep1);
    BOOST_CHECK(!v2.apply(pa6));
    BOOST_CHECK_EQUAL(rep1,std::string("each element of field1_1 of field1 must be greater than or equal to 9"));
    rep1.clear();
    BOOST_CHECK(!v2_1.apply(pa6));
    BOOST_CHECK_EQUAL(rep1,std::string("each element of field1_1 of field1 must be greater than or equal to 9"));
    rep1.clear();

    static_assert(std::is_base_of<any_tag,decltype(wrap_it(1,string_any,values))>::value,"");
    static_assert(detail::is_member_with_any<size_t,decltype(ANY)>::value,"");
    static_assert(decltype(_["field1"]["field1_1"][wrap_it(1,string_any,values)])::is_with_any::value,"");

    auto v3=validator(
                _["field1"]["field1_1"](ANY(value(gte,9))),
                _["field2"](size(gte,100))
            );
    auto v3_1=validator(
                _["field1"]["field1_1"][ANY](gte,9),
                _["field2"](size(gte,100))
            );
    BOOST_CHECK(v3.apply(pa2));
    rep1.clear();
    BOOST_CHECK(v3.apply(pa3)); // must be ok because ANY is not strict
    rep1.clear();
    BOOST_CHECK(v3_1.apply(pa2));
    rep1.clear();
    BOOST_CHECK(v3_1.apply(pa3)); // must be ok because ANY is not strict
    rep1.clear();

    BOOST_CHECK(v3.apply(pa6));
    rep1.clear();
    BOOST_CHECK(v3.apply(pa5)); // must be ok because ANY is not strict
    rep1.clear();
    BOOST_CHECK(v3_1.apply(pa6));
    rep1.clear();
    BOOST_CHECK(v3_1.apply(pa5)); // must be ok because ANY is not strict
    rep1.clear();

    //"check that there is no extra copy in strict_any()"
    {
        std::ignore=make_prevalidation_adapter(_["field1"]["field1_1"],strict_any(NonCopyable{}),rep1);
        NonCopyable nc;
        std::ignore=make_prevalidation_adapter(_["field1"]["field1_1"],strict_any(nc),rep1);
    }

    // check strict ANY

    int val8=8;
    auto pa8=make_prevalidation_adapter(_["field1"]["field1_1"][1],strict_any(val8),rep1);
    BOOST_CHECK(!v3.apply(pa8));
    BOOST_CHECK_EQUAL(rep1,std::string("at least one element of field1_1 of field1 must be greater than or equal to 9"));
    rep1.clear();
    BOOST_CHECK(!v3_1.apply(pa8));
    BOOST_CHECK_EQUAL(rep1,std::string("at least one element of field1_1 of field1 must be greater than or equal to 9"));
    rep1.clear();

    int val10=10;
    auto pa10=make_prevalidation_adapter(_["field1"]["field1_1"][2],strict_any(val10),rep1);
    BOOST_CHECK(v3.apply(pa10));
    rep1.clear();
    BOOST_CHECK(v3_1.apply(pa10));
    rep1.clear();

    auto pa11=make_prevalidation_adapter(_["field1"]["field1_1"][3],strict_any(11),rep1);
    BOOST_CHECK(v3.apply(pa11));
    rep1.clear();
    BOOST_CHECK(v3_1.apply(pa11));
    rep1.clear();

    auto pa11_1=make_prevalidation_adapter(_["field1"]["field1_1"][4],strict_any(7),rep1);
    BOOST_CHECK(!v3.apply(pa11_1));
    BOOST_CHECK_EQUAL(rep1,std::string("at least one element of field1_1 of field1 must be greater than or equal to 9"));
    rep1.clear();
    BOOST_CHECK(!v3_1.apply(pa11_1));
    BOOST_CHECK_EQUAL(rep1,std::string("at least one element of field1_1 of field1 must be greater than or equal to 9"));
    rep1.clear();

    static_assert(hana::is_a<element_aggregation_tag,decltype(ANY)>,"");
    static_assert(hana::is_a<element_aggregation_tag,decltype(ALL)>,"");

    auto eq1=path_types_equal(ANY,ALL);
    static_assert(decltype(eq1)::value,"");
    auto eq2=same_member_path_types(_["field1"]["field1_1"][ANY],_["field1"]["field1_1"][ALL]);
    static_assert(decltype(eq2)::value,"");

    // validation with vector
    auto vec7=std::vector<int>{1,2,3,4};
    auto pa7=make_prevalidation_adapter(_["field1"]["field1_1"],strict_any(range(vec7)),rep1);
    BOOST_CHECK(!v3.apply(pa7));

    BOOST_CHECK_EQUAL(rep1,std::string("at least one element of field1_1 of field1 must be greater than or equal to 9"));
    rep1.clear();
    BOOST_CHECK(!v3_1.apply(pa7));
    BOOST_CHECK_EQUAL(rep1,std::string("at least one element of field1_1 of field1 must be greater than or equal to 9"));
    rep1.clear();

    auto vec9=std::vector<int>{1,2,3,4,9};
    auto pa9=make_prevalidation_adapter(_["field1"]["field1_1"],strict_any(range(vec9)),rep1);
    BOOST_CHECK(v3.apply(pa9));
    rep1.clear();

    BOOST_CHECK(v3_1.apply(pa9));
    rep1.clear();

    auto pa12=make_prevalidation_adapter(_["field1"]["field1_1"],strict_any(range({1,2,3,4})),rep1);
    BOOST_CHECK(!v3.apply(pa12));
    BOOST_CHECK_EQUAL(rep1,std::string("at least one element of field1_1 of field1 must be greater than or equal to 9"));
    rep1.clear();
    BOOST_CHECK(!v3_1.apply(pa12));
    BOOST_CHECK_EQUAL(rep1,std::string("at least one element of field1_1 of field1 must be greater than or equal to 9"));
    rep1.clear();

    // check literals
    auto v4=validator(
                _["field1"]["field1_1"](ANY(value(gte,"Hello"))),
                _["field2"](size(gte,100))
            );

    auto pa13=make_prevalidation_adapter(_["field1"]["field1_1"][0],strict_any("Hello"),rep1);
    auto pa14=make_prevalidation_adapter(_["field1"]["field1_1"][1],"Hello",rep1);
    auto pa14_1=make_prevalidation_adapter(_["field1"]["field1_1"][2],strict_any("Aaa"),rep1);

    BOOST_CHECK(v4.apply(pa13));
    rep1.clear();
    BOOST_CHECK(v4.apply(pa14));
    rep1.clear();
    BOOST_CHECK(!v4.apply(pa14_1));
    BOOST_CHECK_EQUAL(std::string("at least one element of field1_1 of field1 must be greater than or equal to Hello"),rep1);
    rep1.clear();

    auto v5=validator(
                _["field1"]["field1_1"](ALL(value(gte,"Hello"))),
                _["field2"](size(gte,100))
            );
    auto pa15=make_prevalidation_adapter(_["field1"]["field1_1"][0],"Zzz",rep1);
    BOOST_CHECK(v5.apply(pa15));
    rep1.clear();
    auto pa16=make_prevalidation_adapter(_["field1"]["field1_1"][1],"Aaa",rep1);
    BOOST_CHECK(!v5.apply(pa16));
    BOOST_CHECK_EQUAL(std::string("each element of field1_1 of field1 must be greater than or equal to Hello"),rep1);
    rep1.clear();
}

BOOST_AUTO_TEST_CASE(CheckUpdateValidatedWithSample)
{
    error_report err;
    std::map<std::string,std::string> m6{
        {"field1","value1"},
        {"field2","value2"},
        {"field3","value3"}
    };
    auto v6=validator(
                _["field3"](value(gte,_(m6)))
            );
    set_validated(m6,_["field3"],"aaaaa",v6,err);
    BOOST_CHECK(err);
    BOOST_CHECK_EQUAL(err.message(),std::string("field3 must be greater than or equal to field3 of sample"));
    set_validated(m6,_["field3"],"zzzzz",v6,err);
    BOOST_CHECK(!err);

    std::map<std::string,std::vector<std::string>> sample7{
        {"field1",{"value1"}},
        {"field2",{"value2"}},
        {"field3",{"value3"}}
    };

    // test member not existing in sample
    std::map<std::string,std::vector<std::string>> m7{
        {"field1",{"value1"}},
        {"field2",{"value2"}},
        {"field3",{"0"}}
    };
    auto v7=validator(
                _["field3"](ALL(value(gte,_(sample7))))
            );

    set_validated(m7,_["field3"][0],"aaaaa",v7,err);
    BOOST_CHECK(!err);

    auto v8=validator(
                _["field3"](ANY(value(gte,_(sample7))))
            );
    set_validated(m7,_["field3"][0],"aaaaa",strict_any(v8),err);
    BOOST_CHECK(!err); // not existing member in sample is ignored
    set_validated(m7,_["field3"][0],"zzzzz",strict_any(v8),err);
    BOOST_CHECK(!err);
    BOOST_CHECK_EQUAL(m7["field3"][0],"zzzzz");
}

#endif

BOOST_AUTO_TEST_SUITE_END()
