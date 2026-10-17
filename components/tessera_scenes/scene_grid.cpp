#include <cstring>
#include <string>
#include <vector>
#include "esp_heap_caps.h"
#include "ground.h"
#include "scene_colors.h"
#include "scene_tile.h"
#include "tessera_scenes.h"

// private LVGL header (misc/cache/instance/lv_image_cache.h), not in lvgl.h
extern "C" void lv_image_cache_drop(const void *src);

namespace esphome::tessera_scenes {

using tessera::Font;
namespace ui = tessera::ui;

namespace {

std::string lower(std::string text) {
  for (char &c : text) c = (char) tolower((unsigned char) c);  // ASCII only: umlauts must match as typed
  return text;
}

std::string trimmed(const std::string &text) {
  const size_t a = text.find_first_not_of(" \t"), b = text.find_last_not_of(" \t");
  return a == std::string::npos ? "" : text.substr(a, b - a + 1);
}

}  // namespace

class SceneGrid : public tessera::Tile {
 public:
  explicit SceneGrid(TesseraScenes *plugin) : plugin_(plugin) {}
  ~SceneGrid() override {
    for (Cell &cell : cells_) {
      if (cell.pixels == nullptr) continue;
      lv_image_cache_drop(&cell.dsc);
      heap_caps_free(cell.pixels);
    }
  }

  void create(const tessera::TileContext &c) override {
    const std::string room = trimmed(c.options["room"] | "");
    const std::string wanted = c.options["scenes"] | "";
    pick_(room, wanted);
    if (picked_.empty()) {
      lv_obj_t *note = ui::label(c.parent, Font::BODY_LARGE, theme::MUTED);
      lv_obj_set_size(note, c.width, ui::line_height(Font::BODY_LARGE));
      ui::set_text(note, room.empty() ? plugin_->text("no_room") : plugin_->text("unknown_room") + std::string(" ") + room);
      return;
    }
    // Columns of at least 22 mm, rows that share the height: as many cells as the tile takes, never smaller than 9 mm.
    const int gap = ui::px(8);
    const int cols = std::clamp(c.width / ui::mm(22), 1, 8);
    const int cell_w = (c.width - gap * (cols - 1)) / cols;
    int rows = ((int) picked_.size() + cols - 1) / cols;
    const int min_h = ui::mm(9);
    rows = std::max(1, std::min(rows, (c.height + gap) / (min_h + gap)));
    const int cell_h = std::min(cell_w * 3 / 4, (c.height - gap * (rows - 1)) / rows);
    const int shown = std::min((int) picked_.size(), rows * cols);
    cells_.resize(shown);
    for (int i = 0; i < shown; i++) build_cell_(c.parent, i, (i % cols) * (cell_w + gap), (i / cols) * (cell_h + gap), cell_w, cell_h);
  }

  void on_tick(uint32_t) override {}

 protected:
  struct Cell {
    const SceneColors *scene = nullptr;
    lv_obj_t *box = nullptr, *image = nullptr;
    uint8_t *pixels = nullptr;
    lv_image_dsc_t dsc{};
  };

  // The scenes of the room, in the order of `wanted` ("Kochen, Hell, Miami") or, without it, by name.
  void pick_(const std::string &room, const std::string &wanted) {
    if (room.empty()) return;
    std::vector<const SceneColors *> all;
    for (const SceneColors &s : SCENE_COLORS)
      if (lower(s.room) == lower(room)) all.push_back(&s);
    if (trimmed(wanted).empty()) {
      picked_ = all;
      return;
    }
    size_t at = 0;
    while (at <= wanted.size()) {
      const size_t comma = wanted.find(',', at);
      const std::string name = lower(trimmed(wanted.substr(at, comma == std::string::npos ? std::string::npos : comma - at)));
      for (const SceneColors *s : all)
        if (lower(s->name) == name) {
          picked_.push_back(s);
          break;
        }
      if (comma == std::string::npos) break;
      at = comma + 1;
    }
  }

