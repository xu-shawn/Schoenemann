#include "timeman.h"

int getTimeForMove()
{
	int timeLeft = getTime();
	int increment = getIncrement();
	int timeForThisMove = timeLeft / 20 + (increment / 2);

	if (timeForThisMove >= timeLeft)
	{
		timeForThisMove = timeLeft - 300;
	}

	if (timeForThisMove < 0)
	{
		timeForThisMove = -20;
	}

	return timeForThisMove;
}