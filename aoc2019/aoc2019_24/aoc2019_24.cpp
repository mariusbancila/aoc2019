// aoc2019_24.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include <string>
#include <set>
#include <assert.h>

constexpr char C_BUG = '#';
constexpr char C_EMPTY = '.';

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
      assert(x >=0 && x < width && y >=0 && y < height);
      return data[y * width + x];
   }

   char& at(size_t const x, size_t const y)
   {
      assert(x >= 0 && x < width && y >= 0 && y < height);
      return data[y * width + x];
   }

   long long get_biodiversity() const
   {
      long long bio = 0;
      for (int i = 0; i < data.size(); ++i)
      {
         if (data[i] == C_BUG) bio += (1 << i);
      }
      return bio;
   }

   int count_bugs(char const c1, const char c2)
   {
      return (c1 == C_BUG ? 1 : 0) + (c2 == C_BUG ? 1 : 0);
   }
   int count_bugs(char const c1, const char c2, const char c3)
   {
      return (c1 == C_BUG ? 1 : 0) + (c2 == C_BUG ? 1 : 0) + (c3 == C_BUG ? 1 : 0);
   }
   int count_bugs(char const c1, const char c2, const char c3, char const c4)
   {
      return (c1 == C_BUG ? 1 : 0) + (c2 == C_BUG ? 1 : 0) + (c3 == C_BUG ? 1 : 0) + (c4 == C_BUG ? 1 : 0);
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

      // corners
      auto tl = count_bugs(at(1, 0), at(0, 1));
      newgrid.at(0, 0) = evolve(at(0, 0), tl);

      auto tr = count_bugs(at(width-2, 0), at(width-1, 1));
      newgrid.at(width-1, 0) = evolve(at(width-1, 0), tr);

      auto bl = count_bugs(at(0, height-2), at(1, height-1));
      newgrid.at(0, height-1) = evolve(at(0, height-1), bl);

      auto br = count_bugs(at(width-2, height - 1), at(width-1, height - 2));
      newgrid.at(width-1, height - 1) = evolve(at(width-1, height-1), br);

      // lines
      // top
      for (int c = 1; c < width - 1; c++)
      {
         auto v = count_bugs(at(c-1, 0), at(c+1, 0), at(c, 1));
         newgrid.at(c, 0) = evolve(at(c, 0), v);
      }
      // bottom
      for (int c = 1; c < width - 1; c++)
      {
         auto v = count_bugs(at(c - 1, height-1), at(c + 1, height - 1), at(c, height - 2));
         newgrid.at(c, height - 1) = evolve(at(c, height - 1), v);
      }
      // left
      for (int r = 1; r < height - 1; ++r)
      {
         auto v = count_bugs(at(0, r - 1), at(0, r + 1), at(1, r));
         newgrid.at(0, r) = evolve(at(0, r), v);
      }
      // right
      for (int r = 1; r < height - 1; ++r)
      {
         auto v = count_bugs(at(width-1, r - 1), at(width-1, r + 1), at(width-2, r));
         newgrid.at(width-1, r) = evolve(at(width-1, r), v);
      }

      // middle
      for (int r = 1; r < height - 1; ++r)
      {
         for (int c = 1; c < width - 1; c++)
         {
            auto v = count_bugs(at(c, r-1), at(c-1, r), at(c, r+1), at(c+1, r));
            newgrid.at(c, r) = evolve(at(c, r), v);
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
