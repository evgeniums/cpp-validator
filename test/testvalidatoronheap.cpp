#include <boost/test/unit_test.hpp>

#include <dracosha/validator/validator.hpp>

using namespace DRACOSHA_VALIDATOR_NAMESPACE;

namespace
{
struct NonCopyable
{
    NonCopyable()=default;
    ~NonCopyable()=default;
    NonCopyable(const NonCopyable&)=delete;
    NonCopyable(NonCopyable&&)=default;
    NonCopyable& operator= (const NonCopyable&)=delete;
    NonCopyable& operator= (NonCopyable&&)=default;

    std::string val;
};
}

BOOST_AUTO_TEST_SUITE(TestValidatorOnHeap)

BOOST_AUTO_TEST_CASE(CheckValidation)
{
    std::map<std::string,size_t> m1={{"field1",1}};
    auto a1=make_default_adapter(m1);

    auto v0n=new_validator(
                _["field1"](eq,1)
            );
    auto v0=shared_validator(
                _["field1"](eq,1)
            );
    static_assert(std::is_same<decltype(v0n),decltype(v0.get())>::value,"Types of validator on heap and shared validator must match");

    BOOST_CHECK(v0->apply(a1));
    BOOST_CHECK(v0n->apply(m1));

    delete v0n;

    auto v1=shared_validator(
                _["field1"](gte,10)
            );

    BOOST_CHECK(!v1->apply(a1));

    auto v2=shared_validator(
                _["field1"](length(gte,10))
            );
    BOOST_CHECK(!v2->apply(a1));

    auto get_obj=[&m1]() -> decltype(auto)
    {
        return hana::id(m1);
    };
    auto a2=make_default_adapter(lazy(get_obj));
    m1["field1"]=10;
    BOOST_CHECK(!v0->apply(a2));
    BOOST_CHECK(v1->apply(a2));
    BOOST_CHECK(v2->apply(a2));

    size_t val1=10;
    auto get_val=[&val1]() -> decltype(auto)
    {
        return hana::id(val1);
    };

    auto v3=shared_validator(
                _["field1"](eq,lazy(get_val))
            );
    BOOST_CHECK(v3->apply(a2));
    val1=1;
    BOOST_CHECK(!v3->apply(a2));
}

BOOST_AUTO_TEST_CASE(CheckLazyValidation)
{
    std::map<std::string,size_t> m1={{"field1",1}};
    auto v0=shared_validator(
                _["field1"](eq,1)
            );
    auto v1=shared_validator(
                _["field1"](gte,10)
            );
    auto v2=shared_validator(
                _["field1"](length(gte,10))
            );

    auto get_obj=[&m1]() -> decltype(auto)
    {
        return hana::id(m1);
    };
    auto a2=make_default_adapter(lazy(get_obj));
    BOOST_CHECK(v0->apply(a2));
    BOOST_CHECK(!v1->apply(a2));
    BOOST_CHECK(!v2->apply(a2));
    m1["field1"]=10;
    BOOST_CHECK(!v0->apply(a2));
    BOOST_CHECK(v1->apply(a2));
    BOOST_CHECK(v2->apply(a2));

    size_t val1=10;
    auto get_val=[&val1]() -> decltype(auto)
    {
        return hana::id(val1);
    };

    auto v3=shared_validator(
                _["field1"](eq,lazy(get_val))
            );
    BOOST_CHECK(v3->apply(a2));
    val1=1;
    BOOST_CHECK(!v3->apply(a2));
}

