// aoc2019_23.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include <array>
#include <set>
#include <string>
#include <string_view>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <functional>
#include <thread>
#include <mutex>
#include <queue>
#include <chrono>
#include <assert.h>

using memory_unit = long long;
using memory_t = std::vector<memory_unit>;
using offset_t = ptrdiff_t;
using result_t = std::pair<memory_t, bool>;

class program_t
{
   memory_t memory;
   offset_t ip = 0;
   offset_t rel_base = 0;

   constexpr static int OP_ADD = 1;
   constexpr static int OP_MUL = 2;
   constexpr static int OP_IN = 3;
   constexpr static int OP_OUT = 4;
   constexpr static int OP_JMPNZ = 5;
   constexpr static int OP_JMPZ = 6;
   constexpr static int OP_LS = 7;
   constexpr static int OP_EQ = 8;
   constexpr static int OP_BASEOFF = 9;
   constexpr static int OP_HALT = 99;

   constexpr static int MOD_POSITION = 0;
   constexpr static int MOD_IMMEDIATE = 1;
   constexpr static int MOD_RELBASE = 2;

private:
   memory_unit read_memory(offset_t const off)
   {
      if (off < 0) throw std::runtime_error("index out of bounds");

      if (static_cast<size_t>(off) >= memory.size())
         memory.resize(off + 1, 0);

      return memory[off];
   }

   void write_memory(offset_t const off, memory_unit const value)
   {
      if (off < 0) throw std::runtime_error("index out of bounds");

      if (static_cast<size_t>(off) >= memory.size())
         memory.resize(off + 1, 0);

      memory[off] = value;
   }

   memory_unit read_value(ptrdiff_t const ip, ptrdiff_t const rel_base, int const mode)
   {
      offset_t off = -1;

      switch (mode)
      {
      case MOD_POSITION:  off = memory[ip];            break;
      case MOD_IMMEDIATE: off = ip;                    break;
      case MOD_RELBASE:   off = rel_base + memory[ip]; break;
      default:            throw std::runtime_error("invalid access mode");
      }

      return read_memory(off);
   }

   void write_value(ptrdiff_t const ip, offset_t const rel_base, int const mode, memory_unit const value)
   {
      offset_t off = -1;

      switch (mode)
      {
      case MOD_POSITION:  off = memory[ip];            break;
      case MOD_IMMEDIATE: throw std::runtime_error("cannot write in immediate mode");
      case MOD_RELBASE:   off = rel_base + memory[ip]; break;
      default:            throw std::runtime_error("invalid access mode");
      }

      write_memory(off, value);
   }

public:
   program_t(memory_t const& mem) :memory(mem) {}

   program_t(std::initializer_list<memory_unit> mem) : memory(mem) {}

   void execute(std::function<int(void)> fin, std::function<bool(memory_unit)> fout)
   {
      while (true)
      {
         memory_unit inst = memory[ip];
         int opcode = inst % 100;
         inst /= 100;
         int mod1 = inst % 10; inst /= 10;
         int mod2 = inst % 10; inst /= 10;
         int mod3 = inst % 10; inst /= 10;

         if (opcode == OP_HALT) break;

         assert(opcode >= OP_ADD && opcode <= OP_BASEOFF);

         switch (opcode)
         {
         case OP_ADD:      execute_add(mod1, mod2, mod3); break;
         case OP_MUL:      execute_mul(mod1, mod2, mod3); break;
         case OP_IN:       execute_in(mod1, fin()); break;
         case OP_OUT:      if (fout(execute_out(mod1))) return; break;
         case OP_JMPNZ:    execute_jump_nz(mod1, mod2); break;
         case OP_JMPZ:     execute_jump_z(mod1, mod2); break;
         case OP_LS:       execute_less(mod1, mod2, mod3); break;
         case OP_EQ:       execute_equal(mod1, mod2, mod3); break;
         case OP_BASEOFF:  execute_baseoff(mod1); break;
         }
      }
   }

   void reset() { ip = 0; rel_base = 0; }
private:
   void execute_add(int const mod1, int const mod2, int const mod3)
   {
      assert(mod3 != MOD_IMMEDIATE);
      memory_unit param1 = read_value(ip + 1, rel_base, mod1);
      memory_unit param2 = read_value(ip + 2, rel_base, mod2);
      write_value(ip + 3, rel_base, mod3, param1 + param2);
      ip += 4;
   }

   void execute_mul(int const mod1, int const mod2, int const mod3)
   {
      assert(mod3 != MOD_IMMEDIATE);
      memory_unit param1 = read_value(ip + 1, rel_base, mod1);
      memory_unit param2 = read_value(ip + 2, rel_base, mod2);
      write_value(ip + 3, rel_base, mod3, param1 * param2);
      ip += 4;
   }

   void execute_in(int const mod1, memory_unit const input)
   {
      assert(mod1 != MOD_IMMEDIATE);
      write_value(ip + 1, rel_base, mod1, input);
      ip += 2;
   }

   memory_unit execute_out(int const mod1)
   {
      memory_unit param = read_value(ip + 1, rel_base, mod1);
      ip += 2;
      return param;
   }

