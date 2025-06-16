#pragma once

// component reflection macros for generic `ComponentEditor`

#define REGISTER_COMPONENT_TABLE_HEADER_FIELDS(...) \
    static constexpr auto reflect_table_header_fields() { return std::make_tuple(__VA_ARGS__); }

#define REGISTER_COMPONENT_FIELDS(...) \
    static constexpr auto reflect_edit_fields() { return std::make_tuple(__VA_ARGS__); }
