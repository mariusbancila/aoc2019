// aoc2019_01.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <fstream>
#include <iterator>
#include <numeric>
#include <assert.h>

int compute_total_fuel(int const mass)
{
   int total = 0;
   int fuel = mass;
   while(fuel = (int)(fuel / 3) - 2, fuel > 0)
   {
      total += fuel;
   }
   return total;
}

int main()
{
   std::ifstream input("..\\data\\aoc2019_01_input1.txt");
   if (!input.is_open()) return -1;

   std::vector<int> masses;
   std::copy(std::istream_iterator<int>(input),
             std::istream_iterator<int>(),
             std::back_inserter(masses));

   auto result1 = std::accumulate(
      std::begin(masses), std::end(masses), 
      0, 
      [](int const sum, int const number) 
      {
         return sum + ((int)(number / 3) - 2);
      });

   std::cout << result1 << '\n';

   assert(compute_total_fuel(14) == 2);
   assert(compute_total_fuel(1969) == 966);
   assert(compute_total_fuel(100756) == 50346);

   auto result2 = std::accumulate(
      std::begin(masses), std::end(masses),
      0,
      [](int const sum, int const number)
      {
         return sum + compute_total_fuel(number);
      });

   std::cout << result2 << '\n';
}
