//
// Created by Administrator on 2022/4/23.
//

#ifndef EXTSORT_SRC_INCLUDE_IO_BUFFER_ITERATOR_H
#define EXTSORT_SRC_INCLUDE_IO_BUFFER_ITERATOR_H

#include "io/buffer.h"
#include <memory>

namespace extsort {
namespace io {

template<class T>
class BufferIterator {
  using BufferPtr = std::shared_ptr<Buffer>;
public:
  explicit BufferIterator(BufferPtr ptr) : ptr_(std::move(ptr)), offset_(0), empty_(false) {}

  BufferIterator() : ptr_(nullptr), offset_(0), empty_(true) {}

  Optional <T> Value();

  void Next();

  bool IsValid() const;

private:
  bool empty_;
  BufferPtr ptr_;
  std::size_t offset_;
};

template<class T>
Optional<T> BufferIterator<T>::Value() {
  if (empty_) {
    return {};
  }
  return ptr_->Read<T>(offset_);
}

template<class T>
void BufferIterator<T>::Next() {
  if (empty_) {
    return;
  }
  offset_ += sizeof(T);
}

template<class T>
bool BufferIterator<T>::IsValid() const {
  if (empty_) {
    return false;
  }
  return offset_ + sizeof(T) <= ptr_->Size();
}

} // io
} // extsort

#endif //EXTSORT_SRC_INCLUDE_IO_BUFFER_ITERATOR_H
