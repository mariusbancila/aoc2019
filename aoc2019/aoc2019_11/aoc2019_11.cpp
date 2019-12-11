// aoc2019_11.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include <map>
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

      if (off >= memory.size())
         memory.resize(off + 1, 0);

      return memory[off];
   }

   void write_memory(offset_t const off, memory_unit const value)
   {
      if (off < 0) throw std::runtime_error("index out of bounds");

      if (off >= memory.size())
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

   program_t(std::initializer_list<memory_unit> mem): memory(mem){}

   result_t execute(memory_unit const input)
   {
      memory_t output;
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
         case OP_IN:       execute_in(mod1, input); break;
         case OP_OUT:      output.push_back(execute_out(mod1)); 
                           if (output.size() == 2) return {output, false}; 
                           break;
         case OP_JMPNZ:    execute_jump_nz(mod1, mod2); break;
         case OP_JMPZ:     execute_jump_z(mod1, mod2); break;
         case OP_LS:       execute_less(mod1, mod2, mod3); break;
         case OP_EQ:       execute_equal(mod1, mod2, mod3); break;
         case OP_BASEOFF:  execute_baseoff(mod1); break;
         }
      }

      return { output, true };
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

struct position_t
{
   int x;
   int y;
};

struct panel_t
{   
   int color;
   int paint_count;
};

enum class direction_t {left, right, up, down};

constexpr int TURN_LEFT = 0;
constexpr int TURN_RIGHT = 1;

bool operator==(position_t const& p1, position_t const& p2)
{
   return p1.x == p2.x && p1.y == p2.y;
}

bool operator<(position_t const& p1, position_t const& p2)
{
   return p1.x == p2.x ? p1.y < p2.y : p1.x < p2.x;
}

using panel_collection_t = std::map<position_t, panel_t>;

panel_collection_t count_panels(program_t program, int const start_color = 0)
{
   panel_collection_t panels;

   position_t crt{ 0,0 };
   int color = start_color;
   direction_t direction = direction_t::up;

   while (true)
   {
      auto pit = panels.find(crt);
      if (pit != panels.end())
         color = pit->second.color;
      else if(!panels.empty())
         color = 0;

      auto res = program.execute(color);
      if (res.first.size() == 2)
      {
         auto newcolor = res.first.front();
         auto nextturn = res.first.back();

         if (pit != panels.end())
         {
            pit->second.color = newcolor;
            pit->second.paint_count++;
         }
         else
         {
            panel_t p = { newcolor, 1 };
            panels.insert(std::make_pair(crt, p));
         }

         if (nextturn == TURN_LEFT)
         {
            switch (direction)
            {
            case direction_t::left:
               crt.y--; direction = direction_t::down;
               break;
            case direction_t::right:
               crt.y++; direction = direction_t::up;
               break;
            case direction_t::up:
               crt.x--; direction = direction_t::left;
               break;
            case direction_t::down:
               crt.x++; direction = direction_t::right;
               break;
            }
         }
         else
         {
            switch (direction)
            {
            case direction_t::left:
               crt.y++; direction = direction_t::up;
               break;
            case direction_t::right:
               crt.y--; direction = direction_t::down;
               break;
            case direction_t::up:
               crt.x++; direction = direction_t::right;
               break;
            case direction_t::down:
               crt.x--; direction = direction_t::left;
               break;
            }
         }
      }

      if (res.second)
         break;
   }

   return panels;
}

void paint(panel_collection_t& panels)
{
   int xmin = std::numeric_limits<int>::max();
   int xmax = std::numeric_limits<int>::min();
   int ymin = std::numeric_limits<int>::max();
   int ymax = std::numeric_limits<int>::min();

   for (auto const& kvp : panels)
   {
      if (kvp.first.x > xmax) xmax = kvp.first.x;
      if (kvp.first.x < xmin) xmin = kvp.first.x;
      if (kvp.first.y > ymax) ymax = kvp.first.y;
      if (kvp.first.y < ymin) ymin = kvp.first.y;
   }

   size_t width = static_cast<size_t>(xmax) - static_cast<size_t>(xmin) + 1;
   size_t height = static_cast<size_t>(ymax) - static_cast<size_t>(ymin) + 1;

   std::vector<int> data(width * height, 0);
   for (auto const& kvp : panels)
   {
      int row = ymax - kvp.first.y;
      int col = kvp.first.x - xmin;

      data[row * width + col] = kvp.second.color;
   }

   for (int r = 0; r < height; ++r)
   {
      for (int c = 0; c < width; ++c)
      {
         if (data[r * width + c] == 1)
            std::cout << '#';
         else
            std::cout << ' ';
      }
      std::cout << '\n';
   }

   std::cout << '\n';
}

