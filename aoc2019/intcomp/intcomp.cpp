// intcomp.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <string_view>
#include <map>

#define FMT_HEADER_ONLY 1
#include "fmt/core.h"
#include "fmt/format.h"

using memory_unit = long long;
using memory_t = std::vector<memory_unit>;

memory_t read_program(std::string_view text)
{
   memory_t memory;

   std::string_view delimiter = ",";
   size_t start = 0;
   size_t end = text.find(delimiter);

   std::string_view token;
   while (end != std::string::npos)
   {
      token = text.substr(start, end - start);

      memory.push_back(std::atoll(token.data()));

      start = end + 1;
      end = text.find(delimiter, start);
   }

   token = text.substr(start, end);

   memory.push_back(std::atoll(token.data()));

   return memory;
}

void print_program(memory_t const& memory, std::string path)
{
   std::map<int, std::string> opcodes{ {1, "add"}, {2,"mul"}, {3, "in"}, {4, "out"}, {5, "jnz"}, {6, "jz"}, {7, "le"}, {8, "eq"}, {9, "bso"}, {99, "hlt"} };

   size_t ip = 0;
   while (ip < memory.size())
   {
      memory_unit inst = memory[ip];
      int opcode = inst % 100;
      inst /= 100;
      int mod1 = inst % 10; inst /= 10;
      int mod2 = inst % 10; inst /= 10;
      int mod3 = inst % 10; inst /= 10;

      fmt::print("{0:5}", opcodes[opcode]);
      switch (opcode) 
      {
      case 1:
         fmt::print("{0},{1},{2}\n", memory[ip+1], memory[ip+2], memory[ip+3]);
         ip += 4;
         break;
      case 2:
         fmt::print("{0},{1},{2}\n", memory[ip + 1], memory[ip + 2], memory[ip + 3]);
         ip += 4;
         break;
      case 3:
         ip += 2;
         break;
      case 4:
         ip += 2;
         break;
      case 5:
         ip += 3;
         break;
      case 6:
         ip += 3;
         break;
      case 7:
         ip += 4;
         break;
      case 8:
         ip += 4;
         break;
      case 9:
         ip += 2;
         break;
      case 99:
         ip += 2;
         break;
      }
   }
}

int main()
{
   std::ifstream input("..\\data\\aoc2019_11_input1.txt");
   if (!input.is_open()) return -1;

   std::string text;

   input.seekg(0, std::ios::end);
   text.reserve(input.tellg());
   input.seekg(0, std::ios::beg);

   text.assign(
      std::istreambuf_iterator<char>(input),
      std::istreambuf_iterator<char>());

   auto memory = read_program("1102, 34915192, 34915192, 7, 4, 7, 99, 0");

   print_program(memory, "program.asm");
}