BOOST_AUTO_TEST_CASE(CheckValidationAggregation)
{
    std::map<std::string,std::string> m1={{"field1","value1"}};
    auto a1=make_default_adapter(m1);

    auto v1=shared_validator(
                _["field1"](value(gte,"z1000") ^OR^ length(lt,3))
            );
    BOOST_CHECK(!v1->apply(a1));

    auto v2=shared_validator(
                _["field1"](gte,"v"),
                _["field1"](size(gte,100))
            );
    BOOST_CHECK(!v2->apply(a1));

    auto v3=shared_validator(
                _["field1"](gte,"xxxxxx")
                 ^OR^
                _["field1"](size(gte,100) ^OR^ value(gte,"zzzzzzzzzzzz"))
            );
    BOOST_CHECK(!v3->apply(a1));

    auto v4=shared_validator(
                _["field1"](gte,"xxxxxx")
                 ^OR^
                _["field1"](size(gte,100) ^AND^ value(gte,"zzzzzzzzzzzz"))
            );
    BOOST_CHECK(!v4->apply(a1));

    auto v5=shared_validator(
                _["field1"](gte,"xxxxxx")
                 ^OR^
                _["field1"](size(gte,1) ^AND^ value(gte,"zzzzzzzzzzzz"))
            );
    BOOST_CHECK(!v5->apply(a1));
}

BOOST_AUTO_TEST_CASE(CheckLazyValidationAggregation)
{
    std::map<std::string,std::string> m1={{"field1","value1"}};
    auto get_obj=[&m1]() -> decltype(auto)
    {
        return hana::id(m1);
    };
    auto a1=make_default_adapter(lazy(get_obj));
    std::string str1="zzzzzzzzzzzz";
    auto get_val=[&str1]() -> decltype(auto)
    {
        return hana::id(str1);
    };

    auto v1=shared_validator(
                _["field1"](value(gte,"z1000") ^OR^ length(lt,3))
            );
    BOOST_CHECK(!v1->apply(a1));

    auto v2=shared_validator(
                _["field1"](gte,"v"),
                _["field1"](size(gte,100))
            );
    BOOST_CHECK(!v2->apply(a1));

    auto v3=shared_validator(
                _["field1"](gte,"xxxxxx")
                 ^OR^
                _["field1"](size(gte,100) ^OR^ value(gte,lazy(get_val)))
            );
    BOOST_CHECK(!v3->apply(a1));

    auto v4=shared_validator(
                _["field1"](gte,"xxxxxx")
                 ^OR^
                _["field1"](size(gte,100) ^AND^ value(gte,lazy(get_val)))
            );
    BOOST_CHECK(!v4->apply(a1));

    auto v5=shared_validator(
                _["field1"](gte,"xxxxxx")
                 ^OR^
                _["field1"](size(gte,1) ^AND^ value(gte,lazy(get_val)))
            );
    BOOST_CHECK(!v5->apply(a1));
}

BOOST_AUTO_TEST_CASE(CheckValidationNot)
{
    std::map<std::string,size_t> m1={{"field1",10}};
    auto a1=make_default_adapter(m1);

    auto v1=shared_validator(
                NOT(_["field1"](eq,10))
            );
    BOOST_CHECK(!v1->apply(a1));

    auto v2=shared_validator(
                _["field1"](NOT(value(gte,1)))
            );
    BOOST_CHECK(!v2->apply(a1));

    auto v3=shared_validator(
                NOT(
                    _["field1"](eq,10),
                    _["field1"](gte,8)
                )
            );
    BOOST_CHECK(!v3->apply(a1));

    auto v4=shared_validator(
                NOT(
                    _["field1"](eq,10)
                    ^OR^
                    _["field1"](gte,100)
                )
            );
    BOOST_CHECK(!v4->apply(a1));
}

BOOST_AUTO_TEST_CASE(CheckNestedValidation)
{
    std::map<std::string,std::map<size_t,size_t>> m1={
            {"field1",{{1,10}}}
        };
    auto a1=make_default_adapter(m1);

    auto v1=shared_validator(
                _[size](gte,0),
                _["field1"][1](gte,10)
            );
    BOOST_CHECK(v1->apply(a1));

    auto v2=shared_validator(
                _["field1"][1](lt,5)
            );
    BOOST_CHECK(!v2->apply(a1));

    auto v3=shared_validator(
                _[100](lt,5)
            );
    BOOST_CHECK(v3->apply(a1));
}

