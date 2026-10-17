#pragma once
#include "esphome/components/smart_display/plugin_api.h"

namespace esphome::tessera_scenes {

class TesseraScenes;

// A scene or a light as a tile on a ground of colour: a soft gradient in the colours of the scene (from the Hue
// bridge) or of the light, the name in white on it. A tap starts the scene or switches the light.
tessera::Tile *make_scene_tile(TesseraScenes *plugin);

// The scenes of a room as a grid of tiles, each on a gradient of its own colours: the room is an option of the tile
// ("Küche"), a second option names and orders the scenes, else every scene of the room is there. A tap on one starts it.
tessera::Tile *make_scene_grid(TesseraScenes *plugin);

}  // namespace esphome::tessera_scenes
