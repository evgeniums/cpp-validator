#include <optional>
#include <functional>
#include <set>

#include <boost/test/unit_test.hpp>

#include <dracosha/validator/utils/reference_wrapper.hpp>
#include <dracosha/validator/utils/optional.hpp>
#include <dracosha/validator/utils/class_method_args.hpp>
#include <dracosha/validator/check_member_path.hpp>
#include <dracosha/validator/get_member.hpp>
#include <dracosha/validator/check_exists.hpp>
#include <dracosha/validator/variadic_property.hpp>
#include <dracosha/validator/validator.hpp>
#include <dracosha/validator/adapters/reporting_adapter.hpp>

using namespace DRACOSHA_VALIDATOR_NAMESPACE;

namespace {

struct TestStruct
{
    int& func1(int)
    {
        return var;
    }

    int& func2(int,int) &
    {
        return var;
    }

    int func3(int,int,int) &&
    {
        return var;
    }

    const int& func4(int,int,int,int) const
    {
        return var;
    }

    const int& func5(int,int,int,int,int) const &
    {
        return var;
    }

    const int& func6(int,int,int,int,int,int) const &&
    {
        return var;
    }

    int& func7(int,int,int,int,int,int,int) noexcept
    {
        return var;
    }

    int& func8(int,int,int,int,int,int,int,int) & noexcept
    {
        return var;
    }

    int& func9(int,int,int,int,int,int,int,int,int) && noexcept
    {
        return var;
    }

    const int& func10(int,int,int,int,int,int,int,int,int,int) const noexcept
    {
        return var;
    }

    const int& func11(int,int,int,int,int,int,int,int,int,int,int) const & noexcept
    {
        return var;
    }

    const int& func12(int,int,int,int,int,int,int,int,int,int,int,int) const && noexcept
    {
        return var;
    }


    int var=0;
};
}

BOOST_AUTO_TEST_SUITE(TestVariadicProperty)

