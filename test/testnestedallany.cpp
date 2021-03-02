#include <set>
#include <iterator>

#include <boost/test/unit_test.hpp>

#include <dracosha/validator/validator.hpp>

using namespace DRACOSHA_VALIDATOR_NAMESPACE;

BOOST_AUTO_TEST_SUITE(TestNestedAllAne)

BOOST_AUTO_TEST_CASE(TestNestedAll)
{
//    auto v1=validator(
//                _[ALL](size(gte,5))
//            );
//    std::map<std::string,std::string> m1{
//        {"field1","value1"},
//        {"field2","value2"},
//        {"field3","value3"}
//    };
//    BOOST_CHECK(v1.apply(m1));
//    std::map<std::string,std::string> m2{
//        {"field1","value1"},
//        {"field2","value2"},
//        {"field3","val"}
//    };
//    BOOST_CHECK(!v1.apply(m2));

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

BOOST_AUTO_TEST_SUITE_END()
