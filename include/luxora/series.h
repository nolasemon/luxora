#pragma once

#include <algorithm>
#include <cstddef>
#include <iostream>
#include <optional>
#include <string>
#include <vector>

namespace Luxora {

template <typename T>
class Series : public std::vector<std::optional<T>> {
  public:
	T	   mean();
	T	   median();
	size_t count();

	void fill_na(const T& fill);
};

class SeriesUntyped {
	std::string			   name = "";
	std::vector<std::byte> data;
	size_t				   block_size = 0;

  public:
	SeriesUntyped();
	SeriesUntyped(std::string);
	template <typename T>
	SeriesUntyped(std::string, const std::vector<T>&);
	template <typename T>
	SeriesUntyped(std::string, const Series<T>&);

	std::string get_name();

	template <typename T>
	Series<T> typed();

	friend bool operator==(const SeriesUntyped&, const SeriesUntyped&);
};

template <typename T>
T Series<T>::mean() {
	T sum = 0;
	for (std::optional<T>& x : *this) {
		sum += x.value_or(0);
	}
	return sum / count();
}

template <typename T>
T Series<T>::median() {
	std::vector<T> buf(count());
	int			   i = 0;
	for (std::optional<T>& x : *this) {
		if (x.has_value()) {
			buf[i++] = x.value();
		}
	}
	std::sort(buf.begin(), buf.end());
	if (buf.size() % 2 == 1) {
		return buf[buf.size() / 2];
	} else {
		return (buf[buf.size() / 2 - 1] + buf[buf.size() / 2]) / 2;
	}
}

template <typename T>
size_t Series<T>::count() {
	size_t len = 0;
	for (std::optional<T>& x : *this) {
		len += x.has_value();
	}
	return len;
}

template <typename T>
void Series<T>::fill_na(const T& fill) {
	for (std::optional<T>& x : *this) {
		if (!x.has_value()) {
			x = fill;
		}
	}
}

template <typename T>
SeriesUntyped::SeriesUntyped(std::string name, const std::vector<T>& vec)
	: name(name), block_size(sizeof(std::optional<T>)), data(sizeof(std::optional<T>) * vec.size()) {
	for (size_t i = 0; i < vec.size(); ++i) {
		std::optional<T> value = std::optional(vec[i]);
		const std::byte* start = reinterpret_cast<const std::byte*>(&value);
		std::copy(start, start + block_size, data.begin() + i * block_size);
	}
}

template <typename T>
SeriesUntyped::SeriesUntyped(std::string name, const Series<T>& series)
	: name(name), block_size(sizeof(std::optional<T>)), data(sizeof(std::optional<T>) * series.size()) {
	for (size_t i = 0; i < series.size(); ++i) {
		const std::byte* start = reinterpret_cast<const std::byte*>(&series[i]);
		std::copy(start, start + block_size, data.begin() + i * block_size);
	}
}

template <typename T>
Series<T> SeriesUntyped::typed() {
	std::optional<T>* typed_data = reinterpret_cast<std::optional<T>*>(data.data());

	size_t size = data.size() / block_size;

	std::vector<std::optional<T>> vec(typed_data, typed_data + size);
	return Series<T>(vec);
}

} // namespace Luxora