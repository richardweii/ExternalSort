//
// Created by Administrator on 2022/4/23.
//
#include "io/buffer.h"

extsort::io::Buffer::Buffer(std::size_t size) : buffer_size_(size), write_size_(0), raw_data_(new char[size]) {
}

extsort::io::Buffer::Buffer(extsort::io::Buffer &&buffer) noexcept {
  this->raw_data_ = buffer.raw_data_;
  this->buffer_size_ = buffer.buffer_size_;
  this->write_size_ = buffer.write_size_;
}

extsort::io::Buffer &extsort::io::Buffer::operator=(extsort::io::Buffer &&buffer) noexcept {
  delete[] this->raw_data_;
  this->raw_data_ = buffer.raw_data_;
  this->buffer_size_ = buffer.buffer_size_;
  return *this;
}

extsort::io::Buffer::~Buffer() {
  delete[] raw_data_;
}


extsort::Optional<std::size_t> extsort::io::Buffer::Write(std::size_t off, const char *data, std::size_t length) {
  if (off > write_size_ || off + length > buffer_size_) {
    return {};
  }
  std::memcpy(raw_data_ + off, data, length);
  write_size_ = write_size_ > off + length ? write_size_ : off + length;
  return Optional<std::size_t>(length);
}

std::shared_ptr<extsort::io::Buffer> extsort::io::CreateBuffer(std::size_t size) {
  return std::shared_ptr<extsort::io::Buffer>(new extsort::io::Buffer(size));
}