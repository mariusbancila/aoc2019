// aoc2019_24.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include <string>
#include <set>
#include <algorithm>
#include <assert.h>

constexpr char C_BUG = '#';
constexpr char C_EMPTY = '.';
constexpr char C_ENTRY = '?';

enum class side_t { left, right, top, bottom };

constexpr int xdir[] = { 0, -1, 0, 1 };
constexpr int ydir[] = { -1, 0, 1, 0 };
constexpr side_t sidedir[] = {side_t::top, side_t::left, side_t::bottom, side_t::right};

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

   static char evolve(char const current, int const neighbors)
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

   static grid_t evolve(grid_t const grid, grid_t const & grid_zoom)
   {
      std::string newdata(grid.data.size(), C_EMPTY);
      grid_t newgrid;
      newgrid.load(newdata, grid.width, grid.height);

      for (int r = 0; r < grid.height; ++r)
      {
         for (int c = 0; c < grid.width; c++)
         {
            int neighbors = 0;

            for (int k = 0; k < 4; ++k)
            {
               int x = c + xdir[k];
               int y = r + ydir[k];
               if (grid.is_valid_position(x, y))
               {
                  if (grid.at(x, y) == C_ENTRY)
                     neighbors += grid_zoom.bugs_count(sidedir[k]);
                  else if (grid.at(x, y) == C_BUG)
                     neighbors++;                     
               }
            }

            newgrid.at(c, r) = evolve(grid.at(c, r), neighbors);
         }
      }

      return newgrid;
   }

   int bugs_count() const
   {
      return std::count_if(data.begin(), data.end(), [](char const c) {return c == C_BUG; });
   }

   int bugs_count(side_t const side) const
   {
      int bugs = 0;

      switch (side)
      {
      case side_t::top:
         bugs = std::count_if(data.begin(), data.begin()+width, [](char const c) {return c == C_BUG; });
         break;
      case side_t::bottom:
         bugs = std::count_if(data.rbegin(), data.rbegin() + width, [](char const c) {return c == C_BUG; });
         break;
      case side_t::left:
         for (int r = 0; r < height; ++r) if (at(0, r) == C_BUG) bugs++;
         break;
      case side_t::right:
         for (int r = 0; r < height; ++r) if (at(width-1, r) == C_BUG) bugs++;
         break;
      }

      return bugs;
   }

   void print() const
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
      grid.print();
      std::set<long long> biodiversity;
      biodiversity.insert(grid.get_biodiversity());

      while (true)
      {
         grid.evolve();
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

   // part 2
   {
      grid_t grid;
      //grid.load("#..#.#.#.#..?#.....##.#.#", 5, 5);
      grid.load("....##..#.#.?##..#..#....", 5, 5);

      grid_t empty;
      empty.load("............?............", 5, 5);

      constexpr int minutes = 10;
      std::vector<grid_t> grids(minutes * 2 + 1, empty);
      grids[minutes] = grid;

      for (int m = 1; m <= minutes; ++m)
      {
         std::vector<grid_t> newgrids(minutes * 2 + 1, empty);

         for (int l = 0; l < m; ++l)
         {
            auto const & crt1 = grids[minutes + l];
            auto const & next1 = grids[minutes + l + 1];
            newgrids[minutes+l] = grid_t::evolve(crt1, next1);

            auto const & crt2 = grids[minutes - l - 1];
            auto const & next2 = grids[minutes - l];
            newgrids[minutes - l - 1] = grid_t::evolve(crt2, next2);
         }

         grids.swap(newgrids);
      }

      for (int m = minutes - 5; m <= minutes + 5; ++m)
      {
         std::cout << "Depth " << m - minutes << '\n';
         grids[m].print();
      }

      int bugs = 0;
      for (auto const& grid : grids)
         bugs += grid.bugs_count();

      std::cout << bugs << '\n';
   }
}
