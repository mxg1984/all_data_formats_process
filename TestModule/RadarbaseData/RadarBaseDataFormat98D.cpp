#include "stdafx.h"
#include "RadarBaseDataFormat98D.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

stdRadial98DS::stdRadial98DS()
{
	memset(this, 0x0, SIZE_RADIAL_98DS);
}

stdRadial98DC::stdRadial98DC()
{
	memset(this, 0x0, SIZE_RADIAL_98DC);
}