BOOST_AUTO_TEST_CASE(CheckOtherField)
{
    std::map<std::string,int> m1={
            {"field1",10},
            {"field2",5},
            {"field3",5}
        };
    auto a1=make_default_adapter(m1);

    auto v1=shared_validator(
                _["field1"](gt,_["field2"]),
                _["field2"](eq,_["field3"])
            );
    BOOST_CHECK(v1->apply(a1));

    auto v2=shared_validator(
                _["field2"](gt,_["field3"]) ^OR^ _["field3"](eq,_["field1"])
            );
    BOOST_CHECK(!v2->apply(a1));
}

BOOST_AUTO_TEST_CASE(CheckLazyOtherField)
{
    std::map<std::string,int> m1={
            {"field1",10},
            {"field2",5},
            {"field3",5}
        };
    auto get_obj=[&m1]() -> decltype(auto)
    {
        return hana::id(m1);
    };
    auto a1=make_default_adapter(lazy(get_obj));

    auto v1=shared_validator(
                _["field1"](gt,_["field2"]),
                _["field2"](eq,_["field3"])
            );
    BOOST_CHECK(v1->apply(a1));

    auto v2=shared_validator(
                _["field2"](gt,_["field3"]) ^OR^ _["field3"](eq,_["field1"])
            );
    BOOST_CHECK(!v2->apply(a1));
}

BOOST_AUTO_TEST_CASE(CheckSampleObject)
{
    std::map<std::string,int> m1={
            {"field1",10},
            {"field2",5},
            {"field3",5}
        };
    std::map<std::string,int> m2={
            {"field1",10},
            {"field2",5},
            {"field3",5}
        };
    auto a1=make_default_adapter(m1);

    auto v1=shared_validator(
                _["field1"](gte,_(m2)),
                _["field2"](eq,_(m2))
            );
    BOOST_CHECK(v1->apply(a1));

    auto v2=shared_validator(
                _["field2"](gt,_(m2)) ^OR^ _["field3"](lt,_(m2))
            );
    BOOST_CHECK(!v2->apply(a1));
}

BOOST_AUTO_TEST_CASE(CheckLazySampleObject)
{
    std::map<std::string,int> m1={
            {"field1",10},
            {"field2",5},
            {"field3",5}
        };
    std::map<std::string,int> m2={
            {"field1",10},
            {"field2",5},
            {"field3",5}
        };
    auto get_obj=[&m1]() -> decltype(auto)
    {
        return hana::id(m1);
    };
    auto a1=make_default_adapter(lazy(get_obj));

    auto v1=shared_validator(
                _["field1"](gte,_(m2)),
                _["field2"](eq,_(m2))
            );
    BOOST_CHECK(v1->apply(a1));

    auto v2=shared_validator(
                _["field2"](gt,_(m2)) ^OR^ _["field3"](lt,_(m2))
            );
    BOOST_CHECK(!v2->apply(a1));
}

BOOST_AUTO_TEST_CASE(CheckNotExistingMemberIgnore)
{
    std::map<std::string,int> m1={
            {"field1",10},
            {"field3",5}
        };
    auto a1=make_default_adapter(m1);
    a1.set_check_member_exists_before_validation(true);
    BOOST_REQUIRE(a1.is_check_member_exists_before_validation());

    auto v1=shared_validator(
                _["field1"](gte,9),
                _["field2"](eq,100)
            );
    BOOST_CHECK(v1->apply(a1));

    auto v2=shared_validator(
                _["field2"](gt,100)
            );
    BOOST_CHECK(v2->apply(a1));

    auto v3=shared_validator(
                _["field2"](gt,100) ^OR^ _["field1"](eq,20)
            );
    BOOST_CHECK(!v3->apply(a1));

    auto v4=shared_validator(
                _["field2"](value(gt,100) ^OR^ value(eq,20))
            );
    BOOST_CHECK(v4->apply(a1));

    auto v5=shared_validator(
                _["field2"](value(gt,100) ^OR^ value(eq,20)),
                _["field1"](gte,9)
            );
    BOOST_CHECK(v5->apply(a1));

    auto v6=shared_validator(
                _["field2"](value(gt,100) ^OR^ value(eq,20)),
                _["field1"](gte,100)
            );
    BOOST_CHECK(!v6->apply(a1));
}

