// aoc2019_14.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
#include <fstream>
#include <map>
#include <vector>
#include <regex>
#include <algorithm>
#include <numeric>
#include <deque>
#include <set>
#include <assert.h>

struct chemical_t
{
   long long   quantity;
   std::string name;
};

bool operator==(chemical_t const& t1, chemical_t const& t2)
{
   return t1.quantity == t2.quantity && t1.name == t2.name;
}

bool operator<(chemical_t const& t1, chemical_t const& t2)
{
   return t1.name == t2.name ? t1.quantity < t2.quantity : t1.name < t2.name;
}

chemical_t operator*(chemical_t const& t1, long long const n)
{
   return chemical_t{ t1.quantity * n, t1.name };
}

using chemicals_collection = std::vector<chemical_t>;

using reactions_collection = std::map<chemical_t, chemicals_collection>;

reactions_collection read_reaction(std::string path)
{
   std::ifstream file(path);
   if (!file.is_open()) throw std::runtime_error("input file could not be opened");

   reactions_collection reactions;

   std::regex rx_line{ R"((.+) => (.+))" };
   std::regex rx_chem{ R"((\d+) (\w+))" };
   std::string line;
   while (std::getline(file, line))
   {
      std::smatch match_line;
      if (std::regex_match(line, match_line, rx_line))
      {
         if (match_line.size() == 3)
         {
            chemicals_collection chemicals;

            std::string inputs = match_line[1].str();
            std::string outputs = match_line[2].str();

            // inputs
            {
               std::sregex_iterator iter(inputs.begin(), inputs.end(), rx_chem);
               std::sregex_iterator end;
               while (iter != end)
               {
                  std::string q = (*iter)[1];
                  std::string n = (*iter)[2];

                  chemicals.push_back(chemical_t{ std::stoll(q), n });
                  ++iter;
               }
            }

            // outputs
            {
               std::smatch result;
               if (std::regex_match(outputs, result, rx_chem))
               {
                  if (result.size() == 3)
                  {
                     std::string q = result[1].str();
                     std::string n = result[2].str();

                     chemical_t chem{std::stoll(q), n};

                     reactions.emplace(chem, chemicals);
                  }
               }
            }
         }
      }
   }

   return reactions;
}

std::pair<long long, chemicals_collection> find_reaction(reactions_collection const& reactions, std::string const& name)
{
   auto it = std::find_if(reactions.begin(), reactions.end(),
      [name](std::pair<chemical_t, chemicals_collection> const& p)
      {
         return p.first.name == name;
      });

   if (it == reactions.end()) throw std::runtime_error("invalid chemical");
   return { it->first.quantity, it->second };
}

void sort(reactions_collection const& reactions, std::deque<std::string>& order, std::set<std::string>& visited, std::string const& name)
{
   if (name == "ORE") return;

   visited.insert(name);
   auto const& [q, r] = find_reaction(reactions, name);
   for (auto const& c : r)
   {
      if (!visited.count(c.name))
      {
         sort(reactions, order, visited, c.name);
      }
   }
   order.push_front(name);
}

std::deque<std::string> get_order(reactions_collection const& reactions, std::string const & name)
{
   std::deque<std::string> order;
   std::set<std::string> visited;

   sort(reactions, order, visited, name);

   return order;
}

long long get_quantity_needed(long long const a, long long const b)
{
   return a == b ? 1 : static_cast<long long>(std::ceil(static_cast<double>(a) / b));
};

long long compute_ore(reactions_collection const& collection, std::string const & name, long long const quantity)
{
   auto it = collection.find(chemical_t{ 1, name });
   if (it == collection.end()) throw std::runtime_error("Fuel reaction not found");

   std::map<std::string, long long> amountNeeded{ {name, quantity} };
   auto order = get_order(collection, name);
   for (auto const& name : order)
   {
      auto const& [produced, reactions] = find_reaction(collection, name);
      auto reactions_needed = amountNeeded[name];
      auto factor = get_quantity_needed(reactions_needed, produced);

      for (auto const c : reactions)
      {
         amountNeeded[c.name] += c.quantity * factor;
      }
   }

   return amountNeeded["ORE"];
}

long long compute_maximum_fuel(
   reactions_collection const& reactions, 
   std::string const & name,
   long long const ore_total,
   long long const ore_per_unit)
{
   auto fuel = 1ll;
   auto low = ore_total / ore_per_unit;
   auto high = low * 2;
   while (low + 1 < high)
   {
      fuel = static_cast<long long>((low + high) / 2);
      auto cost = compute_ore(reactions, name, fuel);
      if (cost > ore_total) 
         high = fuel;
      else if (cost < ore_total) 
         low = fuel;
      else break;
   }

   if (compute_ore(reactions, name, fuel) > ore_total)
      fuel--;

   return fuel;
}

int main()
{
   // tests
   {
      auto reactions = read_reaction(R"(..\data\aoc2019_14_test1.txt)");
      auto ore = compute_ore(reactions, "FUEL", 1);
      assert(ore == 31);
   }

   {
      auto reactions = read_reaction(R"(..\data\aoc2019_14_test2.txt)");
      auto ore = compute_ore(reactions, "FUEL", 1);
      assert(ore == 165);
   }

   {
      auto reactions = read_reaction(R"(..\data\aoc2019_14_test3.txt)");
      auto ore = compute_ore(reactions, "FUEL", 1);
      assert(ore == 13312);

      auto fuel = compute_maximum_fuel(reactions, "FUEL", 1000000000000ll, ore);
      assert(fuel == 82892753);
   }

   {
      auto reactions = read_reaction(R"(..\data\aoc2019_14_test4.txt)");
      auto ore = compute_ore(reactions, "FUEL", 1);
      assert(ore == 180697);

      auto fuel = compute_maximum_fuel(reactions, "FUEL", 1000000000000ll, ore);
      assert(fuel == 5586022);
   }

   {
      auto reactions = read_reaction(R"(..\data\aoc2019_14_test4_1.txt)");
      auto ore = compute_ore(reactions, "FUEL", 1);
      assert(ore == 180697);
   }

   {
      auto reactions = read_reaction(R"(..\data\aoc2019_14_test5.txt)");
      auto ore = compute_ore(reactions, "FUEL", 1);
      assert(ore == 2210736);

      auto fuel = compute_maximum_fuel(reactions, "FUEL", 1000000000000ll, ore);
      assert(fuel == 460664);
   }

   {
      auto reactions = read_reaction(R"(..\data\aoc2019_14_test5_1.txt)");
      auto ore = compute_ore(reactions, "FUEL", 1);
      assert(ore == 2210736);
   }

   // puzzle
   {
      auto reactions = read_reaction(R"(..\data\aoc2019_14_input1.txt)");
      auto ore = compute_ore(reactions, "FUEL", 1);
      std::cout << ore << '\n';

      auto fuel = compute_maximum_fuel(reactions, "FUEL", 1000000000000ll, ore);
      std::cout << fuel << '\n';
   }
}

