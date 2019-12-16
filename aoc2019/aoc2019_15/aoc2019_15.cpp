// aoc2019_15.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include <array>
#include <set>
#include <string>
#include <fstream>
#include <algorithm>
#include <assert.h>
#include <functional>

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
         case OP_OUT:      if(fout(execute_out(mod1))) return; break;
         case OP_JMPNZ:    execute_jump_nz(mod1, mod2); break;
         case OP_JMPZ:     execute_jump_z(mod1, mod2); break;
         case OP_LS:       execute_less(mod1, mod2, mod3); break;
         case OP_EQ:       execute_equal(mod1, mod2, mod3); break;
         case OP_BASEOFF:  execute_baseoff(mod1); break;
         }
      }
   }

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

struct point_t
{
   long long x;
   long long y;
};

bool operator==(point_t const & p1, point_t const & p2)
{
   return p1.x == p2.x && p1.y == p2.y;
}

bool operator<(point_t const& p1, point_t const& p2)
{
   return p1.x == p2.x ? p1.y < p2.y : p1.x < p2.x;
}

static const std::array<int, 4> directions = 
{
    1, // north
    2, // south
    3, // west
    4  // east
};

constexpr int S_WALL = 0;
constexpr int S_EMPTY = 1;
constexpr int S_OXYGEN = 2;

static const std::array<int, 4> opposites = { 2, 1, 4, 3 };

static uint8_t get_opposite(int dir)
{
   return opposites[dir - 1];
};

point_t update_position(point_t p, int const direction)
{
   switch (direction)
   {
   case 1: --p.y; break; // north
   case 2: ++p.y; break; // south
   case 3: --p.x; break; // west
   case 4: ++p.x; break; // east
   }

   return p;
}

std::tuple<point_t, size_t, size_t, std::set<point_t>> find_path(program_t program)
{
   point_t pos{ 0, 0 };

   auto move_droid = [&](int direction)
   {
      int status = 0;
      auto l_input = [direction]() { return direction; };
      auto l_output = [&status](memory_unit value) {status = static_cast<int>(value); return true; };

      program.execute(l_input, l_output);
      return status;
   };

   std::set<point_t> walls;
   std::set<point_t> visited;
   point_t oxygen_pos;
   size_t oxygen_path_length = 0;
   size_t open_locations = 0;

   std::function<void(int64_t)> explore = [&](int direction)
   {
      auto p = update_position(pos, direction);
      if (visited.count(p) == 1 || walls.count(p) == 1)
      {
         return;
      }

      auto status = move_droid(direction);
      assert(status < 3);
      if (status == S_WALL)
      {
         walls.insert(p);
         return;
      }

      ++open_locations;
      pos = update_position(pos, direction);
      visited.insert(pos);
      if (status == S_OXYGEN)
      {
         oxygen_pos = pos;
         oxygen_path_length = visited.size();
      }

      for (auto d : directions)
      {
         explore(d);
      }

      visited.erase(pos);
      pos = update_position(pos, get_opposite(direction));
      move_droid(get_opposite(direction));
   };

   for (auto d : directions)
   {
      explore(d);
   }

   return { oxygen_pos, oxygen_path_length, open_locations, walls };
}

void compute_oxygenation_time(std::set<point_t> const & walls, size_t const open_locations, point_t const & oxygen_pos)
{
   std::set<point_t> filled;
   size_t max_depth = 0;
   std::function<void(point_t, size_t)> oxygenate = [&](point_t p, size_t depth) 
   {
      if (filled.count(p) > 0 || walls.count(p) > 0) {
         return;
      }

      max_depth = std::max(depth, max_depth);
      filled.insert(p);

      if (filled.size() == open_locations) 
      {
         std::cout << "minutes: " << max_depth << '\n';
         return;
      }

      for (auto d : directions) 
      {
         oxygenate(update_position(p, d), depth + 1);
      }
   };

   oxygenate(oxygen_pos, 0);
}

int main()
{
   std::ifstream input("..\\data\\aoc2019_15_input1.txt");
   if (!input.is_open()) return -1;

   std::string text;

   input.seekg(0, std::ios::end);
   text.reserve(input.tellg());
   input.seekg(0, std::ios::beg);

   text.assign(
      std::istreambuf_iterator<char>(input),
      std::istreambuf_iterator<char>());

   auto memory = read_program(text);
   program_t program{ memory };

   // part 1
   auto const & [oxygen_pos, oxygen_path_length, open_locations, walls] = find_path(program);
   std::cout << "Oxygen position: " << oxygen_pos.x << ',' << oxygen_pos.y << '\n';
   std::cout << "Oxygen path length: " << oxygen_path_length << '\n';

   // part 2
   compute_oxygenation_time(walls, open_locations, oxygen_pos);
}