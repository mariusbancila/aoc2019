#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <map>
#include <vector>
#include <queue>
#include <set>
#include <assert.h>

constexpr char C_ROBOT = '@';
constexpr char C_WALL = '#';
constexpr char C_PATH = '.';

struct point_t
{
   int x;
   int y;
};

inline bool operator==(point_t a, point_t b)
{
   return a.x == b.x && a.y == b.y;
}

inline bool operator<(point_t a, point_t b)
{
   return (a.x != b.x) ? (a.x < b.x) : (a.y < b.y);
}

constexpr point_t DIRECTIONS[] =
{
  point_t{  0, -1 }, // north = 0 = 0b00
  point_t{  0,  1 }, // south = 1 = 0b01
  point_t{ -1,  0 }, // west  = 2 = 0b10
  point_t{  1,  0 }, // east  = 3 = 0b11
};

constexpr int kTooFar = 1000000;

inline bool is_key(char const ch)
{
   return ch >= 'a' && ch <= 'z';
}

inline bool is_door(char const ch)
{
   return ch >= 'A' && ch <= 'Z';
}

inline int get_key_index(char const ch)
{
   assert(is_key(ch));
   return ch - 'a';
}

inline int get_door_index(char const ch)
{
   assert(is_door(ch));
   return ch - 'A';
}

inline uint32_t make_key_bit(char const ch)
{
   return 1u << get_key_index(ch);
}

inline uint32_t make_door_bit(char const ch)
{
   return 1u << get_door_index(ch);
}

inline bool has_key_for_door(uint32_t const keys, char const door)
{
   return (keys & make_door_bit(door)) != 0;
}

inline point_t get_next_step(point_t const pos, int const direction)
{
   return point_t{ pos.x + DIRECTIONS[direction].x, pos.y + DIRECTIONS[direction].y };
}

namespace part1
{
   struct Map
   {
      std::map<point_t, char> tiles;
      point_t size{ 0, 0 };
      point_t entrance{ 0, 0 };
      point_t keys[26];
      int nkeys = 0;

      inline bool contains(point_t const pos) const
      {
         return tiles.find(pos) != tiles.end();
      }


      inline char at(point_t const pos) const
      {
         auto it = tiles.find(pos);
         return (it != tiles.end()) ? it->second : C_WALL;
      }


      bool load(std::string const & filename)
      {
         std::ifstream file(filename);
         if (!file.is_open()) throw std::runtime_error("cannot open file " + filename);

         point_t pos{ 0, 0 };
         int tmp;
         bool foundEntrance = false;
         while ((tmp = file.get()) != EOF)
         {
            char ch = static_cast<char>(tmp);
            if (ch == '\n') 
            {
               size.x = std::max(size.x, pos.x);
               pos.x = 0;
               pos.y++;
               continue;
            }

            if (ch == C_ROBOT)
            {
               entrance = pos;
               foundEntrance = true;
            }
            else if (is_key(static_cast<char>(tmp)))
            {
               keys[get_key_index(ch)] = pos;
               ++nkeys;
            }
            else if (ch != C_PATH && !is_door(ch))
            {
               // Deliberately not storing wall tiles. Everything in `tiles` is a location we can visit.
               pos.x++;
               continue;
            }

            tiles[pos] = ch;
            pos.x++;
         }
         size.y = pos.y;

         return foundEntrance;
      }

      // A dead end is any location where we can't reach a key without backtracking.
      // Return value is true if there's a key at the end of the path, false if it's a dead end.
      void remove_dead_ends(point_t prev, point_t pos, std::set<point_t>& visited)
      {
         if (!contains(pos)) return;
         if (!visited.insert(pos).second) return;

         for (int direction = 0; direction < 4; direction++)
         {
            point_t next = get_next_step(pos, direction);
            if (next != prev)
            {
               remove_dead_ends(pos, next, visited);
            }
         }

         if (is_key(at(pos))) return;

         for (int direction = 0; direction < 4; direction++)
         {
            point_t next = get_next_step(pos, direction);
            if (next != prev && contains(next))
            {
               return;
            }
         }

         tiles.erase(pos);
      }

      struct SolveKey 
      {
         point_t  pos;
         uint32_t visited;

         bool operator == (const SolveKey& other) const { return pos == other.pos && visited == other.visited; }
         bool operator != (const SolveKey& other) const { return pos != other.pos || visited != other.visited; }
         bool operator <  (const SolveKey& other) const { return (pos != other.pos) ? (pos < other.pos) : (visited < other.visited); }
      };

