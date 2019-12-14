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
#include <assert.h>

struct chemical_t
{
   int         quantity;
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

chemical_t operator*(chemical_t const& t1, int const n)
{
   return chemical_t{ t1.quantity * n, t1.name };
}

using chemicals_collection = std::vector<chemical_t>;

/*
struct reaction_t
{
   chemicals_collection inputs;
   chemical_t           output;
};

using reactions_collection = std::vector<reaction_t>;
*/
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

                  chemicals.push_back(chemical_t{ std::stoi(q), n });
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

                     chemical_t chem{std::stoi(q), n};

                     reactions.emplace(chem, chemicals);
                  }
               }
            }
         }
      }
   }

   return reactions;
}

int compute_ore(reactions_collection const& collection)
{
   auto it = collection.find(chemical_t{1, "FUEL"});
   if (it == collection.end()) throw std::runtime_error("Fuel reaction not found");

   auto l_is_first_product = [collection](chemical_t const& c)
   {
      return std::find_if(
         collection.begin(), collection.end(),
         [c](std::pair<chemical_t, chemicals_collection> const &p ) 
         {
            return 
               p.first.name == c.name &&
               p.second.size() == 1 && p.second.front().name == "ORE";
         }) != collection.end();
   };

   auto l_get_reaction = [collection](chemical_t const& c)
   {
      auto it = std::find_if(
         collection.begin(), collection.end(),
         [c](std::pair<chemical_t, chemicals_collection> const& p)
         {
            return p.first.name == c.name;
         });

      if (it == collection.end()) throw std::runtime_error("chemical not found");
      return *it;
   };

   auto l_minimum_factor = [](int const a, int const b)
   {
      return a == b ? 1 : static_cast<int>(std::ceil(static_cast<double>(a) / b));
   };

   auto l_reduce = [](chemicals_collection const& chemicals)
   {
      std::map<std::string, int> quantities;
      for (auto const& c : chemicals)
      {
         auto it = quantities.find(c.name);
         if (it == quantities.end())
            quantities.insert(std::make_pair(c.name, c.quantity));
         else
            it->second += c.quantity;
      }

      chemicals_collection newchemicals;
      for (auto const& [n, q] : quantities)
      {
         newchemicals.push_back(chemical_t{ q, n });
      }

      return newchemicals;
   };

   chemicals_collection chemicals(it->second);
   do
   {
      chemicals_collection newchemicals;
      for(auto const & c : chemicals)
      {
         if (l_is_first_product(c))
         {
            newchemicals.push_back(c);
         }
         else
         {
            auto const & [prod, reactants] = l_get_reaction(c);
            auto factor = l_minimum_factor(c.quantity, prod.quantity);

            for (auto const& r : reactants)
            {
               newchemicals.push_back(r * factor);
            }
         }
      }

      auto result = l_reduce(newchemicals);
      chemicals.swap(result);
   } while (
      !std::all_of(
         chemicals.begin(), chemicals.end(), 
         [l_is_first_product](chemical_t const& c) 
         {
            return l_is_first_product(c); 
         }));

   std::map<std::string, int> quantities;
   for (auto const& c : chemicals)
   {
      auto it = quantities.find(c.name);
      if (it == quantities.end())
         quantities.insert(std::make_pair(c.name, c.quantity));
      else
         it->second += c.quantity;
   }

   auto total = 0;
   for (auto const& [name, quantity] : quantities)
   {
      auto c = l_get_reaction(chemical_t{ 1, name });
      assert(c.second.front().name == "ORE");

      auto factor = l_minimum_factor(quantity, c.first.quantity);
      total += factor * c.second.front().quantity;
   }

   return total;
}

int main()
{
   //{
   //   auto reactions = read_reaction(R"(..\data\aoc2019_14_test1.txt)");
   //   auto ore = compute_ore(reactions);
   //   assert(ore == 31);
   //}

   //{
   //   auto reactions = read_reaction(R"(..\data\aoc2019_14_test2.txt)");
   //   auto ore = compute_ore(reactions);
   //   assert(ore == 165);
   //}

   //{
   //   auto reactions = read_reaction(R"(..\data\aoc2019_14_test3.txt)");
   //   auto ore = compute_ore(reactions);
   //   assert(ore == 13312);
   //}

   //{
   //   auto reactions = read_reaction(R"(..\data\aoc2019_14_test4.txt)");
   //   auto ore = compute_ore(reactions);
   //   assert(ore == 180697);
   //}

   //{
   //   auto reactions = read_reaction(R"(..\data\aoc2019_14_test6.txt)");
   //   auto ore = compute_ore(reactions);
   //   assert(ore == 180697);
   //}

   //{
   //   auto reactions = read_reaction(R"(..\data\aoc2019_14_test5.txt)");
   //   auto ore = compute_ore(reactions);
   //   assert(ore == 2210736);
   //}

   {
      auto reactions = read_reaction(R"(..\data\aoc2019_14_input1.txt)");
      auto ore = compute_ore(reactions);
      std::cout << ore << '\n';
   }

   {
      auto reactions = read_reaction(R"(..\data\aoc2019_14_test7.txt)");
      auto ore = compute_ore(reactions);
      assert(ore == 2210736);
   }
}
