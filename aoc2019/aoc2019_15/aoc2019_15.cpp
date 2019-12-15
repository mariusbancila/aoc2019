// aoc2019_15.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include <array>
#include <map>
#include <set>
#include <string>
#include <string_view>
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

enum class movement_t
{
   north = 1,
   south = 2,
   west = 3,
   east = 4
};

enum class status_t
{
   uncharted = -1,
   wall = 0,
   empty = 1,
   oxygen_system = 2,
};

std::pair<size_t, size_t> move_robot(size_t x, size_t y, movement_t const movement)
{
   switch (movement)
   {
   case movement_t::north: y--; break;
   case movement_t::south: y++; break;
   case movement_t::west: x--; break;
   case movement_t::east: x++; break;
   }

   return { x, y };
}

class robot_t
{
   size_t cx;
   size_t cy;
   std::vector<int> surface;
   size_t x;
   size_t y;
   bool finished = false;
   movement_t last_movement;

public:
   robot_t(size_t const w = 50, size_t const h = 50): 
      cx(w), cy(h), x(w), y(h)
   {      
      surface.resize(get_width() * get_height(), static_cast<int>(status_t::uncharted));
   }

   size_t get_width() const { return 2 * cx + 1; }
   size_t get_height() const { return 2 * cy + 1; }

   void on_status_report(status_t status)
   {
      auto [nx, ny] = move_robot(x, y, last_movement);

      if (nx < 0 || nx > get_width() || y < 0 || ny > get_height())
      {
         resize();
      }

      set_status_at(nx, ny, status);

      if (status == status_t::oxygen_system) finished = true;
      else if (status != status_t::wall)
      {
         x = nx;
         y = ny;
      }

      static int i = 0;
      i++;
      if(i % 50 == 0)
         print();
   }

   status_t cell_at(size_t const c, size_t const r) const
   {
      auto value = surface[r * get_width() + c];
      if (value == -1) return status_t::uncharted;

      return static_cast<status_t>(value & 0xffff);
   }

   int hits_at(size_t const c, size_t const r) const
   {
      return surface[r * get_width() + c] >> 16;
   }

   void set_status_at(size_t const c, size_t const r, status_t const status)
   {
      if (cell_at(c, r) == status_t::empty)
      {
         assert(status == status_t::empty);
         auto hits = hits_at(c, r);
         surface[r * get_width() + c] = ((hits + 1) << 16) | static_cast<int>(status);
      }
      else if (status == status_t::empty)
      {
         surface[r * get_width() + c] = (1 << 16) | static_cast<int>(status);
      }
      else
      {
         surface[r * get_width() + c] = static_cast<int>(status);
      }

   }

   movement_t get_next_movement()
   {
      last_movement = movement_t::north;

      if (x == 0 || x == get_width() - 1 || y == 0 || y == get_height() - 1)
         resize();

      if (cell_at(x, y - 1) == status_t::uncharted) last_movement = movement_t::north;
      else if (cell_at(x-1, y) == status_t::uncharted) last_movement = movement_t::west;
      else if (cell_at(x, y+1) == status_t::uncharted) last_movement = movement_t::south;
      else if (cell_at(x +1, y) == status_t::uncharted) last_movement = movement_t::east;
      else
      {
         std::array<std::pair<movement_t, int>, 4> arr
         {
            std::make_pair(movement_t::north, 0),
            std::make_pair(movement_t::south, 0),
            std::make_pair(movement_t::west, 0),
            std::make_pair(movement_t::east, 0),
         };
         if (cell_at(x, y - 1) == status_t::empty) arr[0].second = hits_at(x, y -1);
         else if (cell_at(x, y - 1) == status_t::wall) arr[0].second = std::numeric_limits<int>::max();

         if (cell_at(x - 1, y) == status_t::empty) arr[2].second = hits_at(x - 1, y);
         else if (cell_at(x - 1, y) == status_t::wall) arr[2].second = std::numeric_limits<int>::max();

         if (cell_at(x, y + 1) == status_t::empty) arr[1].second = hits_at(x, y + 1);
         else if (cell_at(x, y + 1) == status_t::wall) arr[1].second = std::numeric_limits<int>::max();
         
         if (cell_at(x + 1, y) == status_t::empty) arr[3].second = hits_at(x + 1, y);
         else if (cell_at(x + 1, y) == status_t::wall) arr[3].second = std::numeric_limits<int>::max();

         std::sort(arr.begin(), arr.end(), [](std::pair<movement_t, int> const & p1, std::pair<movement_t, int> const & p2) {
            return p1.second < p2.second;
         });

         last_movement = arr.front().first;
      }

      return last_movement;
   }

   bool is_finished() const { return finished; }

   void print()
   {
      std::cout << "\033[2J\033[1;1H";
      for (size_t r = 0; r < get_height(); ++r)
      {
         for (size_t c = 0; c < get_width(); ++c)
         {
            if (c == x && r == y)
               std::cout << '@';
            else if (c == cx && r == cy)
               std::cout << 'X';
            else
            {
               switch (cell_at(c, r))
               {
               case status_t::uncharted: std::cout << ' '; break;
               case status_t::wall: std::cout << char(178); break;
               case status_t::empty: std::cout << char(176); break;
               case status_t::oxygen_system: std::cout << 'o'; break;
               }
            }
         }
         std::cout << '\n';
      }
   }

private:
   void resize()
   {
      int const increase = 2;
      auto w = get_width();
      auto h = get_height();
      auto ncx = cx + increase;
      auto ncy = cy + increase;
      auto nw = ncx * 2 + 1;
      auto nh = ncy * 2 + 1;
      std::vector<int> newsurface(nw * nh, static_cast<int>(status_t::uncharted));

      for (size_t r = 0; r < h; ++r)
      {
         for (size_t c = 0; c < w; ++c)
         {
            newsurface[(r+ increase) * nw + (c+ increase)] = surface[r * w + c];
         }
      }

      surface.swap(newsurface);
      cx = ncx;
      cy = ncy;
      x += increase;
      y += increase;
   }
};

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

   robot_t robot(20, 20);

   auto l_input = [&robot]() {return static_cast<int>(robot.get_next_movement()); };
   auto l_output = [&robot](memory_unit status) {robot.on_status_report(static_cast<status_t>(status)); return false; /* status == static_cast<memory_unit>(status_t::oxygen_system);*/ };

   program.execute(l_input, l_output);
   robot.print();
}
