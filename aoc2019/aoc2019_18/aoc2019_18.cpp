// aoc2019_18.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <queue>
#include <map>
#include <set>
#include <assert.h>

constexpr char C_ROBOT = '@';
constexpr char C_EMPTY = '.';
constexpr char C_WALL = '#';

struct grid_t
{
   std::string data;
   size_t      width = 0;
   size_t      height = 0;
};

constexpr int xdir[] = {0, 0, 1, -1};
constexpr int ydir[] = {1, -1, 0, 0};

inline char element_at(grid_t const & grid, size_t const x, size_t const y)
{
   return grid.data[y * grid.width + x];
}

inline char& element_at(grid_t & grid, size_t const x, size_t const y)
{
   return grid.data[y * grid.width + x];
}

struct point_t
{
   int x;
   int y;
};

inline bool operator==(point_t const& p1, point_t const& p2)
{
   return p1.x == p2.x && p1.y == p2.y;
}

inline bool operator<(point_t const& p1, point_t const& p2)
{
   return p1.x == p2.x ? p1.y < p2.y : p1.x < p2.x;
}

grid_t get_grid(std::string const& path)
{
   std::ifstream file(path);
   if (!file.is_open()) throw std::runtime_error("cannot open file " + path);

   grid_t grid;
   std::string line;
   while (std::getline(file, line))
   {
      grid.height++;
      grid.data += line;
   }

   grid.width = grid.data.size() / grid.height;

   return grid;
}

point_t find_robot(grid_t const& grid)
{
   for (int r = 0; r < grid.height; ++r)
   {
      for (int c = 0; c < grid.width; ++c)
      {
         if (element_at(grid, c, r) == C_ROBOT)
            return {c, r};
      }
   }

   throw std::runtime_error("could not find robot on the grid");
}

int find_shortest_path(grid_t const& grid)
{
   auto robot_pos = find_robot(grid);

   std::queue<std::pair<point_t, int>> queue;
   std::vector<std::map<int, int>> dst(grid.width * grid.height);
   point_t best{ 0, 0 };

   auto l_dst_at = [&grid, &dst](int const x, int const y) -> std::map<int, int>&
   {
      return dst[y * grid.width + x]; 
   };

   queue.push(std::make_pair(robot_pos, 0));
   l_dst_at(robot_pos.x, robot_pos.y)[0] = 0;

   while (!queue.empty())
   {
      auto top = queue.front();
      queue.pop();

      int topx = top.first.x;
      int topy = top.first.y;
      int topm = top.second;

      for (int d = 0; d < 4; ++d)
      {
         int nx = topx + xdir[d];
         int ny = topy + ydir[d];
         if(nx < 0 || nx >= grid.width || ny < 0 || ny >= grid.height)
            continue;

         char nelement = element_at(grid, nx, ny);
         if(nelement == C_WALL)
            continue;

         int cell = nelement - 'A';
         if (cell >= 0 && cell <= 26)
         {
            if(!(topm & (1 << cell)))
               continue;
         }

         int nm = topm;
         cell = nelement - 'a';
         if (cell >= 0 && cell <= 26)
            nm |= (1 << cell);

         if(l_dst_at(nx, ny).count(nm))
            continue;

         l_dst_at(nx, ny)[nm] = l_dst_at(topx, topy)[topm] + 1;
         best = std::min(best, point_t{-nm, l_dst_at(nx, ny)[nm]});

         queue.push(std::make_pair(point_t{nx, ny}, nm));
      }
   }

   return best.y;
}


int main()
{
   {
      auto grid = get_grid(R"(..\data\aoc2019_18_test1.txt)");
      assert(find_shortest_path(grid) == 8);
   }

   {
      auto grid = get_grid(R"(..\data\aoc2019_18_test2.txt)");
      assert(find_shortest_path(grid) == 86);
   }

   {
      auto grid = get_grid(R"(..\data\aoc2019_18_test3.txt)");
      assert(find_shortest_path(grid) == 132);
   }

   {
      auto grid = get_grid(R"(..\data\aoc2019_18_test4.txt)");
      assert(find_shortest_path(grid) == 136);
   }

   {
      auto grid = get_grid(R"(..\data\aoc2019_18_test5.txt)");
      assert(find_shortest_path(grid) == 81);
   }

   {
      auto grid = get_grid(R"(..\data\aoc2019_18_input1.txt)");
      auto sp = find_shortest_path(grid);
      std::cout << sp << '\n';
   }

   {
      auto grid = get_grid(R"(..\data\aoc2019_18_input1.txt)");
      auto robot_pos = find_robot(grid);

      element_at(grid, robot_pos.x, robot_pos.y) = C_WALL;
      element_at(grid, robot_pos.x+1, robot_pos.y) = C_WALL;
      element_at(grid, robot_pos.x-1, robot_pos.y) = C_WALL;
      element_at(grid, robot_pos.x, robot_pos.y+1) = C_WALL;
      element_at(grid, robot_pos.x, robot_pos.y-1) = C_WALL;
      element_at(grid, robot_pos.x + 1, robot_pos.y + 1) = C_ROBOT;
      element_at(grid, robot_pos.x + 1, robot_pos.y - 1) = C_ROBOT;
      element_at(grid, robot_pos.x - 1, robot_pos.y + 1) = C_ROBOT;
      element_at(grid, robot_pos.x - 1, robot_pos.y - 1) = C_ROBOT;

      // ???
   }
}
