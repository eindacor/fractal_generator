#include "jeploot.h"

namespace jep
{
	void init()
	{
		srand(time(NULL));
	}

	//has a d% chance to return true
	bool booRoll(double d)
	{
		if (d >= 1)
			return true;

		if (d <= 0)
			return false;

		return (rand() % 1000 < d * 1000);
	}

	//returns a random int i such that min <= i <= max
	int intRoll(int min, int max)
	{
		if (min == max)
			return min;
			
		int upper = (min < max ? max : min);
		int lower = (min < max ? min : max);

		return lower + (rand() % (1 + upper - lower));
	}

	//returns a random float f such that min <= f <= max, with the specified precision
	const float floatRoll(float min, float max, int precision)
	{
		float lower = (min<max ? min : max);
		float upper = (min<max ? max : min);

		float zero_offset = lower;
		lower -= zero_offset;
		upper -= zero_offset;

		double precision_multiplier = pow(10, precision);
		unsigned int max_int = int(upper * precision_multiplier);

		int random_number = rand() % (max_int + 1);
		float random_float = float(random_number) / precision_multiplier;
		random_float += zero_offset;
		return random_float;
	}

	int catRoll(int n, ...)
	{
		std::vector<int> ranges;

		int seed_range=0;

		va_list ratios;
		va_start (ratios, n);

		for (std::vector<int>::iterator i = ranges.begin(); i != ranges.end(); i++)
		{
			seed_range += va_arg(ratios, int);
			ranges.push_back(seed_range);
		}

		va_end (ratios);

		int seed = intRoll(1, seed_range);
		
		for (int i=0; i<n; i++)
		{
			if (seed <= ranges.at(i))
				return (i+1);
		}	
	}

	int catRoll(std::vector<int> proportions)
	{
		std::vector<int> ranges;
		int seed_range = 0;

		for (std::vector<int>::iterator i = proportions.begin(); i != proportions.end(); i++)
		{
			seed_range += *i;
			ranges.push_back(seed_range);
		}

		int seed = intRoll(1, seed_range);

		for (int i = 0; i<ranges.size(); i++)
		{
			if (seed <= ranges.at(i))
				return (i + 1);
		}
	}

	template <typename T> const T catRoll(const std::map<T, unsigned int> &m)
	{
		std::vector<unsigned int> ranges;

		int seedRange = 0;

		for (typename std::map<T, unsigned int>::const_iterator it = m.begin(); it != m.end(); it++)
		{
			seedRange += it->second;
			ranges.push_back(seedRange);
		}

		int random_number = rand() % seedRange;

		typename std::map<T, unsigned int>::const_iterator to_return = m.begin();
		for (unsigned int range : ranges)
		{
			if (random_number < range)
				return ((*to_return).first);

			to_return++;
		}
	}
}
