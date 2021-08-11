#include <time.h>
#include <cstdarg>
#include <stdlib.h>
#include <vector>
#include <map>

namespace jep
{
	void init();
	bool booRoll(double chance);
	int intRoll(int n1, int n2);
	int catRoll(int n, ...);
	int catRoll(std::vector<int> proportions);
	const float floatRoll(float min, float max, int precision);
	template <typename T> const T catRoll(const std::map<T, unsigned int> &m);
}
