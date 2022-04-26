//
// Created by Administrator on 2022/4/25.
//

#ifndef EXTSORT_SRC_INCLUDE_IO_TWO_LEVEL_ITERATOR_H
#define EXTSORT_SRC_INCLUDE_IO_TWO_LEVEL_ITERATOR_H
#include <memory>
#include "io/buffer.h"
#include "io/buffer_iterator.h"
#include "io/file_iterator.h"
namespace extsort {
namespace io {

template<class T>
class TwoLevelIterator {
  using BufferIter = BufferIterator<T>;
  using BufferPtr = std::shared_ptr<Buffer>;
  using FilePtr = std::shared_ptr<File>;
public:
  explicit TwoLevelIterator(FilePtr filePtr) : TwoLevelIterator(std::move(filePtr), 0) {}

  TwoLevelIterator(FilePtr filePtr, std::size_t off) : fileIter_(std::move(filePtr), off) {
    valid_ = NextBuffer();
  }

  TwoLevelIterator(FilePtr filePtr, std::size_t off, int limit) : fileIter_(std::move(filePtr), off), limit_(limit) {
    valid_ = NextBuffer();
  }

  Optional<T> Value() {
    if (valid_) {
      return bufferIter_.Value();
    }
    return {};
  }

  void Next() {
    if (valid_) {
      bufferIter_.Next();
      cnt_++;
      if (!bufferIter_.IsValid()) {
        valid_ = NextBuffer();
      }
    }
  }

  bool IsValid() const {
    if (limit_ == -1) {
      return valid_;
    }
    return valid_ && cnt_ < limit_;
  }

  int Count() const {
    return cnt_;
  }
  
private:
  bool NextBuffer() {
    if (fileIter_.IsValid()) {
      auto buf = fileIter_.Value().Release();
      bufferIter_ = BufferIterator<T>(buf);
      fileIter_.Next();
      return true;
    }
    return false;
  }

  FileIterator fileIter_;
  BufferIter bufferIter_;
  bool valid_;
  int cnt_{0};
  int limit_ = -1;
};

}
}
#endif //EXTSORT_SRC_INCLUDE_IO_TWO_LEVEL_ITERATOR_H
