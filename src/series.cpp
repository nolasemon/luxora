#include <cstring>
#include <luxora/series.h>
#include <stdexcept>

namespace Luxora {

bool operator==(const SeriesUntyped& lhs, const SeriesUntyped& rhs) {
	if (lhs.type() != rhs.type()) {
		throw std::runtime_error("Mismatched types");
	}
	if (lhs.size() != rhs.size()) {
		return false;
	}
	return std::memcmp(lhs.data(), rhs.data(), lhs.size() * lhs.type_size());
}

} // namespace Luxora