      struct Work 
      {
         point_t  pos;
         uint32_t visited;
         int   distance;
      };

      int solve() 
      {
         std::set<point_t> visited;
         remove_dead_ends(entrance, entrance, visited);

         const uint32_t allKeys = (1u << nkeys) - 1;

         std::map<SolveKey, int> state; // values are the distance travelled.
         std::queue<Work> q;

         q.push(Work{ entrance, 0, 0 });
         while (!q.empty()) 
         {
            Work work = q.front();
            q.pop();

            char ch = at(work.pos);
            if (is_key(ch)) 
            {
               uint32_t bit = make_key_bit(ch);
               work.visited |= bit;
               if (work.visited == allKeys) 
               {
                  return work.distance;
               }
            }
            else if (is_door(ch) && !has_key_for_door(work.visited, ch)) 
            {
               continue;
            }

            SolveKey stateKey{ work.pos, work.visited };
            auto stateIt = state.find(stateKey);
            int stateDist = (stateIt != state.end()) ? stateIt->second : kTooFar;
            if (work.distance >= stateDist) 
            {
               continue;
            }

            state[stateKey] = work.distance;

            ++work.distance;
            for (int dir = 0; dir < 4; dir++) 
            {
               point_t next = get_next_step(work.pos, dir);
               if (contains(next)) 
               {
                  q.push(Work{ next, work.visited, work.distance });
               }
            }
         }

         return kTooFar;
      }
   };
}

namespace part2
{
   struct Map 
   {
      std::map<point_t, char> tiles;
      point_t size{ 0, 0 };
      point_t initialEntrance{ 0, 0 };
      point_t entrances[4];
      point_t keys[26]{};
      int nkeys = 0;

      inline bool contains(point_t const pos) const
      {
         return tiles.find(pos) != tiles.end();
      }

      inline char at(point_t const pos) const
      {
         auto it = tiles.find(pos);
         return (it != tiles.end()) ? it->second : C_WALL;
      }

      bool load(std::string const & filename)
      {
         std::ifstream file(filename);
         if (!file.is_open()) throw std::runtime_error("cannot open file " + filename);

         point_t pos{ 0, 0 };
         int tmp;
         bool foundEntrance = false;
         while ((tmp = file.get()) != EOF)
         {
            char ch = static_cast<char>(tmp);
            if (ch == '\n')
            {
               size.x = std::max(size.x, pos.x);
               pos.x = 0;
               pos.y++;
               continue;
            }

            if (ch == C_ROBOT)
            {
               initialEntrance = pos;
               foundEntrance = true;
            }
            else if (is_key(static_cast<char>(tmp)))
            {
               keys[get_key_index(ch)] = pos;
               ++nkeys;
            }
            else if (ch != '.' && !is_door(ch))
            {
               // Deliberately not storing wall tiles. Everything in `tiles` is a location we can visit.
               pos.x++;
               continue;
            }

            tiles[pos] = ch;
            pos.x++;
         }
         size.y = pos.y;

         if (foundEntrance)
         {
            tiles.erase(initialEntrance);
            for (int direction = 0; direction < 4; direction++)
            {
               tiles.erase(get_next_step(initialEntrance, direction));
            }
            entrances[0] = get_next_step(get_next_step(initialEntrance, 0), 2);
            entrances[1] = get_next_step(get_next_step(initialEntrance, 0), 3);
            entrances[2] = get_next_step(get_next_step(initialEntrance, 1), 2);
            entrances[3] = get_next_step(get_next_step(initialEntrance, 1), 3);
            for (int i = 0; i < 4; i++)
            {
               tiles[entrances[i]] = C_ROBOT;
            }
         }

         return foundEntrance;
      }

      // A dead end is any location where we can't reach a key without backtracking.
      // Return value is true if there's a key at the end of the path, false if it's a dead end.
      void remove_dead_ends(point_t prev, point_t pos, std::set<point_t>& visited)
      {
         if (!contains(pos)) return;
         if (!visited.insert(pos).second) return;

         for (int direction = 0; direction < 4; direction++)
         {
            point_t next = get_next_step(pos, direction);
            if (next != prev) 
            {
               remove_dead_ends(pos, next, visited);
            }
         }
         if (is_key(at(pos))) return;

         for (int direction = 0; direction < 4; direction++) 
         {
            point_t next = get_next_step(pos, direction);
            if (next != prev && contains(next)) 
            {
               return;
            }
         }

         tiles.erase(pos);
      }

