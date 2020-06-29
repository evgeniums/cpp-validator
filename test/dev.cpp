#include <string>
#include <vector>
#include <iostream>

#include <boost/test/unit_test.hpp>

#include <dracosha/validator/validator.hpp>

using namespace dracosha::validator;

BOOST_AUTO_TEST_SUITE(TestOperators)

struct TestRefStruct
{
    TestRefStruct()
    {
        BOOST_TEST_MESSAGE("Default TestRefStruct ctor");
    }
    ~TestRefStruct()
    {
        BOOST_TEST_MESSAGE("TestRefStruct dtor");
    }

    TestRefStruct(const TestRefStruct&)=delete;

    TestRefStruct(TestRefStruct&&)
    {
        BOOST_TEST_MESSAGE("Move TestRefStruct ctor");
    }
};

BOOST_AUTO_TEST_CASE(CheckGetRef)
{
    std::map<size_t,TestRefStruct> m;
    m.emplace(std::make_pair(1,TestRefStruct()));

    using T1 = decltype(m);
    using T2 = decltype(1u);

    static_assert(!can_get<T1, T2>.property(),"");
    static_assert(can_get<T1, T2>.at(), "");
    static_assert(can_get<T1, T2>.brackets(), "");

    const auto& a=get(m,1u);
    std::ignore=a;
}

BOOST_AUTO_TEST_CASE(CheckGteOperator)
{
    BOOST_CHECK(gte(10,5));
    BOOST_CHECK(gte(10,10));
    BOOST_CHECK(!gte(5,10));

    BOOST_CHECK(validate(10,gte,5));
    BOOST_CHECK(validate(10,gte,10));
    BOOST_CHECK(!validate(5,gte,10));

    BOOST_CHECK(validate(lazy([](){return 10;}),gte,lazy([](){return 5;})));
    BOOST_CHECK(validate(lazy([](){return 10;}),gte,lazy([](){return 10;})));
    BOOST_CHECK(!validate(lazy([](){return 5;}),gte,lazy([](){return 10;})));
}

struct Aaa
{
    size_t size=10;
};

struct Bbb
{
};

BOOST_AUTO_TEST_CASE(CheckProperty)
{
    std::vector<char> v(5);
    BOOST_CHECK_EQUAL(property(v,size),v.size());
    BOOST_CHECK(!property(v,empty));
    BOOST_CHECK(validate(size,v,gte,3));

    Aaa a;
    BOOST_CHECK_EQUAL(property(a,size),10);
    BOOST_CHECK(!validate(size,a,gte,100));

    auto check=prepare_dispatcher(size,gte,9);
    BOOST_CHECK(check(a));

    auto sz=size(gte,7);
    BOOST_CHECK(!sz(v));
    BOOST_CHECK(sz(a));

    auto val=value(gte,3);
    BOOST_CHECK(val(10));
    BOOST_CHECK(val(3));
    BOOST_CHECK(!val(1));
}

BOOST_AUTO_TEST_CASE(CheckGet)
{
    std::string str("hello");
    static_assert(decltype(size)::has<std::string>(),"");
    static_assert(has_property<std::string,decltype(size)>(),"");
    BOOST_CHECK_EQUAL(get(str,size),str.size());
    BOOST_CHECK_EQUAL(get(str,4),str.at(4));
}

BOOST_AUTO_TEST_CASE(CheckSingleValidatorCtor)
{
    BOOST_CHECK_EQUAL(_["hello"].key(),std::string("hello"));
    BOOST_CHECK_EQUAL(_[100].key(),100);
    std::string h="hello";
    BOOST_CHECK_EQUAL(_[h].key(),std::string("hello"));
}

