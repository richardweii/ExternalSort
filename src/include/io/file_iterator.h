//
// Created by Administrator on 2022/4/24.
//

#ifndef EXTSORT_SRC_INCLUDE_IO_FILE_ITERATOR_H
#define EXTSORT_SRC_INCLUDE_IO_FILE_ITERATOR_H

#include "io/buffer.h"
#include "io/file.h"

namespace extsort {
namespace io {

class FileIterator {
  using BufferPtr = std::shared_ptr<Buffer>;
  using FilePtr = std::shared_ptr<File>;
public:
  explicit FileIterator(FilePtr ptr) : offset_(0), file_(std::move(ptr)) {}
  FileIterator(FilePtr ptr, std::size_t off) : offset_(off), file_(std::move(ptr)) {}

  Optional<BufferPtr> Value();

  void Next();

  bool Seek(std::size_t offset);

  bool IsValid() const;
private:
  std::size_t offset_;
  FilePtr file_;
};

} // io
} // extsort
#endif //EXTSORT_SRC_INCLUDE_IO_FILE_ITERATOR_H
