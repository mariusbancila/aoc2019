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

constexpr int OP_ADD = 1;
constexpr int OP_MUL = 2;
constexpr int OP_IN = 3;
constexpr int OP_OUT = 4;
constexpr int OP_JMPNZ = 5;
constexpr int OP_JMPZ = 6;
constexpr int OP_LS = 7;
constexpr int OP_EQ = 8;
constexpr int OP_BASEOFF = 9;
constexpr int OP_HALT = 99;

constexpr int MOD_POSITION = 0;
constexpr int MOD_IMMEDIATE = 1;
constexpr int MOD_RELBASE = 2;

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

std::string format_parameter(memory_unit const value, int const mode)
{
   switch (mode)
   {
   case MOD_POSITION:
      return fmt::format("[{0}]", value);
   case MOD_IMMEDIATE:
      return fmt::format("{0}", value);
   case MOD_RELBASE:
      return fmt::format(value > 0 ? "[base+{0}]":"[base{0}]", value);
   default:
      throw std::runtime_error("invalid parameter mode");
   }
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

      fmt::print("{0:04x} {1:5}", ip, opcodes[opcode]);
      switch (opcode) 
      {
      case 0:
         ip++;
         break;
      case OP_ADD:
         fmt::print("{0}, {1}, {2}", 
            format_parameter(memory[ip+1], mod1), 
            format_parameter(memory[ip+2], mod2),
            format_parameter(memory[ip+3], mod3));
         ip += 4;
         break;
      case OP_MUL:
         fmt::print("{0}, {1}, {2}",
            format_parameter(memory[ip + 1], mod1),
            format_parameter(memory[ip + 2], mod2),
            format_parameter(memory[ip + 3], mod3));
         ip += 4;
         break;
      case OP_IN:
         fmt::print(format_parameter(memory[ip + 1], mod1));
         ip += 2;
         break;
      case OP_OUT:
         fmt::print(format_parameter(memory[ip + 1], mod1));
         ip += 2;
         break;
      case OP_JMPNZ:
         fmt::print("{0}, {1}",
            format_parameter(memory[ip + 1], mod1),
            format_parameter(memory[ip + 2], mod2));
         ip += 3;
         break;
      case OP_JMPZ:
         fmt::print("{0}, {1}",
            format_parameter(memory[ip + 1], mod1),
            format_parameter(memory[ip + 2], mod2));
         ip += 3;
         break;
      case OP_LS:
         fmt::print("{0}, {1}, {2}",
            format_parameter(memory[ip + 1], mod1),
            format_parameter(memory[ip + 2], mod2),
            format_parameter(memory[ip + 3], mod3));
         ip += 4;
         break;
      case OP_EQ:
         fmt::print("{0}, {1}, {2}",
            format_parameter(memory[ip + 1], mod1),
            format_parameter(memory[ip + 2], mod2),
            format_parameter(memory[ip + 3], mod3));
         ip += 4;
         break;
      case OP_BASEOFF:
         fmt::print("base{0}{1}", memory[ip + 1] > 0 ? "+" : "", memory[ip + 1]);
         ip += 2;
         break;
      case 99:
         ip += 2;
         break;
      }
      fmt::print("\n");
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

   auto memory = read_program("3,12,6,12,15,1,13,14,13,4,13,99,-1,0,1,9");

   print_program(memory, "program.asm");
}
