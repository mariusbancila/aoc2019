// aoc2019_02.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include <assert.h>

int execute_program(std::vector<int> numbers)
{
   assert(numbers.size() >= 5);

   int i = 1;
   while (true)
   {
      int opcode = numbers[i - 1];
      if (opcode == 99) break;

      assert(opcode == 1 || opcode == 2);

      if (opcode == 1)
      {
         numbers[numbers[i + 2]] = numbers[numbers[i]] + numbers[numbers[i + 1]];
      }
      else if (opcode == 2)
      {
         numbers[numbers[i + 2]] = numbers[numbers[i]] * numbers[numbers[i + 1]];
      }
      else
      {
         assert(false);
      }

      i += 4;
   }

   return numbers[0];
}

int main()
{
   assert(2 == execute_program({ 1,0,0,0,99 }));
   assert(2 == execute_program({ 2,3,0,3,99 }));
   assert(2 == execute_program({ 2,4,4,5,99,0 }));
   assert(30 == execute_program({ 1,1,1,4,99,5,6,0,99 }));

   std::vector<int> input { 1, 0, 0, 3, 1, 1, 2, 3, 1, 3, 4, 3, 1, 5, 0, 3, 2, 10, 1, 19, 1, 6, 19, 23, 1, 23, 13, 27, 2, 6, 27, 31, 1, 5, 31, 35, 2, 10, 35, 39, 1, 6, 39, 43, 1, 13, 43, 47, 2, 47, 6, 51, 1, 51, 5, 55, 1, 55, 6, 59, 2, 59, 10, 63, 1, 63, 6, 67, 2, 67, 10, 71, 1, 71, 9, 75, 2, 75, 10, 79, 1, 79, 5, 83, 2, 10, 83, 87, 1, 87, 6, 91, 2, 9, 91, 95, 1, 95, 5, 99, 1, 5, 99, 103, 1, 103, 10, 107, 1, 9, 107, 111, 1, 6, 111, 115, 1, 115, 5, 119, 1, 10, 119, 123, 2, 6, 123, 127, 2, 127, 6, 131, 1, 131, 2, 135, 1, 10, 135, 0, 99, 2, 0, 14, 0 };
   input[1] = 12;
   input[2] = 2;

   auto result = execute_program(input);
   std::cout << result << '\n';

   for (int noun = 0; noun <= 99; ++noun)
   {
      for (int verb = 0; verb <= 99; ++verb)
      {
         input[1] = noun;
         input[2] = verb;

         int first = execute_program(input);

         if (first == 19690720)
         {
            std::cout << 100 * noun + verb << '\n';
         }
      }
   }
}