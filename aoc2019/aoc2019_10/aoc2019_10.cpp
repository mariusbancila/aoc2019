// aoc2019_10.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <cmath>
#include <algorithm>
#include <assert.h>

struct point_t
{
   int x;
   int y;
};

using coordinate_collection = std::vector<point_t>;

enum class direction_t { before, after };

enum class quadrant_t
{
   I   = 1,
   II  = 2,
   III = 3,
   IV  = 4
};

struct slope_t
{
   double      slope;
   direction_t direction;
};

struct segment_t
{
   slope_t     slope;
   double      distance;
   quadrant_t  quadrant;
};

constexpr double get_slope(point_t const& p1, point_t const& p2) noexcept
{
   return p2.x == p1.x ? NAN : static_cast<double>(p2.y - p1.y) / static_cast<double>(p2.x - p1.x);
}

constexpr quadrant_t get_quadrant(point_t const& o, point_t const& p) noexcept
{
   auto x = p.x - o.x;
   auto y = p.y - o.y;
   if (x >= 0)
   {
      return y <= 0 ? quadrant_t::I : quadrant_t::II;
   }
   else
      return y <= 0 ? quadrant_t::IV : quadrant_t::III;
}

double get_distance(point_t const& p1, point_t const& p2)
{
   return std::sqrt((p1.x - p2.x)*(p1.x - p2.x) + (p1.y - p2.y)*(p1.y - p2.y));
}

direction_t get_direction(coordinate_collection const& map, size_t const i, size_t const j)
{
   return map[i].x == map[j].x ? ((map[i].y < map[j].y) ? direction_t::before : direction_t::after) : map[i].x < map[j].x ? direction_t::before : direction_t::after;
}

bool nearly_equal(double const a, double const b)
{
   if (std::isnan(a) && std::isnan(b)) return true;
   return std::fabs(a - b) < std::numeric_limits<double>::epsilon();
}

bool operator<(slope_t const& s1, slope_t const& s2)
{
   if (std::isnan(s1.slope) && std::isnan(s2.slope))
      return s1.direction < s2.direction;

   if (std::isnan(s1.slope)) return true;
   else if (std::isnan(s2.slope)) return false;
   else if (nearly_equal(s1.slope, s2.slope))
      return s1.direction < s2.direction;
   else
      return s1.slope < s2.slope;
}

bool operator==(slope_t const& s1, slope_t const& s2)
{
   if (std::isnan(s1.slope) && std::isnan(s2.slope))
      return s1.direction == s2.direction;
   else if (nearly_equal(s1.slope, s2.slope))
      return s1.direction == s2.direction;
   else
      return false;
}

bool operator<(segment_t const& l1, segment_t const& l2)
{
   if ((std::isnan(l1.slope.slope) && std::isnan(l2.slope.slope)) ||
      nearly_equal(l1.slope.slope, l2.slope.slope))
   {
      if (l1.quadrant == l2.quadrant)
         return l1.distance < l2.distance;
      else
         return l1.quadrant < l2.quadrant;
   }
   else if (std::isnan(l1.slope.slope))
   {
      return
         l1.quadrant == quadrant_t::I ||
         l2.quadrant == quadrant_t::III || l2.quadrant == quadrant_t::IV;
   }
   else if (std::isnan(l2.slope.slope))
   {
      return
         l2.quadrant != quadrant_t::I &&
         (l1.quadrant == quadrant_t::I || l1.quadrant == quadrant_t::II);
   }
   else if (((l1.quadrant == quadrant_t::I || l1.quadrant == quadrant_t::II) &&
      (l2.quadrant == quadrant_t::I || l2.quadrant == quadrant_t::II)) ||
      ((l1.quadrant == quadrant_t::III || l1.quadrant == quadrant_t::IV) &&
      (l2.quadrant == quadrant_t::III || l2.quadrant == quadrant_t::IV)))
   {
      return l1.slope.slope < l2.slope.slope;
   }
   else
      return l1.quadrant < l2.quadrant;
}

coordinate_collection read_map(std::string const& path)
{
   std::ifstream file(path.c_str());
   if (!file.is_open()) throw std::runtime_error("file not found");

   coordinate_collection map;

   std::string line;
   int row = 0;
   while (std::getline(file, line))
   {
      for (int col = 0; col < line.size(); ++col)
      {
         if (line[col] == '#')
            map.push_back({ col, row });
      }
      row++;
   }

   return map;
}

