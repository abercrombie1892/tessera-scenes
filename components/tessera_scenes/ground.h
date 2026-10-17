#pragma once
#include <algorithm>
#include <cmath>
#include <vector>
#include "esphome/components/smart_display/plugin_api.h"

namespace esphome::tessera_scenes {

// A cover's colour as a lit ground for white words (the dashboard's glow_color): saturated a little more, and never
// brighter than `peak`, less for a grey cover.
inline lv_color_t lit(uint32_t tone, float peak) {
  float r = (tone >> 16) & 0xFF, g = (tone >> 8) & 0xFF, b = tone & 0xFF;
  const float hi = std::max({r, g, b, 1.0f}), lo = std::min({r, g, b}), avg = (r + g + b) / 3;
  r = std::max(0.0f, avg + (r - avg) * 1.2f), g = std::max(0.0f, avg + (g - avg) * 1.2f),
  b = std::max(0.0f, avg + (b - avg) * 1.2f);
  peak *= 0.4f + 0.6f * std::min(1.0f, (hi - lo) / hi * 2.0f);
  const float scale = peak / std::max({r, g, b, 1.0f});
  return lv_color_make((uint8_t) (r * scale), (uint8_t) (g * scale), (uint8_t) (b * scale));
}

// A picture of `w` x `h` whose colour at (x, y) `want` gives, Floyd-Steinberg dithered into RGB565 as the
// dashboard's glow is, so a soft blend shows no bands.
template <class Want> inline void dither(uint16_t *pixels, int w, int h, Want want_at) {
  std::vector<float> rows(6 * (w + 2), 0.0f);
  float *cur = rows.data(), *next = cur + 3 * (w + 2);
  for (int y = 0; y < h; y++) {
    for (int x = 0; x < w; x++) {
      float want[3];
      want_at(x, y, want);
      const int levels[3] = {31, 63, 31};
      int q[3];
      for (int c = 0; c < 3; c++) {
        const int i = (x + 1) * 3 + c;
        const float v = std::clamp(want[c] + cur[i], 0.0f, 255.0f);
        q[c] = (int) (v * levels[c] / 255.0f + 0.5f);
        const float e = v - q[c] * 255.0f / levels[c];
        cur[i + 3] += e * 7 / 16;
        next[i - 3] += e * 3 / 16;
        next[i] += e * 5 / 16;
        next[i + 3] += e / 16;
      }
      pixels[y * w + x] = (uint16_t) ((q[0] << 11) | (q[1] << 5) | q[2]);
    }
    std::swap(cur, next);
    std::fill(next, next + 3 * (w + 2), 0.0f);
  }
}

// A scene without a colour of its own: two hues from its entity id, always the same ones.
inline std::vector<uint32_t> colors_from_name(const std::string &entity) {
  uint32_t h = 2166136261u;  // FNV-1a
  for (unsigned char c : entity) h = (h ^ c) * 16777619u;
  const float hue = (h % 360) / 360.0f, hue2 = std::fmod(hue + 0.12f + ((h >> 9) % 10) / 100.0f, 1.0f);
  const auto rgb = [](float hh) {
    const float r = std::clamp(std::fabs(hh * 6 - 3) - 1, 0.0f, 1.0f), g = std::clamp(2 - std::fabs(hh * 6 - 2), 0.0f, 1.0f),
                b = std::clamp(2 - std::fabs(hh * 6 - 4), 0.0f, 1.0f);
    return ((uint32_t) (255 * (0.35f + 0.65f * r)) << 16) | ((uint32_t) (255 * (0.35f + 0.65f * g)) << 8) |
           (uint32_t) (255 * (0.35f + 0.65f * b));
  };
  return {rgb(hue), rgb(hue2)};
}

// The gradient from the top left to the foot right through the colours, each as a lit ground for white words (one
// colour: light to dark), dithered into RGB565.
inline void gradient(uint16_t *pixels, int w, int h, const std::vector<uint32_t> &colors) {
  std::vector<lv_color_t> stops;
  if (colors.size() == 1) {
    stops = {lit(colors[0], 150), lit(colors[0], 62)};
  } else {
    for (size_t i = 0; i < colors.size(); i++) stops.push_back(lit(colors[i], i + 1 == colors.size() ? 105 : 140));
  }
  const int last = (int) stops.size() - 1;
  dither(pixels, w, h, [&](int x, int y, float *want) {
    const float t = std::min(1.0f, 0.55f * y / std::max(1, h - 1) + 0.45f * x / std::max(1, w - 1));
    const float pos = t * last;
    const int i = std::min(last - 1, (int) pos);
    const float f = pos - i;
    want[0] = stops[i].red + (stops[i + 1].red - stops[i].red) * f;
    want[1] = stops[i].green + (stops[i + 1].green - stops[i].green) * f;
    want[2] = stops[i].blue + (stops[i + 1].blue - stops[i].blue) * f;
  });
}

}  // namespace esphome::tessera_scenes
