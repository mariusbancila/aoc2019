// aoc2019_24.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include <string>
#include <set>
#include <assert.h>

constexpr char C_BUG = '#';
constexpr char C_EMPTY = '.';

constexpr int xdir[] = { 0, -1, 0, 1 };
constexpr int ydir[] = { -1, 0, 1, 0 };

struct grid_t
{
   std::string data;
   size_t      width;
   size_t      height;

public:
   void load(std::string const& path)
   {

   }

   void load(std::string const& text, size_t const w, size_t const h)
   {
      data = text;
      width = w;
      height = h;
   }

   char at(size_t const x, size_t const y) const
   {
      assert(is_valid_position(x, y));
      return data[y * width + x];
   }

   char& at(size_t const x, size_t const y)
   {
      assert(is_valid_position(x, y));
      return data[y * width + x];
   }

   bool is_valid_position(size_t const x, size_t const y) const
   {
      return x >= 0 && x < width && y >= 0 && y < height;
   }

   long long get_biodiversity() const
   {
      long long bio = 0;
      for (int i = 0; i < data.size(); ++i)
      {
         if (data[i] == C_BUG) bio |= (1 << i);
      }
      return bio;
   }

   char evolve(char const current, int const neighbors)
   {
      if (current == C_BUG && neighbors != 1) return C_EMPTY;
      else if (current == C_EMPTY && (neighbors == 1 || neighbors == 2)) return C_BUG;
      return current;
   }

   void evolve()
   {
      std::string newdata(data.size(), C_EMPTY);
      grid_t newgrid;
      newgrid.load(newdata, width, height);

      // middle
      for (int r = 0; r < height; ++r)
      {
         for (int c = 0; c < width; c++)
         {
            int neighbors = 0;
            for (int k = 0; k < 4; ++k)
            {
               int x = c + xdir[k];
               int y = r + ydir[k];
               if (is_valid_position(x, y) && at(x, y) == C_BUG)
                  neighbors++;
            }

            newgrid.at(c, r) = evolve(at(c, r), neighbors);
         }
      }

      data.swap(newgrid.data);
   }

   void print()
   {
      for(size_t r = 0; r < width; ++r)
      { 
         for (size_t c = 0; c < width; ++c)
         {
            std::cout << data[r * width + c];
         }
         std::cout << '\n';
      }
      std::cout << '\n';
   }
};


int main()
{  
   // part 1
   {
      grid_t grid;
      grid.load("#..#.#.#.#...#.....##.#.#", 5, 5);
      //grid.load("....##..#.#..##..#..#....", 5, 5);
      grid.print();
      std::set<long long> biodiversity;
      biodiversity.insert(grid.get_biodiversity());

      while (true)
      {
         grid.evolve();
         grid.print();
         auto bio = grid.get_biodiversity();

         if (biodiversity.contains(bio))
         {
            std::cout << bio << '\n';
            break;
         }
         else 
         {
            biodiversity.insert(bio);
         }
      }
   }
}
