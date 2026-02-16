'use strict';

const { UnitTypes } = require('d2r/types');

const binding = internalBinding('d2r');

const MarkerType = {
  Monster: 300,
};

class AutomapMarkers {
  constructor(objectManager) {
    this._objMgr = objectManager;
    this._enabled = true;
  }

  get enabled() { return this._enabled; }
  set enabled(v) { this._enabled = v; }

  tick() {
    binding.clearAutomapMarkers();
    if (!this._enabled || !this._objMgr.me) return;

    this._drawMonsters();
  }

  _drawMonsters() {
    const monsters = this._objMgr.getUnits(UnitTypes.Monster);
    for (const [id, monster] of monsters) {
      if (!monster.isAlive) continue;
      binding.addAutomapMarker(monster.posX, monster.posY, MarkerType.Monster);
    }
  }
}

module.exports = { AutomapMarkers, MarkerType };
