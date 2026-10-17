# Changelog

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
