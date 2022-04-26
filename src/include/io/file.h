//
// Created by Administrator on 2022/4/24.
//

#ifndef EXTSORT_SRC_INCLUDE_IO_FILE_H
#define EXTSORT_SRC_INCLUDE_IO_FILE_H
#include <utility>
#include <vector>
#include <memory>
#include "buffer.h"
#include "optional.h"

namespace extsort {
namespace io {
class File {
public:
  File();
  Optional<std::size_t> Read(std::size_t offset, std::shared_ptr<Buffer> buffer, std::size_t length);

  Optional<std::size_t> Write(std::size_t destOff, const char *src, std::size_t length);

  Optional<std::size_t> WriteBuffer(std::size_t destOff,
                                    std::shared_ptr<Buffer> buffer,
                                    std::size_t bufferOff,
                                    std::size_t length);

  Optional<std::size_t> Append(const char *src, std::size_t length) {
    return Write(file_size_, src, length);
  };

  Optional<std::size_t> AppendBuffer(std::shared_ptr<Buffer> buffer,
                                     std::size_t bufferOff,
                                     std::size_t length) {
    return WriteBuffer(file_size_, std::move(buffer), bufferOff, length);
  }

  std::size_t Size() const { return file_size_; };

  void Truncate(std::size_t off) {
    file_size_ = off;
    std::size_t pos = file_size_ / kBufferSize + 1;
    cap_ = pos * kBufferSize;
    buffers_.erase(buffers_.begin() + pos, buffers_.end());
  };
private:
  void AllocBuffer();

  std::pair<std::shared_ptr<Buffer>, std::size_t> GetBuffer(std::size_t offset);

  std::vector<std::shared_ptr<Buffer>> buffers_;
  std::size_t file_size_;
  std::size_t cap_;
};

} // io
} // extsort
#endif //EXTSORT_SRC_INCLUDE_IO_FILE_H
