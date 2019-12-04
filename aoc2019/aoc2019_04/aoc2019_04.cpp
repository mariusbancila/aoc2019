// aoc2019_04.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <array>
#include <vector>
#include <functional>
#include <assert.h>

using password_t = std::array<int, 6>;

bool operator<(password_t const& p1, password_t const p2)
{
   return
      p1[0] < p2[0] ||
      p1[1] < p2[1] ||
      p1[2] < p2[2] ||
      p1[3] < p2[3] ||
      p1[4] < p2[4] ||
      p1[5] < p2[5];
}

bool meets_criteria(password_t const & value)
{
   return
      value[0] <= value[1] &&
      value[1] <= value[2] &&
      value[2] <= value[3] &&
      value[3] <= value[4] &&
      value[4] <= value[5] && (
         value[0] == value[1] ||
         value[1] == value[2] ||
         value[2] == value[3] ||
         value[3] == value[4] ||
         value[4] == value[5]);
}

bool meets_criteria2(password_t const& value)
{
   if (!(value[0] <= value[1] &&
      value[1] <= value[2] &&
      value[2] <= value[3] &&
      value[3] <= value[4] &&
      value[4] <= value[5])) return false;

   int minconsecutive = 99;
   int last = value[0];
   int consecutive = 1;
   for (int i = 1; i < value.size(); ++i)
   {
      if (value[i] == last)
      {
         consecutive++;
      }
      else
      {
         if (consecutive > 1 && consecutive < minconsecutive)
            minconsecutive = consecutive;
         consecutive = 1;
      }
      last = value[i];
   }

   if (consecutive > 1 && consecutive < minconsecutive)
      minconsecutive = consecutive;

   return minconsecutive == 2;
}

password_t from_int(int value)
{
   std::vector<int> digits;

   while (value > 0)
   {
      digits.push_back(value % 10);
      value = value / 10;
   }

   if (digits.size() != 6)
      throw std::runtime_error("invalid size");

   password_t pwd;
   std::copy_n(std::crbegin(digits), pwd.size(), std::begin(pwd));
   return pwd;
}

int count_total(int const first, int const last, std::function<bool(password_t const&)> predicate)
{
   int total = 0;

   for (int n = first; n <= last; n++)
   {
      if (predicate(from_int(n)))
         total++;
   }

   return total;
}

int main()
{
   assert(meets_criteria({1,1,1,1,1,1}));
   assert(!meets_criteria({2,2,3,4,5,0}));
   assert(!meets_criteria({1,2,3,7,8,9}));

   std::cout << count_total(264793, 803935, &meets_criteria) << '\n';

   assert(meets_criteria2({1,1,2,2,3,3}));
   assert(!meets_criteria2({1,2,3,4,4,4}));
   assert(meets_criteria2({ 1,1,1,1,2,2 }));

   std::cout << count_total(264793, 803935, &meets_criteria2) << '\n';
}
