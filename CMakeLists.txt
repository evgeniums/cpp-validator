PROJECT(dracoshavalidator)

SET (HEADERS
    include/dracosha/validator/version.hpp
    include/dracosha/validator/config.hpp

    include/dracosha/validator/scalar_compare.hpp
    include/dracosha/validator/adjust_storable_type.hpp

    include/dracosha/validator/operator.hpp

    include/dracosha/validator/detail/check_has_method.hpp

    include/dracosha/validator/expression.h
    include/dracosha/validator/operators.h
    include/dracosha/validator/stringoperators.h
    include/dracosha/validator/condition.h
)

SET (SOURCES
)

BUILD_DRACOSHA_MODULE()
