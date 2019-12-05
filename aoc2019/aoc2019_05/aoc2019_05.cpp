// aoc2019_05.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include <fstream>
#include <iterator>
#include <string>
#include <string_view>
#include <numeric>
#include <assert.h>

constexpr int OP_ADD = 1;
constexpr int OP_MUL = 2;
constexpr int OP_IN = 3;
constexpr int OP_OUT = 4;
constexpr int OP_JMPNZ = 5;
constexpr int OP_JMPZ = 6;
constexpr int OP_LS = 7;
constexpr int OP_EQ = 8;
constexpr int OP_HALT = 99;

constexpr int MOD_POSITION = 0;
constexpr int MOD_IMMEDIATE = 1;

void execute_program(std::vector<int> numbers)
{
   int ip = 0;
   while (true)
   {
      int inst = numbers[ip];
      int opcode = inst % 100;
      inst /= 100;
      int mod1 = inst % 10; inst /= 10;
      int mod2 = inst % 10; inst /= 10;
      int mod3 = inst % 10; inst /= 10;

      if (opcode == OP_HALT) break;

      assert(opcode >= OP_ADD && opcode <= OP_EQ);

      switch (opcode)
      {
      case OP_ADD:
      {
         assert(mod3 == MOD_POSITION);
         int param1 = mod1 == MOD_POSITION ? numbers[numbers[ip + 1]] : numbers[ip + 1];
         int param2 = mod2 == MOD_POSITION ? numbers[numbers[ip + 2]] : numbers[ip + 2];
         numbers[numbers[ip + 3]] = param1 + param2;
         ip += 4;
      }
      break;
      case OP_MUL:
      {
         assert(mod3 == MOD_POSITION);
         int param1 = mod1 == MOD_POSITION ? numbers[numbers[ip + 1]] : numbers[ip + 1];
         int param2 = mod2 == MOD_POSITION ? numbers[numbers[ip + 2]] : numbers[ip + 2];
         numbers[numbers[ip + 3]] = param1 * param2;
         ip += 4;
      }
      break;
      case OP_IN:
      {
         assert(mod1 == MOD_POSITION);
         int value;
         std::cout << ':'; std::cin >> value;
         numbers[numbers[ip + 1]] = value;
         ip += 2;
      }
      break;
      case OP_OUT:
      {
         int param = mod1 == MOD_POSITION ? numbers[numbers[ip + 1]] : numbers[ip + 1];
         std::cout << param << '\n';
         ip += 2;
      }
      break;
      case OP_JMPNZ:
      {
         int param1 = mod1 == MOD_POSITION ? numbers[numbers[ip + 1]] : numbers[ip + 1];
         if (param1 != 0)
            ip = mod2 == MOD_POSITION ? numbers[numbers[ip + 2]] : numbers[ip + 2];
         else
            ip += 3;
      }
      break;
      case OP_JMPZ:
      {
         int param1 = mod1 == MOD_POSITION ? numbers[numbers[ip + 1]] : numbers[ip + 1];
         if (param1 == 0)
            ip = mod2 == MOD_POSITION ? numbers[numbers[ip + 2]] : numbers[ip + 2];
         else
            ip += 3;
      }
      break;
      case OP_LS:
      {
         int param1 = mod1 == MOD_POSITION ? numbers[numbers[ip + 1]] : numbers[ip + 1];
         int param2 = mod2 == MOD_POSITION ? numbers[numbers[ip + 2]] : numbers[ip + 2];
         numbers[numbers[ip + 3]] = param1 < param2 ? 1 : 0;
         ip += 4;
      }
      break;
      case OP_EQ:
      {
         int param1 = mod1 == MOD_POSITION ? numbers[numbers[ip + 1]] : numbers[ip + 1];
         int param2 = mod2 == MOD_POSITION ? numbers[numbers[ip + 2]] : numbers[ip + 2];
         numbers[numbers[ip + 3]] = param1 == param2 ? 1 : 0;
         ip += 4;
      }
      break;
      }
   }
}

std::vector<int> ParseLine(std::string line)
{
   std::vector<int> memory;

   std::string_view delimiter = ",";
   size_t start = 0;
   size_t end = line.find(delimiter);

   std::string token;
   while (end != std::string::npos)
   {
      token = line.substr(start, end - start);

      memory.push_back(std::stoi(token));

      start = end + 1;
      end = line.find(delimiter, start);
   }

   token = line.substr(start, end);

   memory.push_back(std::stoi(token));

   return memory;
}


int main()
{
   std::ifstream input("..\\data\\aoc2019_05_input1.txt");
   if (!input.is_open()) return -1;

   std::string str;
   
   input.seekg(0, std::ios::end);
   str.reserve(input.tellg());
   input.seekg(0, std::ios::beg);

   str.assign(
      std::istreambuf_iterator<char>(input),
      std::istreambuf_iterator<char>());

   auto memory = ParseLine(str);

   execute_program(memory);

   //execute_program(ParseLine("3,9,8,9,10,9,4,9,99,-1,8"));
   //execute_program(ParseLine("3,9,7,9,10,9,4,9,99,-1,8"));
   //execute_program(ParseLine("3,3,1108,-1,8,3,4,3,99"));
   //execute_program(ParseLine("3,3,1107,-1,8,3,4,3,99"));
   //execute_program(ParseLine("3,21,1008,21,8,20,1005,20,22,107,8,21,20,1006,20,31,1106,0,36,98,0,0,1002,21,125,20,4,20,1105,1,46,104,999,1105,1,46,1101,1000,1,20,4,20,1105,1,46,98,99"));

}