BOOST_AUTO_TEST_CASE(TestArgs)
{
    TestStruct ts1;

    auto types1=class_method_args<decltype(&decltype(ts1)::func1)>::types();
    static_assert(hana::value(hana::size(types1))==1,"");

    auto types2=class_method_args<decltype(&decltype(ts1)::func2)>::types();
    static_assert(hana::value(hana::size(types2))==2,"");

    auto types3=class_method_args<decltype(&decltype(ts1)::func3)>::types();
    static_assert(hana::value(hana::size(types3))==3,"");

    auto types4=class_method_args<decltype(&decltype(ts1)::func4)>::types();
    static_assert(hana::value(hana::size(types4))==4,"");

    auto types4_1=class_method_args<decltype(&decltype(ts1)::func4)>::types();
    static_assert(hana::value(hana::size(types4_1))==4,"");

    auto types5=class_method_args<decltype(&decltype(ts1)::func5)>::types();
    static_assert(hana::value(hana::size(types5))==5,"");

    auto types6=class_method_args<decltype(&decltype(ts1)::func6)>::types();
    static_assert(hana::value(hana::size(types6))==6,"");

    auto types7=class_method_args<decltype(&decltype(ts1)::func7)>::types();
    static_assert(hana::value(hana::size(types7))==7,"");

    auto types8=class_method_args<decltype(&decltype(ts1)::func8)>::types();
    static_assert(hana::value(hana::size(types8))==8,"");

    auto types9=class_method_args<decltype(&decltype(ts1)::func9)>::types();
    static_assert(hana::value(hana::size(types9))==9,"");

    auto types10=class_method_args<decltype(&decltype(ts1)::func10)>::types();
    static_assert(hana::value(hana::size(types10))==10,"");

    auto types11=class_method_args<decltype(&decltype(ts1)::func11)>::types();
    static_assert(hana::value(hana::size(types11))==11,"");

    auto types12=class_method_args<decltype(&decltype(ts1)::func12)>::types();
    static_assert(hana::value(hana::size(types12))==12,"");

    BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(TestOptional)
{
    int a=10;
    const auto& b=a;
    using ref_type=decltype(cref(b));
    auto c=optional<ref_type>{};
    c.emplace(cref(b));

    BOOST_REQUIRE(c.has_value());
    BOOST_CHECK_EQUAL(c.value(),a);
}

namespace {

struct WithAt{

    size_t at(size_t v) const
    {
        return v;
    }

    bool has(size_t v) const
    {
        return v>30;
    }
};

struct WithRvalueAt{

    WithAt at(size_t) const
    {
        return WithAt{};
    }

    bool has(size_t index) const
    {
        return index<10;
    }
};

struct WithSet{

    const auto& at(size_t) const
    {
        return _set;
    }

    bool has(size_t v) const
    {
        return v>30;
    }

    WithSet():_set({"value1","value2","value3"})
    {}

    std::set<std::string> _set;
};

struct WithSetAt{

    WithSet at(size_t) const
    {
        return WithSet{};
    }

    bool has(size_t index) const
    {
        return index<10;
    }
};

}

BOOST_AUTO_TEST_CASE(TestMemberValueType)
{
    std::map<std::string,std::set<std::string>> m1
    {
        {"key1",{"value1_1","value1_2"}},
        {"key2",{"value2_1","value2_2"}}
    };

    using type=std::decay_t<decltype(get_member(m1,hana::make_tuple("key1","value1_1")))>;
    static_assert(std::is_same<std::string,type>::value,"");

    BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(TestCheckExistsWithOptional)
{
    std::map<std::string,std::set<std::string>> m1
    {
        {"key1",{"value1_1","value1_2"}},
        {"key2",{"value2_1","value2_2"}}
    };
    BOOST_CHECK(check_exists(m1,hana::make_tuple("key1","value1_1")));
    BOOST_CHECK(!check_exists(m1,hana::make_tuple("key1","value1_3")));
    BOOST_CHECK(!check_exists(m1,hana::make_tuple("key2","value1_1")));

    WithRvalueAt wat1;
    BOOST_CHECK(check_exists(wat1,hana::make_tuple(1,50)));
    BOOST_CHECK(!check_exists(wat1,hana::make_tuple(1,20)));
    BOOST_CHECK(!check_exists(wat1,hana::make_tuple(20,50)));

    WithSetAt wat2;
    BOOST_CHECK(check_exists(wat2,hana::make_tuple(1,50,std::string("value1"))));
    BOOST_CHECK(!check_exists(wat2,hana::make_tuple(1,50,std::string("value5"))));
    BOOST_CHECK(!check_exists(wat2,hana::make_tuple(1,20,std::string("value1"))));
    BOOST_CHECK(!check_exists(wat2,hana::make_tuple(20,50,std::string("value1"))));

    BOOST_CHECK(!check_exists(m1,hana::make_tuple("key1",10,20)));
}

namespace {

struct WithChild
{
    ~WithChild()
    {
    }

    WithChild():_m
         {
            {
                22,
                {
                    {100,std::string("hundred")},
                    {1000,std::string("thousand")}
                }
            }
         }
    {}

    int child(int val) const
    {
        return val+1;
    }

    int child_word(int val, const std::string& word) const noexcept
    {
        return val+static_cast<int>(word.size());
    }

    const std::map<int,std::string>& my_map(int index, int offset) const
    {
        return _m.at(index+offset);
    }

    int safe_child_word(int val, const std::string& word) const noexcept
    {
        return val+static_cast<int>(word.size());
    }

    bool has_safe_child_word(int val, const std::string& word) const noexcept
    {
        return val>10 && word.size()>=5;
    }

    bool sum_gte_10(int a, int b) const noexcept
    {
        return (a+b)>=10;
    }

    std::map<
        int,
        std::map<int,std::string>
    > _m;
};

struct WithoutChild
{
};

DRACOSHA_VALIDATOR_VARIADIC_PROPERTY(child)
DRACOSHA_VALIDATOR_VARIADIC_PROPERTY(child_word)
DRACOSHA_VALIDATOR_VARIADIC_PROPERTY(my_map)

DRACOSHA_VALIDATOR_VARIADIC_PROPERTY(has_safe_child_word)
DRACOSHA_VALIDATOR_VARIADIC_PROPERTY_HAS(safe_child_word,has_safe_child_word)

DRACOSHA_VALIDATOR_VARIADIC_PROPERTY_FLAG(sum_gte_10,"must be 10 and more","must be less than 10")

}

BOOST_AUTO_TEST_CASE(TestPropertyAlwaysExistArg1)
{
    static_assert(has_property<WithChild,decltype(child)>(),"");
    static_assert(!has_property<WithoutChild,decltype(child)>(),"");

    WithChild o1;
    static_assert(check_member_path(o1,hana::make_tuple(child)),"");
    std::ignore=get_member(o1,hana::make_tuple(child));
    static_assert(check_member_path(o1,hana::make_tuple(child,10)),"");
    auto res1=get_member(o1,hana::make_tuple(child,10));
    static_assert(std::is_same<decltype(res1),int>::value,"");
    BOOST_CHECK_EQUAL(get_member(o1,hana::make_tuple(child,10)),11);

    auto v1=validator(
        _[child][20](eq,21)
    );
    BOOST_CHECK(v1.apply(o1));
}

BOOST_AUTO_TEST_CASE(TestPropertyAlwaysExistArg2)
{
    WithChild o1;

    auto v1=validator(
        _[child_word][20]["hello"](eq,25)
    );
    BOOST_CHECK(v1.apply(o1));
}

BOOST_AUTO_TEST_CASE(TestPropertyAlwaysExistArg2Nested)
{
    WithChild o1;

    auto v1=validator(
        _[my_map][20][2][1000](eq,"thousand")
    );
    BOOST_CHECK(v1.apply(o1));
    auto v2=validator(
        _[my_map][20][2][1000](eq,"hundred")
    );
    BOOST_CHECK(!v2.apply(o1));
}

BOOST_AUTO_TEST_CASE(TestPropertyWithHasArg2)
{
    WithChild o1;

    auto v1=validator(
        _[safe_child_word][20]["hello"](exists,true)
    );
    BOOST_CHECK(v1.apply(o1));

    auto v2=validator(
        _[safe_child_word][5]["hello"](exists,true)
    );
    BOOST_CHECK(!v2.apply(o1));

    auto v3=validator(
        _[safe_child_word][11]["hi"](exists,true)
    );
    BOOST_CHECK(!v3.apply(o1));

    auto v4=validator(
        _[safe_child_word][20]["hello"](eq,25)
    );
    BOOST_CHECK(v4.apply(o1));

    auto v5=validator(
        _[safe_child_word][20]["hello"](eq,20)
    );
    BOOST_CHECK(!v5.apply(o1));
}

BOOST_AUTO_TEST_CASE(TestPropertyNotation)
{
    WithChild o1;

    auto p1=child_word(20,"hello");
    BOOST_CHECK_EQUAL(p1.get(o1),25);

    auto v1=p1(eq,25);
    BOOST_CHECK(v1(o1));

    auto v2=validator(
        child_word(20,"hello")(eq,25)
    );
    BOOST_CHECK(v2.apply(o1));

    auto v3=validator(
        child_word(20,"hello")(eq,30)
    );
    BOOST_CHECK(!v3.apply(o1));
}

BOOST_AUTO_TEST_CASE(TestFlag)
{
    //! @todo Maybe construct some other report with flags
    //! For example: _property_name_ + "(args list)" + "flag description"
    WithChild o1;
    std::string rep;
    auto ra1=make_reporting_adapter(o1,rep);

    auto v1=validator(
        sum_gte_10(5,7)(flag,true)
    );
    BOOST_CHECK(v1.apply(o1));

    auto v2=validator(
        sum_gte_10(2,7)(flag,true)
    );
    BOOST_CHECK(!v2.apply(ra1));
    BOOST_CHECK_EQUAL(rep,std::string("sum_gte_10(2,7) must be 10 and more"));
    rep.clear();

    auto v3=validator(
        sum_gte_10(12,7)(flag,false)
    );
    BOOST_CHECK(!v3.apply(ra1));
    BOOST_CHECK_EQUAL(rep,std::string("sum_gte_10(12,7) must be less than 10"));
    rep.clear();

    auto v4=validator(
        _[sum_gte_10][5][7](value(flag,true))
    );
    BOOST_CHECK(v4.apply(o1));

    auto v5=validator(
        _[sum_gte_10][2][7](value(flag,true))
    );
    BOOST_CHECK(!v5.apply(ra1));
    BOOST_CHECK_EQUAL(rep,std::string("element #7 of element #2 of sum_gte_10 must be true"));
    rep.clear();

    auto v6=validator(
        _[sum_gte_10][12][7](value(flag,false))
    );
    BOOST_CHECK(!v6.apply(ra1));
    BOOST_CHECK_EQUAL(rep,std::string("element #7 of element #12 of sum_gte_10 must be false"));
    rep.clear();

    std::vector<WithChild> vec7;
    vec7.resize(10);
    auto ra7=make_reporting_adapter(vec7,rep);

    auto v7=validator(
        _[5](sum_gte_10(12,7)(flag,false))
    );
    BOOST_CHECK(!v7.apply(ra7));
    BOOST_CHECK_EQUAL(rep,std::string("sum_gte_10(12,7) of element #5 must be less than 10"));
    rep.clear();

    auto v8=validator(
        _[5](sum_gte_10(12,7)(eq,false))
    );
    BOOST_CHECK(!v8.apply(ra7));
    BOOST_CHECK_EQUAL(rep,std::string("sum_gte_10(12,7) of element #5 must be equal to false"));
    rep.clear();

    auto v9=validator(
        sum_gte_10(2,7)(flag(flag_set_unset),true)
    );
    BOOST_CHECK(!v9.apply(ra1));
    BOOST_CHECK_EQUAL(rep,std::string("sum_gte_10(2,7) must be set"));
    rep.clear();

    auto v10=validator(
        _[5](sum_gte_10(12,7)(flag(flag_set_unset),false))
    );
    BOOST_CHECK(!v10.apply(ra7));
    BOOST_CHECK_EQUAL(rep,std::string("sum_gte_10(12,7) of element #5 must be unset"));
    rep.clear();
}

BOOST_AUTO_TEST_SUITE_END()
