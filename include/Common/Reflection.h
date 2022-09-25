#pragma once

namespace Common {

template<typename T>
constexpr bool has_fields_v = requires(T const & t) { t.fields; };

}
