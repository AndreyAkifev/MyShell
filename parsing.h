#pragma once

#include "structures.h"

bool ProcessLeftArrow(CommandsList* list, Vector* cmd, int* c);

bool ProcessRightArrow(CommandsList* list, Vector* cmd, int* c);

bool ParseInput(int* c);