// aoc2019_03.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <cctype>
#include <charconv>
#include <string>
#include <string_view>
#include <vector>
#include <algorithm>
#include <assert.h>

enum class Direction
{
   Left,
   Right,
   Up,
   Down
};

Direction LetterToDirect(char const letter)
{
   switch (std::toupper(letter))
   {
   case 'R': return Direction::Right;
   case 'L': return Direction::Left;
   case 'U': return Direction::Up;
   case 'D': return Direction::Down;
   default:
      throw std::runtime_error("Invalid character.");
   }
}

struct Movement
{
   Direction    direction;
   int          offset;
};

bool from_string(std::string_view text, Movement& movement)
{
   auto success = false;
   if (!text.empty())
   {
      try
      {
         auto direction = LetterToDirect(text[0]);
         int offset;
         auto result = std::from_chars(text.data() + 1, text.data() + text.size(), offset);
         if (result.ec == std::errc())
         {
            movement = Movement{ direction, offset };
            return true;
         }
      }
      catch (std::exception const& e)
      {

      }
   }

   return success;
}

std::vector<Movement> ParseLine(std::string_view line)
{
   std::vector<Movement> moves;

   std::string_view delimiter = ",";
   size_t start = 0;
   size_t end = line.find(delimiter);

   std::string_view token;
   while (end != std::string::npos) 
   {
      token = line.substr(start, end - start);
      
      Movement movement;
      if (from_string(token, movement))
      {
         moves.push_back(movement);
      }

      start = end + 1;
      end = line.find(delimiter, start);
   }

   token = line.substr(start, end);

   Movement movement;
   if (from_string(token, movement))
   {
      moves.push_back(movement);
   }

   return moves;
}

std::tuple<int, int, int, int> GetBounds(std::vector<Movement> const & moves)
{   
   int l = 0;
   int r = std::numeric_limits<int>::min();
   int t = std::numeric_limits<int>::min();
   int b = 0;

   int x = 0, y = 0;
   for (auto const& m : moves)
   {
      switch (m.direction)
      {
      case Direction::Left:
         x -= m.offset;
         if (x < l) l = x;
         break;
      case Direction::Right:
         x += m.offset;
         if (x > r) r = x;
         break;
      case Direction::Up:
         y += m.offset;
         if (y > t) t = y;
         break;
      case Direction::Down:
         y -= m.offset;
         if (y < b) b = y;
         break;
      }
   }

   return {l, r, t, b};
}

void MarkWire(std::vector<Movement> const& moves, std::vector<char>& grid, int const w, int const h, int x, int y, char const symbol)
{
   auto mark = [&grid, w, h, symbol](int const row, int const column)
   {
      int pos = row * w + column;
      if (grid[pos] == ' ') grid[pos] = symbol;
      else if (grid[pos] != symbol) grid[pos] = 'x';
   };

   for (auto const& m : moves)
   {
      switch (m.direction)
      {
      case Direction::Left:
         for (int i = 1; i <= m.offset; ++i) mark(h-y-1, x-i);
         x -= m.offset;
         break;
      case Direction::Right:
         for (int i = 1; i <= m.offset; ++i) mark(h-y-1, x + i);

         x += m.offset;
         break;
      case Direction::Up:
         for (int i = 1; i <= m.offset; ++i) mark(h-(y+i)-1, x);
         y += m.offset;
         break;
      case Direction::Down:
         for (int i = 1; i <= m.offset; ++i) mark(h-(y - i)-1, x);
         y -= m.offset;
         break;
      }
   }
}

void PrintWires(std::vector<char>& grid, int const w, int const h)
{
   for (int y = 0; y < h; ++y)
   {
      for (int x = 0; x < w; ++x)
      {
         std::cout << grid[y * w + x];
      }

      std::cout << '\n';
   }
}

int compute_manhattan_distance(std::string_view line1, std::string_view line2)
{
   auto moves1 = ParseLine(line1);
   auto moves2 = ParseLine(line2);

   auto [l1, r1, t1, b1] = GetBounds(moves1);
   auto [l2, r2, t2, b2] = GetBounds(moves2);

   auto l = std::min(l1, l2);
   auto b = std::min(b1, b2);
   auto r = std::max(r1, r2);
   auto t = std::max(t1, t2);

   auto ox = l < 0 ? -l : 0;
   auto oy = b < 0 ? -b : 0;

   auto w = r - l + 1;
   auto h = t - b + 1;

   std::vector<char> grid(w * h, ' ');

   MarkWire(moves1, grid, w, h, ox, oy, '1');
   MarkWire(moves2, grid, w, h, ox, oy, '2');

   auto minmd = std::numeric_limits<int>::max();
   for (int y = 0; y < h; ++y)
   {
      for (int x = 0; x < w; ++x)
      {
         if (grid[y * w + x] == 'x')
         {
            int md = std::abs(x - ox) + std::abs(h - y - 1 - oy);
            if (md < minmd) minmd = md;
         }
      }
   }

   return minmd;
}

