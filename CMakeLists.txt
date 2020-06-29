PROJECT(dracoshavalidator)

SET (HEADERS
    include/dracosha/validator/version.hpp
    include/dracosha/validator/config.hpp

    include/dracosha/validator/tuple_to_variadic.hpp
    include/dracosha/validator/scalar_compare.hpp
    include/dracosha/validator/adjust_storable_type.hpp

    include/dracosha/validator/property.hpp
    include/dracosha/validator/can_get.hpp
    include/dracosha/validator/get.hpp
    include/dracosha/validator/can_check_contains.hpp
    include/dracosha/validator/contains.hpp
    include/dracosha/validator/check_member.hpp
    include/dracosha/validator/check_exists.hpp
    include/dracosha/validator/lazy.hpp
    include/dracosha/validator/extract.hpp
    include/dracosha/validator/get_member.hpp
    include/dracosha/validator/validate.hpp
    include/dracosha/validator/prepare_dispatcher.hpp
    include/dracosha/validator/apply.hpp
    include/dracosha/validator/member.hpp
    include/dracosha/validator/master_reference.hpp
    include/dracosha/validator/validator.hpp
    include/dracosha/validator/make_validator.hpp
    include/dracosha/validator/dispatcher.hpp
    include/dracosha/validator/check_member_path.hpp

    include/dracosha/validator/operators/operator.hpp
    include/dracosha/validator/operators/exists.hpp
    include/dracosha/validator/operators/comparison.hpp
    include/dracosha/validator/operators/and.hpp
    include/dracosha/validator/operators/or.hpp
    include/dracosha/validator/operators/not.hpp

    include/dracosha/validator/properties/value.hpp
    include/dracosha/validator/properties/size.hpp
    include/dracosha/validator/properties/empty.hpp

    include/dracosha/validator/adapters/adapter.hpp

    include/dracosha/validator/detail/has_method.hpp
    include/dracosha/validator/detail/has_property.hpp
    include/dracosha/validator/detail/get_impl.hpp
    include/dracosha/validator/detail/validator_impl.hpp
    include/dracosha/validator/detail/aggregate_and.hpp
    include/dracosha/validator/detail/aggregate_or.hpp
    include/dracosha/validator/detail/dispatcher_impl.hpp
    include/dracosha/validator/detail/take_address_of.hpp
    include/dracosha/validator/detail/logical_not.hpp

    include/dracosha/validator/impl.hpp
)

SET (SOURCES
)

BUILD_DRACOSHA_MODULE()
