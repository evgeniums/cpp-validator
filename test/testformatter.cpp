//#define DRACOSHA_VALIDATOR_TEST_FORMATTER
#ifdef DRACOSHA_VALIDATOR_TEST_FORMATTER

namespace
{

template <typename WrapStringFn>
void checkOrderAndPresentation(const WrapStringFn& wrapper)
{
    std::string str1;
    auto w1=wrapper(str1);
    detail::reorder_and_present(
                w1,
                make_cref_tuple(default_strings,default_operand_formatter),
                gte,10
            );
    BOOST_CHECK_EQUAL(str1,std::string("must be greater than or equal to 10"));

    std::string str2;
    auto w2=wrapper(str2);
    detail::reorder_and_present(
                w2,
                make_cref_tuple(default_strings,default_operand_formatter),
                flag,true
            );
    BOOST_CHECK_EQUAL(str2,std::string("must be true"));

    std::string str3;
    auto w3=wrapper(str3);
    detail::reorder_and_present(
                w3,
                make_cref_tuple(default_strings,default_operand_formatter),
                flag,false
            );
    BOOST_CHECK_EQUAL(str3,std::string("must be false"));

    auto mn=get_default_member_names();
    std::string str4;
    auto w4=wrapper(str4);
    detail::reorder_and_present(
                w4,
                make_cref_tuple(mn,mn,default_strings,default_operand_formatter),
                "field1",size,eq,100
            );
    BOOST_CHECK_EQUAL(str4,std::string("size of field1 must be equal to 100"));

    std::string str5;
    auto w5=wrapper(str5);
    detail::reorder_and_present(
                w5,
                make_cref_tuple(get_default_member_names(),get_default_member_names(),default_strings,default_operand_formatter),
                "field2",value,flag,true
            );
    BOOST_CHECK_EQUAL(str5,std::string("field2 must be true"));

    std::string str6;
    auto w6=wrapper(str6);
    detail::reorder_and_present(
                w6,
                make_cref_tuple(get_default_member_names(),default_strings,default_operand_formatter),
                empty,flag,false
            );
    BOOST_CHECK_EQUAL(str6,std::string("must be not empty"));

    std::string str7;
    auto w7=wrapper(str7);
    detail::reorder_and_present(
                w7,
                make_cref_tuple(get_default_member_names(),get_default_member_names(),default_strings,default_operand_formatter),
                "field1",empty,flag,false
            );
    BOOST_CHECK_EQUAL(str7,std::string("field1 must be not empty"));

    std::string str8;
    auto w8=wrapper(str8);
    detail::reorder_and_present(
                w8,
                make_cref_tuple(get_default_member_names(),get_default_member_names(),default_strings,default_operand_formatter),
                "field2",empty,flag,true
            );
    BOOST_CHECK_EQUAL(str8,std::string("field2 must be empty"));

    std::string str9;
    auto w9=wrapper(str9);
    detail::reorder_and_present(
                w9,
                make_cref_tuple(get_default_member_names(),get_default_member_names(),default_strings,default_operand_formatter),
                "field2",value,lte,10
            );
    BOOST_CHECK_EQUAL(str9,std::string("field2 must be less than or equal to 10"));

    std::string str10;
    auto w10=wrapper(str10);
    detail::reorder_and_present(
                w10,
                make_cref_tuple(get_default_member_names(),get_default_member_names(),default_strings,default_strings),
                "field2",value,lte,"field2",string_master_sample
            );
    BOOST_CHECK_EQUAL(str10,std::string("field2 must be less than or equal to field2 of sample"));

    std::string str11;
    auto w11=wrapper(str11);
    detail::reorder_and_present(
                w11,
                make_cref_tuple(get_default_member_names(),get_default_member_names(),default_strings,get_default_member_names()),
                "field2",value,lte,make_member_operand("field1")
            );
    BOOST_CHECK_EQUAL(str11,std::string("field2 must be less than or equal to field1"));

    std::string str12;
    auto w12=wrapper(str12);
    detail::reorder_and_present(
                w12,
                make_cref_tuple(get_default_member_names(),get_default_member_names(),default_strings,get_default_member_names()),
                "field2",size,lte,make_member_operand("field1")
            );
    BOOST_CHECK_EQUAL(str12,std::string("size of field2 must be less than or equal to size of field1"));
}

template <typename WrapStringFn>
void checkPropertyFlag(const WrapStringFn& wrapper)
{
    std::string str1;
    auto w1=wrapper(str1);
    auto mn=get_default_member_names();

    detail::reorder_and_present(
                w1,
                make_cref_tuple(mn,default_strings,default_operand_formatter),
                empty,flag,true
            );
    BOOST_CHECK_EQUAL(str1,std::string("must be empty"));
    str1.clear();

    detail::reorder_and_present(
                w1,
                make_cref_tuple(mn,default_strings,default_operand_formatter),
                empty,flag,false
            );
    BOOST_CHECK_EQUAL(str1,std::string("must be not empty"));
    str1.clear();

    detail::reorder_and_present(
                w1,
                make_cref_tuple(mn,default_strings,default_operand_formatter),
                empty,flag(flag_true_false),true
            );
    BOOST_CHECK_EQUAL(str1,std::string("empty must be true"));
    str1.clear();
    detail::reorder_and_present(
                w1,
                make_cref_tuple(mn,default_strings,default_operand_formatter),
                empty,flag(flag_true_false),false
            );
    BOOST_CHECK_EQUAL(str1,std::string("empty must be false"));
    str1.clear();

    detail::reorder_and_present(
                w1,
                make_cref_tuple(mn,default_strings,default_operand_formatter),
                empty,flag(flag_on_off),true
            );
    BOOST_CHECK_EQUAL(str1,std::string("empty must be on"));
    str1.clear();
    detail::reorder_and_present(
                w1,
                make_cref_tuple(mn,default_strings,default_operand_formatter),
                empty,flag(flag_on_off),false
            );
    BOOST_CHECK_EQUAL(str1,std::string("empty must be off"));
    str1.clear();

    detail::reorder_and_present(
                w1,
                make_cref_tuple(mn,default_strings,default_operand_formatter),
                empty,flag("expected"),true
            );
    BOOST_CHECK_EQUAL(str1,std::string("empty expected"));
    str1.clear();
    detail::reorder_and_present(
                w1,
                make_cref_tuple(mn,default_strings,default_operand_formatter),
                empty,flag("not expected"),false
            );
    BOOST_CHECK_EQUAL(str1,std::string("empty not expected"));
    str1.clear();
}

template <typename FormatterT, typename WrapStringFn>
void testFormatter(const FormatterT& formatter1, const WrapStringFn& wrapper)
{
    std::string str1;
    auto w1=wrapper(str1);
    formatter1.validate_operator(w1,gte,10);
    BOOST_CHECK_EQUAL(str1,std::string("must be greater than or equal to 10"));

    std::string str2;
    auto w2=wrapper(str2);
    formatter1.validate_property(w2,empty,flag,false);
    BOOST_CHECK_EQUAL(str2,std::string("must be not empty"));
    std::string str3;
    auto w3=wrapper(str3);
    formatter1.validate_property(w3,empty,flag,true);
    BOOST_CHECK_EQUAL(str3,std::string("must be empty"));
    std::string str2_1;

    std::string str4;
    auto w4=wrapper(str4);
    formatter1.validate_property(w4,size,lte,100);
    BOOST_CHECK_EQUAL(str4,std::string("size must be less than or equal to 100"));
    std::string str5;
    auto w5=wrapper(str5);
    formatter1.validate_property(w5,value,flag,true);
    BOOST_CHECK_EQUAL(str5,std::string("must be true"));

    std::string str11;
    auto w11=wrapper(str11);
    formatter1.validate(w11,"field1",value,flag,true);
    BOOST_CHECK_EQUAL(str11,std::string("field1 must be true"));
    std::string str6;
    auto w6=wrapper(str6);
    formatter1.validate(w6,"field1",value,flag,false);
    BOOST_CHECK_EQUAL(str6,std::string("field1 must be false"));
    std::string str7;
    auto w7=wrapper(str7);
    formatter1.validate(w7,"field1",value,gt,15);
    BOOST_CHECK_EQUAL(str7,std::string("field1 must be greater than 15"));
    std::string str8;
    auto w8=wrapper(str8);
    formatter1.validate(w8,"field1",size,lt,100);
    BOOST_CHECK_EQUAL(str8,std::string("size of field1 must be less than 100"));
    std::string str9;
    auto w9=wrapper(str9);
    formatter1.validate(w9,"field1",empty,flag,true);
    BOOST_CHECK_EQUAL(str9,std::string("field1 must be empty"));
    std::string str10;
    auto w10=wrapper(str10);
    formatter1.validate(w10,"field1",empty,flag,false);
    BOOST_CHECK_EQUAL(str10,std::string("field1 must be not empty"));

    std::string str12;
    auto w12=wrapper(str12);
    formatter1.validate_exists(w12,"field1",exists,true);
    BOOST_CHECK_EQUAL(str12,std::string("field1 must exist"));
    std::string str13;
    auto w13=wrapper(str13);
    formatter1.validate_exists(w13,"field1",exists,false);
    BOOST_CHECK_EQUAL(str13,std::string("field1 must not exist"));

    std::string str14;
    auto w14=wrapper(str14);
    formatter1.validate_with_other_member(w14,"field1",value,gt,"field2");
    BOOST_CHECK_EQUAL(str14,std::string("field1 must be greater than field2"));
    std::string str15;
    auto w15=wrapper(str15);
    formatter1.validate_with_other_member(w15,"field1",size,lte,"field2");
    BOOST_CHECK_EQUAL(str15,std::string("size of field1 must be less than or equal to size of field2"));

    std::string str16;
    auto w16=wrapper(str16);
    formatter1.validate_with_master_sample(w16,"field1",value,gt,"field1","");
    BOOST_CHECK_EQUAL(str16,std::string("field1 must be greater than field1 of sample"));
    std::string str17;
    auto w17=wrapper(str17);
    formatter1.validate_with_master_sample(w17,"field1",size,lte,"field1","");
    BOOST_CHECK_EQUAL(str17,std::string("size of field1 must be less than or equal to size of field1 of sample"));

    std::string str18;
    auto w18=wrapper(str18);
    report_aggregation<std::string> and_op(string_and);
    and_op.parts={"one","two","three"};
    formatter1.aggregate(w18,and_op);
    BOOST_CHECK_EQUAL(str18,std::string("one AND two AND three"));

    std::string str19;
    auto w19=wrapper(str19);
    report_aggregation<std::string> or_op(string_or);
    or_op.parts={"one","two","three"};
    or_op.single=false;
    formatter1.aggregate(w19,or_op);
    BOOST_CHECK_EQUAL(str19,std::string("(one OR two OR three)"));

    std::string str20;
    auto w20=wrapper(str20);
    report_aggregation<std::string> not_op(string_not);
    not_op.parts={"one"};
    formatter1.aggregate(w20,not_op);
    BOOST_CHECK_EQUAL(str20,std::string("NOT one"));

    std::string str21;
    auto w21=wrapper(str21);
    report_aggregation<std::string> not_op1(string_not);
    not_op1.parts={"two"};
    formatter1.aggregate(w21,not_op1);
    BOOST_CHECK_EQUAL(str21,std::string("NOT two"));

    auto str22=formatter1.member_to_string(size);
    BOOST_CHECK_EQUAL(str22,std::string("size"));
}

auto make_test_strings(translator_repository& rep)
{
    // don't call it twice with the same rep because references to previous translators will be invalidated
    std::map<std::string,std::string> m=
    {
        {"one","one_translated"},
        {"two","two_translated"},
        {"three","three_translated"}
    };
    auto translator1=std::make_shared<mapped_translator>(std::move(m));
    std::set<std::string> locales1={"en_US.UTF-8","en_US","en"};
    rep.add_translator(translator1,locales1);
    return make_translated_strings(rep,"en");
}

const auto& make_test_translator(translator_repository& rep)
{
    // don't call it twice with the same rep because references to previous translators will be invalidated
    std::map<std::string,std::string> m=
    {
        {"one","one_translated"},
        {"two","two_translated"},
        {"three","three_translated"}
    };
    auto translator1=std::make_shared<mapped_translator>(std::move(m));
    std::set<std::string> locales1={"en_US.UTF-8","en_US","en"};
    rep.add_translator(translator1,locales1);
    return *rep.find_translator("en");
}
void fill_test_repository(translator_repository& rep)
{
    // don't call it twice with the same rep because references to previous translators will be invalidated
    std::map<std::string,std::string> m=
    {
        {"one","one_translated"},
        {"two","two_translated"},
        {"three","three_translated"}
    };
    auto translator1=std::make_shared<mapped_translator>(std::move(m));
    std::set<std::string> locales1={"en_US.UTF-8","en_US","en"};
    rep.add_translator(translator1,locales1);
}

template <typename WrapStringFn>
void checkDefaultFormatter(const WrapStringFn& wrapper)
{
    const auto& fm=get_default_formatter();

    using fm_type=decltype(fm);
    static_assert(std::is_lvalue_reference<typename std::decay_t<fm_type>::member_names_type>::value,"");
    static_assert(std::is_lvalue_reference<typename std::decay_t<fm_type>::operands_type>::value,"");
    static_assert(std::is_lvalue_reference<typename std::decay_t<fm_type>::strings_type>::value,"");
    static_assert(std::is_lvalue_reference<typename std::decay_t<fm_type>::order_and_presentation_type>::value,"");

    testFormatter(fm,wrapper);
}

template <typename WrapStringFn>
void checkFormatterFromStrings(const WrapStringFn& wrapper)
{
    translator_repository rep;
    auto fm=make_formatter(make_test_strings(rep));

    using fm_type=decltype(fm);
    static_assert(!std::is_lvalue_reference<typename fm_type::member_names_type>::value,"");
    static_assert(!std::is_lvalue_reference<typename fm_type::operands_type>::value,"");
    static_assert(!std::is_lvalue_reference<typename fm_type::strings_type>::value,"");
    static_assert(std::is_lvalue_reference<typename fm_type::order_and_presentation_type>::value,"");

    testFormatter(fm,wrapper);
}

template <typename WrapStringFn>
void checkFormatterFromTranslator(const WrapStringFn& wrapper)
{
    translator_repository rep;
    auto fm=make_formatter(make_test_translator(rep));

    using fm_type=decltype(fm);
    static_assert(!std::is_lvalue_reference<typename fm_type::member_names_type>::value,"");
    static_assert(!std::is_lvalue_reference<typename fm_type::operands_type>::value,"");
    static_assert(!std::is_lvalue_reference<typename fm_type::strings_type>::value,"");
    static_assert(std::is_lvalue_reference<typename fm_type::order_and_presentation_type>::value,"");

    testFormatter(fm,wrapper);
}

template <typename WrapStringFn>
void checkFormatterFromTranslatorRepository(const WrapStringFn& wrapper)
{
    translator_repository rep;
    fill_test_repository(rep);
    auto fm=make_formatter(rep,"en");

    using fm_type=decltype(fm);
    static_assert(!std::is_lvalue_reference<typename fm_type::member_names_type>::value,"");
    static_assert(!std::is_lvalue_reference<typename fm_type::operands_type>::value,"");
    static_assert(!std::is_lvalue_reference<typename fm_type::strings_type>::value,"");
    static_assert(std::is_lvalue_reference<typename fm_type::order_and_presentation_type>::value,"");

    testFormatter(fm,wrapper);
}

template <typename WrapStringFn>
void checkFormatterFromMemberNames(const WrapStringFn& wrapper)
{
    translator_repository rep;
    auto fm=make_formatter(make_translated_member_names(make_test_translator(rep)));

    using fm_type=decltype(fm);
    static_assert(!std::is_lvalue_reference<typename fm_type::member_names_type>::value,"");
    static_assert(std::is_lvalue_reference<typename fm_type::operands_type>::value,"");
    static_assert(std::is_lvalue_reference<typename fm_type::strings_type>::value,"");
    static_assert(std::is_lvalue_reference<typename fm_type::order_and_presentation_type>::value,"");

    testFormatter(fm,wrapper);
}

template <typename WrapStringFn>
void checkFormatterFromMemberNamesAndOperands(const WrapStringFn& wrapper)
{
    translator_repository rep;

    auto fm=make_formatter(make_translated_member_names(make_test_translator(rep)),make_translated_operand_formatter(rep,"en"));

    using fm_type=decltype(fm);
    static_assert(!std::is_lvalue_reference<typename fm_type::member_names_type>::value,"");
    static_assert(!std::is_lvalue_reference<typename fm_type::operands_type>::value,"");
    static_assert(std::is_lvalue_reference<typename fm_type::strings_type>::value,"");
    static_assert(std::is_lvalue_reference<typename fm_type::order_and_presentation_type>::value,"");

    testFormatter(fm,wrapper);
}

template <typename WrapStringFn>
void checkFormatterWithRefs(const WrapStringFn& wrapper)
{
    translator_repository rep;
    auto st=make_test_strings(rep);
    auto mn=make_translated_member_names(rep);
    auto vs=make_translated_operand_formatter(rep,"en");
    auto fm=make_formatter(
                mn,
                vs,
                st
                );

    using fm_type=decltype(fm);
    static_assert(std::is_lvalue_reference<typename fm_type::member_names_type>::value,"");
    static_assert(std::is_lvalue_reference<typename fm_type::operands_type>::value,"");
    static_assert(std::is_lvalue_reference<typename fm_type::strings_type>::value,"");
    static_assert(std::is_lvalue_reference<typename fm_type::order_and_presentation_type>::value,"");

    testFormatter(fm,wrapper);
}

template <typename WrapStringFn>
void checkFormatterWithRvals(const WrapStringFn& wrapper)
{
    mapped_translator tr1;
    auto fm=make_formatter(
                make_translated_member_names(tr1),
                make_translated_operand_formatter(tr1),
                make_translated_strings(tr1)
                );

    using fm_type=decltype(fm);
    static_assert(!std::is_lvalue_reference<typename fm_type::member_names_type>::value,"");
    static_assert(!std::is_lvalue_reference<typename fm_type::operands_type>::value,"");
    static_assert(!std::is_lvalue_reference<typename fm_type::strings_type>::value,"");
    static_assert(std::is_lvalue_reference<typename fm_type::order_and_presentation_type>::value,"");

    static_assert(!std::is_rvalue_reference<typename fm_type::member_names_type>::value,"");
    static_assert(!std::is_rvalue_reference<typename fm_type::operands_type>::value,"");
    static_assert(!std::is_rvalue_reference<typename fm_type::strings_type>::value,"");

    testFormatter(fm,wrapper);
}

}

#endif