   void execute_jump_nz(int const mod1, int const mod2)
   {
      memory_unit param1 = read_value(ip + 1, rel_base, mod1);
      if (param1 != 0)
         ip = read_value(ip + 2, rel_base, mod2);
      else
         ip += 3;
   }

   void execute_jump_z(int const mod1, int const mod2)
   {
      memory_unit param1 = read_value(ip + 1, rel_base, mod1);
      if (param1 == 0)
         ip = read_value(ip + 2, rel_base, mod2);
      else
         ip += 3;
   }

   void execute_less(int const mod1, int const mod2, int const mod3)
   {
      memory_unit param1 = read_value(ip + 1, rel_base, mod1);
      memory_unit param2 = read_value(ip + 2, rel_base, mod2);
      write_value(ip + 3, rel_base, mod3, param1 < param2 ? 1 : 0);
      ip += 4;
   }

   void execute_equal(int const mod1, int const mod2, int const mod3)
   {
      memory_unit param1 = read_value(ip + 1, rel_base, mod1);
      memory_unit param2 = read_value(ip + 2, rel_base, mod2);
      write_value(ip + 3, rel_base, mod3, param1 == param2 ? 1 : 0);
      ip += 4;
   }

   void execute_baseoff(int const mod1)
   {
      memory_unit param1 = read_value(ip + 1, rel_base, mod1);
      rel_base += param1;
      ip += 2;
   }
};

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

using packet_t = std::array<memory_unit, 2>;
using packet_data_t = std::pair<int, packet_t>;

struct program_data_t
{
   packet_data_t        output{};
   std::queue<packet_t> queue;
   std::mutex           mt;
   int                  id = 0;
   bool                 booted = false;
   int                  input_index = 0;
   int                  output_index = 0;

public:
   void set_id(int const id)
   {
      this->id = id;
   }
   int get_id() const { return id; }

   void push(packet_t const& packet)
   {
      std::unique_lock<std::mutex> l(mt);
      queue.push(packet);
   }

   memory_unit next_input()
   {
      if (!booted)
      {
         booted = true;
         return id;
      }
      else
      {
         memory_unit result = -1;
         std::unique_lock<std::mutex> l(mt);
         if (!queue.empty())
         {
            result = queue.front()[input_index++];

            if (input_index == 2)
            {
               queue.pop();
               input_index = 0;
            }
         }

         return result;
      }
   }

   packet_data_t get_output() const { return output; }

   bool next_output(memory_unit const v)
   {
      if (output_index == 0) output.first = v;
      else output.second[output_index - 1] = v;

      output_index++;
      output_index %= 3;
      return output_index == 0;
   }
};

int main()
{
   std::ifstream input("..\\data\\aoc2019_23_input1.txt");
   if (!input.is_open()) return -1;

   std::string text;

   input.seekg(0, std::ios::end);
   text.reserve(input.tellg());
   input.seekg(0, std::ios::beg);

   text.assign(
      std::istreambuf_iterator<char>(input),
      std::istreambuf_iterator<char>());

   auto memory = read_program(text);

   constexpr int count = 50;
   std::vector<program_t> programs(count, program_t{memory});
   std::vector<program_data_t> program_contexts(count);
   bool finished = false;

   for (int i = 0; i < count; ++i)
   {
      program_contexts[i].set_id(i);
   }

   std::mutex mc;

   std::vector<std::thread> program_threads(count);
   for (int i = 0; i < count; ++i)
   {
      program_threads[i] = std::thread([&mc, i, &finished, &programs, &program_contexts]()
      {
            auto l_input = [&mc, i, &program_contexts]()
            {
               using namespace std::chrono_literals;
               auto v = program_contexts[i].next_input();
               if (v == -1)
                  std::this_thread::sleep_for(200ms);
               else 
               {
                  //std::unique_lock<std::mutex> l(mc);
                  //std::cout << '[' << program_contexts[i].get_id() << ']' << ':' << v << '\n';
               }
               return v;
            };

            auto l_output = [&mc, i, &program_contexts, &finished](memory_unit const value)
            {
               {
                  //std::unique_lock<std::mutex> l(mc);
                  //std::cout << '[' << program_contexts[i].get_id() << ']' << '+' << value << '\n';
               }

               auto ready = program_contexts[i].next_output(value);
               if (ready)
               {
                  auto output = program_contexts[i].get_output();
                  if (output.first == 255)
                  {
                     {
                        std::unique_lock<std::mutex> l(mc);
                        std::cout << program_contexts[i].get_id() << "->" << output.first << " : " << output.second[0] << ',' << output.second[1] << '\n';
                        std::cout << "Y: " << output.second[1] << '\n';
                        finished = true;
                     }
                     std::exit(0);
                  }
                  else
                  {
                     {
                        std::unique_lock<std::mutex> l(mc);
                        std::cout << program_contexts[i].get_id() << "->" << output.first << " : " << output.second[0] << ',' << output.second[1] << '\n';
                     }
                     assert(output.first >= 0 && output.first < count);
                     program_contexts[output.first].push(output.second);
                  }
               }

               return finished;
            };

            programs[i].execute(l_input, l_output);
      });
   }

   for (int i = 0; i < count; ++i)
      program_threads[i].join();
}
