//
// Created by Administrator on 2022/4/23.
//

#ifndef EXTSORT_SRC_INCLUDE_UTIL_OPTIONAL_H
#define EXTSORT_SRC_INCLUDE_UTIL_OPTIONAL_H
#include <type_traits>
#include <utility>
#include <string>
#include <stdexcept>

namespace extsort {

template<class T>
class Optional {
  using data_t = typename std::aligned_storage<sizeof(T), std::alignment_of<T>::value>::type;
public:
  ~Optional() {
    if (hasValue_) {
      hasValue_ = false;
      reinterpret_cast<T *>(&data_)->~T();
    }
  }

  Optional() noexcept: hasValue_(false) {}

  Optional(Optional &&opt) noexcept: hasValue_(true) {
    new(&data_) T(std::move(*reinterpret_cast<T *>(&opt.data_)));
    opt.hasValue_ = false;
  };

  Optional &operator=(Optional &&opt) noexcept {
    hasValue_ = opt.hasValue_;
    new(&data_) T(std::move(*reinterpret_cast<T *>(&opt.data_)));
    opt.hasValue_ = false;
    return *this;
  }

  explicit Optional(T &&t) noexcept: hasValue_(true) {
    new(&data_) T(std::forward<T &&>(t));
  };

  explicit Optional(const T &t) noexcept: hasValue_(true) {
    new(&data_) T(t);
  };

  template<typename ...Args>
  explicit Optional(Args &&...args) noexcept:hasValue_(true) {
    new(&data_) T(std::forward<Args &&>(args)...);
  }

  Optional(const Optional &opt) = delete;
  Optional &operator=(const Optional &opt) = delete;

  T &&Release() {
    if (hasValue_) {
      hasValue_ = false;
      return std::move(*reinterpret_cast<T *>(&data_));
    }
    throw std::runtime_error("Try to get invalid value!");
  }

  T &Value() {
    if (hasValue_) {
      return *reinterpret_cast<T *>(&data_);
    }
    throw std::runtime_error("Try to get invalid value!");
  }

  bool HasValue() const {
    return hasValue_;
  }

private:
  bool hasValue_;
  data_t data_;
};
} // extsort
#endif //EXTSORT_SRC_INCLUDE_UTIL_OPTIONAL_H
