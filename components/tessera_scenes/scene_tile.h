#pragma once
#include "esphome/components/smart_display/plugin_api.h"

namespace esphome::tessera_scenes {

class TesseraScenes;

// A scene or a light as a tile on a ground of colour: a soft gradient in the colours of the scene (from the Hue
// bridge) or of the light, the name in white on it. A tap starts the scene or switches the light.
// `named`: the tile has no entity of its own; its name picks a scene of the plugin's table ("Miami").
tessera::Tile *make_scene_tile(TesseraScenes *plugin, bool named);

}  // namespace esphome::tessera_scenes