  void build_cell_(lv_obj_t *parent, int i, int x, int y, int w, int h) {
    Cell &cell = cells_[i];
    cell.scene = picked_[i];
    cell.box = lv_obj_create(parent);
    lv_obj_remove_style_all(cell.box);
    lv_obj_remove_flag(cell.box, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_pos(cell.box, x, y);
    lv_obj_set_size(cell.box, w, h);
    lv_obj_set_style_radius(cell.box, ui::px(14), 0);
    lv_obj_set_style_clip_corner(cell.box, true, 0);
    lv_obj_set_style_bg_color(cell.box, lv_color_hex(0x303540), 0);
    lv_obj_set_style_bg_opa(cell.box, LV_OPA_COVER, 0);
    lv_obj_add_flag(cell.box, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_user_data(cell.box, const_cast<SceneColors *>(cell.scene));
    lv_obj_add_event_cb(cell.box, on_press_, LV_EVENT_ALL, this);

    cell.image = lv_image_create(cell.box);
    lv_obj_remove_flag(cell.image, LV_OBJ_FLAG_CLICKABLE);
    const size_t bytes = (size_t) w * h * 2;
    cell.pixels = static_cast<uint8_t *>(heap_caps_malloc(bytes, MALLOC_CAP_SPIRAM));
    if (cell.pixels != nullptr) {
      const SceneColors &s = *cell.scene;
      const std::vector<uint32_t> colors = s.count > 0 ? std::vector<uint32_t>(s.colors, s.colors + s.count)
                                                       : colors_from_name(s.entity);
      gradient(reinterpret_cast<uint16_t *>(cell.pixels), w, h, colors);
      cell.dsc.header.magic = LV_IMAGE_HEADER_MAGIC;
      cell.dsc.header.cf = LV_COLOR_FORMAT_RGB565;
      cell.dsc.header.w = w;
      cell.dsc.header.h = h;
      cell.dsc.header.stride = w * 2;
      cell.dsc.data = cell.pixels;
      cell.dsc.data_size = bytes;
      lv_image_set_src(cell.image, &cell.dsc);
    }

    // The name as large as the cell's width takes it, white, at the foot.
    const Font steps[] = {Font::TITLE, Font::BODY_LARGE, Font::BODY};
    Font font = steps[2];
    for (int k = h >= ui::mm(20) ? 0 : 1; k < 3; k++) {
      font = steps[k];
      if (ui::text_width(cell.scene->name, font) <= w - ui::px(14)) break;
    }
    lv_obj_t *label = ui::label(cell.box, font, theme::INK);
    lv_obj_set_style_text_color(label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_pos(label, ui::px(8), h - ui::line_height(font) - ui::px(6));
    lv_obj_set_size(label, w - ui::px(12), ui::line_height(font));
    ui::set_text(label, cell.scene->name);
  }

  // A press dims the cell's picture a little, a tap starts the scene.
  static void on_press_(lv_event_t *e) {
    auto *grid = static_cast<SceneGrid *>(lv_event_get_user_data(e));
    auto *box = static_cast<lv_obj_t *>(lv_event_get_current_target(e));
    const lv_event_code_t code = lv_event_get_code(e);
    const auto *scene = static_cast<const SceneColors *>(lv_obj_get_user_data(box));
    lv_obj_t *image = lv_obj_get_child(box, 0);
    if (code == LV_EVENT_PRESSED) {
      lv_obj_set_style_image_recolor(image, lv_color_hex(0xFFFFFF), 0);
      lv_obj_set_style_image_recolor_opa(image, 70, 0);
    } else if (code == LV_EVENT_RELEASED || code == LV_EVENT_PRESS_LOST) {
      lv_obj_set_style_image_recolor_opa(image, 0, 0);
    } else if (code == LV_EVENT_CLICKED && scene != nullptr) {
      tessera::action("scene.turn_on", scene->entity);
    }
    (void) grid;
  }

  TesseraScenes *plugin_;
  std::vector<const SceneColors *> picked_;
  std::vector<Cell> cells_;
};

tessera::Tile *make_scene_grid(TesseraScenes *plugin) { return new SceneGrid(plugin); }

}  // namespace esphome::tessera_scenes
