#include <set>
#include <iterator>

#include <boost/test/unit_test.hpp>

#include <dracosha/validator/validator.hpp>
#include <dracosha/validator/adapters/reporting_adapter.hpp>
#include <dracosha/validator/utils/unwrap_object.hpp>

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
                    hana::is_a<element_aggregation_tag,unwrap_object_t<decltype(v)>>
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
    rep.clear();

    auto v1_1=validator(
                _["level1"](size(gte,1)),
                _["level1"][ANY]["level3"]("any level3 of level1")(size(gte,5) ^AND^ size(lt,100))
            );
    BOOST_CHECK(!v1_1.apply(a1));
    BOOST_CHECK_EQUAL(rep,std::string("size of any level3 of level1 must be greater than or equal to 5"));
    rep.clear();

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

BOOST_AUTO_TEST_CASE(TestAllMap)
{
    auto v1=validator(
        _[ALL](gte,"value")
    );

    std::map<std::string,std::string> m1{
        {"key1","value1"},
        {"key2","value2"},
        {"key3","value3"}
    };
    BOOST_CHECK(v1.apply(m1));

    std::map<std::string,std::string> m2{
        {"key1","val1"},
        {"key2","val2"},
        {"key3","val3"}
    };
    BOOST_CHECK(!v1.apply(m2));

    std::set<std::string> s3{
        "value1",
        "value2",
        "value3"
    };
    BOOST_CHECK(v1.apply(s3));

    std::set<std::string> s4{
        "val1",
        "val2",
        "val3"
    };
    BOOST_CHECK(!v1.apply(s4));
}

BOOST_AUTO_TEST_CASE(TestPair)
{
    auto v1=validator(
        _[first](gt,"key")
    );
    auto p1=std::make_pair(std::string("key1"),std::string("value1"));
    BOOST_CHECK(v1.apply(p1));
    auto p2=std::make_pair(std::string("key"),std::string("value1"));
    BOOST_CHECK(!v1.apply(p2));
}

BOOST_AUTO_TEST_CASE(TestAllWithModifiers)
{
    std::map<std::string,std::string> m1{
        {"key1","value1"},
        {"key2","value2"},
        {"key3","value3"}
    };
    std::map<std::string,std::string> m2{
        {"key1","val1"},
        {"key2","val2"},
        {"key","val3"}
    };

    auto v1=validator(
        _[ALL(keys)](gt,"key")
    );
    BOOST_CHECK(v1.apply(m1));
    BOOST_CHECK(!v1.apply(m2));

    auto v2=validator(
        _[ALL(values)](gt,"value")
    );
    BOOST_CHECK(v2.apply(m1));
    BOOST_CHECK(!v2.apply(m2));

    static_assert(decltype(is_element_aggregation(hana::true_{},_[ALL(iterators)].key()))::value,"");
    static_assert(decltype(_[ALL(iterators)][second])::is_aggregated::value,"");
    BOOST_CHECK(hana::value(is_member_path_valid(m1,_[ALL(iterators)].path())));

    auto v3=validator(
        _[ALL(iterators)](second(gt,"value"))
    );
    BOOST_CHECK(v3.apply(m1));
    BOOST_CHECK(!v3.apply(m2));

    auto v4=validator(
        _[ALL(iterators)](first(gt,"key"))
    );
    BOOST_CHECK(v4.apply(m1));
    BOOST_CHECK(!v4.apply(m2));

    static_assert(std::is_base_of<all_tag,decltype(ALL(iterators))>::value,"");

    auto v5=validator(
        _[ALL(iterators)][first](gt,"key")
    );
    BOOST_CHECK(v5.apply(m1));
    BOOST_CHECK(!v5.apply(m2));

    auto v6=validator(
        _[ALL(iterators)][second](gt,"value")
    );
    BOOST_CHECK(v6.apply(m1));
    BOOST_CHECK(!v6.apply(m2));
}

BOOST_AUTO_TEST_CASE(TestAnyWithModifiers)
{
    std::map<std::string,std::string> m1{
        {"key1","value1"},
        {"key2","value2"},
        {"key3","value3"}
    };
    std::map<std::string,std::string> m2{
        {"k1","val1"},
        {"k2","val2"},
        {"k3","val3"}
    };

    auto v1=validator(
        _[ANY(keys)](gt,"key")
    );
    BOOST_CHECK(v1.apply(m1));
    BOOST_CHECK(!v1.apply(m2));

    auto v2=validator(
        _[ANY(values)](gt,"value")
    );
    BOOST_CHECK(v2.apply(m1));
    BOOST_CHECK(!v2.apply(m2));

    auto v3=validator(
        _[ANY(iterators)](second(gt,"value"))
    );
    BOOST_CHECK(v3.apply(m1));
    BOOST_CHECK(!v3.apply(m2));

    auto v4=validator(
        _[ANY(iterators)](first(gt,"key"))
    );
    BOOST_CHECK(v4.apply(m1));
    BOOST_CHECK(!v4.apply(m2));

    auto v5=validator(
        _[ANY(iterators)][first](gt,"key")
    );
    BOOST_CHECK(v5.apply(m1));
    BOOST_CHECK(!v5.apply(m2));

    auto v6=validator(
        _[ANY(iterators)][second](gt,"value")
    );
    BOOST_CHECK(v6.apply(m1));
    BOOST_CHECK(!v6.apply(m2));
}

BOOST_AUTO_TEST_SUITE_END()