int main()
{
   program_t program{ {3,8,1005,8,330,1106,0,11,0,0,0,104,1,104,0,3,8,102,-1,8,10,101,1,10,10,4,10,1008,8,0,10,4,10,102,1,8,29,3,8,1002,8,-1,10,1001,10,1,10,4,10,1008,8,0,10,4,10,101,0,8,51,1,1103,2,10,1006,0,94,1006,0,11,1,1106,13,10,3,8,1002,8,-1,10,101,1,10,10,4,10,1008,8,1,10,4,10,1001,8,0,87,3,8,102,-1,8,10,101,1,10,10,4,10,1008,8,0,10,4,10,1001,8,0,109,2,1105,5,10,2,103,16,10,1,1103,12,10,2,105,2,10,3,8,102,-1,8,10,1001,10,1,10,4,10,108,1,8,10,4,10,1001,8,0,146,1006,0,49,2,1,12,10,2,1006,6,10,1,1101,4,10,3,8,1002,8,-1,10,1001,10,1,10,4,10,108,0,8,10,4,10,1001,8,0,183,1,6,9,10,1006,0,32,3,8,102,-1,8,10,1001,10,1,10,4,10,1008,8,1,10,4,10,101,0,8,213,2,1101,9,10,3,8,1002,8,-1,10,1001,10,1,10,4,10,1008,8,1,10,4,10,101,0,8,239,1006,0,47,1006,0,4,2,6,0,10,1006,0,58,3,8,1002,8,-1,10,1001,10,1,10,4,10,1008,8,0,10,4,10,102,1,8,274,2,1005,14,10,1006,0,17,1,104,20,10,1006,0,28,3,8,102,-1,8,10,1001,10,1,10,4,10,108,1,8,10,4,10,1002,8,1,309,101,1,9,9,1007,9,928,10,1005,10,15,99,109,652,104,0,104,1,21101,0,937263411860,1,21102,347,1,0,1105,1,451,21101,932440724376,0,1,21102,1,358,0,1105,1,451,3,10,104,0,104,1,3,10,104,0,104,0,3,10,104,0,104,1,3,10,104,0,104,1,3,10,104,0,104,0,3,10,104,0,104,1,21101,0,29015167015,1,21101,0,405,0,1106,0,451,21102,1,3422723163,1,21101,0,416,0,1106,0,451,3,10,104,0,104,0,3,10,104,0,104,0,21101,0,868389376360,1,21101,0,439,0,1105,1,451,21102,825544712960,1,1,21102,1,450,0,1106,0,451,99,109,2,21201,-1,0,1,21101,0,40,2,21102,482,1,3,21102,1,472,0,1106,0,515,109,-2,2106,0,0,0,1,0,0,1,109,2,3,10,204,-1,1001,477,478,493,4,0,1001,477,1,477,108,4,477,10,1006,10,509,1101,0,0,477,109,-2,2106,0,0,0,109,4,2101,0,-1,514,1207,-3,0,10,1006,10,532,21102,1,0,-3,22101,0,-3,1,22102,1,-2,2,21102,1,1,3,21101,551,0,0,1106,0,556,109,-4,2105,1,0,109,5,1207,-3,1,10,1006,10,579,2207,-4,-2,10,1006,10,579,22102,1,-4,-4,1106,0,647,21201,-4,0,1,21201,-3,-1,2,21202,-2,2,3,21102,1,598,0,1106,0,556,22101,0,1,-4,21101,1,0,-1,2207,-4,-2,10,1006,10,617,21102,0,1,-1,22202,-2,-1,-2,2107,0,-3,10,1006,10,639,21201,-1,0,1,21102,639,1,0,105,1,514,21202,-2,-1,-2,22201,-4,-2,-4,109,-5,2105,1,0} };
   
   // part 1
   {
      auto panels = count_panels(program);
      std::cout << panels.size() << '\n';
   }

   // part 2
   {
      auto panels = count_panels(program, 1);
      paint(panels);
   }
}
