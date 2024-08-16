#include "timeman.h"

int getTimeForMove()
{
	int timeLeft = getTime();
	int increment = getIncrement();
	int timeForThisMove = timeLeft / 40 + (increment / 2);

	if (timeForThisMove >= timeLeft)
	{
		timeForThisMove = timeLeft - 500;
	}

	if (timeForThisMove < 0)
	{
		timeForThisMove = -20;
	}

	return timeForThisMove;
}