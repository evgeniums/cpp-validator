PROJECT(dracoshavalidator-examples)

SET(SOURCES
    example01_validate.cpp
    example02_validate_with_report.cpp
    example03_validate_with_exception.cpp
    example04_apply_validator.cpp
    example05_dynamic_validator_allocation.cpp
    example06_nested_validators.cpp
    example07_nested_members.cpp
    example08_member_existence.cpp
    example09_custom_property.cpp
    example10_custom_property_flag.cpp
    example11_heterogeneous_property_implicit.cpp
    example12_heterogeneous_property_explicit.cpp
)

ENABLE_TESTING(true)
FOREACH(SRC ${SOURCES} )
    STRING(REPLACE ".cpp" "" EXEC_NAME ${SRC})
    ADD_EXECUTABLE(${EXEC_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/${SRC})
    TARGET_LINK_LIBRARIES(${EXEC_NAME} dracoshavalidator)

    ADD_TEST(NAME ${EXEC_NAME} COMMAND ${EXEC_NAME})
    SET_TESTS_PROPERTIES(${EXEC_NAME} PROPERTIES LABELS "examples")
ENDFOREACH()
