// aoc2019_07.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include <algorithm>
#include <tuple>
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

int execute_program(std::vector<int> numbers, std::vector<int> const & inputs)
{
   int inputs_index = 0;
   int output = 0;
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
         assert(inputs.size() > inputs_index);
         numbers[numbers[ip + 1]] = inputs[inputs_index++];
         ip += 2;
      }
      break;
      case OP_OUT:
      {
         output = mod1 == MOD_POSITION ? numbers[numbers[ip + 1]] : numbers[ip + 1];
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

   return output;
}

std::tuple<int, bool> execute_program(
   std::vector<int> & numbers, 
   int& ip,
   std::vector<int> & inputs,
   int& inputs_index)
{
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
         assert(inputs.size() > inputs_index);
         numbers[numbers[ip + 1]] = inputs[inputs_index++];
         ip += 2;
      }
      break;
      case OP_OUT:
      {
         auto output = mod1 == MOD_POSITION ? numbers[numbers[ip + 1]] : numbers[ip + 1];
         ip += 2;

         return { output, false };
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

   return { 0, true };
}


int run_configuration(std::vector<int> const& program, std::vector<int> const & phases, int const start)
{
   auto result = 0;
   for (auto phase : phases)
   {
      result = execute_program(program, {phase, result});
   }

   return result;
}

int run_thrusters(std::vector<int> const& program, int const start)
{
   int maxsignal = std::numeric_limits<int>::min();
   std::vector<int> phases{ 0,1,2,3,4 };

   do 
   {
      int signal = run_configuration(program, phases, start);

      if (signal > maxsignal)
         maxsignal = signal;
   } while (std::next_permutation(std::begin(phases), std::end(phases)));

   return maxsignal;
}

int run_configuration_loop(
   std::vector<int> const& program, 
   std::vector<int> const& phases, 
   int const start)
{
   auto program1 = program;
   auto program2 = program;
   auto program3 = program;
   auto program4 = program;
   auto program5 = program;

   std::vector<int> inputs1{ phases[0], start };
   std::vector<int> inputs2{ phases[1] };
   std::vector<int> inputs3{ phases[2] };
   std::vector<int> inputs4{ phases[3] };
   std::vector<int> inputs5{ phases[4] };

   int ip1 = 0, ip2 = 0, ip3 = 0, ip4 = 0, ip5 = 0;
   int input_index1 = 0, input_index2 = 0, input_index3 = 0, input_index4 = 0, input_index5 = 0;

   do 
   {
      auto [output1, done1] = execute_program(program1, ip1, inputs1, input_index1);
      if (!done1) inputs2.push_back(output1);

      auto [output2, done2] = execute_program(program2, ip2, inputs2, input_index2);
      if (!done2) inputs3.push_back(output2);

      auto [output3, done3] = execute_program(program3, ip3, inputs3, input_index3);
      if (!done3) inputs4.push_back(output3);

      auto [output4, done4] = execute_program(program4, ip4, inputs4, input_index4);
      if (!done4) inputs5.push_back(output4);

      auto [output5, done5] = execute_program(program5, ip5, inputs5, input_index5);
      if (!done5) inputs1.push_back(output5);
      else break;
   } while (true);

   return inputs1.back();
}

int run_thrusters_loop(std::vector<int> const& program, int const start)
{
   int maxsignal = std::numeric_limits<int>::min();
   std::vector<int> phases{ 5,6,7,8,9 };

   do
   {
      int signal = run_configuration_loop(program, phases, start);

      if (signal > maxsignal)
         maxsignal = signal;
   } while (std::next_permutation(std::begin(phases), std::end(phases)));

   return maxsignal;
}

