CMAKE_MINIMUM_REQUIRED (VERSION 3.12)
PROJECT(hatnvalidator)

SET (HEADERS
    include/hatn/validator/version.hpp
    include/hatn/validator/config.hpp
    include/hatn/validator/ignore_compiler_warnings.hpp

    include/hatn/validator/status.hpp

    include/hatn/validator/utils/hana_to_std_tuple.hpp
    include/hatn/validator/utils/safe_compare.hpp
    include/hatn/validator/utils/adjust_storable_type.hpp
    include/hatn/validator/utils/adjust_storable_ignore.hpp
    include/hatn/validator/utils/enable_to_string.hpp
    include/hatn/validator/utils/make_types_tuple.hpp
    include/hatn/validator/utils/reference_wrapper.hpp
    include/hatn/validator/utils/string_view.hpp
    include/hatn/validator/utils/object_wrapper.hpp
    include/hatn/validator/utils/unwrap_object.hpp
    include/hatn/validator/utils/wrap_object.hpp
    include/hatn/validator/utils/is_container.hpp
    include/hatn/validator/utils/is_pair.hpp
    include/hatn/validator/utils/get_it.hpp
    include/hatn/validator/utils/feature_bitmask.hpp
    include/hatn/validator/utils/invoke_and.hpp
    include/hatn/validator/utils/invoke_or.hpp
    include/hatn/validator/utils/value_as_container.hpp
    include/hatn/validator/utils/conditional_fold.hpp
    include/hatn/validator/utils/to_string.hpp
    include/hatn/validator/utils/class_method_args.hpp
    include/hatn/validator/utils/optional.hpp
    include/hatn/validator/utils/copy.hpp
    include/hatn/validator/utils/heterogeneous_size.hpp
    include/hatn/validator/utils/foreach_if.hpp
    include/hatn/validator/utils/pointer_as_reference.hpp
    include/hatn/validator/utils/has_reset.hpp

    include/hatn/validator/adapter.hpp
    include/hatn/validator/property.hpp
    include/hatn/validator/basic_property.hpp
    include/hatn/validator/can_get.hpp
    include/hatn/validator/get.hpp
    include/hatn/validator/can_check_contains.hpp
    include/hatn/validator/check_contains.hpp
    include/hatn/validator/check_member.hpp
    include/hatn/validator/check_exists.hpp
    include/hatn/validator/lazy.hpp
    include/hatn/validator/extract.hpp
    include/hatn/validator/get_member.hpp
    include/hatn/validator/validate.hpp
    include/hatn/validator/property_validator.hpp
    include/hatn/validator/apply.hpp
    include/hatn/validator/member.hpp
    include/hatn/validator/operand.hpp
    include/hatn/validator/master_sample.hpp
    include/hatn/validator/validator.hpp
    include/hatn/validator/make_validator.hpp
    include/hatn/validator/dispatcher.hpp
    include/hatn/validator/check_member_path.hpp
    include/hatn/validator/with_check_member_exists.hpp
    include/hatn/validator/check_member_exists_traits_proxy.hpp
    include/hatn/validator/validators.hpp
    include/hatn/validator/member_property.hpp
    include/hatn/validator/interval.hpp
    include/hatn/validator/range.hpp
    include/hatn/validator/error.hpp
    include/hatn/validator/member_path.hpp
    include/hatn/validator/variadic_property.hpp
    include/hatn/validator/variadic_arg.hpp
    include/hatn/validator/embedded_object.hpp
    include/hatn/validator/base_validator.hpp
    include/hatn/validator/heterogeneous_property.hpp
    include/hatn/validator/value_transformer.hpp
    include/hatn/validator/member_with_name.hpp
    include/hatn/validator/member_with_name_list.hpp
    include/hatn/validator/make_member.hpp
    include/hatn/validator/prepend_super_member.hpp
    include/hatn/validator/prepend_super_member.ipp
    include/hatn/validator/variadic_arg_tag.hpp
    include/hatn/validator/generate_paths.hpp
    include/hatn/validator/apply_generated_paths.hpp
    include/hatn/validator/apply_member_path.hpp
    include/hatn/validator/apply_generated_paths.ipp
    include/hatn/validator/apply_member_path.ipp
    include/hatn/validator/invoke_member_if_exists.hpp
    include/hatn/validator/filter_member.hpp
    include/hatn/validator/filter_path.hpp
    include/hatn/validator/compact_variadic_property.hpp

    include/hatn/validator/aggregation/and.hpp
    include/hatn/validator/aggregation/or.hpp
    include/hatn/validator/aggregation/not.hpp
    include/hatn/validator/aggregation/any.hpp
    include/hatn/validator/aggregation/all.hpp
    include/hatn/validator/aggregation/aggregation.hpp
    include/hatn/validator/aggregation/aggregation.ipp
    include/hatn/validator/aggregation/element_aggregation.hpp
    include/hatn/validator/aggregation/element_aggregation.ipp
    include/hatn/validator/aggregation/wrap_it.hpp
    include/hatn/validator/aggregation/wrap_index.hpp
    include/hatn/validator/aggregation/wrap_heterogeneous_index.hpp
    include/hatn/validator/aggregation/tree.hpp

    include/hatn/validator/operators/operator.hpp
    include/hatn/validator/operators/exists.hpp
    include/hatn/validator/operators/comparison.hpp
    include/hatn/validator/operators/logical.hpp
    include/hatn/validator/operators/invert_op.hpp
    include/hatn/validator/operators/flag.hpp
    include/hatn/validator/operators/wrap_op.hpp
    include/hatn/validator/operators/contains.hpp
    include/hatn/validator/operators/in.hpp
    include/hatn/validator/operators/lexicographical.hpp
    include/hatn/validator/operators/lex_in.hpp
    include/hatn/validator/operators/regex.hpp
    include/hatn/validator/operators/op_report_without_operand.hpp
    include/hatn/validator/operators/string_patterns.hpp
    include/hatn/validator/operators/number_patterns.hpp
    include/hatn/validator/operators.hpp

    include/hatn/validator/properties/value.hpp
    include/hatn/validator/properties/size.hpp
    include/hatn/validator/properties/length.hpp
    include/hatn/validator/properties/empty.hpp
    include/hatn/validator/properties/pair.hpp
    include/hatn/validator/properties/h_size.hpp
    include/hatn/validator/properties.hpp

    include/hatn/validator/adapters/adapter_traits_wrapper.hpp
    include/hatn/validator/adapters/prevalidation_adapter.hpp
    include/hatn/validator/adapters/default_adapter.hpp
    include/hatn/validator/adapters/reporting_adapter.hpp
    include/hatn/validator/adapters/impl/default_adapter_impl.hpp
    include/hatn/validator/adapters/impl/intermediate_adapter_traits.hpp
    include/hatn/validator/adapters/make_intermediate_adapter.hpp
    include/hatn/validator/adapters/failed_members_adapter.hpp

    include/hatn/validator/reporting/reporting_adapter_impl.hpp
    include/hatn/validator/reporting/reporter.hpp
    include/hatn/validator/reporting/formatter.hpp
    include/hatn/validator/reporting/member_operand.hpp
    include/hatn/validator/reporting/member_names.hpp
    include/hatn/validator/reporting/strings.hpp
    include/hatn/validator/reporting/translator.hpp
    include/hatn/validator/reporting/translator_repository.hpp
    include/hatn/validator/reporting/no_translator.hpp
    include/hatn/validator/reporting/mapped_translator.hpp
    include/hatn/validator/reporting/operand_formatter.hpp
    include/hatn/validator/reporting/order_and_presentation.hpp
    include/hatn/validator/reporting/report_aggregation.hpp
    include/hatn/validator/reporting/aggregation_strings.hpp
    include/hatn/validator/reporting/single_member_name.hpp
    include/hatn/validator/reporting/nested_member_name.hpp
    include/hatn/validator/reporting/backend_formatter.hpp
    include/hatn/validator/reporting/decorator.hpp
    include/hatn/validator/reporting/quotes_decorator.hpp
    include/hatn/validator/reporting/flag_presets.hpp
    include/hatn/validator/reporting/grammar_categories.hpp
    include/hatn/validator/reporting/concrete_phrase.hpp
    include/hatn/validator/reporting/translate.hpp
    include/hatn/validator/reporting/property_member_name.hpp
    include/hatn/validator/reporting/phrase_translator.hpp
    include/hatn/validator/reporting/reporter_with_object_name.hpp
    include/hatn/validator/reporting/extend_translator.hpp
    include/hatn/validator/reporting/phrase_grammar_cats.hpp
    include/hatn/validator/reporting/prepare_operand_for_formatter.hpp
    include/hatn/validator/reporting/format_bool_operand.hpp
    include/hatn/validator/reporting/format_operand.hpp
    include/hatn/validator/reporting/format_join_grammar_cats.hpp
    include/hatn/validator/reporting/prepare_property_formatting.hpp
    include/hatn/validator/reporting/dotted_member_names.hpp
    include/hatn/validator/reporting/original_member_names.hpp
    include/hatn/validator/reporting/failed_members_reporter.hpp

    include/hatn/validator/reporting/locale/sample_locale.hpp
    include/hatn/validator/reporting/locale/ru.hpp

    include/hatn/validator/prevalidation/set_validated.hpp
    include/hatn/validator/prevalidation/unset_validated.hpp
    include/hatn/validator/prevalidation/resize_validated.hpp
    include/hatn/validator/prevalidation/clear_validated.hpp
    include/hatn/validator/prevalidation/prevalidation_adapter_tag.hpp
    include/hatn/validator/prevalidation/prevalidation_adapter_impl.hpp
    include/hatn/validator/prevalidation/strict_any.hpp
    include/hatn/validator/prevalidation/validate_empty.hpp
    include/hatn/validator/prevalidation/validate_value.hpp
    include/hatn/validator/prevalidation/true_if_empty.hpp
    include/hatn/validator/prevalidation/true_if_size.hpp

    include/hatn/validator/detail/has_method.hpp
    include/hatn/validator/detail/has_property.hpp
    include/hatn/validator/detail/get_impl.hpp
    include/hatn/validator/detail/aggregate_and.hpp
    include/hatn/validator/detail/aggregate_or.hpp
    include/hatn/validator/detail/aggregate_any.hpp
    include/hatn/validator/detail/aggregate_all.hpp
    include/hatn/validator/detail/logical_not.hpp
    include/hatn/validator/detail/dispatcher_impl.hpp
    include/hatn/validator/detail/formatter_fmt.hpp
    include/hatn/validator/detail/reorder_and_present.hpp
    include/hatn/validator/detail/formatter_std.hpp
    include/hatn/validator/detail/backend_formatter_helper.hpp
    include/hatn/validator/detail/hint_helper.hpp
    include/hatn/validator/detail/member_helper.hpp
    include/hatn/validator/detail/member_helper.ipp
)

