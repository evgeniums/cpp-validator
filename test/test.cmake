SET(SOURCES
    ${SOURCES}
    ${VALIDATOR_TEST_SRC}/testvalidatorimpl.cpp
    ${VALIDATOR_TEST_SRC}/testreporting.cpp
    ${VALIDATOR_TEST_SRC}/testtranslator.cpp
    ${VALIDATOR_TEST_SRC}/testfmtformatter.cpp
    ${VALIDATOR_TEST_SRC}/teststdformatter.cpp
    ${VALIDATOR_TEST_SRC}/teststrings.cpp
)

SET (HEADERS
    ${HEADERS}
    ${VALIDATOR_TEST_SRC}/testformatter.ipp
)

TARGET_SOURCES(${PROJECT_NAME} PUBLIC ${SOURCES})

FUNCTION(TestValidator)
    COPY_LIBRARY_HERE(dracoshacommon${LIB_POSTFIX} ../common/)
ENDFUNCTION(TestValidator)
