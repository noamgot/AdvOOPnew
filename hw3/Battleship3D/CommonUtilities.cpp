
#include "CommonUtilities.h"
#include <filesystem>
#include <Windows.h>

using namespace std;

namespace CommonUtilities
{
	eShipType CommonUtilities::charToShipType(char c)
	{
		switch (toupper(c))
		{
		case BOAT:
			return eShipType::SHIP_TYPE_B;
		case MISSLE_SHIP:
			return eShipType::SHIP_TYPE_P;
		case SUBMARINE:
			return eShipType::SHIP_TYPE_M;
		case DESTROYER:
			return eShipType::SHIP_TYPE_D;
		default:
			return eShipType::SHIP_TYPE_ERROR;
		}
	}


}