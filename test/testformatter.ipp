BOOST_AUTO_TEST_CASE(CheckBypassNamesDefaultStrings)
{
    auto formatter1=formatter();
#if 1
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
    BOOST_CHECK_EQUAL(str5,std::string("value is equal to true"));
#endif

    std::string str6;
    detail::reorder_strings(
                hana::partial(format_append,hana::type_c<std::string>,std::ref(str6)),
                make_cref_tuple(member_names(),default_strings,values),
                empty,eq,false
            );
    BOOST_TEST_MESSAGE(str6);
}
