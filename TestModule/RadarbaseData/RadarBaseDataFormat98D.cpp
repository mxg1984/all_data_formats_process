#include "RadarBaseDataFormat98D.h"

namespace format_98d {
	stdRadial98DS::stdRadial98DS()
	{
		memset(this, 0x0, SIZE_RADIAL_98DS);
	}

	stdRadial98DC::stdRadial98DC()
	{
		memset(this, 0x0, SIZE_RADIAL_98DC);
	}
};