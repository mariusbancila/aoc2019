// aoc2019_22.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <numeric>
#include "absl/numeric/int128.h"
#include <assert.h>

using cardstack_t = std::vector<int>;

void deal(cardstack_t& stack)
{
   std::reverse(stack.begin(), stack.end());
}

void deal(cardstack_t& stack, int const increment)
{
   assert(increment >= 0 && increment < stack.size());

   if (increment == 0)
   {
      deal(stack);
   }
   else
   {
      cardstack_t newstack(stack.size());
      size_t j = 0;
      for (size_t i = 0; i < stack.size(); ++i)
      {
         newstack[j] = stack[i];
         j += increment;
         if (j >= stack.size()) j = j % stack.size();
      }

      stack.swap(newstack);
   }
}

void cut(cardstack_t& stack, int const n)
{
   assert(n > 0 ? n < stack.size() : -n < stack.size());

   if (n > 0)
   {
      std::rotate(stack.begin(), stack.begin() + n, stack.end());
   }
   else
   {
      std::rotate(stack.begin(), stack.begin() + (stack.size() + n), stack.end());
   }
}

enum class action_type { deal, cut };

struct action_t
{
   action_type action;
   int         value;
};

using action_collection = std::vector<action_t>;

action_collection read_actions(std::string const& filename)
{
   std::ifstream file(filename);
   if (!file.is_open()) throw std::runtime_error("cannot open file " + filename);

   action_collection actions;
   std::string line;
   while (std::getline(file, line))
   {
      if (line == "deal into new stack")
      {
         actions.push_back({ action_type::deal, 0 });
      }
      else if (line.find("deal with increment ") == 0)
      {
         auto str = line.substr(std::strlen("deal with increment "));
         actions.push_back({action_type::deal, std::stoi(str)});
      }
      else if (line.find("cut ") == 0)
      {
         auto str = line.substr(std::strlen("cut "));
         actions.push_back({ action_type::cut, std::stoi(str) });
      }
   }

   return actions;
}

void apply_actions(cardstack_t& stack, action_collection const& actions)
{
   for (auto const& action : actions)
   {
      switch (action.action)
      {
      case action_type::cut:
         cut(stack, action.value);
         break;
      case action_type::deal:
         deal(stack, action.value);
         break;
      }
   }
}

int index_of(cardstack_t const& stack, int const card)
{
   int pos = -1;
   for (size_t i = 0; i < stack.size(); ++i)
   {
      if (stack[i] == card)
      {
         pos = static_cast<int>(i);
         break;
      }
   }
   return pos;
}

int apply_actions(action_collection const& actions, int const size, int const card)
{
   int pos = card;

   for (auto const& action : actions)
   {
      switch (action.action)
      {
      case action_type::cut:
         pos = (pos - action.value) % size;
         break;
      case action_type::deal:
         if (action.value == 0)
            pos = size - 1 - pos;
         else
            pos = (pos * action.value) % size;
         break;
      }
   }

   return pos;
}

using int128_t = absl::int128;

int128_t mmod(int128_t const a, int128_t const b)
{
   return (b + (a % b)) % b;
}

int128_t index_of(int128_t const i, int128_t const cards, int128_t const offset, int128_t const increment)
{
   return mmod(offset + i * increment, cards);
}

template <typename T>
T modpow(T base, T exp, T modulus)
{
   base %= modulus;
   T result = 1 % modulus;
   while (exp > 0)
   {
      if (exp & 1) result = (result * base) % modulus;
      base = (base * base) % modulus;
      exp >>= 1;
   }
   return result;
}

int128_t inverse(int128_t const n, int128_t const cards)
{
   auto i = modpow<int128_t>(n, cards - 2, cards);
   return i;
}