      void remove_all_dead_ends() 
      {
         std::set<point_t> visited;
         for (int i = 0; i < 4; i++)
         {
            visited.clear();
            remove_dead_ends(entrances[i], entrances[i], visited);
         }
      }

      struct SolveKey 
      {
         point_t  pos;
         uint32_t visited;

         bool operator == (const SolveKey& other) const { return pos == other.pos && visited == other.visited; }
         bool operator != (const SolveKey& other) const { return pos != other.pos || visited != other.visited; }
         bool operator <  (const SolveKey& other) const { return (pos != other.pos) ? (pos < other.pos) : (visited < other.visited); }
      };

      struct Work 
      {
         point_t  pos;
         uint32_t visited;
         int      distance;
      };

      int solve_quadrant(point_t entrance, uint32_t startingKeys)
      {
         const uint32_t allKeys = (1u << nkeys) - 1;

         std::map<SolveKey, int> state; // values are the distance travelled.

         std::queue<Work> q;
         q.push(Work{ entrance, startingKeys, 0 });

         while (!q.empty())
         {
            Work work = q.front();
            q.pop();

            char ch = at(work.pos);
            if (is_key(ch)) 
            {
               uint32_t bit = make_key_bit(ch);
               work.visited |= bit;
               if (work.visited == allKeys) 
               {
                  return work.distance;
               }
            }
            else if (is_door(ch) && !has_key_for_door(work.visited, ch))
            {
               continue;
            }

            SolveKey stateKey{ work.pos, work.visited };
            auto stateIt = state.find(stateKey);
            int stateDist = (stateIt != state.end()) ? stateIt->second : kTooFar;
            if (work.distance >= stateDist)
            {
               continue;
            }

            state[stateKey] = work.distance;

            ++work.distance;
            for (int dir = 0; dir < 4; dir++)
            {
               point_t next = get_next_step(work.pos, dir);
               if (contains(next))
               {
                  q.push(Work{ next, work.visited, work.distance });
               }
            }

         }

         return kTooFar;
      }


      int solve()
      {
         remove_all_dead_ends();

         uint32_t keysByQuadrant[4] = { 0u, 0u, 0u, 0u };
         for (int i = 0; i < nkeys; i++) 
         {
            uint32_t idx = 0;
            idx |= (keys[i].y > initialEntrance.y) ? 2u : 0u;
            idx |= (keys[i].x > initialEntrance.x) ? 1u : 0u;
            keysByQuadrant[idx] |= (1u << i);
         }

         int totalDist = 0;
         const uint32_t allKeys = (1u << nkeys) - 1;
         for (int i = 0; i < 4; i++)
         {
            uint32_t startingKeys = allKeys ^ keysByQuadrant[i];
            totalDist += solve_quadrant(entrances[i], startingKeys);
         }
         return totalDist;
      }
   };
}

int main(int argc, char** argv)
{
   // part 1
   {
      {
         part1::Map map;
         auto loaded = map.load(R"(..\data\aoc2019_18_test1.txt)");
         assert(loaded);
         assert(8 == map.solve());
      }

      {
         part1::Map map;
         auto loaded = map.load(R"(..\data\aoc2019_18_test2.txt)");
         assert(loaded);
         assert(86 == map.solve());
      }

      {
         part1::Map map;
         auto loaded = map.load(R"(..\data\aoc2019_18_test3.txt)");
         assert(loaded);
         assert(132 == map.solve());
      }

      {
         part1::Map map;
         auto loaded = map.load(R"(..\data\aoc2019_18_test4.txt)");
         assert(loaded);
         assert(136 == map.solve());
      }

      {
         part1::Map map;
         auto loaded = map.load(R"(..\data\aoc2019_18_test5.txt)");
         assert(loaded);
         assert(81 == map.solve());
      }

      {
         part1::Map map;
         auto loaded = map.load(R"(..\data\aoc2019_18_input1.txt)");
         assert(loaded);
         int shortest = map.solve();
         std::cout << shortest << '\n';
      }
   }

   // part 2
   {
      {
         part2::Map map;
         auto loaded = map.load(R"(..\data\aoc2019_18_test6.txt)");
         assert(loaded);
         assert(8 == map.solve());
      }

      {
         part2::Map map;
         auto loaded = map.load(R"(..\data\aoc2019_18_test7.txt)");
         assert(loaded);
         assert(24 == map.solve());
      }

      {
         part2::Map map;
         auto loaded = map.load(R"(..\data\aoc2019_18_input1.txt)");
         assert(loaded);

         int shortest = map.solve();
         std::cout << shortest << '\n';
      }
   }
}