PROJECT(dracoshavalidator)

SET (HEADERS
    include/dracosha/validator/version.hpp
    include/dracosha/validator/config.hpp

    include/dracosha/validator/status.hpp

    include/dracosha/validator/utils/hana_to_std_tuple.hpp
    include/dracosha/validator/utils/safe_compare.hpp
    include/dracosha/validator/utils/adjust_storable_type.hpp
    include/dracosha/validator/utils/enable_to_string.hpp
    include/dracosha/validator/utils/make_types_tuple.hpp
    include/dracosha/validator/utils/reference_wrapper.hpp
    include/dracosha/validator/utils/string_view.hpp
    include/dracosha/validator/utils/object_wrapper.hpp
    include/dracosha/validator/utils/is_container.hpp
    include/dracosha/validator/utils/is_pair.hpp
    include/dracosha/validator/utils/get_it.hpp
    include/dracosha/validator/utils/wrap_it.hpp
    include/dracosha/validator/utils/feature_bitmask.hpp

    include/dracosha/validator/property.hpp
    include/dracosha/validator/can_get.hpp
    include/dracosha/validator/get.hpp
    include/dracosha/validator/can_check_contains.hpp
    include/dracosha/validator/check_contains.hpp
    include/dracosha/validator/check_member.hpp
    include/dracosha/validator/check_exists.hpp
    include/dracosha/validator/lazy.hpp
    include/dracosha/validator/extract.hpp
    include/dracosha/validator/get_member.hpp
    include/dracosha/validator/validate.hpp
    include/dracosha/validator/property_validator.hpp
    include/dracosha/validator/apply.hpp
    include/dracosha/validator/member.hpp
    include/dracosha/validator/operand.hpp
    include/dracosha/validator/master_sample.hpp
    include/dracosha/validator/validator.hpp
    include/dracosha/validator/make_validator.hpp
    include/dracosha/validator/dispatcher.hpp
    include/dracosha/validator/check_member_path.hpp
    include/dracosha/validator/with_check_member_exists.hpp
    include/dracosha/validator/check_member_exists_traits_proxy.hpp
    include/dracosha/validator/validators.hpp
    include/dracosha/validator/member_property.hpp
    include/dracosha/validator/interval.hpp

    include/dracosha/validator/operators/operator.hpp
    include/dracosha/validator/operators/exists.hpp
    include/dracosha/validator/operators/comparison.hpp
    include/dracosha/validator/operators/and.hpp
    include/dracosha/validator/operators/or.hpp
    include/dracosha/validator/operators/not.hpp
    include/dracosha/validator/operators/any.hpp
    include/dracosha/validator/operators/all.hpp
    include/dracosha/validator/operators/aggregation.hpp
    include/dracosha/validator/operators/logical.hpp
    include/dracosha/validator/operators/invert_op.hpp
    include/dracosha/validator/operators/flag.hpp
    include/dracosha/validator/operators/wrap_op.hpp
    include/dracosha/validator/operators/contains.hpp
    include/dracosha/validator/operators/in.hpp
    include/dracosha/validator/operators/lexicographical.hpp
    include/dracosha/validator/operators.hpp

    include/dracosha/validator/properties/value.hpp
    include/dracosha/validator/properties/size.hpp
    include/dracosha/validator/properties/length.hpp
    include/dracosha/validator/properties/empty.hpp
    include/dracosha/validator/properties.hpp

    include/dracosha/validator/adapters/adapter.hpp
    include/dracosha/validator/adapters/default_adapter.hpp
    include/dracosha/validator/adapters/reporting_adapter.hpp
    include/dracosha/validator/adapters/single_member_adapter.hpp

    include/dracosha/validator/reporting/reporter.hpp
    include/dracosha/validator/reporting/formatter.hpp
    include/dracosha/validator/reporting/member_operand.hpp
    include/dracosha/validator/reporting/member_names.hpp
    include/dracosha/validator/reporting/strings.hpp
    include/dracosha/validator/reporting/translator.hpp
    include/dracosha/validator/reporting/translator_repository.hpp
    include/dracosha/validator/reporting/no_translator.hpp
    include/dracosha/validator/reporting/mapped_translator.hpp
    include/dracosha/validator/reporting/operand_formatter.hpp
    include/dracosha/validator/reporting/order_and_presentation.hpp
    include/dracosha/validator/reporting/report_aggregation.hpp
    include/dracosha/validator/reporting/aggregation_strings.hpp
    include/dracosha/validator/reporting/single_member_name.hpp
    include/dracosha/validator/reporting/nested_member_name.hpp
    include/dracosha/validator/reporting/backend_formatter.hpp
    include/dracosha/validator/reporting/decorator.hpp
    include/dracosha/validator/reporting/quotes_decorator.hpp
    include/dracosha/validator/reporting/flag_presets.hpp
    include/dracosha/validator/reporting/grammar_categories.hpp
    include/dracosha/validator/reporting/concrete_phrase.hpp
    include/dracosha/validator/reporting/translate.hpp
    include/dracosha/validator/reporting/property_member_name.hpp
    include/dracosha/validator/reporting/phrase_translator.hpp
    include/dracosha/validator/reporting/reporter_with_object_name.hpp
    include/dracosha/validator/reporting/extend_translator.hpp
    include/dracosha/validator/reporting/phrase_grammar_cats.hpp
    include/dracosha/validator/reporting/prepare_operand_for_formatter.hpp

    include/dracosha/validator/reporting/locale/sample_locale.hpp
    include/dracosha/validator/reporting/locale/ru.hpp

    include/dracosha/validator/detail/has_method.hpp
    include/dracosha/validator/detail/has_property.hpp
    include/dracosha/validator/detail/get_impl.hpp
    include/dracosha/validator/detail/aggregate_and.hpp
    include/dracosha/validator/detail/aggregate_or.hpp
    include/dracosha/validator/detail/aggregate_any.hpp
    include/dracosha/validator/detail/aggregate_all.hpp
    include/dracosha/validator/detail/logical_not.hpp
    include/dracosha/validator/detail/dispatcher_impl.hpp
    include/dracosha/validator/detail/take_address_of.hpp
    include/dracosha/validator/detail/to_string.hpp
    include/dracosha/validator/detail/format_operand.hpp
    include/dracosha/validator/detail/formatter_fmt.hpp
    include/dracosha/validator/detail/reorder_and_present.hpp
    include/dracosha/validator/detail/formatter_std.hpp
    include/dracosha/validator/detail/backend_formatter_helper.hpp
    include/dracosha/validator/detail/default_adapter_impl.hpp
    include/dracosha/validator/detail/reporting_adapter_impl.hpp
    include/dracosha/validator/detail/single_member_adapter_impl.hpp
    include/dracosha/validator/detail/hint_helper.hpp

    include/dracosha/validator/detail/default_adapter_impl.ipp
)

SET (SOURCES
)

BUILD_DRACOSHA_MODULE()

INCLUDE(dracosha/deps/fmt)
IF (NOT FMT_HEADER_ONLY)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} INTERFACE fmt)
ENDIF(NOT FMT_HEADER_ONLY)
IF (NOT SYSTEM_FMTLIB)
    TARGET_INCLUDE_DIRECTORIES(${PROJECT_NAME} INTERFACE "${DRACOSHA_SOURCE_DIR}/3rdparty/fmt/include")    
ENDIF()

IF (INSTALL_DEV)
    INCLUDE(dracosha/Install)
ENDIF (INSTALL_DEV)
