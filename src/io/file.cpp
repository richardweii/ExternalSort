//
// Created by Administrator on 2022/4/24.
//
#include "io/file.h"
#include <cstring>
#include <cassert>

namespace extsort {
namespace io {

Optional<std::size_t> File::Write(std::size_t destOff, const char *src, std::size_t length) {
  if (destOff > file_size_) {
    return {};
  }

  while (destOff + length > cap_) {
    AllocBuffer();
  }
  auto offsetOld = destOff;
  auto remaining = length;
  while (remaining > 0) {
    auto pair = GetBuffer(destOff);
    auto &buf = pair.first;
    auto bufOffset = pair.second;
    auto writeLength = buf->BufferSize() - bufOffset > remaining ? remaining : buf->BufferSize() - bufOffset;
    auto ret = buf->Write(bufOffset, src, writeLength);
    assert(ret.HasValue());
    assert(ret.Value() == writeLength);
    remaining -= writeLength;
    src += writeLength;
    destOff += writeLength;
  }
  assert(remaining == 0);
  file_size_ = file_size_ > offsetOld + length ? file_size_ : offsetOld + length;
  return Optional<std::size_t>(length);
}

Optional<std::size_t> File::Read(std::size_t offset, std::shared_ptr<Buffer> buffer, std::size_t length) {
  if (offset > file_size_) {
    return {};
  }
  // get total read length
  auto remaining = length > buffer->BufferSize() ? buffer->BufferSize() : length;
  remaining = remaining > file_size_ - offset ? file_size_ - offset : remaining;

  std::size_t writeOffset = 0;
  while (remaining > 0) {
    auto pair = GetBuffer(offset);
    auto buf = pair.first;
    auto bufOffset = pair.second;
    auto readLen = buf->Size() - bufOffset > remaining ? remaining : buf->Size() - bufOffset;
    auto ret = buffer->Write(writeOffset, buf->raw_data_ + bufOffset, readLen);
    assert(ret.HasValue());
    assert(ret.Value() == readLen);
    remaining -= readLen;
    offset += readLen;
    writeOffset += readLen;
  }
  return Optional<std::size_t>(writeOffset);
}

void File::AllocBuffer() {
  buffers_.emplace_back(CreateBuffer(kBufferSize));
  cap_ += kBufferSize;
}

File::File() : file_size_(0), cap_(kBufferSize) {
  buffers_.emplace_back(CreateBuffer(kBufferSize)); // initially allocate one buffer
}

Optional<std::size_t> File::WriteBuffer(std::size_t destOff,
                                        std::shared_ptr<Buffer> buffer,
                                        std::size_t bufferOff,
                                        std::size_t length) {
  if (bufferOff + length > buffer->BufferSize()) {
    return Write(destOff, buffer->raw_data_ + bufferOff, buffer->BufferSize() - bufferOff);
  }
  return Write(destOff, buffer->raw_data_ + bufferOff, length);
}

std::pair<std::shared_ptr<Buffer>, std::size_t> File::GetBuffer(std::size_t offset) {
  std::size_t pos = offset / kBufferSize;
  return {buffers_[pos], offset % kBufferSize};
}

} // io
} // extsort