int main()
{
   std::vector<int> program{ 3,8,1001,8,10,8,105,1,0,0,21,38,59,84,93,110,191,272,353,434,99999,3,9,101,5,9,9,1002,9,5,9,101,5,9,9,4,9,99,3,9,1001,9,3,9,1002,9,2,9,101,4,9,9,1002,9,4,9,4,9,99,3,9,102,5,9,9,1001,9,4,9,1002,9,2,9,1001,9,5,9,102,4,9,9,4,9,99,3,9,1002,9,2,9,4,9,99,3,9,1002,9,5,9,101,4,9,9,102,2,9,9,4,9,99,3,9,101,2,9,9,4,9,3,9,1002,9,2,9,4,9,3,9,1001,9,2,9,4,9,3,9,101,2,9,9,4,9,3,9,1001,9,1,9,4,9,3,9,102,2,9,9,4,9,3,9,1002,9,2,9,4,9,3,9,1002,9,2,9,4,9,3,9,101,2,9,9,4,9,3,9,102,2,9,9,4,9,99,3,9,102,2,9,9,4,9,3,9,101,2,9,9,4,9,3,9,1002,9,2,9,4,9,3,9,1002,9,2,9,4,9,3,9,1001,9,1,9,4,9,3,9,1001,9,1,9,4,9,3,9,101,2,9,9,4,9,3,9,1002,9,2,9,4,9,3,9,101,2,9,9,4,9,3,9,1001,9,2,9,4,9,99,3,9,102,2,9,9,4,9,3,9,1002,9,2,9,4,9,3,9,1002,9,2,9,4,9,3,9,101,2,9,9,4,9,3,9,1002,9,2,9,4,9,3,9,1001,9,1,9,4,9,3,9,1001,9,1,9,4,9,3,9,1002,9,2,9,4,9,3,9,102,2,9,9,4,9,3,9,101,1,9,9,4,9,99,3,9,1001,9,2,9,4,9,3,9,101,2,9,9,4,9,3,9,1001,9,1,9,4,9,3,9,102,2,9,9,4,9,3,9,101,2,9,9,4,9,3,9,1001,9,2,9,4,9,3,9,101,2,9,9,4,9,3,9,1002,9,2,9,4,9,3,9,102,2,9,9,4,9,3,9,1002,9,2,9,4,9,99,3,9,101,2,9,9,4,9,3,9,1002,9,2,9,4,9,3,9,1001,9,2,9,4,9,3,9,102,2,9,9,4,9,3,9,1001,9,2,9,4,9,3,9,1001,9,2,9,4,9,3,9,101,1,9,9,4,9,3,9,1001,9,1,9,4,9,3,9,101,1,9,9,4,9,3,9,1001,9,1,9,4,9,99 };

   {
      assert(run_thrusters({ 3,15,3,16,1002,16,10,16,1,16,15,15,4,15,99,0,0 }, 0) == 43210);
      assert(run_thrusters({ 3,23,3,24,1002,24,10,24,1002,23,-1,23,101,5,23,23,1,24,23,23,4,23,99,0,0 }, 0) == 54321);
      assert(run_thrusters({ 3,31,3,32,1002,32,10,32,1001,31,-2,31,1007,31,0,33,1002,33,7,33,1,33,31,31,1,32,31,31,4,31,99,0,0,0 }, 0) == 65210);

      int signal = run_thrusters(program, 0);
      std::cout << signal << '\n';
   }

   {
      assert(run_thrusters_loop({ 3,26,1001,26,-4,26,3,27,1002,27,2,27,1,27,26,
27,4,27,1001,28,-1,28,1005,28,6,99,0,0,5 }, 0) == 139629729);

      assert(run_thrusters_loop({ 3,52,1001,52,-5,52,3,53,1,52,56,54,1007,54,5,55,1005,55,26,1001,54,
-5,54,1105,1,12,1,53,54,53,1008,54,0,55,1001,55,1,55,2,53,55,53,4,
53,1001,56,-1,56,1005,56,6,99,0,0,0,0,10 }, 0) == 18216);

      int signal = run_thrusters_loop(program, 0);
      std::cout << signal << '\n';
   }
}

