#pragma once

#include "Ship.h"

#define PARAM_QUIET "-quiet"
#define PARAM_DELAY "-delay"

eShipType charToShipType(char c);

int calculateSinkScore(char c);

void changeCurrentPlayer(int *attackerNum, int *defenderNum);