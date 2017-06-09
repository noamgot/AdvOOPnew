
#include "CommonUtilities.h"
#include <filesystem>
#include <Windows.h>

using namespace std;

namespace CommonUtilities
{
	Ship::eShipType CommonUtilities::charToShipType(char c)
	{
		switch (toupper(c))
		{
		case BOAT:
			return Ship::eShipType::SHIP_TYPE_B;
		case MISSLE_SHIP:
			return Ship::eShipType::SHIP_TYPE_P;
		case SUBMARINE:
			return Ship::eShipType::SHIP_TYPE_M;
		case DESTROYER:
			return Ship::eShipType::SHIP_TYPE_D;
		default:
			return Ship::eShipType::SHIP_TYPE_ERROR;
		}
	}


}