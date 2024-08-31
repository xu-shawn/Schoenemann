#include "timeman.h"

int getTimeForMove()
{
	int timeLeft = getTime();
	int increment = getIncrement();
	int timeForThisMove = timeLeft / 20 + (increment / 2);

	return timeForThisMove;
}