int count_steps(std::vector<Movement> const& moves, int const w, int const h, int x, int y, int const row, int const column)
{
   int steps = 0;
   for (auto const& m : moves)
   {
      switch (m.direction)
      {
      case Direction::Left:
         for (int i = 1; i <= m.offset; ++i)
         {
            steps++;
            if (y == row && x - i == column) return steps;
         };
         x -= m.offset;
         break;
      case Direction::Right:
         for (int i = 1; i <= m.offset; ++i)
         {
            steps++;
            if (y == row && x + i == column) return steps;
         }

         x += m.offset;
         break;
      case Direction::Up:
         for (int i = 1; i <= m.offset; ++i)
         {
            steps++;
            if(y + i == row && x == column) return steps;
         }
         y += m.offset;
         break;
      case Direction::Down:
         for (int i = 1; i <= m.offset; ++i)
         {
            steps++;
            if(y - i == row && x == column) return steps;
         }
         y -= m.offset;
         break;
      }
   }

   return steps;
}

int compute_minimum_steps(std::string_view line1, std::string_view line2)
{
   auto moves1 = ParseLine(line1);
   auto moves2 = ParseLine(line2);

   auto [l1, r1, t1, b1] = GetBounds(moves1);
   auto [l2, r2, t2, b2] = GetBounds(moves2);

   auto l = std::min(l1, l2);
   auto b = std::min(b1, b2);
   auto r = std::max(r1, r2);
   auto t = std::max(t1, t2);

   auto ox = l < 0 ? -l : 0;
   auto oy = b < 0 ? -b : 0;

   auto w = r - l + 1;
   auto h = t - b + 1;

   std::vector<char> grid(w * h, ' ');

   MarkWire(moves1, grid, w, h, ox, oy, '1');
   MarkWire(moves2, grid, w, h, ox, oy, '2');

   auto mind = std::numeric_limits<int>::max();
   for (int y = 0; y < h; ++y)
   {
      for (int x = 0; x < w; ++x)
      {
         if (grid[y * w + x] == 'x')
         {
            int s1 = count_steps(moves1, w, h, ox, oy, h - y - 1, x);
            int s2 = count_steps(moves2, w, h, ox, oy, h - y - 1, x);
            int steps = s1 + s2;
            if (steps < mind) mind = steps;
         }
      }
   }

   return mind;
}