BOOST_AUTO_TEST_CASE(CheckSingleValidatorValueOp)
{
    std::vector<int> vec1={1,2,3,4,5};
    std::vector<int> vec2={10,20,30};

    auto v1=_[2](gte,30);
    BOOST_CHECK(!v1.apply(vec1));
    BOOST_CHECK(v1.apply(vec2));

    auto v2=_[size](gte,5);
    BOOST_CHECK(v2.apply(vec1));
    BOOST_CHECK(!v2.apply(vec2));
}

BOOST_AUTO_TEST_CASE(CheckAggregate)
{
    auto v1=value(gte,"abcd");
    auto v2=_[size](gte,5);

    std::string str1("abcdef");
    BOOST_CHECK(apply(str1,v2));
    BOOST_CHECK(apply(str1,v1));

    auto and_tuple=hana::make_tuple(v1,v2);
    BOOST_CHECK(detail::aggregate_and(str1,and_tuple));
    BOOST_CHECK(detail::aggregate_or(str1,and_tuple));

    std::string str2("00000000");
    BOOST_CHECK(!detail::aggregate_and(str2,and_tuple));
    BOOST_CHECK(detail::aggregate_or(str2,and_tuple));

    std::string str3("000");
    BOOST_CHECK(!detail::aggregate_and(str3,and_tuple));
    BOOST_CHECK(!detail::aggregate_or(str3,and_tuple));
}

BOOST_AUTO_TEST_CASE(CheckLogicalAndOr)
{
    auto v1=value(gte,"abcd");
    auto v2=_[size](gte,5);

    std::string str1("abcdef");
    std::string str2("00000000");
    std::string str3("000");

    auto and1=AND(v1,v2);
    BOOST_CHECK(and1.apply(str1));
    BOOST_CHECK(!and1.apply(str2));
    BOOST_CHECK(!and1.apply(str3));

    auto and2=value(gte,"abcd") ^AND^ _[size](gte,5);
    BOOST_CHECK(and2.apply(str1));
    BOOST_CHECK(!and2.apply(str2));
    BOOST_CHECK(!and2.apply(str3));

    auto or1=OR(v1,v2);
    BOOST_CHECK(or1.apply(str1));
    BOOST_CHECK(or1.apply(str2));
    BOOST_CHECK(!or1.apply(str3));

    auto or2=value(gte,"abcd") ^OR^ _[size](gte,5);
    BOOST_CHECK(or2.apply(str1));
    BOOST_CHECK(or2.apply(str2));
    BOOST_CHECK(!or2.apply(str3));
}

BOOST_AUTO_TEST_CASE(CheckMapValidator)
{
    std::map<std::string,std::string> m;
    m["one"]="one_value";
    m["two"]="two_value";
    m["three"]="three_value";
    m["four"]="four_value";
    m["five"]="five_value";

    auto v01=_["one"](gte,"on");
    auto v02=_["four"](gte,"fo");
    auto v0=AND(v01,v02);
    BOOST_CHECK(v0.apply(m));

    auto vv1=_["four"](value(gte,"four_val"));
    BOOST_CHECK(vv1.apply(m));

    auto vv2=_["five"](size(gte,9));
    BOOST_CHECK(vv2.apply(m));

    std::map<int,int> m_int={{1,50},{2,40},{3,30},{4,20},{5,10}};
    auto vv3=_[1](value(gte,_[5]));
    BOOST_CHECK(vv3.apply(m_int));

    auto vv4=_[1](AND(value(gte,_[5])));
    BOOST_CHECK(vv4.apply(m_int));

    auto v1=validator(
            _[size](gte,5),
            _["one"](gte,"one_v"),
            _["two"](gte,"t"),
            _["four"](value(gte,"four_val")),
            _["five"](value(gte,"five") ^AND^ size(gte,9))
            );

    BOOST_CHECK(v1.apply(m));

    std::map<std::string,std::string> m1;
    m1["one"]="one_value";
    m1["two"]="two_value";
    BOOST_CHECK(!v1.apply(m1));
}

