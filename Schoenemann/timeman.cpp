#include "timeman.h"
#include "Schoenemann.h"

int get_time_for_move()
{
	int time_left = get_time();
	int increment = get_increment();
	int time_for_this_move = time_left / 40 + (increment / 2);
	if (time_for_this_move >= time_left)
	{
		time_for_this_move = time_left - 500;
	}
	if (time_for_this_move < 0)
	{
		time_for_this_move = 100;
	}
	return time_for_this_move;
}