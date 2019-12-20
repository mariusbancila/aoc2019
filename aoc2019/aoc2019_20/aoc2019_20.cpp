// aoc2019_20.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <fstream>
#include <array>
#include <iostream>
#include <istream>
#include <map>
#include <set>
#include <string>
#include <vector>
#include <assert.h>

constexpr char C_WALL = '#';
constexpr char C_EMPTY = ' ';
constexpr char C_PATH = '.';
constexpr char C_ROBOT = '@';
constexpr char const* P_ENTRY = "AA";
constexpr char const* P_EXIT = "ZZ";

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

using field_t = std::vector<std::string>;
using portals_t = std::map<point_t, point_t>;
using data_t = std::tuple<field_t, portals_t, point_t, point_t>;
using access_t = std::set<point_t>;
using level_access_t = std::set<std::pair<int, point_t>>;

inline bool is_upper_letter(char const c) 
{ 
   return 'A' <= c && c <= 'Z'; 
}

constexpr int xdir[] = { 0, 0, 1, -1 };
constexpr int ydir[] = { 1, -1, 0, 0 };

field_t read_field(std::string const& filename)
{
   std::ifstream stream(filename);
   if (!stream.is_open()) throw std::runtime_error("cannot open file " + filename);

   field_t field;

   std::string line;
   size_t width = 0;
   while (std::getline(stream, line))
   {
      if (!std::empty(line))
      {
         width = std::max(width, std::size(line));
         field.push_back(std::move(line));
      }
   }

   for (size_t i = 0; i != std::size(field); ++i)
   {
      field[i].resize(width);
   }

   return field;
}

data_t read_data(std::string const & filename)
{
   data_t data;
   auto& [field, portals, begin, end] = data;

   field = read_field(filename);
   std::map<std::string, std::pair<point_t, point_t>> mouths;
   for (int i = 2; i != static_cast<int>(std::size(field)) - 2; ++i) 
   {
      for (int j = 2; j != static_cast<int>(std::size(field[i])) - 2; ++j)
      {
         if (field[i][j] == '.') 
         {
            std::map<std::string, std::pair<point_t, point_t>> new_mouths;
            if (is_upper_letter(field[i - 1][j]))
            {
               std::string name{ field[i - 2][j], field[i - 1][j] };
               new_mouths[name] = { point_t{i - 1, j}, point_t{i, j} };
            }
            if (is_upper_letter(field[i + 1][j]))
            {
               std::string name{ field[i + 1][j], field[i + 2][j] };
               new_mouths[name] = { point_t{i + 1, j}, point_t{i, j} };
            }
            if (is_upper_letter(field[i][j - 1]))
            {
               std::string name{ field[i][j - 2], field[i][j - 1] };
               new_mouths[name] = { point_t{i, j - 1}, point_t{i, j} };
            }
            if (is_upper_letter(field[i][j + 1]))
            {
               std::string name{ field[i][j + 1], field[i][j + 2] };
               new_mouths[name] = { point_t{i, j + 1}, point_t{i, j} };
            }

            for (auto const & mouth : new_mouths) 
            {
               auto const & [name, points] = mouth;
               auto const & [in, out] = points;
               if (name == P_ENTRY) 
               {
                  begin = out;
               }
               else if (name == P_EXIT) 
               {
                  end = out;
               }
               else if (auto iter = mouths.find(name); iter != mouths.end()) 
               {
                  portals[in] = iter->second.second;
                  portals[iter->second.first] = out;
               }
               else 
               {
                  mouths.insert(mouth);
               }
            }
         }
      }
   }

   return data;
}

int find_shortest_path(data_t data) 
{
   auto& [field, portals, begin, end] = data;

   field[begin.x][begin.y] = C_ROBOT;
   int distance = 0;
   for (access_t access = { begin }; !std::empty(access); ++distance) 
   {
      access_t new_access;
      for (auto p : access) 
      {
         for(int i = 0; i < 4; ++i)
         {
            point_t np{ p.x + xdir[i], p.y + ydir[i] };

            if (np == end)
            {
               return distance + 1;
            }
            if (auto iter = portals.find(np); iter != portals.end())
            {
               np = iter->second;
            }
            if (field[np.x][np.y] == C_PATH)
            {
               field[np.x][np.y] = C_ROBOT;
               new_access.insert(np);
            }
         }
      }

      std::swap(access, new_access);
   }

   throw std::runtime_error("the function was unsuccessful");
}

int find_shortest_path_outermost_layers(data_t const & data, int const limit = 100)
{
   auto const & [field, portals, begin, end] = data;
   std::vector<field_t> levels = { field };

   levels[0][begin.x][begin.x] = C_ROBOT;
   int distance = 0;
   int level = 0;
   for (level_access_t access = { {0, begin} }; !std::empty(access); ++distance)
   {
      level_access_t new_access;
      for (auto const [l, p] : access)
      {
         for (int i = 0; i < 4; ++i)
         {
            point_t np{ p.x + xdir[i], p.y + ydir[i] };

            level = l;
            if (level == 0 && np == end) 
            {
               return distance + 1;
            }

            if (auto iter = portals.find(np); iter != portals.end()) 
            {
               if (np.y < 2 || np.y >= static_cast<int>(std::size(field[np.x])) - 2 ||
                   np.x < 2 || np.x >= static_cast<int>(std::size(field)) - 2)
               {
                  if (level == 0) 
                  {
                     continue;
                  }

                  --level;
               }
               else 
               {
                  if (level == limit) 
                  {
                     continue;
                  }
                  ++level;
                  if (level == static_cast<int>(std::size(levels)))
                  {
                     levels.push_back(field);
                  }
               }
               np = iter->second;
            }
            if (levels[level][np.x][np.y] == C_PATH) 
            {
               levels[level][np.x][np.y] = C_ROBOT;
               new_access.insert({ level, np });
            }
         }
      }

      std::swap(access, new_access);
   }

   throw std::runtime_error("the function was unsuccessful");
}

int main() 
{
   // part 1
   {
      auto data = read_data(R"(..\data\aoc2019_20_test1.txt)");
      assert(23 == find_shortest_path(data));
   }

   {
      auto data = read_data(R"(..\data\aoc2019_20_test2.txt)");
      assert(58 == find_shortest_path(data));
   }

   {
      auto data = read_data(R"(..\data\aoc2019_20_input1.txt)");
      auto path = find_shortest_path(data);
      std::cout << path << '\n';
   }

   // part 2
   {
      auto data = read_data(R"(..\data\aoc2019_20_test1.txt)");
      assert(26 == find_shortest_path_outermost_layers(data));
   }

   {
      auto data = read_data(R"(..\data\aoc2019_20_test3.txt)");
      assert(396 == find_shortest_path_outermost_layers(data));
   }

   {
      auto data = read_data(R"(..\data\aoc2019_20_input1.txt)");
      auto path = find_shortest_path_outermost_layers(data);
      std::cout << path << '\n';
   }
}