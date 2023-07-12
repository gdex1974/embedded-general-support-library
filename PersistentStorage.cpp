#include "PersistentStorage.h"

embedded::PersistentStorage::PersistentStorage(BytesView storage, bool init)
: buffer_(storage), free_head_(0)
{
    if (init)
    {
        std::memset(buffer_.begin(), 0, buffer_.size());
    }
    else
    {
        while(free_head_ + sizeof(NodeDescriptor) <= buffer_.size()
              && reinterpret_cast<const NodeDescriptor&>(buffer_[free_head_]).size != 0
              && free_head_ + getNodeDescriptor(free_head_).size <= buffer_.size())
        {
            free_head_ += getNodeDescriptor(free_head_).size;
        }
    }

}