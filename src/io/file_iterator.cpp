//
// Created by Administrator on 2022/4/24.
//
#include "io/file_iterator.h"

namespace extsort {
namespace io {

Optional<FileIterator::BufferPtr> FileIterator::Value() {
  auto buf = CreateBuffer(kBufferSize);
  auto ret = file_->Read(offset_, buf, kBufferSize);
  if (ret.HasValue()) {
    return Optional<BufferPtr>(std::move(buf));
  }
  return {};
}

void FileIterator::Next() {
  offset_ += kBufferSize;
}

bool FileIterator::IsValid() const {
  return offset_ < file_->Size();
}

bool FileIterator::Seek(std::size_t offset) {
  if (offset > file_->Size()) {
    return false;
  }
  offset_ = offset;
  return true;
}

} // io
} // extsort