#include "tessera_scenes.h"
#include "scene_tile.h"

namespace esphome::tessera_scenes {

void TesseraScenes::setup() {
  add_tile("scene", [this]() { return make_scene_tile(this); });
}

}  // namespace esphome::tessera_scenes
