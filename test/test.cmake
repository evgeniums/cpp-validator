SET (VALIDATOR_TEST_SOURCES
    ${VALIDATOR_TEST_SRC}/testvalidator.cpp
    ${VALIDATOR_TEST_SRC}/testvalidatorimpl.cpp
    ${VALIDATOR_TEST_SRC}/testreporter.cpp
    ${VALIDATOR_TEST_SRC}/testreporting.cpp
    ${VALIDATOR_TEST_SRC}/testallanyreport.cpp
    ${VALIDATOR_TEST_SRC}/testreportinghints.cpp
    ${VALIDATOR_TEST_SRC}/testnomemberreporting.cpp
    ${VALIDATOR_TEST_SRC}/testtranslator.cpp
    ${VALIDATOR_TEST_SRC}/testfmtformatter.cpp
    ${VALIDATOR_TEST_SRC}/teststdformatter.cpp
    ${VALIDATOR_TEST_SRC}/teststrings.cpp
    ${VALIDATOR_TEST_SRC}/testformatter.cpp
    ${VALIDATOR_TEST_SRC}/testtranslator_ru.cpp
    ${VALIDATOR_TEST_SRC}/testlexicographical.cpp
    ${VALIDATOR_TEST_SRC}/testoperatorin.cpp
    ${VALIDATOR_TEST_SRC}/testregex.cpp
    ${VALIDATOR_TEST_SRC}/teststrnumbers.cpp
    ${VALIDATOR_TEST_SRC}/testhabrexamples_ru.cpp
    ${VALIDATOR_TEST_SRC}/testvalidatoronheap.cpp
    ${VALIDATOR_TEST_SRC}/testinvokeandor.cpp
    ${VALIDATOR_TEST_SRC}/testprevalidation.cpp
    ${VALIDATOR_TEST_SRC}/testvalidate.cpp
    ${VALIDATOR_TEST_SRC}/testsetvalidated.cpp
    ${VALIDATOR_TEST_SRC}/testunsetvalidated.cpp
    ${VALIDATOR_TEST_SRC}/testresizevalidated.cpp
    ${VALIDATOR_TEST_SRC}/testclearvalidated.cpp
    ${VALIDATOR_TEST_SRC}/testnestedallany.cpp
    ${VALIDATOR_TEST_SRC}/testpartialpaths.cpp
    ${VALIDATOR_TEST_SRC}/testnestedvalidators.cpp
    ${VALIDATOR_TEST_SRC}/testvariadicproperty.cpp
    ${VALIDATOR_TEST_SRC}/testoperatorexists.cpp
    ${VALIDATOR_TEST_SRC}/testheterogeneouscontainers.cpp
    ${VALIDATOR_TEST_SRC}/testvaluetransformer.cpp
    ${VALIDATOR_TEST_SRC}/testtree.cpp
    ${VALIDATOR_TEST_SRC}/testpointers.cpp
)

TARGET_SOURCES(${PROJECT_NAME} PUBLIC ${VALIDATOR_TEST_SOURCES})

IF (MINGW)
    # Fix string table overflow when compiling in debug mode
    SET_SOURCE_FILES_PROPERTIES(${VALIDATOR_TEST_SOURCES} PROPERTIES COMPILE_FLAGS -Og)
ENDIF (MINGW)

FUNCTION(TestValidator)
ENDFUNCTION(TestValidator)