BOOST_AUTO_TEST_CASE(CheckNestedValidator)
{
    auto v0=_["first_map"]["one_2"];
    BOOST_CHECK_EQUAL(v0.path_str(),std::string("first_map.one_2"));

    std::map<std::string,std::map<std::string,std::string>> m;
    m["first_map"]={std::make_pair("one","one_value"),std::make_pair("two","two_value"),std::make_pair("three","three_value")};
    m["second_map"]={std::make_pair("one_2","one_value_2"),std::make_pair("two_2","two_value_2"),std::make_pair("three_2","three_value_2")};

    auto v1=validator(
            _[size](gte,2),
            _["first_map"](size(gte,3)),
            _["first_map"]["one"](size(gte,9)),
            _["second_map"]["two_2"](gte,"two_value_2")
            );
    BOOST_CHECK(v1.apply(m));

    auto v2=validator(
            _[size](gte,2),
            _["first_map"](size(gte,3)),
            _["first_map"]["one"](size(gte,9)),
            _["second_map"]["one_2"](gte,"one_value_200")
            );
    BOOST_CHECK(!v2.apply(m));
}

BOOST_AUTO_TEST_CASE(CheckOtherSelfField)
{
    std::map<std::string,std::string> m;
    m["one"]="one_value";
    m["two"]="two_value";
    m["three"]="three_value";
    m["four"]="four_value";
    m["five"]="one_value";

    auto v01=_["one"](gte,_["five"]);
    BOOST_CHECK(v01.apply(m));

    auto v02=_["one"](value(gte,_["three"]));
    BOOST_CHECK(!v02.apply(m));

    auto v1=validator(
            _["one"](gte,_["five"]),
            _["two"](value(gte,_["five"])),
            _["three"](size(gte,_["one"]) ^OR^ value(gte,_["two"]))
            );
    BOOST_CHECK(v1.apply(m));
}

BOOST_AUTO_TEST_CASE(CheckMasterReferenceField)
{
    std::map<int,int> m={{1,50},{2,40},{3,30},{4,20},{5,10}};
    const std::map<int,int> ref1={{1,50},{2,40}};
    const std::map<int,int> ref2={{1,500},{2,40},{1,1},{1,1},{1,1}};

    auto v01=_[1](gte,_(ref1));
    BOOST_CHECK(v01.apply(m));
    auto v02=_[1](gte,_(ref2));
    BOOST_CHECK(!v02.apply(m));

    auto v1=validator(
        _[1](gte,_(ref1)),
        _[2](gte,_(ref2))
    );
    BOOST_CHECK(v1.apply(m));

    auto v2=validator(
        _[1](gte,_(ref2)),
        _[2](gte,_(ref1))
    );
    BOOST_CHECK(!v2.apply(m));

    std::map<std::string,std::string> m_str;
    m_str["one"]="one_value";
    m_str["two"]="two_value";

    std::map<std::string,std::string> ref_str;
    ref_str["one"]="one_v";
    ref_str["two"]="two_v";
    auto v3=validator(
        _["one"](size(gte,_(ref_str))),
        _["two"](size(gte,_(ref_str)))
    );
    BOOST_CHECK(v3.apply(m_str));
}

BOOST_AUTO_TEST_CASE(CheckContains)
{
    std::map<std::string,std::string> m;
    m["one"]="one_value";
    m["two"]="two_value";

    BOOST_CHECK(contains(m,"one"));
    BOOST_CHECK(!contains(m,"ten"));
    BOOST_CHECK(contains(m,size));

    int dummy=0;
    BOOST_CHECK(!contains(dummy,"one"));

    std::map<int,int> ref1={{1,50},{2,40}};
    BOOST_CHECK(contains(ref1,1));
    BOOST_CHECK(!contains(ref1,3));

    BOOST_CHECK(contains(m,"one"));
}

