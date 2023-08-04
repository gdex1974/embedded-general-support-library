#pragma once

#include <cstdint>
#include <cstddef>
#include <array>
#include <string_view>

namespace embedded
{

template<typename T>
class MemoryView
{
public:
    typedef T value_type;
    typedef T* pointer;
    typedef typename std::add_const<typename std::remove_const<T>::type>::type* const_pointer;
    typedef T &reference;
    typedef typename std::add_const<typename std::remove_const<T>::type>::type &const_reference;
    typedef pointer iterator;
    typedef const_pointer const_iterator;
    typedef std::size_t size_type;
    typedef std::ptrdiff_t difference_type;

    constexpr MemoryView() : begin_(nullptr), size_(0) {}

    constexpr MemoryView(T* p, uint16_t s) : begin_(p), size_(s) {}

    template<typename C = std::remove_const<T>, std::enable_if_t<
            std::is_same_v<T, std::add_const_t<C>> || std::is_same_v<T, C>, bool> = true>
    constexpr MemoryView(const MemoryView<C> &other) : begin_(other.begin()), size_(other.size()) {}

    template<std::size_t N, typename C = std::remove_const<T>, std::enable_if_t<
            std::is_same_v<T, std::add_const_t<C>> || std::is_same_v<T, C>, bool> = true>
    constexpr MemoryView(std::array<C, N> &ar) : begin_(ar.begin()), size_(ar.size()) {}

    template<std::size_t N>
    constexpr MemoryView(T(&ar)[N]) : begin_(ar), size_(N) {}

    template<typename C = T, std::enable_if_t<std::is_same_v<char, std::remove_const_t<C>>, bool> = true>
    constexpr operator std::string_view() const
    {
        return { begin_, size_ };
    }

    T &operator[](uint16_t i) { return begin_[i]; }

    constexpr T &operator[](uint16_t i) const { return begin_[i]; }

    constexpr auto begin() const { return begin_; }

    constexpr auto end() const { return begin_ + size_; }

    constexpr auto size() const { return size_; }

private:
    T* begin_;
    uint16_t size_;
};

using CharView = MemoryView<char>;
using BytesView = MemoryView<uint8_t>;
using ConstBytesView = MemoryView<const uint8_t>;

}