BOOST_AUTO_TEST_CASE(CheckNotExistingMemberAbort)
{
    std::map<std::string,int> m1={
            {"field1",10},
            {"field3",5}
        };
    auto a1=make_default_adapter(m1);
    a1.set_check_member_exists_before_validation(true);
    a1.set_unknown_member_mode(if_member_not_found::abort);

    auto v1=shared_validator(
                _["field1"](gte,9),
                _["field2"](eq,100)
            );
    BOOST_CHECK(!v1->apply(a1));

    auto v2=shared_validator(
                _["field2"](gt,100)
            );
    BOOST_CHECK(!v2->apply(a1));

    auto v3=shared_validator(
                _["field2"](gt,100) ^OR^ _["field1"](eq,20)
            );
    BOOST_CHECK(!v3->apply(a1));

    auto v4=shared_validator(
                _["field2"](value(gt,100) ^OR^ value(eq,20))
            );
    BOOST_CHECK(!v4->apply(a1));

    auto v5=shared_validator(
                _["field2"](value(gt,100) ^OR^ value(eq,20)),
                _["field1"](gte,9)
            );
    BOOST_CHECK(!v5->apply(a1));

    auto v6=shared_validator(
                _["field2"](value(gt,100) ^OR^ value(eq,20)),
                _["field1"](gte,100)
            );
    BOOST_CHECK(!v6->apply(a1));
}

BOOST_AUTO_TEST_CASE(CheckNotExistingOtherMemberIgnore)
{
    std::map<std::string,int> m1={
            {"field1",10},
            {"field3",5}
        };
    auto a1=make_default_adapter(m1);
    a1.set_check_member_exists_before_validation(true);

    auto v1=shared_validator(
                _["field1"](gte,9),
                _["field2"](eq,_["field1"])
            );
    BOOST_CHECK(v1->apply(a1));
    auto v1_1=shared_validator(
                _["field1"](gte,9),
                _["field3"](eq,_["field4"])
            );
    BOOST_CHECK(v1_1->apply(a1));

    auto v2=shared_validator(
                _["field2"](gt,100)
            );
    BOOST_CHECK(v2->apply(a1));
    auto v2_1=shared_validator(
                _["field1"](gt,_["field4"])
            );
    BOOST_CHECK(v2_1->apply(a1));

    auto v3=shared_validator(
                _["field2"](gt,_["field3"]) ^OR^ _["field1"](eq,20)
            );
    BOOST_CHECK(!v3->apply(a1));
    auto v3_1=shared_validator(
                _["field1"](gt,_["field4"]) ^OR^ _["field1"](eq,20)
            );
    BOOST_CHECK(!v3_1->apply(a1));

    auto v4=shared_validator(
                _["field2"](value(gt,_["field1"]) ^OR^ value(eq,_["field1"]))
            );
    BOOST_CHECK(v4->apply(a1));
    auto v4_1=shared_validator(
                _["field1"](value(gt,_["field4"]) ^OR^ value(eq,_["field4"]))
            );
    BOOST_CHECK(v4_1->apply(a1));

    auto v5=shared_validator(
                _["field2"](value(gt,_["field1"]) ^OR^ value(eq,_["field1"])),
                _["field1"](gte,9)
            );
    BOOST_CHECK(v5->apply(a1));
    auto v5_1=shared_validator(
                _["field3"](value(gt,_["field4"]) ^OR^ value(eq,_["field4"])),
                _["field1"](gte,9)
            );
    BOOST_CHECK(v5_1->apply(a1));

    auto v6=shared_validator(
                _["field2"](value(gt,_["field1"]) ^OR^ value(eq,_["field1"])),
                _["field1"](gte,100)
            );
    BOOST_CHECK(!v6->apply(a1));
    auto v6_1=shared_validator(
                _["field3"](value(gt,_["field4"]) ^OR^ value(eq,_["field4"])),
                _["field1"](gte,100)
            );
    BOOST_CHECK(!v6_1->apply(a1));
}

