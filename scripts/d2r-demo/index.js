'use strict';

import { ObjectManager, UnitTypes, DebugPanel, revealLevel } from 'nyx:d2r';
import { withGameLock } from 'nyx:memory';

const binding = internalBinding('d2r');

try {
  const objMgr = new ObjectManager();
  const debugPanel = new DebugPanel(objMgr);

  objMgr.tick();

  const players = objMgr.getUnits(UnitTypes.Player);
  const monsters = objMgr.getUnits(UnitTypes.Monster);
  const items = objMgr.getUnits(UnitTypes.Item);
  const objects = objMgr.getUnits(UnitTypes.Object);
  const missiles = objMgr.getUnits(UnitTypes.Missile);
  const tiles = objMgr.getUnits(UnitTypes.Tile);

  console.log(`Objects`);
  console.log(`  Players:  ${players.size}`);
  console.log(`  Monsters: ${monsters.size}`);
  console.log(`  Items:    ${items.size}`);
  console.log(`  Objects:  ${objects.size}`);
  console.log(`  Missiles: ${missiles.size}`);
  console.log(`  Tiles:    ${tiles.size}`);

  if (objMgr.me) {
    console.log(`\nLocal player: id=${objMgr.me.id} at (${objMgr.me.posX}, ${objMgr.me.posY})`);
    console.log(JSON.stringify(objMgr.me, (_, v) => typeof v === 'bigint' ? v.toString(16) : v, 2));
  }

  // Show first few monsters
  let count = 0;
  for (const [id, monster] of monsters) {
    if (count >= 5) break;
    console.log(`  Monster id=${id} classId=${monster.classId} at (${monster.posX}, ${monster.posY}) alive=${monster.isAlive}`);
    count++;
  }

  // --- Automap cell ID discovery ---
  // Places a grid of automap cells with sequential IDs near the player.
  // Change startId/count and re-inject to scan different ID ranges.
  // Known valid ranges from log: 100-115, 257-273, 307, 454-456, 497-498
  let cellTestDone = false;
  const CELL_TEST_ENABLED = true;
  const CELL_TEST_START_ID = 300;
  const CELL_TEST_COUNT = 10;

  let revealed_levels = [];
  setInterval(() => {
    objMgr.tick();
    debugPanel.refresh();
    const me = objMgr.me;
    if (!me && revealed_levels.length > 0) {
      console.log("Resetting revealed levels");
      revealed_levels = [];
      cellTestDone = false;
    }
    if (me) {
      const currentLevelId = me.path?.room?.drlgRoom?.level?.id;
      if (currentLevelId !== undefined && !revealed_levels.includes(currentLevelId)) {
        withGameLock(_ => {
          if (binding.revealLevel(currentLevelId)) {
            console.log(`Revealed level ${currentLevelId}`);
            revealed_levels.push(currentLevelId);
          }
        });
      }

      // Place test automap cells once per session
      if (CELL_TEST_ENABLED && !cellTestDone) {
        cellTestDone = true;
        withGameLock(_ => {
          const ok = binding.testAutomapCells(me.posX, me.posY, CELL_TEST_START_ID, CELL_TEST_COUNT);
          console.log(`testAutomapCells(${me.posX}, ${me.posY}, ${CELL_TEST_START_ID}, ${CELL_TEST_COUNT}) = ${ok}`);
        });
      }
    }
  }, 20);
} catch (err) {
  console.error(err.message);
  console.error(err.stack);
}
