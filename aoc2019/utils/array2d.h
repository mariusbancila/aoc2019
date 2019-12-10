#pragma once

#include <array>

template <class T, std::size_t R, std::size_t C>
class array2d
{
public:
   using value_type = T;
   using size_type = std::size_t;
   using difference_type = std::ptrdiff_t;
   using reference = value_type&;
   using const_reference = const value_type&;
   using pointer = value_type*;
   using const_pointer = const value_type*;

public:
   constexpr std::size_t Rows() noexcept { return R; }
   constexpr std::size_t Columns() noexcept { return C; }

   constexpr T* data() noexcept { return data_.data(); }
   constexpr const T* data() const noexcept { return data_.data(); }

   [[nodiscard]] constexpr bool empty() const noexcept { return R > 0 && C > 0; }

   constexpr reference at(size_type const r, size_type const c) { return data_.at(r*C + c); }
   constexpr const_reference at(size_type const r, size_type const c) const { return data_.at(r * C + c); }

   constexpr reference operator()(size_type const r, size_type const c) { return data_[r * C + c]; }
   constexpr const_reference operator()(size_type const r, size_type const c) const { return data_[r * C + c]; }

   template <class Iterator, class T, std::size_t R, std::size_t C>
   friend static array2d<T, R, C> make_array2d(Iterator begin, Iterator end);

   template <class Iterator, class T, std::size_t R, std::size_t C>
   friend static array2d<T, R, C> make_array2d(std::initializer_list<T> data);

private:
   std::array<T, R * C> data_;
};

template <class Iterator, class T, std::size_t R, std::size_t C>
static array2d<T, R, C> make_array2d(Iterator begin, Iterator end)
{
   array2d<T, R, C> arr;
   int const size = R * C;
   int index = 0;
   int row = 0;
   int col = 0;
   while (begin != end && row < R && col < C)
   {
      arr.data_[row * C + col] = *(begin++);
      col++; if (col == C) { row++; col = 0; }
   }
   return arr;
}

template <class Iterator, class T, std::size_t R, std::size_t C>
static array2d<T, R, C> make_array2d(std::initializer_list<T> data)
{
   return make_array2d(std::cbegin(data), std::cend(data));
}