BOOST_AUTO_TEST_CASE(CheckSimpleExists)
{
    std::map<std::string,std::string> m1;
    m1["one"]="one_value";
    m1["two"]="two_value";

    auto chain1=hana::make_tuple("one");
    BOOST_CHECK(check_exists(m1,chain1));

    auto chain2=hana::make_tuple("hello");
    BOOST_CHECK(!check_exists(m1,chain2));

    std::map<std::string,std::map<std::string,std::string>> m2;
    m2["first_map"]={std::make_pair("one","one_value"),std::make_pair("two","two_value"),std::make_pair("three","three_value")};
    m2["second_map"]={std::make_pair("one_2","one_value_2"),std::make_pair("two_2","two_value_2"),std::make_pair("three_2","three_value_2")};

    auto chain3=hana::make_tuple("first_map");
    BOOST_CHECK(check_exists(m2,chain3));
    BOOST_CHECK(!check_exists(m2,chain2));
    auto chain4=hana::make_tuple("first_map","one");
    BOOST_CHECK(check_exists(m2,chain4));
    auto chain5=hana::make_tuple("first_map","ten");
    BOOST_CHECK(!check_exists(m2,chain5));

    std::map<std::string,std::map<int,std::string>> m3;
    m3["first_map"]={std::make_pair(1,"one_value"),std::make_pair(2,"two_value"),std::make_pair(3,"three_value")};
    m3["second_map"]={std::make_pair(10,"one_value_2"),std::make_pair(20,"two_value_2"),std::make_pair(30,"three_value_2")};
    auto chain6=hana::make_tuple("first_map",1);
    BOOST_CHECK(check_exists(m3,chain6));
    auto chain7=hana::make_tuple("first_map",10);
    BOOST_CHECK(!check_exists(m3,chain7));
    auto chain8=hana::make_tuple("second_map",1);
    BOOST_CHECK(!check_exists(m3,chain8));
    auto chain9=hana::make_tuple("second_map",10);
    BOOST_CHECK(check_exists(m3,chain9));
}

BOOST_AUTO_TEST_CASE(CheckExists)
{
    std::map<std::string,std::string> m1;
    m1["one"]="one_value";
    m1["two"]="two_value";

    auto v01=_["one"](exists,true);
    BOOST_CHECK(v01.apply(m1));

    auto v02=_["ten"](exists,true);
    BOOST_CHECK(!v02.apply(m1));

    auto v03=_["ten"](exists,false);
    BOOST_CHECK(v03.apply(m1));

    auto v04=_["two"](exists,false);
    BOOST_CHECK(!v04.apply(m1));

    auto v1=validator(
        _["one"](exists,true),
        _["ten"](exists,false)
    );
    BOOST_CHECK(v1.apply(m1));
}

