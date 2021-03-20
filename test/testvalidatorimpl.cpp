#include <string>
#include <vector>
#include <iostream>

#include <boost/test/unit_test.hpp>

#include <dracosha/validator/utils/make_types_tuple.hpp>
#include <dracosha/validator/validator.hpp>
#include <dracosha/validator/utils/wrap_object.hpp>
#include <dracosha/validator/utils/conditional_fold.hpp>
#include <dracosha/validator/utils/copy.hpp>

using namespace DRACOSHA_VALIDATOR_NAMESPACE;

BOOST_AUTO_TEST_SUITE(TestValidatorImpl)

#if 1
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
    TestRefStruct& operator = (const TestRefStruct&)=delete;
    TestRefStruct& operator = (TestRefStruct&&)=delete;

    TestRefStruct(TestRefStruct&&) noexcept
    {
        BOOST_TEST_MESSAGE("Move TestRefStruct ctor");
    }
};

struct TestStruct2
{
    TestStruct2()
    {
        BOOST_TEST_MESSAGE("TestStruct2 default ctor");
    }
    ~TestStruct2()
    {
        BOOST_TEST_MESSAGE("TestStruct2 dtor");
    }

    TestStruct2(const TestStruct2&)
    {
        BOOST_TEST_MESSAGE("TestStruct2 copy ctor");
    }
    TestStruct2& operator = (const TestStruct2&)
    {
        BOOST_TEST_MESSAGE("TestStruct2 copy assignment operator");
        return *this;
    }
    TestStruct2& operator = (TestStruct2&&)
    {
        BOOST_TEST_MESSAGE("TestStruct2 move assignment operator");
        return *this;
    }

    TestStruct2(TestStruct2&&) noexcept
    {
        BOOST_TEST_MESSAGE("TestStruct2 move ctor");
    }
};

BOOST_AUTO_TEST_CASE(CheckFoldAnd)
{
    auto f1=hana::partial([](bool in){return in;});
    auto f2=hana::partial(f1,true);
    auto f3=hana::partial(f1,false);

    BOOST_CHECK(f2());
    BOOST_CHECK(!f3());

    auto f4=[](auto&& op)
    {
        return op();
    };
    auto f5=hana::partial(hana::partial(f4),f2);
    BOOST_CHECK(f5());

    auto f6=[](auto&& op1, auto&& op2)
    {
        return op1()&&op2();
    };

    auto fns=hana::make_tuple(f2,f3);
    auto fnst=hana::transform(fns,[f4](auto&& fn){return hana::partial(f4,fn);});
    BOOST_CHECK(!hana::fuse(f6)(fns));
    BOOST_CHECK(!hana::fuse(f6)(fnst));
}

BOOST_AUTO_TEST_CASE(CheckAdjustStorable)
{
    auto c1="hello";
    static_assert(std::is_constructible<const char*,decltype(c1)>::value,"");
    const auto& c2=c1;
    static_assert(std::is_constructible<const char*,decltype(c2)>::value,"");
    char c3[5];
    static_assert(std::is_constructible<const char*,decltype(c3)>::value,"");
    std::string s1{"Hi"};
    static_assert(!std::is_constructible<const char*,decltype(s1)>::value,"");

    auto st1=adjust_storable(s1);
    static_assert(hana::is_a<object_wrapper_tag,decltype(st1)>,"");
    static_assert(std::is_same<std::string,unwrap_object_t<decltype(st1)>>::value,"");

    auto st2=adjust_storable("Hello");
    static_assert(!hana::is_a<object_wrapper_tag,decltype(st2)>,"");
    static_assert(std::is_same<std::string,unwrap_object_t<decltype(st2)>>::value,"");
    BOOST_CHECK_EQUAL(unwrap_object(st1),std::string("Hi"));
    BOOST_CHECK_EQUAL(unwrap_object(st2),std::string("Hello"));
}