BOOST_AUTO_TEST_CASE(CheckNotExistingOtherMemberAbort)
{
    std::map<std::string,int> m1={
            {"field1",10},
            {"field3",5}
        };
    auto a1=make_default_adapter(m1);
    a1.set_check_member_exists_before_validation(true);
    a1.set_unknown_member_mode(if_member_not_found::abort);

    auto v1=shared_validator(
                _["field1"](gte,9),
                _["field2"](eq,_["field1"])
            );
    BOOST_CHECK(!v1->apply(a1));
    auto v1_1=shared_validator(
                _["field1"](gte,9),
                _["field3"](eq,_["field4"])
            );
    BOOST_CHECK(!v1_1->apply(a1));

    auto v2=shared_validator(
                _["field2"](eq,_["field1"])
            );
    BOOST_CHECK(!v2->apply(a1));
    auto v2_1=shared_validator(
                _["field3"](eq,_["field4"])
            );
    BOOST_CHECK(!v2_1->apply(a1));

    auto v3=shared_validator(
                _["field2"](gt,_["field3"]) ^OR^ _["field1"](eq,20)
            );
    BOOST_CHECK(!v3->apply(a1));
    auto v3_1=shared_validator(
                _["field1"](gt,_["field4"]) ^OR^ _["field1"](eq,20)
            );
    BOOST_CHECK(!v3_1->apply(a1));

    auto v4=shared_validator(
                _["field2"](value(gt,_["field1"]) ^OR^ value(eq,_["field1"]))
            );
    BOOST_CHECK(!v4->apply(a1));
    auto v4_1=shared_validator(
                _["field1"](value(gt,_["field4"]) ^OR^ value(eq,_["field4"]))
            );
    BOOST_CHECK(!v4_1->apply(a1));

    auto v5=shared_validator(
                _["field2"](value(gt,_["field1"]) ^OR^ value(eq,_["field1"])),
                _["field1"](gte,9)
            );
    BOOST_CHECK(!v5->apply(a1));
    auto v5_1=shared_validator(
                _["field3"](value(gt,_["field4"]) ^OR^ value(eq,_["field4"])),
                _["field1"](gte,9)
            );
    BOOST_CHECK(!v5_1->apply(a1));

    auto v6=shared_validator(
                _["field2"](value(gt,_["field1"]) ^OR^ value(eq,_["field1"])),
                _["field1"](gte,100)
            );
    BOOST_CHECK(!v6->apply(a1));
    auto v6_1=shared_validator(
                _["field3"](value(gt,_["field4"]) ^OR^ value(eq,_["field4"])),
                _["field1"](gte,100)
            );
    BOOST_CHECK(!v6_1->apply(a1));
}

BOOST_AUTO_TEST_CASE(CheckNotExistingMemberSampleIgnore)
{
    std::map<std::string,int> m1={
            {"field1",10},
            {"field3",5}
        };
    std::map<std::string,int> m2={
            {"field1",10},
            {"field2",5},
            {"field3",5}
        };
    auto a1=make_default_adapter(m1);
    a1.set_check_member_exists_before_validation(true);

    auto v1=shared_validator(
                _["field1"](gte,_(m2)),
                _["field2"](eq,_(m2))
            );
    BOOST_CHECK(v1->apply(a1));

    auto v2=shared_validator(
                _["field2"](gt,_(m2)) ^OR^ _["field3"](lt,_(m2))
            );
    BOOST_CHECK(!v2->apply(a1));
}

