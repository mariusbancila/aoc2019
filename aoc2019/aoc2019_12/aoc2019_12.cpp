// aoc2019_12.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include <cmath>
#include <regex>
#include <fstream>
#include <numeric>
#include <assert.h>

struct position_t
{
   int x;
   int y;
   int z;
};

struct velocity_t
{
   int vx{ 0 };
   int vy{ 0 };
   int vz{ 0 };
};

struct positional_vector_t
{
   int pos;
   int vel;
};

bool operator==(positional_vector_t const& a, positional_vector_t const& b)
{
   return a.pos == b.pos && a.vel == b.vel;
}

velocity_t operator+(velocity_t const& v1, velocity_t const& v2)
{
   return velocity_t{v1.vx+v2.vx, v1.vy + v2.vy, v1.vz + v2.vz};
}

position_t operator+(position_t const& p, velocity_t const& v)
{
   return position_t{ p.x + v.vx, p.y + v.vy, p.z + v.vz };
}

struct body_t
{
   position_t position;
   velocity_t velocity;
};

using body_collection = std::vector<body_t>;

int get_body_potential_energy(body_t const& body) noexcept
{
   return std::abs(body.position.x) + std::abs(body.position.y) + std::abs(body.position.z);
}

int get_body_kinetic_energy(body_t const& body) noexcept
{
   return std::abs(body.velocity.vx) + std::abs(body.velocity.vy) + std::abs(body.velocity.vz);
}

int get_total_energy(body_collection const& bodies)
{
   int total = std::accumulate(
      std::begin(bodies), std::end(bodies),
      0,
      [](int const total, body_t const& body)
      {
         return total + get_body_kinetic_energy(body) * get_body_potential_energy(body);
      });

   return total;
}

int run_simulation(body_collection bodies, int const steps)
{
   for (int s = 0; s < steps; ++s)
   {
      std::vector<velocity_t> gravity;
      for (size_t i = 0; i < bodies.size(); ++i)
      {
         int dvx = 0, dvy = 0, dvz = 0;
         for (size_t j = 0; j < bodies.size(); ++j)
         {
            if (i != j)
            {
               if (bodies[i].position.x < bodies[j].position.x) dvx += 1;
               else if(bodies[i].position.x > bodies[j].position.x) dvx -= 1;

               if (bodies[i].position.y < bodies[j].position.y) dvy += 1;
               else if (bodies[i].position.y > bodies[j].position.y) dvy -= 1;

               if (bodies[i].position.z < bodies[j].position.z) dvz += 1;
               else if (bodies[i].position.z > bodies[j].position.z) dvz -= 1;
            }
         }
         gravity.push_back({ dvx, dvy, dvz });
      }
      
      for (size_t i = 0; i < bodies.size(); ++i)
      {
         bodies[i].velocity = bodies[i].velocity + gravity[i];
         bodies[i].position = bodies[i].position + bodies[i].velocity;
      }
   }

   return get_total_energy(bodies);
}

long long get_steps(std::vector<positional_vector_t> vectors_init)
{
   std::vector<positional_vector_t> vectors{ vectors_init };

   for (long long s = 1;; s++)
   {
      std::vector<int> gravity;
      for (size_t i = 0; i < vectors.size(); ++i)
      {
         int delta = 0;
         for (size_t j = 0; j < vectors.size(); ++j)
         {
            if (i != j)
            {
               if (vectors[i].pos < vectors[j].pos) delta += 1;
               else if (vectors[i].pos > vectors[j].pos) delta -= 1;
            }
         }
         gravity.push_back(delta);
      }

      for (size_t i = 0; i < vectors.size(); ++i)
      {
         vectors[i].vel = vectors[i].vel + gravity[i];
         vectors[i].pos = vectors[i].pos + vectors[i].vel;
      }

      if (vectors == vectors_init)
      {
         return s;
      }
   }
}

long long get_cycle_length(body_collection const & bodies)
{
   std::vector<positional_vector_t> xvectors;
   std::vector<positional_vector_t> yvectors;
   std::vector<positional_vector_t> zvectors;

   for (auto const& body : bodies)
   {
      xvectors.push_back({ body.position.x, body.velocity.vx });
      yvectors.push_back({ body.position.y, body.velocity.vy });
      zvectors.push_back({ body.position.z, body.velocity.vz });
   }

   long long repeatx = get_steps(xvectors);
   long long repeaty = get_steps(yvectors);
   long long repeatz = get_steps(zvectors);

   return std::lcm(std::lcm(repeatx, repeaty), repeatz);
}

body_collection read_input(std::string const& path)
{
   std::ifstream input(path);
   if (!input.is_open())
      throw std::runtime_error("cannot open file " + path);

   body_collection bodies;

   std::regex rx{ R"(<x=(-?\d+),\s*y=(-?\d+),\s*z=(-?\d*)>)" };
   std::string line;
   while (std::getline(input, line))
   {
      std::smatch match;
      if (std::regex_match(line, match, rx))
      {
         if (match.size() == 4)
         {
            std::string x = match[1].str();
            std::string y = match[2].str();
            std::string z = match[3].str();

            position_t p{ std::stoi(x), std::stoi(y), std::stoi(z) };
            velocity_t v{};
            bodies.push_back({ p, v });
         }
      }
   }

   return bodies;
}

int main()
{
   {
      auto bodies = read_input(R"(..\data\aoc2019_12_test1.txt)");

      auto energy = run_simulation(bodies, 10);
      assert(energy == 179);

      auto cycle_length = get_cycle_length(bodies);
      assert(cycle_length == 2772);
   }

   {
      auto bodies = read_input(R"(..\data\aoc2019_12_test2.txt)");

      auto enerty = run_simulation(bodies, 100);
      assert(enerty == 1940);

      auto cycle_length = get_cycle_length(bodies);
      assert(cycle_length == 4686774924);
   }
   
   {
      auto bodies = read_input(R"(..\data\aoc2019_12_input1.txt)");
      auto energy = run_simulation(bodies, 1000);
      std::cout << energy << '\n';
      
      auto cycle_length = get_cycle_length(bodies);
      std::cout << cycle_length << '\n';
   }
}
