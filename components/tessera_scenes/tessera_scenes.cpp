#include "tessera_scenes.h"
#include "scene_tile.h"

namespace esphome::tessera_scenes {

void TesseraScenes::setup() {
  add_tile("scene", [this]() { return make_scene_tile(this, false); });
  add_tile("named", [this]() { return make_scene_tile(this, true); });
}

}  // namespace esphome::tessera_scenes