BOOST_AUTO_TEST_CASE(CheckNotExistingMemberSampleAbort)
{
    std::map<std::string,int> m1={
            {"field1",10},
            {"field3",5}
        };
    std::map<std::string,int> m2={
            {"field1",10},
            {"field2",5},
            {"field3",5}
        };
    auto a1=make_default_adapter(m1);
    a1.set_check_member_exists_before_validation(true);
    a1.set_unknown_member_mode(if_member_not_found::abort);

    auto v1=shared_validator(
                _["field1"](gte,_(m2)),
                _["field2"](eq,_(m2))
            );
    BOOST_CHECK(!v1->apply(a1));

    auto v2=shared_validator(
                _["field2"](gt,_(m2)) ^OR^ _["field3"](lt,_(m2))
            );
    BOOST_CHECK(!v2->apply(a1));
}

BOOST_AUTO_TEST_CASE(CheckValidateMemberAny)
{
    std::map<std::string,std::vector<size_t>> m1={
            {"field1",
             {10,20,30,40,50}
            }
        };
    auto a1=make_default_adapter(m1);

    auto v1=shared_validator(
                _["field1"](ANY(value(gte,10)))
            );
    BOOST_CHECK(v1->apply(a1));

    auto v2=shared_validator(
                _["field1"](ANY(value(gte,50)))
            );
    BOOST_CHECK(v2->apply(a1));

    auto v3=shared_validator(
                _["field1"](ANY(value(gte,100)))
            );
    BOOST_CHECK(!v3->apply(a1));


    std::map<std::string,std::vector<std::string>> m2={
            {"field1",
             {"value10","value100","","hello"}
            }
        };
    auto a2=make_default_adapter(m2);

    auto v4=shared_validator(
                _["field1"](ANY(value(gte,"val") ^AND^ size(gte,5)))
            );
    BOOST_CHECK(v4->apply(a2));

    auto v5=shared_validator(
                _["field1"](ANY(value(gte,"zzzzzz") ^OR^ size(gte,5)))
            );
    BOOST_CHECK(v5->apply(a2));

    auto v6=shared_validator(
                _["field1"](ANY(value(gte,"zzzzzz") ^OR^ size(gte,50)))
            );
    BOOST_CHECK(!v6->apply(a2));

    std::map<std::string,std::map<size_t,std::string>> m3={
            {"field1",
             {
                 {1,"value10"},{2,"value100"},{3,""},{4,"hello"}
             }
            }
        };
    auto a3=make_default_adapter(m3);

    BOOST_CHECK(v4->apply(a3));
    BOOST_CHECK(v5->apply(a3));
    BOOST_CHECK(!v6->apply(a3));

    std::map<std::string,std::string> m4={
            {"field1","value10"}
        };
    auto a4=make_default_adapter(m4);
    BOOST_CHECK(v4->apply(a4));
    BOOST_CHECK(v5->apply(a4));
    // ignored invalid path because std::string is not a container for validator
    // ignored means true
    BOOST_CHECK(v6->apply(a4));
    auto v7=shared_validator(
                _["field1"](ANY(value(gte,"zzzz") ^OR^ size(gte,50)))
            );
    // ignored invalid path because std::string is not a container for validator
    // ignored means true
    BOOST_CHECK(v7->apply(a4));

    std::map<size_t,std::string> m5={
        {1,"value10"},{2,"value100"},{3,""},{4,"hello"}
    };
    auto a5=make_default_adapter(m5);
    auto v8=shared_validator(
                ANY(value(gte,"val") ^AND^ size(gte,5))
            );
    BOOST_CHECK(v8->apply(a5));
    auto v9=shared_validator(
                ANY(value(gte,"zzzzzz") ^OR^ size(gte,5))
            );
    BOOST_CHECK(v9->apply(a5));
    auto v10=shared_validator(
                ANY(value(gte,"zzzzzz") ^OR^ size(gte,50))
            );
    BOOST_CHECK(!v10->apply(a5));
}

