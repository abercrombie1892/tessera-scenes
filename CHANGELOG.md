# Changelog

## 0.4.0
- One tile again: the "Scenes of a room" grid of 0.3.x is gone. The plugin has the tile "Scene / Light" only.

## 0.3.2
- The tile "Scene / Light" calls a scene it knows by its short name ("Miami") instead of Home Assistant's own
  ("Hauptlicht Hue Miami"), unless the tile has a name of its own.

## 0.3.1
- The room of the "Scenes of a room" tile is a list to choose from (every room of the house) instead of a text.

## 0.3.0
- New tile "Scenes of a room": type a room (as the Hue app names it) and the tile shows its scenes as a grid, each on
  a gradient of its own colours, the name in white on it; a tap starts the scene. The option Scenes names and orders
  them ("Kochen, Hell, Miami"), else every scene of the room is there. The table holds the 148 scenes of the house,
  110 of them with the colours of the Hue bridge.
- The tile "Scene by name" of 0.2.0 is gone: Home Assistant's events cannot place a plugin tile anyway.

## 0.2.1
- The name on a tile is as large as the tile's width takes it, a step smaller for a long one ("Tropendämmerung"
  on a small tile).

## 0.2.0
- New tile "Scene (by name)": no entity to choose, the name of the tile ("Miami", "Wald 2") picks the scene from the
  plugin's table. Tiles of it can be placed from Home Assistant events, which cannot set a plugin tile's entity.

## 0.1.0
- The Scene / Light tile of tessera_music, as a plugin of its own: a scene or a light on a gradient of its own colours,
  the name in white on it. The colours of the Hue scenes come from the bridge (up to three, kept in the plugin by
  entity id); a light takes its own colour, one that is off is dimmed, any other scene gets two hues from its name.
  Option "Own colours" (#RRGGBB, up to three) overrides them. A tap starts the scene or switches the light.