// from each point, count the number of slopes with the same value and direction
std::pair<size_t, size_t> find_best_location(coordinate_collection const& map)
{
   size_t best = std::numeric_limits<size_t>::min();
   size_t location;

   for (size_t i = 0; i < map.size(); ++i)
   {
      std::vector<slope_t> slopes_set;
      slopes_set.reserve(map.size() - 1);

      for (size_t j = 0; j < map.size(); ++j)
      {
         if (i != j)
         {
            slopes_set.push_back({ get_slope(map[i], map[j]), get_direction(map, i ,j) });
         }
      }

      std::sort(std::begin(slopes_set), std::end(slopes_set));
      auto end = std::unique(std::begin(slopes_set), std::end(slopes_set));
      slopes_set.erase(end, std::end(slopes_set));

      if (slopes_set.size() > best)
      {
         best = slopes_set.size();
         location = i;
      }
   }

   return { location , best };
}

struct point_info_t
{
   segment_t segment;
   size_t    index;
   bool      hit;
};

bool operator<(point_info_t const& p1, point_info_t const& p2)
{
   return p1.segment < p2.segment;
}

// for each point compute the slope, distance and position
// order the points by their slope and distance
// scan all the points in order; if multiple points have equal slope retain only the first and remove from list
// repeat until all the points are removed
size_t find_nth_asteroid(coordinate_collection const& map, size_t const best_location, size_t const nth)
{
   if (nth > map.size()) throw std::runtime_error("nth out of bounds");

   std::vector<point_info_t> lines_set;
   lines_set.reserve(map.size() - 1);

   for (size_t j = 0; j < map.size(); ++j)
   {
      if (best_location != j)
      {
         segment_t s{
               { get_slope(map[best_location], map[j]), get_direction(map, best_location ,j) },
               get_distance(map[best_location], map[j]),
               get_quadrant(map[best_location], map[j])
         };
         lines_set.push_back({ s, j, false });
      }
   }

   std::sort(std::begin(lines_set), std::end(lines_set));

   std::vector<size_t> result;

   auto l_not_hit = [](point_info_t const& p) {return !p.hit; };
   do
   {
      auto prev_slope = lines_set[0].segment.slope.slope;
      result.push_back(lines_set[0].index);
      lines_set[0].hit = true;
      for(size_t i = 1; i < lines_set.size(); ++i)
      {
         if(!nearly_equal(prev_slope, lines_set[i].segment.slope.slope))
         {
            prev_slope = lines_set[i].segment.slope.slope;
            result.push_back(lines_set[i].index);
            lines_set[i].hit = true;
         }
      }

      lines_set.erase(
         std::remove_if(std::begin(lines_set), std::end(lines_set), 
                        [](point_info_t const& p) {return p.hit; }), 
         std::end(lines_set));

   } while (!lines_set.empty());

   return result[nth - 1];
}

int main()
{
   {
      auto map = read_map("..\\data\\aoc2019_10_test1.txt");
      auto location = find_best_location(map);
      assert(map[location.first].x == 3 && map[location.first].y == 4);
   }
   
   {
      auto map = read_map("..\\data\\aoc2019_10_test2.txt");
      auto location = find_best_location(map);
      assert(map[location.first].x == 5 && map[location.first].y == 8);
   }

   {
      auto map = read_map("..\\data\\aoc2019_10_test3.txt");
      auto location = find_best_location(map);
      assert(map[location.first].x == 1 && map[location.first].y == 2);
   }

   {
      auto map = read_map("..\\data\\aoc2019_10_test4.txt");
      auto location = find_best_location(map);
      assert(map[location.first].x == 6 && map[location.first].y == 3);
   }
   
   {
      auto map = read_map("..\\data\\aoc2019_10_test5.txt");
      auto location = find_best_location(map);
      assert(map[location.first].x == 11 && map[location.first].y == 13);
      auto pos = find_nth_asteroid(map, location.first, 200);
      assert(map[pos].x * 100 + map[pos].y == 802);
   }

   {
      auto map = read_map("..\\data\\aoc2019_10_input1.txt");
      auto location = find_best_location(map);
      std::cout << location.second << '\n';
   }
   
   {
      auto map = read_map("..\\data\\aoc2019_10_test6.txt");
      auto location = find_best_location(map);
      auto pos = find_nth_asteroid(map, 28, 30);
      assert(map[pos].x * 100 + map[pos].y == 700);
   }
   
   {
      auto map = read_map("..\\data\\aoc2019_10_test5.txt");
      auto location = find_best_location(map);
      assert(map[location.first].x == 11 && map[location.first].y == 13);
   }

   {
      auto map = read_map("..\\data\\aoc2019_10_input1.txt");
      auto location = find_best_location(map);
      auto pos = find_nth_asteroid(map, location.first, 200);

      std::cout << map[pos].x * 100 + map[pos].y << '\n';
   }
}
