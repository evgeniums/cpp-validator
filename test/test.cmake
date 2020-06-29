SET(SOURCES
    ${SOURCES}
    ${VALIDATOR_TEST_SRC}/testvalidatorimpl.cpp
)

SET (HEADERS
    ${HEADERS}
)

TARGET_SOURCES(${PROJECT_NAME} PUBLIC ${SOURCES})

FUNCTION(TestValidator)
    COPY_LIBRARY_HERE(dracoshacommon${LIB_POSTFIX} ../common/)
ENDFUNCTION(TestValidator)
