#include <luxora/series.h>

namespace Luxora {

SeriesUntyped::SeriesUntyped() {}

SeriesUntyped::SeriesUntyped(std::string name) : name(name) {}

std::string SeriesUntyped::get_name() {
	return name;
}

bool operator==(const SeriesUntyped& lhs, const SeriesUntyped& rhs) {
	return lhs.data == rhs.data;
}

} // namespace Luxora