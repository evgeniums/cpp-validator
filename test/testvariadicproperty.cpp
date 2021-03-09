#include <optional>
#include <functional>
#include <set>

#include <boost/test/unit_test.hpp>

#include <dracosha/validator/utils/reference_wrapper.hpp>
#include <dracosha/validator/utils/optional.hpp>
#include <dracosha/validator/utils/class_method_args.hpp>
#include <dracosha/validator/get_member.hpp>
#include <dracosha/validator/check_exists.hpp>

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

BOOST_AUTO_TEST_SUITE_END()
