// aoc2019_06.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <set>
#include <map>
#include <string>
#include <regex>
#include <fstream>
#include <numeric>
#include <assert.h>

using orbit_map = std::map<std::string, std::set<std::string>>;

orbit_map read_maps(std::string path)
{
   std::ifstream input(path);
   if (!input.is_open())
      throw std::runtime_error("cannot open file " + path);

   orbit_map maps;

   std::regex rx{"(\\w+)\\)(\\w+)"};
   std::string line;
   while (std::getline(input, line))
   {
      std::smatch match;
      if (std::regex_match(line, match, rx))
      {
         if (match.size() == 3)
         {
            auto target = match[1].str();
            auto subject = match[2].str();

            auto it = maps.find(target);
            if (it == std::end(maps))
            {
               maps.insert(std::make_pair(target, std::set<std::string>{subject}));
            }
            else
            {
               it->second.insert(subject);
            }
         }
      }
   }

   return maps;
}

void count_orbits(
   orbit_map const& maps,
   std::map<std::string, int>& orbits_count,
   std::string const& target)
{
   auto it = maps.find(target);
   if (it != std::end(maps))
   {
      auto count = orbits_count[target];
      for (auto const& subject : it->second)
      {
         orbits_count[subject] = count + 1;

         count_orbits(maps, orbits_count, subject);
      }
   }
}

int count_orbits(orbit_map const & maps)
{
   std::map<std::string, int> orbits_count;
   orbits_count["COM"] = 0;

   count_orbits(maps, orbits_count, "COM");

   int total = std::accumulate(
      std::cbegin(orbits_count),
      std::cend(orbits_count),
      0,
      [](int sum, std::pair<std::string, int> const& p)
      {
         return sum + p.second;
      });

   return total;
}

std::string find_parent(orbit_map const& maps, std::string subject)
{
   for (auto it = std::cbegin(maps);
      it != std::cend(maps);
      ++it)
   {
      if (it->second.contains(subject))
         return it->first;
   }

   throw std::runtime_error("parent not found for " + subject);
}

int find_path(orbit_map const& maps, std::string from, std::string to)
{
   std::map<std::string, int> paths_from;
   std::map<std::string, int> paths_to;

   int orbits = 1;
   std::string parent = find_parent(maps, from);
   while (parent != "COM")
   {
      paths_from[parent] = orbits++;
      parent = find_parent(maps, parent);
   }
   paths_from["COM"] = orbits;

   orbits = 1;
   parent = find_parent(maps, to);
   while (parent != "COM")
   {
      paths_to[parent] = orbits++;
      parent = find_parent(maps, parent);
   }
   paths_to["COM"] = orbits;

   std::map<std::string, std::pair<int, int>> common;
   for (auto [key, value] : paths_from)
   {
      auto it = paths_to.find(key);
      if (it != std::end(paths_to))
      {
         common.insert(std::make_pair(key, std::make_pair(value, it->second)));
         paths_to.erase(key);
      }
   }

   for (auto [key, value] : paths_to)
   {
      auto it = paths_from.find(key);
      if (it != std::end(paths_from))
      {
         common.insert(std::make_pair(key, std::make_pair(value, it->second)));
         paths_from.erase(key);
      }
   }

   auto element = std::min_element(
      std::cbegin(common), std::cend(common),
      [](std::pair<std::string, std::pair<int, int>> const& p1,
         std::pair<std::string, std::pair<int, int>> const& p2) 
      {
         return (p1.second.first + p1.second.second) < (p2.second.first + p2.second.second);
      });

   auto sum = element->second.first + element->second.second;

   return sum-2;
}

int main()
{/*
   {
      auto maps = read_maps("..\\data\\aoc2019_06_test1.txt");
      auto total = count_orbits(maps);
      assert(total == 42);
   }

   {
      auto maps = read_maps("..\\data\\aoc2019_06_input1.txt");
      auto total = count_orbits(maps);
      std::cout << total << '\n';
   }*/

   {
      auto maps = read_maps("..\\data\\aoc2019_06_test2.txt");
      auto total = find_path(maps, "YOU", "SAN");
      assert(total == 4);
   }

   {
      auto maps = read_maps("..\\data\\aoc2019_06_input1.txt");
      auto total = find_path(maps, "YOU", "SAN");
      std::cout << total << '\n';
   }
}
