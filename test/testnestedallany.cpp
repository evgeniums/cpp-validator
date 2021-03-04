#include <set>
#include <iterator>

#include <boost/test/unit_test.hpp>

#include <dracosha/validator/validator.hpp>
#include <dracosha/validator/adapters/reporting_adapter.hpp>
#include <dracosha/validator/utils/extract_object_wrapper.hpp>

using namespace DRACOSHA_VALIDATOR_NAMESPACE;

BOOST_AUTO_TEST_SUITE(TestNestedAllAny)

BOOST_AUTO_TEST_CASE(TestIsAggregated)
{
    auto c1=hana::fold(
        hana::tuple_t<int,char,std::string>,
        hana::bool_<false>{},
        [](auto&& prev, auto&& v)
        {
            return hana::if_(
                prev,
                prev,
                hana::bool_
                <
                    hana::is_a<element_aggregation_tag,typename extract_object_wrapper_t<decltype(v)>::type>
                >{}
            );
    });
    static_assert(!decltype(c1)::value,"");

    {
        auto m1=_[1][2]["hello"][size];
        static_assert(!decltype(m1)::is_aggregated::value,"m2 is not aggregated");
    }

    {
        auto m2=_[1][ANY]["hello"][size];
        static_assert(decltype(m2)::is_aggregated::value,"m2 is aggregated");
    }

    {
        auto m3=_[1]["hello"][ALL];
        static_assert(decltype(m3)::is_aggregated::value,"m2 is aggregated");
    }

    {
        auto m4=_[1][ANY]["hello"][ALL];
        static_assert(decltype(m4)::is_aggregated::value,"m2 is aggregated");
    }

    BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(TestNestedAll)
{
    auto v1=validator(
                _[ALL](size(gte,5))
            );
    std::map<std::string,std::string> m1{
        {"field1","value1"},
        {"field2","value2"},
        {"field3","value3"}
    };
    BOOST_CHECK(v1.apply(m1));
    std::map<std::string,std::string> m2{
        {"field1","value1"},
        {"field2","value2"},
        {"field3","val"}
    };
    std::string rep;
    auto a2=make_reporting_adapter(m2,rep);
    BOOST_CHECK(!v1.apply(a2));
    BOOST_CHECK_EQUAL(rep,std::string("size of each element must be greater than or equal to 5"));

    auto v2=validator(
                _["level1"][ALL]["level3"](size(gte,5))
            );
    std::map<std::string,
             std::map<std::string,
                      std::map<std::string,std::string>
                     >
            > m3
    {{
        "level1",
        {
          {
            "level2_1",
            {{
                "level3",
                {
                    "value1"
                }
            }}
          },
          {
            "level2_2",
            {{
              "level3",
              {
                  "val"
              }
            }}
          }
        }
    }};
    BOOST_CHECK(!v2.apply(m3));

    std::map<std::string,
             std::map<std::string,
                      std::map<std::string,std::string>
                     >
            > m4
    {{
        "level1",
        {
          {
            "level2_1",
            {{
                "level3",
                {
                    "value1"
                }
            }}
          },
          {
            "level2_2",
            {{
              "level3",
              {
                  "value4"
              }
            }}
          }
        }
    }};
    BOOST_CHECK(v2.apply(m4));

    auto v3=validator(
                _["level1"][ALL]["level3"][ALL](size(gte,5))
            );

    std::map<std::string,
             std::map<std::string,
                      std::map<std::string,std::set<std::string>>
                     >
            > m5
    {{
        "level1",
        {
          {
            "level2_1",
            {{
                "level3",
                {
                    {"value1","value2","value3"}
                }
            }}
          },
          {
            "level2_2",
            {{
              "level3",
              {
                  {"value1","value2","val"}
              }
            }}
          }
        }
    }};
    BOOST_CHECK(!v3.apply(m5));

    std::map<std::string,
             std::map<std::string,
                      std::map<std::string,std::set<std::string>>
                     >
            > m6
    {{
        "level1",
        {
          {
            "level2_1",
            {{
                "level3",
                {
                    {"value1","value2","value3"}
                }
            }}
          },
          {
            "level2_2",
            {{
              "level3",
              {
                  {"value1","value2","value3"}
              }
            }}
          }
        }
    }};
    BOOST_CHECK(v3.apply(m6));

    std::map<std::string,
             std::map<std::string,
                      std::map<std::string,std::set<std::string>>
                     >
            > m7
    {{
        "level1",
        {
          {
            "level2_1",
            {{
                "level3",
                {
                    {"value1","value2","value3"}
                }
            }}
          },
          {
            "level2_2",
            {{
              "level3_2",
              {
                  {"value1","value2","value3"}
              }
            }}
          }
        }
    }};
    auto a7=make_default_adapter(m7);
    a7.set_check_member_exists_before_validation(true);
    BOOST_CHECK(v3.apply(a7));
}

BOOST_AUTO_TEST_CASE(TestNestedAny)
{
    auto v1=validator(
                _[ANY](size(gte,5))
            );
    std::map<std::string,std::string> m1{
        {"field1","value1"},
        {"field2","value2"},
        {"field3","value3"}
    };
    BOOST_CHECK(v1.apply(m1));
    std::map<std::string,std::string> m2{
        {"field1","val1"},
        {"field2","val2"},
        {"field3","value3"}
    };
    BOOST_CHECK(v1.apply(m2));
    std::map<std::string,std::string> m2_1{
        {"field1","val1"},
        {"field2","val2"},
        {"field3","val3"}
    };
    BOOST_CHECK(!v1.apply(m2_1));
    std::string rep;
    auto a2=make_reporting_adapter(m2_1,rep);
    BOOST_CHECK(!v1.apply(a2));
    BOOST_CHECK_EQUAL(rep,std::string("size of at least one element must be greater than or equal to 5"));
    rep.clear();

    auto v2=validator(
                _["level1"][ANY]["level3"](size(gte,5))
            );
    std::map<std::string,
             std::map<std::string,
                      std::map<std::string,std::string>
                     >
            > m3
    {{
        "level1",
        {
          {
            "level2_1",
            {{
                "level3",
                {
                    "val1"
                }
            }}
          },
          {
            "level2_2",
            {{
              "level3",
              {
                  "val2"
              }
            }}
          }
        }
    }};
    BOOST_CHECK(!v2.apply(m3));

    std::map<std::string,
             std::map<std::string,
                      std::map<std::string,std::string>
                     >
            > m4
    {{
        "level1",
        {
          {
            "level2_1",
            {{
                "level3",
                {
                    "value1"
                }
            }}
          },
          {
            "level2_2",
            {{
              "level3",
              {
                  "val"
              }
            }}
          }
        }
    }};
    BOOST_CHECK(v2.apply(m4));

    auto v3=validator(
                _["level1"][ANY]["level3"][ALL](size(gte,5))
            );

    std::map<std::string,
             std::map<std::string,
                      std::map<std::string,std::set<std::string>>
                     >
            > m5
    {{
        "level1",
        {
          {
            "level2_1",
            {{
                "level3",
                {
                    {"value1","value2","val"}
                }
            }}
          },
          {
            "level2_2",
            {{
              "level3",
              {
                  {"value1","value2","val"}
              }
            }}
          }
        }
    }};
    auto a5=make_reporting_adapter(m5,rep);
    BOOST_CHECK(!v3.apply(a5));
    BOOST_CHECK_EQUAL(rep,std::string("size of each element of level3 of at least one element of level1 must be greater than or equal to 5"));

    std::map<std::string,
             std::map<std::string,
                      std::map<std::string,std::set<std::string>>
                     >
            > m6
    {{
        "level1",
        {
          {
            "level2_1",
            {{
                "level3",
                {
                    {"value1","value2","value3"}
                }
            }}
          },
          {
            "level2_2",
            {{
              "level3",
              {
                  {"value1","value2","val"}
              }
            }}
          }
        }
    }};
    BOOST_CHECK(v3.apply(m6));
}

BOOST_AUTO_TEST_CASE(TestMixedAgrregations)
{
    std::string rep;

    auto v1=validator(
                _["level1"](size(gte,1)),
                _["level1"][ANY]["level3"](size(gte,5) ^AND^ size(lt,100))
            );
    std::map<std::string,
             std::map<std::string,
                      std::map<std::string,std::string>
                     >
            > m1
    {{
        "level1",
        {
          {
            "level2_1",
            {{
                "level3",
                {
                    "val1"
                }
            }}
          },
          {
            "level2_2",
            {{
              "level3",
              {
                  "val2"
              }
            }}
          }
        }
    }};
    BOOST_CHECK(!v1.apply(m1));
    auto a1=make_reporting_adapter(m1,rep);
    BOOST_CHECK(!v1.apply(a1));
    BOOST_CHECK_EQUAL(rep,std::string("size of level3 of at least one element of level1 must be greater than or equal to 5"));
    std::map<std::string,
             std::map<std::string,
                      std::map<std::string,std::string>
                     >
            > m2
    {{
        "level1",
        {
          {
            "level2_1",
            {{
                "level3",
                {
                    "value1"
                }
            }}
          },
          {
            "level2_2",
            {{
              "level3",
              {
                  "value2"
              }
            }}
          }
        }
    }};
    BOOST_CHECK(v1.apply(m2));
}
BOOST_AUTO_TEST_SUITE_END()