ADD_CUSTOM_TARGET(headers SOURCES ${HEADERS})

IF (HATN_VALIDATOR_SRC)

    MESSAGE(STATUS "Use cpp-validator library as part of hatn library")

    BUILD_HATN_MODULE()

    INCLUDE(hatn/deps/fmt)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} INTERFACE fmt)

    IF (INSTALL_DEV)
        INCLUDE(hatn/Install)
    ENDIF (INSTALL_DEV)

    TARGET_COMPILE_DEFINITIONS(${PROJECT_NAME} INTERFACE -DHATN_VALIDATOR_FMT)

ELSE (HATN_VALIDATOR_SRC)

    MESSAGE(STATUS "Use standalone cpp-validator library")

    OPTION(VALIDATOR_WITH_TESTS "Build tests for cpp-validator library" OFF)
    OPTION(VALIDATOR_WITH_EXAMPLES "Build examples for cpp-validator library" OFF)

    FIND_PACKAGE(Boost 1.65 REQUIRED)

    OPTION(VALIDATOR_WITH_FMT "Use libfmt backend for formatting" On)

    ADD_LIBRARY(${PROJECT_NAME} INTERFACE)
    TARGET_INCLUDE_DIRECTORIES(${PROJECT_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include ${Boost_INCLUDE_DIR})
    
    IF(WIN32)
        IF(MSVC)
            IF (NOT CMAKE_CXX_STANDARD)
                SET (CMAKE_CXX_STANDARD 17)
            ENDIF()
            IF (CMAKE_CXX_STANDARD LESS 17)
                MESSAGE(FATAL_ERROR "\n*****************************\nOnly C++17 and later standards are supported for MSVC\nSet CMAKE_CXX_STANDARD=17 in build environment\n*****************************\n")
            ENDIF()
            TARGET_COMPILE_OPTIONS(${PROJECT_NAME} INTERFACE /Zc:ternary /wd4996 /Zc:__cplusplus)
        ELSE(MSVC)
            TARGET_COMPILE_DEFINITIONS(${PROJECT_NAME} INTERFACE -D_WIN32_WINNT=0x0600 -Dstrcasecmp=stricmp)
        ENDIF(MSVC)
    ELSE (WIN32)
          TARGET_COMPILE_OPTIONS(${PROJECT_NAME} INTERFACE -fstack-protector-all -fno-omit-frame-pointer)
    ENDIF(WIN32)
    
    IF (VALIDATOR_WITH_FMT)

        MESSAGE(STATUS "Use libfmt for formatting in cpp-validator library")
        TARGET_COMPILE_DEFINITIONS(${PROJECT_NAME} INTERFACE -DHATN_VALIDATOR_FMT)

        IF (NOT FMT_ROOT)
            SET (FMT_ROOT $ENV{FMT_ROOT})
        ENDIF()

        FIND_PATH(FMT_INCLUDE_DIRECTORY fmt/format.h
                PATHS ${FMT_INCLUDE_DIR}
                      ${FMT_ROOT}/include
                DOC "libfmt headers directory"
        )
        IF(NOT FMT_INCLUDE_DIRECTORY)
            MESSAGE(FATAL_ERROR "Could not find libfmt headers: install libfmt (don't forget to set FMT_ROOT if not in system path) or run cmake with -DVALIDATOR_WITH_FMT=No")
        ENDIF()

        MESSAGE(STATUS "Using fmt headers from ${FMT_INCLUDE_DIRECTORY}")
        TARGET_INCLUDE_DIRECTORIES(${PROJECT_NAME} INTERFACE ${FMT_INCLUDE_DIRECTORY})

        IF (NOT FMT_HEADER_ONLY)

            FIND_LIBRARY(FMT_LIBRARY fmt
                PATHS ${FMT_LIB_DIR}
                      ${FMT_ROOT}/lib
                DOC "libfmt library"
            )
            IF(NOT FMT_LIBRARY)
                MESSAGE(FATAL_ERROR "Could not find libfmt library: install libfmt that is not header-only or run cmake with -DFMT_HEADER_ONLY=Yes")
            ENDIF()

            MESSAGE(STATUS "Using fmt library ${FMT_LIBRARY}")
            TARGET_LINK_LIBRARIES(${PROJECT_NAME} INTERFACE ${FMT_LIBRARY})

        ENDIF(NOT FMT_HEADER_ONLY)

    ELSE(VALIDATOR_WITH_FMT)

        MESSAGE(STATUS "Use std::stringstream for formatting in cpp-validator library")

    ENDIF(VALIDATOR_WITH_FMT)

    IF (VALIDATOR_WITH_TESTS)
        MESSAGE(STATUS "Enable building tests for cpp-validator library")
        ENABLE_TESTING(true)
        ADD_SUBDIRECTORY(test)
    ELSE(VALIDATOR_WITH_TESTS)
        MESSAGE(STATUS "Skip building tests for cpp-validator library")
    ENDIF(VALIDATOR_WITH_TESTS)

    IF (VALIDATOR_WITH_EXAMPLES)
        MESSAGE(STATUS "Enable building examples for cpp-validator library")
        ENABLE_TESTING(true)
        ADD_SUBDIRECTORY(examples)
    ELSE (VALIDATOR_WITH_EXAMPLES)
        MESSAGE(STATUS "Skip building examples for cpp-validator library")
    ENDIF(VALIDATOR_WITH_EXAMPLES)

    INSTALL(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include/hatn" DESTINATION include)

ENDIF(HATN_VALIDATOR_SRC)