int128_t apply_actions(action_collection const& actions, int128_t const cards, int128_t const repeats, int128_t const index)
{
   int128_t increment_mul = 1;
   int128_t offset_diff = 0;

   for (auto const& action : actions)
   {
      switch (action.action)
      {
      case action_type::cut:
         {
         offset_diff += action.value * increment_mul;
         offset_diff = mmod(offset_diff, cards);
         }
         break;
      case action_type::deal:
         if (action.value == 0)
         {
            increment_mul *= -1;
            increment_mul = mmod(increment_mul, cards);

            offset_diff += increment_mul;
            offset_diff = mmod(offset_diff, cards);
         }
         else
         {
            increment_mul *= inverse(action.value, cards);
            increment_mul = mmod(increment_mul, cards);
         }
         break;
      }
   }

   int128_t increment = modpow<int128_t>(increment_mul, repeats, cards);
   int128_t offset = offset_diff * (1 - increment) * inverse(mmod((1 - increment_mul), cards), cards);
   offset = mmod(offset, cards);

   int128_t card = index_of(index, cards, offset, increment);
   return card;
}

int main()
{
   // tests
   {
      cardstack_t stack{ 0,1,2,3,4,5,6,7,8,9 };
      deal(stack);
      cardstack_t expected{ 9,8,7,6,5,4,3,2,1,0 };
      assert(expected == stack);
   }

   {
      cardstack_t stack{ 0,1,2,3,4,5,6,7,8,9 };
      cut(stack, 3);
      cardstack_t expected{ 3,4,5,6,7,8,9,0,1,2 };
      assert(expected == stack);
   }

   {
      cardstack_t stack{ 0,1,2,3,4,5,6,7,8,9 };
      cut(stack, -4);
      cardstack_t expected{ 6,7,8,9,0,1,2,3,4,5 };
      assert(expected == stack);
   }

   {
      cardstack_t stack{ 0,1,2,3,4,5,6,7,8,9 };
      deal(stack, 3);
      cardstack_t expected{ 0,7,4,1,8,5,2,9,6,3 };
      assert(expected == stack);
   }

   // more tests
   {
      cardstack_t stack{ 0,1,2,3,4,5,6,7,8,9 };
      deal(stack, 7);
      deal(stack);
      deal(stack);
      cardstack_t expected{ 0,3,6,9,2,5,8,1,4,7 };
      assert(expected == stack);
   }

   {
      cardstack_t stack{ 0,1,2,3,4,5,6,7,8,9 };
      cut(stack, 6);
      deal(stack, 7);
      deal(stack);
      cardstack_t expected{ 3,0,7,4,1,8,5,2,9,6 };
      assert(expected == stack);
   }

   {
      cardstack_t stack{ 0,1,2,3,4,5,6,7,8,9 };
      deal(stack, 7);
      deal(stack, 9);
      cut(stack, -2);
      cardstack_t expected{ 6,3,0,7,4,1,8,5,2,9 };
      assert(expected == stack);
   }

   {
      cardstack_t stack{ 0,1,2,3,4,5,6,7,8,9 };
      deal(stack);
      cut(stack, -2);
      deal(stack, 7);
      cut(stack, 8);
      cut(stack, -4);
      deal(stack, 7);
      cut(stack, 3);
      deal(stack, 9);
      deal(stack, 3);
      cut(stack, -1);
      cardstack_t expected{ 9,2,5,8,1,4,7,0,3,6 };
      assert(expected == stack);
   }

   // part 1
   {
      auto actions = read_actions(R"(..\data\aoc2019_22_input1.txt)");

      cardstack_t stack(10007);
      std::iota(stack.begin(), stack.end(), 0);

      apply_actions(stack, actions);
      auto pos = index_of(stack, 2019);

      std::cout << pos << '\n';
   }

   // part 1 - alternative
   {
      auto actions = read_actions(R"(..\data\aoc2019_22_input1.txt)");
      auto pos = apply_actions(actions, 10007, 2019);
      std::cout << pos << '\n';
   }

   // part 2
   {
      int128_t stack_size = 119315717514047;
      int128_t repeats = 101741582076661;
      size_t index = 2020;

      auto actions = read_actions(R"(..\data\aoc2019_22_input1.txt)");
      auto pos = apply_actions(actions, stack_size, repeats, index);
      std::cout << pos << '\n';
   }

}
