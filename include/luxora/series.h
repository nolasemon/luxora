#pragma once

#include <algorithm>
#include <cstddef>
#include <initializer_list>
#include <optional>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <typeindex>
#include <vector>

namespace Luxora {

class SeriesUntyped {
  public:
	virtual ~SeriesUntyped()				   = default;
	virtual std::byte*		 data()			   = 0;
	virtual const std::byte* data() const	   = 0;
	virtual size_t			 size() const	   = 0;
	virtual std::type_index	 type() const	   = 0;
	virtual size_t			 type_size() const = 0;

	virtual std::optional<std::string> operator[](size_t) const = 0;
	friend bool operator==(const SeriesUntyped&, const SeriesUntyped&);
};

template <typename T>
class Series : public SeriesUntyped {
	using Element = std::optional<T>;
	using Storage = std::vector<Element>;
	Storage storage;

  public:
	Series(const std::vector<T>&);
	Series(const std::initializer_list<Element>&);

	template <typename U>
	Series(const Series<U>& other) {
		if constexpr (!std::is_same_v<T, U>) {
			throw std::runtime_error("Different types");
		} else {
			storage = other.storage;
		}
	}

	template <typename U>
	Series(const Series<U>&& other) {
		if constexpr (!std::is_same_v<T, U>) {
			throw std::runtime_error("Different types");
		} else {
			storage = std::move(other.storage);
		}
	}

	std::byte* data() override {
		return reinterpret_cast<std::byte*>(storage.data());
	}
	const std::byte* data() const override {
		return reinterpret_cast<const std::byte*>(storage.data());
	}
	size_t size() const override {
		return storage.size();
	}
	std::type_index type() const override {
		return typeid(T);
	}
	size_t type_size() const override {
		return sizeof(T);
	}

	std::optional<std::string> operator[](size_t index) const override {
		if (!storage[index].has_value()) {
			return {};
		}
		if constexpr (std::is_same_v<T, std::string>) {
			return storage[index].value();
		} else {
			return std::to_string(storage[index].value());
		}
	}

	T	   mean() const;
	T	   median() const;
	size_t count() const;

	void identify_na(const T& na);
	void fill_na(const T& fill);
};

template <typename T>
Series<T>::Series(const std::vector<T>& vec) : storage(vec.size()) {
	for (size_t i = 0; i < vec.size(); ++i) {
		storage[i] = std::make_optional(vec[i]);
	}
}

template <typename T>
Series<T>::Series(const std::initializer_list<Element>& init) : storage(init) {}

template <typename T>
T Series<T>::mean() const {
	T sum = 0;
	for (const std::optional<T>& x : storage) {
		sum += x.value_or(0);
	}
	return sum / count();
}

template <typename T>
T Series<T>::median() const {
	std::vector<T> buf(count());
	int			   i = 0;
	for (const std::optional<T>& x : storage) {
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
size_t Series<T>::count() const {
	size_t len = 0;
	for (const std::optional<T>& x : storage) {
		len += x.has_value();
	}
	return len;
}

template <typename T>
void Series<T>::identify_na(const T& na) {
	for (std::optional<T>& x : storage) {
		if (x.has_value() && x.value() == na) {
			x = {};
		}
	}
}

template <typename T>
void Series<T>::fill_na(const T& fill) {
	for (std::optional<T>& x : storage) {
		if (!x.has_value()) {
			x = fill;
		}
	}
}

} // namespace Luxora