BOOST_AUTO_TEST_CASE(CheckMakeMember)
{
    std::ignore=make_plain_member(int(1));
    auto&& ts1=hana::make_tuple(int(1));
    auto&& i1=hana::front(ts1);
    static_assert(std::is_same<decltype(i1),int&>::value,"");
    std::ignore=make_plain_member(i1);

    std::ignore=make_member(hana::make_tuple(1));
    std::ignore=make_member(hana::make_tuple(1,1.0f,"hello"));
    auto m3=make_member(hana::make_tuple(1,2,std::string("hi")));
    std::ignore=make_member(std::make_tuple(1,1.0f,"hello"));

    auto m3_1=_[1][2]["hi"];
    BOOST_CHECK(m3.equals(m3_1));
    BOOST_CHECK(m3_1.equals(m3));

    std::string hello("hello");
    auto m5=_[1]["haha"][std::string("hi")][hello][size];
    std::ignore=_[hello];
    BOOST_CHECK(!m5.equals(m3_1));
    BOOST_CHECK(!m3_1.equals(m5));

    BOOST_CHECK(hana::back(m3.path())==hana::back(m3_1.path()));

    std::ignore=make_member(std::make_tuple(value));

    auto m1=make_member(std::make_tuple(1,2,"hi",size));
    std::ignore=make_member(m1.path());
    std::ignore=inherit_member(m1.path(),m1);

    std::vector<int> vec1={1,2,3,4,5};

    auto index=wrap_object(1);
    auto ok=can_check_contains<decltype(vec1),decltype(int())>();
    BOOST_CHECK(ok);
    BOOST_CHECK_EQUAL(vec1[index],2);
    auto mv=make_member(std::make_tuple(3));
    BOOST_CHECK(hana::value(is_member_path_valid(vec1,mv.path())));

    auto v2=m1(gte,5);
    BOOST_CHECK(v2.apply(vec1));

    auto m7=make_member(hana::make_tuple("hi"));
    auto m8=make_member(hana::make_tuple(std::string("hi")));
    auto m9=make_member(m7.path());
    auto m10=make_member(m8.path());

    BOOST_TEST_MESSAGE("Key as lvalue begin");
    TestStruct2 ts12{};
    auto m12=_[ts12][10][11];
    auto m13=m12.parent();
    std::ignore=m13;
    BOOST_TEST_MESSAGE("Key as lvalue end");

    BOOST_TEST_MESSAGE("Key as rvalue begin");
    auto m14=_[TestStruct2{}][10];
    BOOST_TEST_MESSAGE("Key as rvalue end");

#ifdef _DRACOSHA_ALWAYS_UNDEF
    // must fail
    std::ignore=make_member(hana::tuple<>());
#endif
}

BOOST_AUTO_TEST_CASE(CheckTupleConversions)
{
    std::string hello{"hello"};

    auto std_tuple1=std::make_tuple(1,2,hello);
    auto hana_tuple1=hana::make_tuple(1,2,hello);

    auto hana_tuple2=std_to_hana_tuple(std_tuple1);
    static_assert(std::is_same<decltype(hana_tuple1),decltype(hana_tuple2)>::value,"Invalid conversion type");
    BOOST_CHECK(hana::equal(hana_tuple1,hana_tuple2));

    auto std_tuple2=hana_to_std_tuple(hana_tuple1);
    static_assert(std::is_same<decltype(std_tuple1),decltype(std_tuple2)>::value,"Invalid conversion type");
    BOOST_CHECK(std_tuple1==std_tuple2);

    constexpr auto types1=make_types_tuple(float{10.0f},"hello",int{100});
    constexpr auto types2=hana::tuple_t<float,const char*,int>;
    static_assert(hana::equal(types1,types2),"");

    BOOST_CHECK(std::get<0>(std_tuple1)==1);
    BOOST_CHECK(hana_tuple1[hana::size_c<0>]==1);
}

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

    BOOST_CHECK(validate_inline(10,gte,5));
    BOOST_CHECK(validate_inline(10,gte,10));
    BOOST_CHECK(!validate_inline(5,gte,10));

    BOOST_CHECK(validate_inline(lazy([](){return 10;}),gte,lazy([](){return 5;})));
    BOOST_CHECK(validate_inline(lazy([](){return 10;}),gte,lazy([](){return 10;})));
    BOOST_CHECK(!validate_inline(lazy([](){return 5;}),gte,lazy([](){return 10;})));
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
    BOOST_CHECK(validate_inline(size,v,gte,3));

    Aaa a;
    BOOST_CHECK_EQUAL(property(a,size),10);
    BOOST_CHECK(!validate_inline(size,a,gte,100));

    auto check=make_property_validator(size,gte,9);
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

    auto vv=get(vec1,2);
    BOOST_CHECK_EQUAL(vv,3);
    auto v0=_[2](exists,true);
    BOOST_CHECK(v0.apply(vec1));

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