BOOST_AUTO_TEST_CASE(CheckContainsCompileTime)
{
    std::map<std::string,std::string> m1;
    m1["one"]="one_v";
    m1["two"]="two_v";

    auto chain1=hana::make_tuple(std::string("one"));
    auto chain2=hana::make_tuple(std::string("one"),100);
    auto chain3=hana::make_tuple(std::string("one"),std::string("ten"));
    auto chain4=hana::make_tuple(size);

    std::map<std::string,std::map<int,int>> m3;
    m3["first_map"]={std::make_pair(1,100),std::make_pair(2,200),std::make_pair(3,300)};
    m3["second_map"]={std::make_pair(10,1000),std::make_pair(20,2000),std::make_pair(30,3000)};

    constexpr auto chain1_c=hana::transform(chain1,hana::make_type);
    constexpr auto chain2_c=hana::transform(chain2,hana::make_type);
    constexpr auto chain3_c=hana::transform(chain3,hana::make_type);
    constexpr auto chain4_c=hana::transform(chain4,hana::make_type);

    constexpr auto m1_c=hana::make_type(m1);
    constexpr auto m3_c=hana::make_type(m3);

    using type = typename check_member_t<decltype(m1),std::string>::type;
    auto type_c = check_member(hana::type_c<decltype(m1)>,hana::type_c<std::string>);
    auto type_c1 = check_member(hana::type_c<decltype(m1)>, hana::front(chain1_c));

    BOOST_HANA_CONSTANT_CHECK(!hana::is_nothing(hana::sfinae(check_member)(m1_c,hana::front(chain1_c))));
    BOOST_HANA_CONSTANT_CHECK(!hana::is_nothing(
        hana::monadic_fold_left<hana::optional_tag>(chain1_c,m1_c,hana::sfinae(check_member))
        )
    );
    BOOST_HANA_CONSTANT_CHECK(!hana::is_nothing(
            hana::monadic_fold_left<hana::optional_tag>(chain2_c,m3_c,hana::sfinae(check_member))
        )
    );
    BOOST_HANA_CONSTANT_CHECK(!hana::is_nothing(
            hana::monadic_fold_left<hana::optional_tag>(chain2_c,m1_c,hana::sfinae(check_member))
        )
    );
    BOOST_HANA_CONSTANT_CHECK(hana::is_nothing(
            hana::monadic_fold_left<hana::optional_tag>(chain3_c,m1_c,hana::sfinae(check_member))
        )
    );
    BOOST_HANA_CONSTANT_CHECK(!hana::is_nothing(
            hana::monadic_fold_left<hana::optional_tag>(chain4_c,m1_c,hana::sfinae(check_member))
        )
    );

    Bbb m4;
    constexpr auto m4_c=hana::make_type(m4);
    BOOST_HANA_CONSTANT_CHECK(hana::is_nothing(
            hana::monadic_fold_left<hana::optional_tag>(chain4_c,m4_c,hana::sfinae(check_member))
        )
    );
    BOOST_HANA_CONSTANT_CHECK(hana::is_nothing(
            hana::monadic_fold_left<hana::optional_tag>(chain1_c,m4_c,hana::sfinae(check_member))
        )
    );

    BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(CheckCanGetAtCompileTime)
{
    std::map<std::string,int> m1;
    m1["one"]=1;
    m1["two"]=2;

    static_assert(can_get<decltype(m1),std::string>(),"");
    static_assert(!can_get<decltype(m1),int>(),"");
    static_assert(can_get<decltype(m1),decltype(size)>(),"");

    BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(CheckExistsWithCompileTime)
{
    std::map<std::string,std::string> m1;
    m1["one"]="one_value";
    m1["two"]="two_value";

    auto v01=_[1.0f](exists,false);
    BOOST_CHECK(v01.apply(m1));

    auto v02=_[10.0f](exists,true);
    BOOST_CHECK(!v02.apply(m1));

    auto v03=_[size](exists,true);
    BOOST_CHECK(v03.apply(m1));
    BOOST_CHECK(!v03.apply(Bbb()));
    BOOST_CHECK(!v02.apply(Bbb()));

    auto v04=_[size](gte,10);
    BOOST_CHECK(v04.apply(Aaa()));

    std::map<std::string,std::map<int,int>> m3;
    m3["first_map"]={std::make_pair(1,100),std::make_pair(2,200),std::make_pair(3,300)};
    m3["second_map"]={std::make_pair(10,1000),std::make_pair(20,2000),std::make_pair(30,3000)};

    auto v1=validator(
        _["first_map"][size](exists,true),
        _["first_map"][1](exists,true),
        _["first_map"][10][size](exists,false)
    );
    BOOST_CHECK(v1.apply(m3));

//    auto v05=_["first_map"][10][size](gte,10);
//    BOOST_CHECK(!v05.apply(m3));

    std::vector<int> vec1={10,20,30,40,50};
    auto v06=_[3](exists,true);
    BOOST_CHECK(!v06.apply(vec1));
}

/**

@todo
    Configurable abort if not found
    Translation
    Multiple elements
    Check single field
    Check non copyable objects and keys
*/

BOOST_AUTO_TEST_SUITE_END()
