#include <pebble.h>

static const int16_t S_Y_TIME      = -80;
static const int16_t S_Y_DATE      = -92;
static const int16_t S_Y_LINE      = -70;
static const int16_t S_Y_DAY       = -88;
static const int16_t S_Y_TIMESTAMP = 6;

static GRect s_bounds;
static Window    *s_window;
static Layer     *s_window_layer;
static Layer     *s_draw_layer;
static TextLayer *s_time_layer;
static TextLayer *s_date_layer;
static TextLayer *s_day_layer;
static TextLayer *s_timestamp_layer;

static int16_t get_y(int16_t y) {
  GRect unobstructed_bounds = layer_get_unobstructed_bounds(s_window_layer);
  int16_t offset = (unobstructed_bounds.size.h - s_bounds.size.h) / 4;

  if (y >= 0) {
    return y + offset;
  } else {
    return unobstructed_bounds.size.h + y;
  }
}

static void update_time() {
  time_t timestamp = time(NULL);
  struct tm *tick_time = localtime(&timestamp);

  static char time_buffer[8];
  strftime(time_buffer, sizeof(time_buffer), "%H:%M", tick_time);
  text_layer_set_text(s_time_layer, time_buffer);

  static char date_buffer[15];
  strftime(date_buffer, sizeof(date_buffer), "%B %d", tick_time);
  text_layer_set_text(s_date_layer, date_buffer);

  static char day_buffer[5];
  strftime(day_buffer, sizeof(day_buffer), "%a", tick_time);
  text_layer_set_text(s_day_layer, day_buffer);

  static char timestamp_buffer[13];
  snprintf(timestamp_buffer, sizeof(timestamp_buffer), "%d", (int) timestamp);
  text_layer_set_text(s_timestamp_layer, timestamp_buffer);
}

static void update_offset() {
  GRect timestamp_frame = layer_get_frame(text_layer_get_layer(s_timestamp_layer));
  timestamp_frame.origin.y = get_y(S_Y_TIMESTAMP);
  layer_set_frame(text_layer_get_layer(s_timestamp_layer), timestamp_frame);

  GRect date_frame = layer_get_frame(text_layer_get_layer(s_date_layer));
  date_frame.origin.y = get_y(S_Y_DATE);
  layer_set_frame(text_layer_get_layer(s_date_layer), date_frame);

  GRect day_frame = layer_get_frame(text_layer_get_layer(s_day_layer));
  day_frame.origin.y = get_y(S_Y_DAY);
  layer_set_frame(text_layer_get_layer(s_day_layer), day_frame);

  GRect time_frame = layer_get_frame(text_layer_get_layer(s_time_layer));
  time_frame.origin.y = get_y(S_Y_TIME);
  layer_set_frame(text_layer_get_layer(s_time_layer), time_frame);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}

static void unobstructed_change(AnimationProgress progress, void *context) {
  update_offset();
}

static void display_layer_update_callback(Layer *layer, GContext *ctx) {
  graphics_context_set_stroke_color(ctx, GColorBlack);
  graphics_draw_rect(ctx, GRect(0, get_y(S_Y_LINE), s_bounds.size.w, 2));
}

static void window_load(Window *window) {
  s_window_layer = window_get_root_layer(window);
  s_bounds = layer_get_bounds(s_window_layer);

  s_draw_layer = layer_create(s_bounds);
  layer_set_update_proc(s_draw_layer, display_layer_update_callback);
  layer_add_child(s_window_layer, s_draw_layer);

  s_timestamp_layer = text_layer_create(GRect(3, get_y(S_Y_TIMESTAMP), s_bounds.size.w - 6, 16));
  text_layer_set_background_color(s_timestamp_layer, GColorClear);
  text_layer_set_text_color(s_timestamp_layer, GColorBlack);
  text_layer_set_font(s_timestamp_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  text_layer_set_text_alignment(s_timestamp_layer, GTextAlignmentLeft);
  layer_add_child(s_draw_layer, text_layer_get_layer(s_timestamp_layer));

  s_date_layer = text_layer_create(GRect(3, get_y(S_Y_DATE), s_bounds.size.w - 6, 20));
  text_layer_set_background_color(s_date_layer, GColorClear);
  text_layer_set_text_color(s_date_layer, GColorBlack);
  text_layer_set_font(s_date_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  text_layer_set_text_alignment(s_date_layer, GTextAlignmentLeft);
  layer_add_child(s_draw_layer, text_layer_get_layer(s_date_layer));

  s_day_layer = text_layer_create(GRect(s_bounds.size.w - 53, get_y(S_Y_DAY), 50, 16));
  text_layer_set_background_color(s_day_layer, GColorClear);
  text_layer_set_text_color(s_day_layer, GColorBlack);
  text_layer_set_font(s_day_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD));
  text_layer_set_text_alignment(s_day_layer, GTextAlignmentRight);
  layer_add_child(s_draw_layer, text_layer_get_layer(s_day_layer));

  s_time_layer = text_layer_create(GRect(0, get_y(S_Y_TIME), s_bounds.size.w, 50));
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorBlack);
  text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_ROBOTO_BOLD_SUBSET_49));
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  layer_add_child(s_draw_layer, text_layer_get_layer(s_time_layer));

  update_time();
}

static void window_unload(Window *window) {
  text_layer_destroy(s_date_layer);
  text_layer_destroy(s_day_layer);
  text_layer_destroy(s_time_layer);
  text_layer_destroy(s_timestamp_layer);
  layer_destroy(s_draw_layer);
}

static void init(void) {
  s_window = window_create();
  window_set_window_handlers(s_window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });

  window_stack_push(s_window, true);
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);

  UnobstructedAreaHandlers unobstruct_handler = {
    .change = unobstructed_change,
  };
  unobstructed_area_service_subscribe(unobstruct_handler, NULL);
}

static void deinit(void) {
  window_destroy(s_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();

  return 0;
}
