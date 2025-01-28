#include "luxora/dataframe.h"
#include <algorithm>
#include <cstddef>
#include <cstring>
#include <luxora/series.h>
#include <stdexcept>

namespace Luxora {

bool operator==(const SeriesUntyped& lhs, const SeriesUntyped& rhs) {
    if (lhs.type() != rhs.type()) {
        throw std::runtime_error("Cannot compare SeriesUntyped of different types " + type_name(lhs.type()) + ", " +
                                 type_name(rhs.type()));
    }
    if (lhs.size() != rhs.size()) {
        return false;
    }
    const std::byte *a = lhs.data(), *b = rhs.data();
    for (int i = 0; i < lhs.size(); ++i) {
        const std::byte *flag_a = a + lhs.type_size(), *flag_b = b + rhs.type_size();
        if (*flag_a != *flag_b) {
            return false;
        }
        if (*flag_a == std::byte(0)) {
            continue;
        }
        if (std::memcmp(a, b, lhs.type_size()) != 0) {
            return false;
        }
        a += lhs.optional_type_size();
        b += rhs.optional_type_size();
    }
    return true;
}

} // namespace Luxora
