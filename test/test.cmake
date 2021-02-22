SET(SOURCES
    ${SOURCES}
    ${VALIDATOR_TEST_SRC}/testvalidatorimpl.cpp
    ${VALIDATOR_TEST_SRC}/testvalidator.cpp
    ${VALIDATOR_TEST_SRC}/testreporter.cpp
    ${VALIDATOR_TEST_SRC}/testreporting.cpp
    ${VALIDATOR_TEST_SRC}/testtranslator.cpp
    ${VALIDATOR_TEST_SRC}/testfmtformatter.cpp
    ${VALIDATOR_TEST_SRC}/teststdformatter.cpp
    ${VALIDATOR_TEST_SRC}/teststrings.cpp
    ${VALIDATOR_TEST_SRC}/testformatter.cpp
    ${VALIDATOR_TEST_SRC}/testsinglemember.cpp
    ${VALIDATOR_TEST_SRC}/testtranslator_ru.cpp
    ${VALIDATOR_TEST_SRC}/testlexicographical.cpp
    ${VALIDATOR_TEST_SRC}/testoperatorin.cpp
    ${VALIDATOR_TEST_SRC}/testregex.cpp
    ${VALIDATOR_TEST_SRC}/teststrnumbers.cpp
    ${VALIDATOR_TEST_SRC}/testvalidate.cpp
    ${VALIDATOR_TEST_SRC}/testsetvalidated.cpp
    ${VALIDATOR_TEST_SRC}/testhabrexamples_ru.cpp
    ${VALIDATOR_TEST_SRC}/testvalidatoronheap.cpp
)

SET (HEADERS
    ${HEADERS}
)

TARGET_SOURCES(${PROJECT_NAME} PUBLIC ${SOURCES})

IF (MINGW)
    # Fix string table overflow when compiling in debug mode
    SET_SOURCE_FILES_PROPERTIES(${VALIDATOR_TEST_SRC}/testreporting.cpp PROPERTIES COMPILE_FLAGS -Og)
    SET_SOURCE_FILES_PROPERTIES(${VALIDATOR_TEST_SRC}/testtranslator_ru.cpp PROPERTIES COMPILE_FLAGS -Og)
    SET_SOURCE_FILES_PROPERTIES(${VALIDATOR_TEST_SRC}/testtranslator.cpp PROPERTIES COMPILE_FLAGS -Og)
    SET_SOURCE_FILES_PROPERTIES(${VALIDATOR_TEST_SRC}/testvalidator.cpp PROPERTIES COMPILE_FLAGS -Og)
    SET_SOURCE_FILES_PROPERTIES(${VALIDATOR_TEST_SRC}/testsinglemember.cpp PROPERTIES COMPILE_FLAGS -Og)
ENDIF (MINGW)

FUNCTION(TestValidator)
ENDFUNCTION(TestValidator)
