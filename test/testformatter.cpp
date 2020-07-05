//#define DRACOSHA_VALIDATOR_TEST_FORMATTER
#ifdef DRACOSHA_VALIDATOR_TEST_FORMATTER

namespace
{

void checkOrderAndPresentation()
{
    std::string str1;
    detail::reorder_and_present(
                str1,
                make_cref_tuple(default_strings,values),
                gte,10
            );
    BOOST_CHECK_EQUAL(str1,std::string("is greater than or equal to 10"));

    std::string str2;
    detail::reorder_and_present(
                str2,
                make_cref_tuple(default_strings,values),
                eq,bool(true)
            );
    BOOST_CHECK_EQUAL(str2,std::string("must be true"));

    std::string str3;
    detail::reorder_and_present(
                str3,
                make_cref_tuple(default_strings,values),
                ne,true
            );
    BOOST_CHECK_EQUAL(str3,std::string("must not be true"));

    auto mn=member_names();
    std::string str4;
    detail::reorder_and_present(
                str4,
                make_cref_tuple(mn,mn,default_strings,values),
                "field1",size,eq,100
            );
    BOOST_CHECK_EQUAL(str4,std::string("size of field1 is equal to 100"));

    std::string str5;
    detail::reorder_and_present(
                str5,
                make_cref_tuple(member_names(),member_names(),default_strings,values),
                "field2",value,eq,true
            );
    BOOST_CHECK_EQUAL(str5,std::string("field2 must be true"));

    std::string str6;
    detail::reorder_and_present(
                str6,
                make_cref_tuple(member_names(),default_strings,values),
                empty,eq,false
            );
    BOOST_CHECK_EQUAL(str6,std::string("is not empty"));

    std::string str7;
    detail::reorder_and_present(
                str7,
                make_cref_tuple(member_names(),member_names(),default_strings,values),
                "field1",empty,ne,true
            );
    BOOST_CHECK_EQUAL(str7,std::string("field1 is not empty"));

    std::string str8;
    detail::reorder_and_present(
                str8,
                make_cref_tuple(member_names(),member_names(),default_strings,values),
                "field2",empty,eq,true
            );
    BOOST_CHECK_EQUAL(str8,std::string("field2 is empty"));

    std::string str9;
    detail::reorder_and_present(
                str9,
                make_cref_tuple(member_names(),member_names(),default_strings,values),
                "field2",value,lte,10
            );
    BOOST_CHECK_EQUAL(str9,std::string("field2 is less than or equal to 10"));

    std::string str10;
    detail::reorder_and_present(
                str10,
                make_cref_tuple(member_names(),member_names(),default_strings,default_strings),
                "field2",value,lte,string_master_sample
            );
    BOOST_CHECK_EQUAL(str10,std::string("field2 is less than or equal to field2 of sample"));

    std::string str11;
    detail::reorder_and_present(
                str11,
                make_cref_tuple(member_names(),member_names(),default_strings,member_names()),
                "field2",value,lte,member_name("field1")
            );
    BOOST_CHECK_EQUAL(str11,std::string("field2 is less than or equal to field1"));

    std::string str12;
    detail::reorder_and_present(
                str12,
                make_cref_tuple(member_names(),member_names(),default_strings,member_names()),
                "field2",size,lte,member_name("field1")
            );
    BOOST_CHECK_EQUAL(str12,std::string("size of field2 is less than or equal to size of field1"));
}

template <typename FormatterT>
void testFormatter(const FormatterT& formatter1)
{
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

    std::string str16;
    formatter1.validate_with_master_sample(str16,"field1",value,gt,"");
    BOOST_CHECK_EQUAL(str16,std::string("field1 is greater than field1 of sample"));
    std::string str17;
    formatter1.validate_with_master_sample(str17,"field1",size,lte,"");
    BOOST_CHECK_EQUAL(str17,std::string("size of field1 is less than or equal to size of field1 of sample"));

    std::string str18;
    report_aggregation<std::string> and_op(string_and);
    and_op.parts={"one","two","three"};
    formatter1.aggregate(str18,and_op);
    BOOST_CHECK_EQUAL(str18,std::string("one AND two AND three"));

    std::string str19;
    report_aggregation<std::string> or_op(string_or);
    or_op.parts={"one","two","three"};
    or_op.single=false;
    formatter1.aggregate(str19,or_op);
    BOOST_CHECK_EQUAL(str19,std::string("(one OR two OR three)"));

    std::string str20;
    report_aggregation<std::string> not_op(string_not);
    not_op.parts={"one"};
    formatter1.aggregate(str20,not_op);
    BOOST_CHECK_EQUAL(str20,std::string("NOT one"));

    std::string str21;
    report_aggregation<std::string> not_op1(string_not);
    not_op1.parts={"two"};
    formatter1.aggregate(str21,not_op1);
    BOOST_CHECK_EQUAL(str21,std::string("NOT two"));

    auto str22=formatter1.member_to_string(size);
    BOOST_CHECK_EQUAL(str22,std::string("size"));
}

auto make_test_strings(translator_repository& rep)
{
    std::map<std::string,std::string> m=
    {
        {"one","one_translated"},
        {"two","two_translated"},
        {"three","three_translated"}
    };
    auto translator1=std::make_shared<mapped_translator>(m);
    std::set<std::string> locales1={"en_US.UTF-8","en_US","en"};
    rep.add_translator(translator1,locales1);
    return make_translated_strings(rep,"en");
}

void checkDefaultFormatter()
{
    testFormatter(formatter());
}

void checkFormatterFromStrings()
{
    translator_repository rep;
    testFormatter(formatter(make_test_strings(rep)));
}

void checkFormatterFromMemberNames()
{
    translator_repository rep;
    testFormatter(formatter(member_names(make_test_strings(rep))));
}

void checkFormatterFromMemberNamesAndValues()
{
    translator_repository rep;
    testFormatter(formatter(
                            member_names(make_test_strings(rep)),
                            make_translated_values(rep,"en")
                            )
                  );
}

void checkFormatterWithRefs()
{
    translator_repository rep;
    auto st=make_test_strings(rep);
    auto mn=member_names(st);
    auto vs=make_translated_values(rep,"en");
    testFormatter(formatter_with_references(
                            mn,
                            vs,
                            st
                            )
                  );
}
}

#endif