int main()
{
   assert(ParseLine("").size() == 0);
   assert(ParseLine("ASD123").size() == 0);
   assert(ParseLine("R1").size() == 1);
   assert(ParseLine("R1,U12").size() == 2);
   assert(ParseLine("R1,U12,L123").size() == 3);
   assert(ParseLine("R1,U12,L123,D1234").size() == 4);

   assert(compute_manhattan_distance("R8,U5,L5,D3", "U7,R6,D4,L4") == 6);
   assert(compute_manhattan_distance("R75,D30,R83,U83,L12,D49,R71,U7,L72", "U62,R66,U55,R34,D71,R55,D58,R83") == 159);
   assert(compute_manhattan_distance("R98,U47,R26,D63,R33,U87,L62,D20,R33,U53,R51", "U98,R91,D20,R16,D67,R40,U7,R15,U6,R7") == 135);

   auto line1 = "R998,U494,L814,D519,R407,U983,R307,D745,R64,D29,L935,D919,L272,D473,R689,U560,L942,U264,R816,U745,R209,U227,R241,U111,L653,D108,R823,U254,L263,U987,L368,D76,R665,D646,L759,U425,L581,D826,R829,D388,L234,U33,L48,U598,L708,D764,L414,D75,L163,U802,L183,U893,L486,U947,L393,D694,L454,D600,R377,U312,R89,D178,L652,D751,R402,D946,R213,U985,R994,D336,R573,D105,L442,U965,R603,U508,L17,U191,L37,U678,L506,U823,R878,D709,L348,U167,L355,U314,L164,D672,L309,U895,R358,D769,R869,U598,R63,D68,R105,U133,R357,U588,L154,D631,L939,D235,R506,D885,R958,D896,L195,U292,L952,D616,L824,D497,R99,D121,R387,D155,L70,U580,L890,D368,L910,U645,L786,U977,R9,U781,L454,U783,L382,U321,L195,U196,L239,U764,R18,D71,R97,U77,L803,U963,L704,U94,L511,U747,L798,D905,L679,D135,R455,U650,R947,U14,L722,D245,L490,D183,L276,U559,L901,D767,R827,U522,L380,U494,R402,U70,R589,D582,R206,U756,L989,U427,L704,D864,R885,D9,R872,U454,R912,U752,R197,U304,L728,U879,R456,D410,L141,U473,R246,U498,R443,D297,R333,D123,R12,D665,R684,D531,R601,D13,L260,U60,R302,D514,R416,D496,L562,D334,L608,U74,R451,U251,R961,U166,L368,U146,R962,U973,R120,U808,R480,D536,L690,D958,R292,U333,R656,U305,R46,U831,L756,D907,L638,D969,L445,U541,R784,U148,R338,D264,R72,D637,R759,D709,L611,D34,R99,U305,R143,D191,R673,D753,R387,U994,R720,D896,R95,U703,L499,D453,R96,U808,L485,U127,L856,U357,L543,U382,R411,U969,L532,U64,R303,U457,L412,D140,R146,D67,R147,D681,L1,D994,L876,D504,R46,U683,L992,U640,L663,D681,L327,U840,R543,U97,R988,U792,R36";
   auto line2 = "L999,U148,L592,D613,L147,D782,R594,U86,R891,D448,R92,U756,R93,D763,L536,U906,L960,D988,L532,U66,R597,U120,L273,D32,R525,U628,L630,U89,L248,U594,R886,D544,L288,U380,L23,D191,L842,U394,L818,U593,L195,U183,L863,D456,L891,D653,R618,U314,L775,D220,R952,U960,R714,U946,L343,D873,L449,U840,R769,U356,L20,D610,L506,U733,R524,D450,L888,D634,R737,U171,R906,U369,L172,D625,L97,D437,R359,D636,R775,U749,L281,U188,R418,D437,R708,D316,L388,D717,R59,U73,R304,U148,L823,U137,R265,U59,R488,D564,R980,U798,L626,U47,L763,U858,L450,U663,R378,U93,L275,U472,L792,U544,R192,D979,L520,U835,L946,D615,L120,U923,L23,U292,R396,U605,L76,U813,L388,U500,L848,U509,L276,D538,R26,D806,R685,D319,R414,D989,L519,U603,R898,D477,L107,D828,R836,U432,L601,U888,L476,D974,L911,U122,L921,D401,L878,D962,L214,D913,L113,U418,R992,U844,L928,U534,L13,U457,L866,D208,L303,D732,L497,U673,R659,D639,R430,D301,L573,U373,L270,D901,L605,D935,R440,D183,R374,D883,L551,U771,R707,D141,R984,U346,R894,U1,R33,U492,R371,D631,R95,D62,L378,D343,R108,D611,L267,D871,L296,U391,R302,D424,R160,U141,R682,U949,R380,U251,L277,D404,R392,D210,L158,U896,R350,D891,L837,D980,R288,U597,L292,D639,L943,D827,L492,D625,L153,D570,R948,D855,L945,U840,L493,U800,L392,D438,R8,U966,R218,U597,R852,D291,L840,D831,L498,U846,R875,D244,R159,U243,R975,D246,R549,D304,R954,D123,R58,U5,L621,U767,R455,U550,R800,D417,R869,D184,L966,D51,L383,U132,L664,U220,L263,D307,R716,U346,L958,U84,L154,U90,L726,U628,L159,U791,L643,D652,L138,U577,R457,U655,L547,D441,L21";

   auto md = compute_manhattan_distance(line1, line2);
   std::cout << md << '\n';

   assert(compute_minimum_steps("R8,U5,L5,D3", "U7,R6,D4,L4") == 30);
   assert(compute_minimum_steps("R75,D30,R83,U83,L12,D49,R71,U7,L72", "U62,R66,U55,R34,D71,R55,D58,R83") == 610);
   assert(compute_minimum_steps("R98,U47,R26,D63,R33,U87,L62,D20,R33,U53,R51", "U98,R91,D20,R16,D67,R40,U7,R15,U6,R7") == 410);

   auto ms = compute_minimum_steps(line1, line2);
   std::cout << ms << '\n';
}
