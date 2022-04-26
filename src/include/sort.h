//
// Created by Administrator on 2022/4/22.
//

#ifndef EXTSORT_SORT_H
#define EXTSORT_SORT_H

#include "optional.h"
#include "spdlog/spdlog.h"
#include "io/file.h"
#include "io/two_level_iterator.h"
#include "io/buffer.h"
#include <memory>
#include <algorithm>

namespace extsort {

template<class T>
class ExternalSort {
  using FilePtr = std::shared_ptr<io::File>;
  using Compare = std::function<bool(const T &, const T &)>;
  using BufferPtr = std::shared_ptr<io::Buffer>;
public:
  explicit ExternalSort(FilePtr filePtr) : file_(std::move(filePtr)), ways_(2) {}
  ExternalSort(FilePtr filePtr, int ways) : file_(std::move(filePtr)), ways_(ways) {}

  Optional<bool> operator()(Compare &&cmp);
private:
  Optional<bool> PreSort(Compare &cmp);

  Optional<bool> Runs(std::size_t off, int num, Compare &cmp);

  Optional<bool> Pass(std::size_t pages, Compare &cmp);

  bool HasValid(std::vector<io::TwoLevelIterator<T>> &iterators) const {
    for (auto &iter : iterators) {
      if (iter.IsValid()) {
        return true;
      }
    }
    return false;
  }

  FilePtr ReadFile() const {
    return filePos_ == 1 ? tmp_ : file_;
  }

  FilePtr WriteFile() const {
    return filePos_ == 0 ? tmp_ : file_;
  }

  void SwapFile() { filePos_ = 1 - filePos_; }

  int filePos_ = 0;
  FilePtr tmp_ = std::make_shared<io::File>();
  FilePtr file_;
  int ways_;
  BufferPtr writeBuffer_ = io::CreateBuffer(kBufferSize);
};

template<class T>
Optional<bool> ExternalSort<T>::operator()(Compare &&cmp) {
  spdlog::info("PASS# 0, pages pre-sort");
  auto retPreSort = PreSort(cmp);
  if (!retPreSort.HasValue()) {
    spdlog::error("Failed to pre-sort");
    return {};
  }
  std::size_t numPages = 1;
  int i = 1;
  do {
    spdlog::info("PASS# {}, pages {}", i++, numPages);
    auto ret = Pass(numPages, cmp);
    if (!ret.HasValue()) {
      spdlog::error("Failed to sort in pass with numPages {}", numPages);
      return {};
    }
    numPages *= 2;
  } while (numPages * sizeof(T) < ReadFile()->Size());

  assert(file_->Size() == tmp_->Size());
  if (filePos_ == 0) {
    spdlog::info("Copy to file");
    file_->Truncate(0);
    for (auto iter = io::FileIterator(tmp_); iter.IsValid(); iter.Next()) {
      auto bufOpt = iter.Value();
      if (!bufOpt.HasValue()) {
        spdlog::error("Failed to iterate tmp file");
      }
      auto &&buf = bufOpt.Release();
      file_->AppendBuffer(buf, 0, buf->Size());
    }
  }
  return Optional<bool>(true);
}

template<class T>
Optional<bool> ExternalSort<T>::Runs(std::size_t off, int num, Compare &cmp) {
  spdlog::info("--------- Runs off {}, num {} ----------", off, num);
  std::vector<io::TwoLevelIterator<T>> iterators;
  auto readOff = off;
  for (int i = 0; i < ways_; i++) {
    iterators.emplace_back(ReadFile(), readOff, num);
    readOff += num * sizeof(T);
  }

  auto writeOff = off;
  while (HasValid(iterators)) {
    int pos = 0;
    for (int i = 0; i < iterators.size(); i++) {
      if (iterators[i].IsValid()) {
        pos = i;
        break;
      }
    }

    // find one
    T min = iterators[pos].Value().Value();
    int index = pos;
    for (int i = 0; i < iterators.size(); i++) {
      if (i == pos || !iterators[i].IsValid()) {
        continue;
      }
      if (cmp(iterators[i].Value().Value(), min)) {
        auto ret = iterators[i].Value();
        if (!ret.HasValue()) {
          spdlog::error("Fetching value from iterators failed!");
          return {};
        }
        min = ret.Value();
        index = i;
      }
    }
    iterators[index].Next();

    // write
    min.print();
    writeBuffer_->Append(min);
    if (writeBuffer_->Size() == writeBuffer_->BufferSize()) {
      WriteFile()->WriteBuffer(writeOff, writeBuffer_, 0, writeBuffer_->BufferSize());
      writeOff += writeBuffer_->BufferSize();
      writeBuffer_->Clear();
    }
  }
  if (writeBuffer_->Size() != 0) {
    WriteFile()->WriteBuffer(writeOff, writeBuffer_, 0, writeBuffer_->Size());
    writeOff += writeBuffer_->Size();
    writeBuffer_->Clear();
  }
  return Optional<bool>(true);
}

template<class T>
Optional<bool> ExternalSort<T>::Pass(std::size_t pages, Compare &cmp) {
  std::size_t off = 0;
  int num = pages * kBufferSize / sizeof(T);
  do {
    auto ret = Runs(off, num, cmp);
    if (!ret.HasValue()) {
      spdlog::error("Failed to sort in runs with off {}, num {}", off, num);
      return {};
    }
    off += pages * kBufferSize * 2;
  } while (off < ReadFile()->Size());
  SwapFile();
  return Optional<bool>(true);
}

template<class T>
Optional<bool> ExternalSort<T>::PreSort(ExternalSort::Compare &cmp) {
  std::size_t off = 0;
  for (auto fileIter = io::FileIterator(ReadFile()); fileIter.IsValid(); fileIter.Next()) {
    auto bufOpt = fileIter.Value();
    if (!bufOpt.HasValue()) {
      return {};
    }
    auto buf = bufOpt.Value();
    auto rd = buf->Read<T>(0, buf->Size() / sizeof(T));
    if (!rd.HasValue()) {
      return {};
    }
    auto vec = rd.Value();
    std::sort(vec.begin(), vec.end(), cmp);
    buf->Clear();
    for (auto &it : vec) {
      it.print();
      buf->Append(it);
    }
    WriteFile()->WriteBuffer(off, buf, 0, buf->Size());
    off += buf->Size();
  }
  SwapFile();
  return Optional<bool>(true);
}

} // extsort

#endif //EXTSORT_SORT_H
