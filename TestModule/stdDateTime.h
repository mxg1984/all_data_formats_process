#pragma once
#ifndef _DATETIME_H
#define _DATETIME_H

struct stdDateTime
{
	int16_t year;
	int16_t mon;
	int16_t day;
	int16_t hor;
	int16_t min;
	int16_t sec;
	stdDateTime();
	void Reset();
};
const size_t size_datetime=sizeof(stdDateTime);

#endif
