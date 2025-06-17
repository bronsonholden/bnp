#pragma once

// component reflection macros for generic `ComponentEditor`

#define REGISTER_COMPONENT_TABLE_HEADER_FIELDS(...) \
    static constexpr auto reflect_table_header_fields() { return std::make_tuple(__VA_ARGS__); }

#define REGISTER_COMPONENT_FIELDS(...) \
    static constexpr auto reflect_edit_fields() { return std::make_tuple(__VA_ARGS__); }

namespace bnp {
namespace Reflection {

template <typename T>
constexpr bool is_effectively_uint32 = std::is_same_v<std::remove_cv_t<std::remove_reference_t<std::decay_t<T>>>, uint32_t>;

}
}
