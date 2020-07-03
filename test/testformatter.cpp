//#define DRACOSHA_VALIDATOR_TEST_FORMATTER
#ifdef DRACOSHA_VALIDATOR_TEST_FORMATTER

#include <string>
#include <vector>
#include <iterator>

#include <boost/test/unit_test.hpp>

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/reporting/formatter.hpp>

BOOST_AUTO_TEST_CASE(CheckOrderAndPresentation)
{
    std::string str1;
    detail::reorder_and_present(
                hana::partial(format_append,hana::type_c<std::string>,std::ref(str1)),
                make_cref_tuple(default_strings,values),
                gte,10
            );
    BOOST_CHECK_EQUAL(str1,std::string("is greater than or equal to 10"));

    std::string str2;
    detail::reorder_and_present(
                hana::partial(format_append,hana::type_c<std::string>,std::ref(str2)),
                make_cref_tuple(default_strings,values),
                eq,true
            );
    BOOST_CHECK_EQUAL(str2,std::string("must be true"));

    std::string str3;
    detail::reorder_and_present(
                hana::partial(format_append,hana::type_c<std::string>,std::ref(str3)),
                make_cref_tuple(default_strings,values),
                ne,true
            );
    BOOST_CHECK_EQUAL(str3,std::string("must not be true"));

    auto mn=member_names();
    std::string str4;
    detail::reorder_and_present(
                hana::partial(format_append,hana::type_c<std::string>,std::ref(str4)),
                make_cref_tuple(mn,mn,default_strings,values),
                "field1",size,eq,100
            );
    BOOST_CHECK_EQUAL(str4,std::string("size of field1 is equal to 100"));

    std::string str5;
    detail::reorder_and_present(
                hana::partial(format_append,hana::type_c<std::string>,std::ref(str5)),
                make_cref_tuple(member_names(),member_names(),default_strings,values),
                "field2",value,eq,true
            );
    BOOST_CHECK_EQUAL(str5,std::string("field2 must be true"));

    std::string str6;
    detail::reorder_and_present(
                hana::partial(format_append,hana::type_c<std::string>,std::ref(str6)),
                make_cref_tuple(member_names(),default_strings,values),
                empty,eq,false
            );
    BOOST_CHECK_EQUAL(str6,std::string("is not empty"));

    std::string str7;
    detail::reorder_and_present(
                hana::partial(format_append,hana::type_c<std::string>,std::ref(str7)),
                make_cref_tuple(member_names(),member_names(),default_strings,values),
                "field1",empty,ne,true
            );
    BOOST_CHECK_EQUAL(str7,std::string("field1 is not empty"));

    std::string str8;
    detail::reorder_and_present(
                hana::partial(format_append,hana::type_c<std::string>,std::ref(str8)),
                make_cref_tuple(member_names(),member_names(),default_strings,values),
                "field2",empty,eq,true
            );
    BOOST_CHECK_EQUAL(str8,std::string("field2 is empty"));

    std::string str9;
    detail::reorder_and_present(
                hana::partial(format_append,hana::type_c<std::string>,std::ref(str9)),
                make_cref_tuple(member_names(),member_names(),default_strings,values),
                "field2",value,lte,10
            );
    BOOST_CHECK_EQUAL(str9,std::string("field2 is less than or equal to 10"));
}

BOOST_AUTO_TEST_CASE(CheckBypassNamesDefaultStrings)
{
    auto formatter1=formatter();
    std::string str1;
    formatter1.validate_operator(str1,gte,10);
    BOOST_CHECK_EQUAL(str1,std::string("is greater than or equal to 10"));

    std::string str2;
    formatter1.validate_property(str2,empty,eq,false);
    BOOST_CHECK_EQUAL(str2,std::string("is not empty"));
    std::string str3;
    formatter1.validate_property(str3,empty,eq,true);
    BOOST_CHECK_EQUAL(str3,std::string("is empty"));
    std::string str2_1;
    formatter1.validate_property(str2_1,empty,ne,false);
    BOOST_CHECK_EQUAL(str2_1,std::string("is empty"));
    std::string str3_1;
    formatter1.validate_property(str3_1,empty,ne,true);
    BOOST_CHECK_EQUAL(str3_1,std::string("is not empty"));

    std::string str4;
    formatter1.validate_property(str4,size,lte,100);
    BOOST_CHECK_EQUAL(str4,std::string("size is less than or equal to 100"));
    std::string str5;
    formatter1.validate_property(str5,value,eq,true);
    BOOST_CHECK_EQUAL(str5,std::string("value must be true"));

    std::string str11;
    formatter1.validate(str11,"field1",value,eq,true);
    BOOST_CHECK_EQUAL(str11,std::string("field1 must be true"));
    std::string str6;
    formatter1.validate(str6,"field1",value,eq,false);
    BOOST_CHECK_EQUAL(str6,std::string("field1 must be false"));
    std::string str7;
    formatter1.validate(str7,"field1",value,gt,15);
    BOOST_CHECK_EQUAL(str7,std::string("field1 is greater than 15"));
    std::string str8;
    formatter1.validate(str8,"field1",size,lt,100);
    BOOST_CHECK_EQUAL(str8,std::string("size of field1 is less than 100"));
    std::string str9;
    formatter1.validate(str9,"field1",empty,eq,true);
    BOOST_CHECK_EQUAL(str9,std::string("field1 is empty"));
    std::string str10;
    formatter1.validate(str10,"field1",empty,ne,true);
    BOOST_CHECK_EQUAL(str10,std::string("field1 is not empty"));

    std::string str12;
    formatter1.validate_exists(str12,"field1",true);
    BOOST_CHECK_EQUAL(str12,std::string("field1 exists"));
    std::string str13;
    formatter1.validate_exists(str13,"field1",false);
    BOOST_CHECK_EQUAL(str13,std::string("field1 does not exist"));

    std::string str14;
    formatter1.validate_with_other_member(str14,"field1",value,gt,"field2");
    BOOST_CHECK_EQUAL(str14,std::string("field1 is greater than field2"));
    std::string str15;
    formatter1.validate_with_other_member(str15,"field1",size,lte,"field2");
    BOOST_CHECK_EQUAL(str15,std::string("size of field1 is less than or equal to size of field2"));
}

#endif
