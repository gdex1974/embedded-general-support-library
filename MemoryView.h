#pragma once

#include <cstdint>
#include <cstddef>
#include <array>

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

    MemoryView() : begin_(nullptr), size_(0) {}
    MemoryView(T* p, uint16_t s) : begin_(p), size_(s) {}

    template<std::size_t N>
    MemoryView(std::array<T, N> &ar) : begin_(ar.begin()), size_(ar.size()) {}

    template<typename C = T, std::enable_if_t<std::is_same_v<char, std::remove_const_t<C>>, bool> = true>
    operator std::string_view() const
    {
        return { begin_, size_ };
    }

    T &operator[](uint16_t i) { return begin_[i]; }
    const T &operator[](uint16_t i) const { return begin_[i]; }

    auto begin() const { return begin_; }
    auto end() const { return begin_ + size_; }
    auto size() const { return size_; }

private:
    T* begin_;
    uint16_t size_;
};

using CharView = MemoryView<char>;
using BytesView = MemoryView<uint8_t>;
using ConstBytesView = MemoryView<const uint8_t>;

}
