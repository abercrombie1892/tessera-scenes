# Scenes for Tessera

Scenes and lights as tiles on a gradient of their own colours, for the ESP32-P4 screens. The name stands in white on a
soft ground in the colours of the scene; a tap starts it. No connection of its own: Tessera and Home Assistant do the
work.

## Set up

1. In Tessera, Plugins, add this repository by its link and choose your screen. The screen builds once.
2. In Layout, add the tile **Scene / Light** from the library (search for it) and choose the scene or the light under Entity.
3. Optional: "Own colours" takes up to three colours as `#RRGGBB`, separated by commas.

## Good to know

- The colours of the Hue scenes of the kitchen are kept in `components/tessera_scenes/scene_colors.h`, by entity id,
  as the Hue bridge has them. A scene that is not there gets two hues from its name until it is added.
- A light takes its own colour; one that is off is dimmed.
