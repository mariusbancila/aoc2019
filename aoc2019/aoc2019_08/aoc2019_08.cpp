// aoc2019_08.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <string>
#include <string_view>
#include <assert.h>

std::vector<int> text_to_binary(std::string_view text)
{
   std::vector<int> data;
   data.resize(text.size());

   for (size_t i = 0; i < text.size(); ++i)
   {
      data[i] = text[i] - '0';
   }

   return data;
}

int find_product(std::vector<int> const& data, int const width, int const height)
{
   int size = width * height;
   int prod = 0;
   int c0min = std::numeric_limits<int>::max();
   auto start = data.begin();
   auto end = start;
   std::advance(end, size);
   while (end != data.end())
   {
      auto c0 = std::count_if(start, end, [](int const v) {return v == 0; });
      auto c1 = std::count_if(start, end, [](int const v) {return v == 1; });
      auto c2 = std::count_if(start, end, [](int const v) {return v == 2; });

      if (c0 < c0min)
      {
         c0min = c0;
         prod = c1 * c2;
      }

      start = end;
      end = start;
      std::advance(end, size);
   }

   return prod;
}

constexpr int TRANSPARENT = 2;
constexpr int WHITE = 1;
constexpr int BLACK = 0;

std::vector<int> find_image(std::vector<int> const& data, int const width, int const height)
{
   std::vector<int> image(width * height);

   size_t layers = data.size() / (width * height);

   for (size_t i = 0; i < width * height; ++i)
   {
      for (size_t l = 0; l < layers; ++l)
      {
         if (data[l * width * height + i] != TRANSPARENT)
         {
            image[i] = data[l * width * height + i];
            break;
         }
      }
   }

   return image;
}

void print_image(std::vector<int> const& image, int const width, int const height)
{
   for (int r = 0; r < height; ++r)
   {
      for (int c = 0; c < width; ++c)
      {
         char ch = image[r * width + c];
         switch (ch)
         {
         case 0: std::cout << ' '; break;
         case 1: std::cout << '#'; break;
         case 2: std::cout << '*'; break;
         }
      }
      std::cout << '\n';
   }
   std::cout << '\n';
}

int main()
{
   std::ifstream input("..\\data\\aoc2019_08_input1.txt");
   if (!input.is_open()) return -1;

   std::string str;

   input.seekg(0, std::ios::end);
   str.reserve(input.tellg());
   input.seekg(0, std::ios::beg);

   str.assign(
      std::istreambuf_iterator<char>(input),
      std::istreambuf_iterator<char>());

   int width = 25;
   int height = 6;
   int size = width * height;

   auto data = text_to_binary(str);

   assert(data.size() % size == 0);

   std::cout << find_product(data, width, height) << '\n';

   auto image = find_image(data, width, height);
   print_image(image, width, height);
}