BOOST_AUTO_TEST_CASE(CheckNestedMember)
{
    auto v0=_["first_map"]["one_2"];

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

BOOST_AUTO_TEST_CASE(CheckMasterSampleField)
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

    BOOST_CHECK(check_contains(m,"one"));
    BOOST_CHECK(!check_contains(m,"ten"));
    BOOST_CHECK(check_contains(m,size));

    int dummy=0;
    BOOST_CHECK(!check_contains(dummy,"one"));

    std::map<int,int> ref1={{1,50},{2,40}};
    BOOST_CHECK(check_contains(ref1,1));
    BOOST_CHECK(!check_contains(ref1,3));

    BOOST_CHECK(check_contains(m,"one"));
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
    std::ignore = type();
    std::ignore = check_member(hana::type_c<decltype(m1)>,hana::type_c<std::string>);
    std::ignore = check_member(hana::type_c<decltype(m1)>, hana::front(chain1_c));

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

BOOST_AUTO_TEST_CASE(CheckExistsValidator)
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

    std::vector<int> vec1={10,20,30,40,50};
    auto v06=_[3](exists,true);
    BOOST_CHECK(v06.apply(vec1));
}

BOOST_AUTO_TEST_CASE(CheckNot)
{
    auto v1=value(gte,"abcd");
    auto v2=_[size](gte,5);

    std::string str1("abcdef");
    std::string str2("012");

    BOOST_CHECK(!detail::logical_not(str1,v1));
    BOOST_CHECK(!detail::logical_not(str1,v2));

    auto not1=NOT(v1);
    auto not2=NOT(v2);

    BOOST_CHECK(!not1.apply(str1));
    BOOST_CHECK(!not2.apply(str1));
    BOOST_CHECK(not1.apply(str2));
    BOOST_CHECK(not2.apply(str2));

    std::map<std::string,std::string> m;
    m["one"]="one_value";
    m["two"]="two_value";
    m["three"]="three_value";
    m["four"]="four_value";
    m["five"]="five_value";

    auto v03=_["one"](gte,"on");
    auto v0=NOT(v03);
    BOOST_CHECK(!v0.apply(m));

    auto v3=validator(
            _[size](gte,5),
            _["one"](gte,"one_v"),
            _["two"](gte,"t"),
            _["four"](value(gte,"four_val")),
            _["five"](NOT(size(gte,100)))
            );
    BOOST_CHECK(v3.apply(m));

    auto v4=validator(
            NOT(
                AND(
                    _[size](gte,5),
                    _["one"](gte,"one_v"),
                    _["two"](gte,"t"),
                    _["four"](value(gte,"four_val")),
                    _["five"](NOT(size(gte,100)))
                )
            )
            );
    BOOST_CHECK(!v4.apply(m));

    auto v5=validator(
            NOT(
                _[size](gte,5),
                _["one"](gte,"one_v"),
                _["two"](gte,"t"),
                _["four"](value(gte,"four_val")),
                _["five"](NOT(size(gte,100)))
            )
            );
    BOOST_CHECK(!v5.apply(m));

    auto v6=NOT(v5);
    BOOST_CHECK(v6.apply(m));
}

