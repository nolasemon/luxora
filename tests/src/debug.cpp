#include <cstring>
#include <iomanip>
#include <iostream>
#include <vector>

template <typename T>
std::ostream& operator<<(std::ostream& out, const std::vector<T>& v) {
    out << '[';
    for (size_t i = 0; i < v.size(); ++i) {
        out << static_cast<short>(v[i]) << (i + 1 == v.size() ? "" : ", ");
    }
    out << "]";
    return out;
}

struct MyType {
    const char* data;
    MyType(const char* s) : data(s) {}
};

int main() {
    int                minus_one = -1;
    std::string        string    = "Hello world!";
    MyType             mt("Hello mydata!");
    std::optional<int> empty = 1234;
    empty                    = {};
    std::array<char, 5> fixed;
    std::strncpy(fixed.data(), "Hello", 5);
    fixed[5] = 32;
    std::cout << std::quoted(fixed.data()) << std::endl;
    auto                             x     = fixed;
    const std::optional<decltype(x)> opt   = x;
    const std::byte*                 bytes = reinterpret_cast<const std::byte*>(&opt);
    std::vector<std::byte>           value(bytes, bytes + sizeof(x));
    std::vector<std::byte>           flag(bytes + sizeof(x), bytes + sizeof(opt));
    std::cout << value << std::endl;
    std::cout << flag << std::endl;
}
