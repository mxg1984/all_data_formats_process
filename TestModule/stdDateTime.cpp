#include "stdafx.h"
#include "stdDateTime.h"

stdDateTime::stdDateTime()
{
	year=0;
	mon=0;
	day=0;
	hor=0;
	min=0;
	sec=0;
}

void stdDateTime::Reset()
{
	stdDateTime::stdDateTime();
}