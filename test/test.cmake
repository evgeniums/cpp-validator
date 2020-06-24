SET(SOURCES
    ${SOURCES}
    ${VALIDATOR_TEST_SRC}/dev.cpp
    ${VALIDATOR_TEST_SRC}/testvalidator.cpp
)

SET (HEADERS
    ${HEADERS}
)

TARGET_SOURCES(${PROJECT_NAME} PUBLIC ${SOURCES})

FUNCTION(TestValidator)
    COPY_LIBRARY_HERE(dracoshacommon${LIB_POSTFIX} ../common/)
    COPY_LIBRARY_HERE(dracoshavalidator${LIB_POSTFIX} ../validator/)
ENDFUNCTION(TestValidator)
