#include <boost/test/unit_test.hpp>

#include <dracosha/validator/validator.hpp>
#include <dracosha/validator/adapters/default_adapter.hpp>
#include <dracosha/validator/adapters/reporting_adapter.hpp>

#include <dracosha/validator/filter_path.hpp>

using namespace DRACOSHA_VALIDATOR_NAMESPACE;

BOOST_AUTO_TEST_SUITE(TestPartialPaths)
#if 1
BOOST_AUTO_TEST_CASE(CheckPlainPaths)
{
    auto v1=validator(
        _["field1"](exists,true),
        _["field2"](exists,true),
        _["field3"](exists,true)
    );

    std::map<std::string,std::string> m1{
        {"field1", "value1"},
        {"field2", "value2"}
    };
    auto a1=make_default_adapter(m1);
    BOOST_CHECK(!v1.apply(a1));

    auto a2=include_paths(a1,
                member_path_list("field1")
            );
    BOOST_CHECK(v1.apply(a2));
    auto a3=include_paths(a1,
                member_path_list(_["field3"])
            );
    BOOST_CHECK(!v1.apply(a3));
    auto a3_1=include_paths(a1,
                member_path_list(_["field1"],_["field2"])
            );
    BOOST_CHECK(v1.apply(a3_1));

    auto a4=exclude_paths(a1,
                member_path_list("field1")
            );
    BOOST_CHECK(!v1.apply(a4));
    auto a5=exclude_paths(a1,
                member_path_list(_["field3"])
            );
    BOOST_CHECK(v1.apply(a5));
    auto a6=exclude_paths(a1,
                member_path_list(_["field1"],_["field3"])
            );
    BOOST_CHECK(v1.apply(a6));

    auto a7=include_and_exclude_paths(a1,
                member_path_list("field1","field3"),
                member_path_list("field1")
    );
    BOOST_CHECK(!v1.apply(a7));
    auto a8=include_and_exclude_paths(a1,
                member_path_list("field1","field3"),
                member_path_list("field3")
    );
    BOOST_CHECK(v1.apply(a8));
}
#endif
#if 1
BOOST_AUTO_TEST_CASE(CheckPlainPathsReport)
{
    auto v1=validator(
        _["field1"](exists,true),
        _["field2"](exists,true),
        _["field3"](exists,true)
    );

    std::map<std::string,std::string> m1{
        {"field1", "value1"},
        {"field2", "value2"}
    };
    std::string rep;
    auto a1=make_reporting_adapter(m1,rep);

    BOOST_CHECK(!v1.apply(a1));
    BOOST_CHECK_EQUAL(std::string("field3 must exist"),rep);
    rep.clear();

    auto a2=include_paths(a1,
                member_path_list("field1")
            );
    BOOST_CHECK(v1.apply(a2));
    auto a3=include_paths(a1,
                member_path_list(_["field3"])
            );
    BOOST_CHECK(!v1.apply(a3));
    BOOST_CHECK_EQUAL(std::string("field3 must exist"),rep);
    rep.clear();
    auto a3_1=include_paths(a1,
                member_path_list(_["field1"],_["field2"])
            );
    BOOST_CHECK(v1.apply(a3_1));

    auto a4=exclude_paths(a1,
                member_path_list("field1")
            );
    BOOST_CHECK(!v1.apply(a4));
    BOOST_CHECK_EQUAL(std::string("field3 must exist"),rep);
    rep.clear();
    auto a5=exclude_paths(a1,
                member_path_list(_["field3"])
            );
    BOOST_CHECK(v1.apply(a5));
    auto a6=exclude_paths(a1,
                member_path_list(_["field1"],_["field3"])
            );
    BOOST_CHECK(v1.apply(a6));

    auto a7=include_and_exclude_paths(a1,
                member_path_list("field1","field3"),
                member_path_list("field1")
    );
    BOOST_CHECK(!v1.apply(a7));
    BOOST_CHECK_EQUAL(std::string("field3 must exist"),rep);
    rep.clear();
    auto a8=include_and_exclude_paths(a1,
                member_path_list("field1","field3"),
                member_path_list("field3")
    );
    BOOST_CHECK(v1.apply(a8));
}

BOOST_AUTO_TEST_CASE(CheckNestedPaths)
{
    auto v1=validator(
        _["level1"]["field1"](exists,true),
        _["level1"]["field2"](exists,true),
        _["level2"]["field3"](exists,true)
    );

    std::map<std::string,std::set<std::string>> m1{
        {"level1",{"field1","field2"}},
        {"level2",{"field4"}},
    };
    auto a1=make_default_adapter(m1);
    BOOST_CHECK(!v1.apply(a1));

    auto a2=include_paths(a1,
                member_path_list(_["level1"])
            );
    BOOST_CHECK(v1.apply(a2));
    auto a3=include_paths(a1,
                member_path_list(_["level2"])
            );
    BOOST_CHECK(!v1.apply(a3));
    auto a4=include_paths(a1,
                member_path_list(_["level2"]["field3"])
            );
    BOOST_CHECK(!v1.apply(a4));
    auto a5=include_paths(a1,
                member_path_list(_["level2"]["field4"])
            );
    BOOST_CHECK(v1.apply(a5));

    auto a6=exclude_paths(a1,
                member_path_list(_["level2"])
            );
    BOOST_CHECK(v1.apply(a6));
    auto a7=exclude_paths(a1,
                member_path_list(_["level1"])
            );
    BOOST_CHECK(!v1.apply(a7));
    auto a8=exclude_paths(a1,
                member_path_list(_["level2"]["field3"])
            );
    BOOST_CHECK(v1.apply(a8));
    auto a9=exclude_paths(a1,
                member_path_list(_["level2"]["field4"])
            );
    BOOST_CHECK(!v1.apply(a9));

    auto a10=exclude_paths(a1,
                member_path_list(_["level2"][ALL])
            );
    BOOST_CHECK(v1.apply(a10));

    auto a11=include_and_exclude_paths(a1,
                member_path_list(_["level1"],_["level2"]),
                member_path_list(_["level2"]["field3"])
            );
    BOOST_CHECK(v1.apply(a11));
}
#endif

BOOST_AUTO_TEST_SUITE_END()
