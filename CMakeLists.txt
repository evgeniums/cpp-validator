PROJECT(dracoshavalidator)

SET (HEADERS
    include/dracosha/validator/version.hpp
    include/dracosha/validator/config.hpp

    include/dracosha/validator/scalar_compare.hpp
    include/dracosha/validator/adjust_storable_type.hpp
    include/dracosha/validator/property.hpp
    include/dracosha/validator/can_get.hpp
    include/dracosha/validator/get.hpp
    include/dracosha/validator/can_check_contains.hpp
    include/dracosha/validator/contains.hpp
    include/dracosha/validator/check_member.hpp

    include/dracosha/validator/operator.hpp

    include/dracosha/validator/detail/has_method.hpp
    include/dracosha/validator/detail/has_property.hpp
    include/dracosha/validator/detail/get_impl.hpp

    include/dracosha/validator/expression.h
    include/dracosha/validator/operators.h
    include/dracosha/validator/stringoperators.h
    include/dracosha/validator/condition.h
)

SET (SOURCES
)

BUILD_DRACOSHA_MODULE()