BOOST_AUTO_TEST_CASE(CheckIsPair)
{
    std::map<size_t,std::string> m1={
        {1,"hi"},
        {2,"hello"}
    };

    auto it=m1.begin();
    static_assert(is_pair_t<decltype(*it)>::value,"");
    BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(CheckIsMasterSample)
{
    int a1=0;
    static_assert(!is_master_sample<decltype(a1)>::value,"");

    auto a2=_(a1,"description");
    static_assert(!is_master_sample<decltype(a2)>::value,"");

    auto s1=_(a1);
    static_assert(is_master_sample<decltype(s1)>::value,"");

    auto s2=_(s1,"description");
    static_assert(is_master_sample<decltype(s2)>::value,"");

    std::map<int,int> m={{1,5},{2,40},{3,30},{4,20},{5,10}};
    const std::map<int,int> ref1={{1,50},{2,40}};
    auto v01=_[1](gte,_(_(ref1),"sample1"));
    BOOST_CHECK(!v01.apply(m));
}

namespace
{

template <typename T, typename=hana::when<true>>
struct foo_tmpl_1arg
{
};

template <typename T>
struct foo_tmpl_1arg<T,hana::when<std::is_integral<T>::value>>
{
    using type=T;
};


template <typename ... Args>
struct foo_tmpl
{
};
template <typename T>
struct foo_tmpl<T>
{
    using type=typename foo_tmpl_1arg<T>::type;
};

}

BOOST_AUTO_TEST_CASE(CheckMemberHelper)
{
    static_assert(std::is_integral<int>::value,"");
    using type=foo_tmpl<int>::type;
    static_assert(std::is_same<type,int>::value,"");

    auto v=_["field1"](gte,100);
    BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(CheckWhileEachOrStatus)
{
    auto ok1=while_each(
                    hana::make_tuple(status::code::ignore,status::code::success),
                    status_predicate_or,
                    status(status::code::ignore),
                    [](status st)
                    {
                        return status(st);
                    }
                );
    BOOST_CHECK(ok1==true);

    auto ok2=while_each(
                    hana::make_tuple(status::code::ignore,status::code::fail),
                    status_predicate_or,
                    status(status::code::ignore),
                    [](status st)
                    {
                        return status(st);
                    }
                );
    BOOST_CHECK(ok2==false);

    auto ok3=while_each(
                    hana::make_tuple(status::code::ignore,status::code::fail,status::code::success),
                    status_predicate_or,
                    status(status::code::ignore),
                    [](status st)
                    {
                        return status(st);
                    }
                );
    BOOST_CHECK(ok3==true);
}

BOOST_AUTO_TEST_CASE(CheckNonCopyableOperand)
{
    TestRefStruct ts;
    std::ignore=_["field1"](gte,ts);
    std::ignore=_["field1"](gte,std::move(ts));

    BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(CheckCopyMoveOperand)
{
    size_t val1=10;
    size_t check_val1=20;
    size_t val2=20;

    auto v1=validator(gte,val1);
    auto v2=validator(gte,copy(val1));
    auto v3=validator(gte,std::move(val2));

    BOOST_CHECK(v1.apply(check_val1));
    BOOST_CHECK(v2.apply(check_val1));
    BOOST_CHECK(v3.apply(check_val1));

    val1=200;
    val2=200;
    BOOST_CHECK(!v1.apply(check_val1));
    BOOST_CHECK(v2.apply(check_val1));
    BOOST_CHECK(v3.apply(check_val1));
}
#endif

BOOST_AUTO_TEST_CASE(CheckMixedMemberAndOr)
{
    std::map<std::string,std::string> m1{
        {"five","five_value"}
    };
    std::map<std::string,std::string> m2{
        {"five","six_value"}
    };

    auto v1=validator(
            _["five"](
                        (value(gte,"five") ^AND^ size(gte,9))
                        ^OR^
                        (value(eq,"six_value") ^AND^ size(eq,9))
                      )
            );
    BOOST_CHECK(v1.apply(m1));
    BOOST_CHECK(v1.apply(m2));
}

BOOST_AUTO_TEST_SUITE_END()
