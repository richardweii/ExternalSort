//
// Created by Administrator on 2022/4/22.
//

#ifndef EXTSORT_PAGE_H
#define EXTSORT_PAGE_H

#include "config.h"
#include "optional.h"
#include <memory>
#include <vector>
#include <cstring>

namespace extsort {
namespace io {
class Buffer {
  friend class File;
public:
  Buffer(Buffer &&buffer) noexcept;
  Buffer &operator=(Buffer &&buffer) noexcept;

  Buffer(const Buffer &buffer) = delete;
  Buffer &operator=(const Buffer &buffer) = delete;
  ~Buffer();

  template<class T>
  Optional<T> Read(std::size_t off);

  template<class T>
  Optional<std::vector<T>> Read(std::size_t off, std::size_t num);

  template<class T>
  Optional<std::size_t> Append(const T &obj) { return Write(write_size_, obj); };

  Optional<std::size_t> Append(const char *data, std::size_t length) { return Write(write_size_, data, length); };

  template<class T>
  Optional<std::size_t> Write(std::size_t off, const T &obj);

  Optional<std::size_t> Write(std::size_t off, const char *data, std::size_t length);

  std::size_t Size() const { return write_size_; };

  std::size_t BufferSize() const { return buffer_size_; };

  void Clear() { write_size_ = 0; }

private:
  friend std::shared_ptr<extsort::io::Buffer> CreateBuffer(std::size_t size);
  explicit Buffer(std::size_t size);
  std::size_t buffer_size_;
  std::size_t write_size_;
  char *raw_data_;
};

template<class T>
Optional<T> extsort::io::Buffer::Read(std::size_t off) {
  if (off + sizeof(T) > write_size_) {
    return {};
  }
  return Optional<T>(std::move(*reinterpret_cast<T *>(raw_data_ + off)));
}

template<class T>
Optional<std::vector<T>> extsort::io::Buffer::Read(std::size_t off, std::size_t num) {
  if (off + sizeof(T) * num > write_size_) {
    return {};
  }
  auto buf = raw_data_ + off;
  std::vector<T> res;
  for (std::size_t i = 0; i < num; i++) {
    res.push_back(std::move(*reinterpret_cast<T *>(buf + sizeof(T) * i)));
  }
  return Optional<std::vector<T>>(std::move(res));
}

template<class T>
extsort::Optional<size_t> extsort::io::Buffer::Write(std::size_t off, const T &obj) {
  if (off > write_size_ || off + sizeof(T) > buffer_size_) {
    return {};
  }
  const char *tmp = reinterpret_cast<const char *>(&obj);
  const std::size_t length = sizeof(T);
  std::memcpy(raw_data_ + off, tmp, length);
  write_size_ = write_size_ > length + off ? write_size_ : length + off;
  return Optional<std::size_t>(length + off);
}

std::shared_ptr<extsort::io::Buffer> CreateBuffer(std::size_t size);
} // extsort
} // io

#endif //EXTSORT_PAGE_H
