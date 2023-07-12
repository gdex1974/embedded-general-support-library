#pragma once

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <string_view>
#include <limits>
#include <vector>
#include <optional>
#include <array>
#include <memory>
#include <cstdint>
#include <array>
#include "MemoryView.h"

namespace embedded
{

template<size_t N>
bool operator==(const std::array<char, N> &lhs, const std::string_view &rhs)
{
    if (lhs.size() < rhs.size())
    {
        return false;
    }
    auto effectiveLength = rhs.size();
    if (std::string_view(lhs.begin(), effectiveLength) != rhs)
    {
        return false;
    }
    for (auto pos = effectiveLength; pos < lhs.size(); ++pos)
    {
        if (lhs[pos] != '\0')
        {
            return false;
        }
    }

    return true;
}

class PersistentStorage
{
    static constexpr std::size_t wrongPos = std::numeric_limits<std::size_t>::max();

public:
    static const size_t DescriptorSize;
    explicit PersistentStorage(BytesView storage, bool init = true);

    template<typename T>
    bool set(std::string_view idx, const T &object)
    {
        if (const auto pos = getIdx<T>(idx); pos != wrongPos)
        {
            auto &node = getNode<T>(pos);
            std::memcpy(static_cast<void*>(&node.data), static_cast<const void*>(&object), sizeof(T));
            return true;
        }
        constexpr auto recordSize = sizeof(Node<T>);
        if (free_head_ + recordSize > buffer_.size())
        {
            // no more free nodes
            return false;
        }
        auto &node = getNode<T>(free_head_);
        for (auto pos = idx.copy(node.idx.data(), node.idx.size()); pos < node.idx.size(); ++pos)
        {
            node.idx[pos] = '\0';
        }
        node.size = recordSize;
        std::memcpy(&node.data, &object, sizeof(T));
        free_head_ += recordSize;
        return true;
    }

    template<typename T>
    std::optional<T> get(std::string_view idx) const
    {
        if (const auto pos = getIdx<T>(idx); pos != wrongPos)
        {
            if (const auto &node = getNode<T>(pos); node.idx == idx && node.size == sizeof(Node<T>))
            {
                return node.data;
            }
        }
        return std::nullopt;
    }

private:
    BytesView buffer_;
    std::uint32_t free_head_;

#pragma pack(push, packed, 1)
    struct NodeDescriptor
    {
        std::uint16_t size;
        std::array<char, 4> idx;
    };

    template<typename T>
    struct Node : NodeDescriptor
    {
        T data;
    };
#pragma pack(pop, packed)

    const NodeDescriptor &getNodeDescriptor(std::size_t index) const
    {
        return *reinterpret_cast<const NodeDescriptor*>(&buffer_[index]);
    }

    template<typename T>
    const Node<T> &getNode(std::size_t index) const
    {
        return *reinterpret_cast<const Node<T>*>(&buffer_[index]);
    }

    template<typename T>
    Node<T> &getNode(std::size_t index)
    {
        return *reinterpret_cast<Node<T>*>(&buffer_[index]);
    }

    template<typename T>
    std::size_t getIdx(std::string_view idx) const
    {
        for (auto pos = 0u; pos + sizeof(NodeDescriptor) <= free_head_;)
        {
            const auto &node = *reinterpret_cast<const Node<T>*>(&buffer_[pos]);
            if (node.idx == idx && node.size == sizeof(Node<T>))
            {
                return pos;
            }
            pos += node.size;
        }
        return wrongPos;
    }
};

} // namespace embedded
