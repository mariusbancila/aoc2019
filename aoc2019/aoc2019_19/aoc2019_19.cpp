// aoc2019_19.cpp : This file contains the 'main' function. Program execution begins and ends there.
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

bool is_hit(memory_t memory, int const x, int const y)
{
   int ix = 0;
   bool hit = false;

   auto l_input = [x, y, &ix]()
   {
      auto v = ix == 0 ? x : y;
      ix++;
      return v;
   };

   auto l_output = [&hit](memory_unit value)
   {
      hit = value == 1;
      return false;
   };

   program_t program{ memory };
   program.execute(l_input, l_output);

   return hit;
};

int find_affected_points(memory_t memory, int const size)
{
   int points = 0;

   for (int r = 0; r < size; r++)
   {
      for (int c = 0; c < size; c++)
      {
         if (is_hit(memory, c, r))
            points++;
      }
   }

   return points;
}

int find_closest_position(memory_t memory, int const size)
{
   int r = size;
   int c = 0;
   int const offset = size - 1;

   while (true)
   {
      while (!is_hit(memory, c, r)) c++;

      if (is_hit(memory, c, r - offset) && is_hit(memory, c + offset, r - offset))
         break;

      r++;
      c = 0;
   }

   return c * 10000 + (r - offset);
}

int main()
{
   std::ifstream input("..\\data\\aoc2019_19_input1.txt");
   if (!input.is_open()) return -1;

   std::string text;

   input.seekg(0, std::ios::end);
   text.reserve(input.tellg());
   input.seekg(0, std::ios::beg);

   text.assign(
      std::istreambuf_iterator<char>(input),
      std::istreambuf_iterator<char>());

   auto memory = read_program(text);

   // part 1
   {     
      auto total = find_affected_points(memory, 50);
      std::cout << total << '\n';
   }

   // part 2
   {
      auto pos = find_closest_position(memory, 100);
      std::cout << pos << '\n';
   }
}
