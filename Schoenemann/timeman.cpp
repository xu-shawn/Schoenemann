#include "timeman.h"

int getTimeForMove()
{
	int timeLeft = getTime();
	int increment = getIncrement();

	return std::min(timeLeft, (int)(timeLeft*0.8 - 10));
}