BOOST_AUTO_TEST_CASE(CheckValidateMmemberAll)
{
    std::map<std::string,std::vector<size_t>> m1={
            {"field1",
             {10,20,30,40,50}
            }
        };
    auto a1=make_default_adapter(m1);

    auto v1=shared_validator(
                _["field1"](ALL(value(gte,10)))
            );
    BOOST_CHECK(v1->apply(a1));

    auto v2=shared_validator(
                _["field1"](ALL(value(gte,20)))
            );
    BOOST_CHECK(!v2->apply(a1));

    std::map<std::string,std::vector<std::string>> m2={
            {"field1",
             {"value10","value100","hi","hello"}
            }
        };
    auto a2=make_default_adapter(m2);

    auto v4=shared_validator(
                _["field1"](ALL(value(gte,"h") ^AND^ size(gte,2)))
            );
    BOOST_CHECK(v4->apply(a2));

    auto v5=shared_validator(
                _["field1"](ALL(value(gte,"zzzzzz") ^OR^ size(gte,2)))
            );
    BOOST_CHECK(v5->apply(a2));

    auto v6=shared_validator(
                _["field1"](ALL(value(gte,"val") ^OR^ size(gte,5)))
            );
    BOOST_CHECK(!v6->apply(a2));

    std::map<std::string,std::map<size_t,std::string>> m3={
            {"field1",
             {
                 {1,"value10"},{2,"value100"},{3,"hi"},{4,"hello"}
             }
            }
        };
    auto a3=make_default_adapter(m3);

    BOOST_CHECK(v4->apply(a3));
    BOOST_CHECK(v5->apply(a3));
    BOOST_CHECK(!v6->apply(a3));

    std::map<std::string,std::string> m4={
            {"field1","value10"}
        };
    auto a4=make_default_adapter(m4);
    BOOST_CHECK(v4->apply(a4));
    BOOST_CHECK(v5->apply(a4));
    BOOST_CHECK(v6->apply(a4));
    auto v7=shared_validator(
                _["field1"](ALL(value(gte,"zzzz") ^OR^ size(gte,50)))
            );
    // ignored invalid path because std::string is not a container for validator
    // ignored means true
    BOOST_CHECK(v7->apply(a4));

    std::map<size_t,std::string> m5={
        {1,"value10"},{2,"value100"},{3,"hi"},{4,"hello"}
    };
    auto a5=make_default_adapter(m5);
    auto v8=shared_validator(
                ALL(value(gte,"h") ^AND^ size(gte,2))
            );
    BOOST_CHECK(v8->apply(a5));
    auto v9=shared_validator(
                ALL(value(gte,"zzzzzz") ^OR^ size(gte,2))
            );
    BOOST_CHECK(v9->apply(a5));
    auto v10=shared_validator(
                ALL(value(gte,"val") ^OR^ size(gte,5))
            );
    BOOST_CHECK(!v10->apply(a5));
}

BOOST_AUTO_TEST_CASE(CheckContains)
{
    std::map<std::string,size_t> m1={{"field1",1}};
    auto a1=make_default_adapter(m1);

    auto v0=shared_validator(
                value(contains,"field1")
            );
    BOOST_CHECK(v0->apply(a1));
    auto v1=shared_validator(
                value(contains,"field2")
            );
    BOOST_CHECK(!v1->apply(a1));

    std::map<std::string,std::map<size_t,std::string>> m2={
            {"field1",
             {
                 {1,"value10"},{2,"value100"},{3,"hi"},{4,"hello"}
             }
            }
        };
    auto a2=make_default_adapter(m2);
    auto v3=shared_validator(
                _["field1"](contains,3)
            );
    BOOST_CHECK(v3->apply(a2));
    auto v4=shared_validator(
                _["field1"](contains,10)
            );
    BOOST_CHECK(!v4->apply(a2));
}

BOOST_AUTO_TEST_SUITE_END()
