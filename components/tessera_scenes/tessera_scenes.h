#pragma once
#include "esphome/core/component.h"
#include "esphome/components/smart_display/plugin_api.h"

namespace esphome::tessera_scenes {

// Scenes and lights as tiles on a gradient of their own colours. The plugin only registers its tile.
class TesseraScenes : public Component, public tessera::Plugin {
 public:
  void setup() override;
  float get_setup_priority() const override { return setup_priority::AFTER_WIFI; }
};

}  // namespace esphome::tessera_scenes
