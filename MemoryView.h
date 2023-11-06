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
    typedef T element_type;
    typedef std::remove_cv_t<T> value_type;
    typedef T* pointer;
    typedef typename std::add_const<std::remove_const_t<T>>::type* const_pointer;
    typedef T &reference;
    typedef typename std::add_const<std::remove_const_t<T>>::type &const_reference;
    typedef pointer iterator;
    typedef const_pointer const_iterator;
    typedef std::reverse_iterator<iterator> reverse_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
    typedef std::size_t size_type;
    typedef std::ptrdiff_t difference_type;
    static constexpr size_type npos = static_cast<size_type>(-1);

    constexpr MemoryView() : begin_(nullptr), size_(0) {}
    constexpr MemoryView(T* p, size_type s) : begin_(p), size_(s) {}
    constexpr MemoryView(T* begin, T* end) : begin_(begin), size_(end - begin) {}

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

    constexpr reference operator[](size_type i) const { return begin_[i]; }
    constexpr iterator begin() const { return begin_; }
    constexpr iterator end() const { return begin_ + size_; }
    constexpr const_iterator cbegin() const { return begin_; }
    constexpr const_iterator cend() const { return begin_ + size_; }
    constexpr reverse_iterator rbegin() const { return reverse_iterator(end()); }
    constexpr reverse_iterator rend() const { return reverse_iterator(begin()); }
    constexpr const_reverse_iterator crbegin() const { return const_reverse_iterator(end()); }
    constexpr const_reverse_iterator crend() const { return const_reverse_iterator(begin()); }
    constexpr auto size() const { return size_; }
    constexpr size_type size_bytes() const noexcept { return size_ * sizeof(T); }
    constexpr pointer data() const noexcept { return begin_; }
    constexpr bool empty() const noexcept { return size_ == 0; }
    constexpr reference front() const { return *begin_; }
    constexpr reference back() const { return *(begin_ + size_ - 1); }
    constexpr MemoryView<T> subspan(size_type offset, size_type count = npos) const
    {
        if (offset >= size_)
            return {};
        if (count == npos)
            return MemoryView<T>(begin_ + offset, size_ - offset);
        return MemoryView<T>(begin_ + offset, std::min(size_ - offset, count));
    }
    constexpr MemoryView<T> first(size_type count) const
    {
        if (count >= size_)
            return *this;
        return MemoryView<T>(begin_, count);
    }
    constexpr MemoryView<T> last(size_type count) const
    {
        if (count >= size_)
            return *this;
        return MemoryView<T>(begin_ + size_ - count, count);
    }
    constexpr MemoryView<const uint8_t> as_bytes() const noexcept
    {
        return { reinterpret_cast<const uint8_t*>(begin_), size_bytes() };
    }

    template<typename = typename std::is_const<T>::type>
    constexpr MemoryView<uint8_t> as_writable_bytes() const noexcept
    {
        return { reinterpret_cast<uint8_t*>(begin_), size_bytes() };
    }

private:
    T* begin_;
    size_type size_;
};

using CharView = MemoryView<char>;
using BytesView = MemoryView<uint8_t>;
using ConstBytesView = MemoryView<const uint8_t>;

}
