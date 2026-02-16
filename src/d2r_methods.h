#pragma once

#include "d2r_structs.h"

namespace d2r {

D2UnitStrc* GetUnit(uint32_t id, uint32_t type);
uint32_t GetPlayerId(uint32_t index);
D2UnitStrc* GetPlayerUnit(uint32_t index);
bool AutomapReveal(D2ActiveRoomStrc* hRoom);
bool RevealLevelById(uint32_t id);

// Automap marker functions
bool AddAutomapMarker(int32_t posX, int32_t posY, uint16_t cellNo);
void ClearAutomapMarkers();
bool TestAutomapCells(int32_t baseX, int32_t baseY, uint16_t startId, uint16_t count);

}
