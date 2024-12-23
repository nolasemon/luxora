#include <luxora/series.h>

namespace Luxora {

SeriesUntyped::SeriesUntyped() {}

SeriesUntyped::SeriesUntyped(std::string name) : name(name) {}

std::string SeriesUntyped::get_name() {
	return name;
}

} // namespace Luxora