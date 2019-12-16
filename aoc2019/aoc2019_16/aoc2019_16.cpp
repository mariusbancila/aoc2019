// aoc2019_16.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <string_view>
#include <cmath>
#include <numeric>
#include <assert.h>

using number_t = std::vector<int>;

number_t from_string(std::string_view text)
{
   number_t number;

   for (auto const& ch : text)
      number.push_back(ch - '0');

   return number;
}

std::string to_string(number_t const& number)
{
   std::stringstream sstr;
   for (auto const& e : number)
      sstr << static_cast<char>(e + '0');
   return sstr.str();
}

number_t fft(number_t number, number_t const & pattern, int const phases)
{
   for (int i = 0; i < phases; ++i)
   {
      number_t newnumber;
      for (size_t d = 0; d < number.size(); ++d)
      {
         long long digit = 0;         
         size_t pattern_index = 0;
         int pattern_index_count = 1;
         for (size_t k = 0; k < number.size(); ++k)
         {
            if (pattern_index_count > d)
            {
               pattern_index++;
               pattern_index_count = 0;
            }

            if (pattern_index == pattern.size()) pattern_index = 0;
            digit += number[k] * pattern[pattern_index];
            pattern_index_count++;
         }
         newnumber.push_back(std::abs(digit) % 10);
      }

      number.swap(newnumber);
   }

   return number;
}

std::string get_message(std::string input, int const phases, size_t const length)
{
   auto bnumber = from_string(input);
   auto offset = std::stoi(input.substr(0, 7));
   auto number = number_t{};
   for (int i = 0; i < 10000; ++i)
      number.insert(number.end(), bnumber.begin(), bnumber.end());

   for (int i = 0; i < phases; ++i)
   {
      auto partial_sum = std::accumulate(
         std::begin(number) + offset,
         std::end(number),
         0ll,
         [](long long const sum, int const v) {return sum + v; });

      for (size_t j = offset; j < number.size(); ++j)
      {
         auto t = partial_sum;
         partial_sum -= number[j];
         number[j] = t >= 0 ? t % 10 : (-t) % 10;
      }
   }

   number_t result(number.begin() + offset, number.begin() + offset + length);

   return to_string(result);
}

int main()
{
   auto base_pattern = number_t{0, 1, 0, -1};

   // part 1
   {
      auto number = from_string("12345678");
      auto result = fft(number, base_pattern, 4);
      auto text = to_string(result);
      assert(text.substr(0, 8) == "01029498");
   }

   {
      auto number = from_string("80871224585914546619083218645595");
      auto result = fft(number, base_pattern, 100);
      auto text = to_string(result);
      assert(text.substr(0, 8) == "24176176");
   }

   {
      auto number = from_string("19617804207202209144916044189917");
      auto result = fft(number, base_pattern, 100);
      auto text = to_string(result);
      assert(text.substr(0, 8) == "73745418");
   }

   {
      auto number = from_string("69317163492948606335995924319873");
      auto result = fft(number, base_pattern, 100);
      auto text = to_string(result);
      assert(text.substr(0, 8) == "52432133");
   }

   {
      auto number = from_string("59765216634952147735419588186168416807782379738264316903583191841332176615408501571822799985693486107923593120590306960233536388988005024546603711148197317530759761108192873368036493650979511670847453153419517502952341650871529652340965572616173116797325184487863348469473923502602634441664981644497228824291038379070674902022830063886132391030654984448597653164862228739130676400263409084489497532639289817792086185750575438406913771907404006452592544814929272796192646846314361074786728172308710864379023028807580948201199540396460310280533771566824603456581043215999473424395046570134221182852363891114374810263887875638355730605895695123598637121");
      auto result = fft(number, base_pattern, 100);
      auto text = to_string(result);
      std::cout << text.substr(0, 8) << '\n';      
   }

   // part 2
   {
      std::string input = "03036732577212944063491565474664";
      auto message = get_message(input, 100, 8);
      assert(message == "84462026");
   }

   {
      std::string input = "02935109699940807407585447034323";
      auto message = get_message(input, 100, 8);
      assert(message == "78725270");
   }

   {
      std::string input = "03081770884921959731165446850517";
      auto message = get_message(input, 100, 8);
      assert(message == "53553731");
   }

   {
      std::string input = "59765216634952147735419588186168416807782379738264316903583191841332176615408501571822799985693486107923593120590306960233536388988005024546603711148197317530759761108192873368036493650979511670847453153419517502952341650871529652340965572616173116797325184487863348469473923502602634441664981644497228824291038379070674902022830063886132391030654984448597653164862228739130676400263409084489497532639289817792086185750575438406913771907404006452592544814929272796192646846314361074786728172308710864379023028807580948201199540396460310280533771566824603456581043215999473424395046570134221182852363891114374810263887875638355730605895695123598637121";
      auto message = get_message(input, 100, 8);
      std::cout << message << '\n';
   }
}
