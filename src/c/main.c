#include "main.h"

#include "pebble.h"

static Window *s_main_window;
static Layer *s_date_layer, *s_simple_bg_layer, *s_hour_layer, *s_min_layer;
static TextLayer *s_day_label, *s_num_label;

static int s_hour_x = 0;
static int s_hour_y = 0;
static int s_min_x = 30;
static int s_min_y = 50;


static char s_num_buffer[4], s_day_buffer[6], s_min_buffer[4];
static char n1_buf[16], n2_buf[16], n3_buf[16];
static int e1_buf = 0; 
static int e2_buf = 0;
static int e3_buf = 0; 
static int s1_buf = 0; 
static int s2_buf = 0; 
static int s3_buf = 0;


static void draw_arc(GContext *ctx, int *start_degrees, int *end_degrees, int *radius, GPoint *center, GColor *color) {
	graphics_context_set_stroke_width(ctx, 2);
	struct GPoint real_center = *center;
	int start = *start_degrees;
	int end = *end_degrees;
	if(end == 59) {
		end = 60;
	}
	for(int i = start * 10; i <= end * 10; i ++) {
		int32_t angle = TRIG_MAX_ANGLE * i / 600;
  		GPoint end_point = {
    		.x = (int16_t)(sin_lookup(angle) * *radius / TRIG_MAX_RATIO) + real_center.x,
    		.y = (int16_t)(-cos_lookup(angle) * *radius / TRIG_MAX_RATIO) + real_center.y,
  		};
		graphics_context_set_stroke_color(ctx, COLOR_FALLBACK(*color, GColorWhite));
		graphics_draw_line(ctx, real_center, end_point);
		if(i >= (end - 1)*10) {
			graphics_context_set_stroke_color(ctx, COLOR_FALLBACK(*color, GColorBlack));
			graphics_draw_line(ctx, real_center, end_point);
		}
	}
	graphics_context_set_stroke_width(ctx, 1);
}

static void date_update_proc(Layer *layer, GContext *ctx) {
  time_t now = time(NULL);
  struct tm *t = localtime(&now);

  strftime(s_day_buffer, sizeof(s_day_buffer), "%a", t);
  text_layer_set_text(s_day_label, s_day_buffer);

  strftime(s_num_buffer, sizeof(s_num_buffer), "%d", t);
  text_layer_set_text(s_num_label, s_num_buffer);
}
static void bg_update_proc(Layer *layer, GContext *ctx) {
	GRect bounds = layer_get_bounds(layer);
 	GPoint center = grect_center_point(&bounds);
	//draw background layer, pretty simple, just black rectangle
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_rect(ctx, layer_get_bounds(layer), 0, GCornerNone);

  // dot in the middle
  graphics_context_set_fill_color(ctx, GColorFromRGB(255, 150, 0));
  graphics_fill_circle(ctx, center, 24);
}

static void draw_guide_marks(GContext *ctx) {
	graphics_context_set_stroke_color(ctx, GColorWhite);
	
	GPoint start1;
	GPoint end1;
	GPoint start2;
	GPoint end2;
  time_t now = time(NULL);
  struct tm *t = localtime(&now);
	struct GPoint hour_hand = GPoint(s_hour_x, s_hour_y);
	
  if(t->tm_min < 15){
		GPoint start1 = {.x = hour_hand.x + 8, .y = hour_hand.y,};
		GPoint end1 = {.x = hour_hand.x + 30, .y = hour_hand.y,};
		
    GPoint start2 = {.x = hour_hand.x, .y = hour_hand.y - 8,};
    GPoint end2 = {.x = hour_hand.x, .y = hour_hand.y - 30,};
		
			graphics_draw_line(ctx, start1, end1);
		graphics_draw_line(ctx, start2, end2);
	 } else if(t->tm_min < 30){
     GPoint start1 = {.x = hour_hand.x + 8, .y = hour_hand.y,};
     GPoint end1 = {.x = hour_hand.x + 30, .y = hour_hand.y,};

		 GPoint start2 = {.x = hour_hand.x, .y = hour_hand.y + 8,};
     GPoint end2 = {.x = hour_hand.x, .y = hour_hand.y + 30,};
		
			graphics_draw_line(ctx, start1, end1);
		graphics_draw_line(ctx, start2, end2);
      } else if(t->tm_min < 45){
     GPoint start1 = {.x = hour_hand.x - 8, .y = hour_hand.y,};
     GPoint end1 = {.x = hour_hand.x - 30, .y = hour_hand.y,};
    
     GPoint start2 = {.x = hour_hand.x, .y = hour_hand.y + 8,};
     GPoint end2 = {.x = hour_hand.x, .y = hour_hand.y + 30,};
		
			graphics_draw_line(ctx, start1, end1);
			graphics_draw_line(ctx, start2, end2);
   }  else {
     GPoint start1 = {.x = hour_hand.x - 8, .y = hour_hand.y,};
     GPoint end1 = {.x = hour_hand.x - 30, .y = hour_hand.y,};
    
     GPoint start2 = {.x = hour_hand.x, .y = hour_hand.y - 8,};
     GPoint end2 = {.x = hour_hand.x, .y = hour_hand.y - 30,};
		
			graphics_draw_line(ctx, start1, end1);
			graphics_draw_line(ctx, start2, end2);
   };
	graphics_draw_line(ctx, start1, end1);
	graphics_draw_line(ctx, start2, end2);
}

static void hour_update_proc(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);
  GPoint center = grect_center_point(&bounds);
	
  int16_t hour_hand_length = 50;
  
  time_t now = time(NULL);
  struct tm *t = localtime(&now);
  
  int32_t hour_angle = TRIG_MAX_ANGLE * t->tm_hour / 12;
  GPoint hour = {
    .x = (int16_t)(sin_lookup(hour_angle) * (int32_t)hour_hand_length / TRIG_MAX_RATIO) + center.x,
    .y = (int16_t)(-cos_lookup(hour_angle) * (int32_t)hour_hand_length / TRIG_MAX_RATIO) + center.y,
  	};
	graphics_context_set_stroke_color(ctx, GColorBlack);
	graphics_context_set_fill_color(ctx,  COLOR_FALLBACK(GColorFromRGB(180, 0, 0), GColorWhite));
	graphics_fill_circle(ctx, hour, 8);
	graphics_draw_circle(ctx, hour, 8);
	
	s_hour_x = hour.x;
	s_hour_y = hour.y;
}

static void min_update_proc(Layer *layer, GContext *ctx) {
	
	int radius = 20;
	int start = (int)s1_buf;
	int end = (int)e1_buf;
	APP_LOG(APP_LOG_LEVEL_INFO, "%d", end);
	APP_LOG(APP_LOG_LEVEL_INFO, "%d", start);
	GPoint hour = GPoint(s_hour_x, s_hour_y);
	
	if(start < 60 && end < 60 && end > 0) {
	
		draw_arc(ctx, &start, &end, &radius, &hour, &GColorFromRGB(11, 11, 130));
		start = (int)s2_buf;
		end = (int)e2_buf;
		APP_LOG(APP_LOG_LEVEL_INFO, "%d", end);
		APP_LOG(APP_LOG_LEVEL_INFO, "%d", start);
		if(start < 60 && end < 60) {
	
			draw_arc(ctx, &start, &end, &radius, &hour, &GColorFromRGB(237, 81, 28));
	
			start = (int)s3_buf;
			end = (int)e3_buf;
			APP_LOG(APP_LOG_LEVEL_INFO, "%d", end);
			APP_LOG(APP_LOG_LEVEL_INFO, "%d", start);
			if(start < 60 && end <60) {
	
				draw_arc(ctx, &start, &end, &radius, &hour, &GColorFromRGB(148, 227, 20));
			}
		}
	}
	
  time_t now = time(NULL);
  struct tm *t = localtime(&now);
	
  int16_t min_hand_length = 24;
	
	struct GPoint real_hour = GPoint(s_hour_x, s_hour_y);
	
	APP_LOG(APP_LOG_LEVEL_DEBUG, "%d", real_hour.x);
  
  int32_t min_angle = TRIG_MAX_ANGLE * t->tm_min / 60;
  GPoint min = {
    .x = (int16_t)(sin_lookup(min_angle) * (int32_t)min_hand_length / TRIG_MAX_RATIO) + real_hour.x,
    .y = (int16_t)(-cos_lookup(min_angle) * (int32_t)min_hand_length / TRIG_MAX_RATIO) + real_hour.y,
  	};
	graphics_context_set_fill_color(ctx,  COLOR_FALLBACK(GColorFromRGB(0, 125, 200), GColorWhite));
	graphics_context_set_stroke_color(ctx, GColorBlack);
	graphics_fill_circle(ctx, min, 5);
	graphics_draw_circle(ctx, min, 5);
	draw_guide_marks(ctx);
	
	s_min_x = min.x;
	s_min_y = min.y;
}

static void main_window_load(Window *window) {
	Layer *window_layer = window_get_root_layer(window);
	GRect bounds = layer_get_bounds(window_layer);
 	GPoint center = grect_center_point(&bounds);
	//make background layer
  	s_simple_bg_layer = layer_create(bounds);
  	layer_set_update_proc(s_simple_bg_layer, bg_update_proc);
  	layer_add_child(window_layer, s_simple_bg_layer);
	
	//make date layer
	s_date_layer = layer_create(bounds);
 	layer_set_update_proc(s_date_layer, date_update_proc);
  	layer_add_child(window_layer, s_date_layer);
	//make day layer, a child of date layer, and text layer type with the day name
  s_day_label = text_layer_create(GRect(center.x-22, center.y - 10, 27, 20));
  text_layer_set_text(s_day_label, s_day_buffer);
  text_layer_set_background_color(s_day_label, GColorFromRGB(255, 150, 0));
  text_layer_set_text_color(s_day_label, GColorBlack);
  text_layer_set_font(s_day_label, fonts_get_system_font(FONT_KEY_GOTHIC_18));

  layer_add_child(s_date_layer, text_layer_get_layer(s_day_label));
	//make num layer, a child of date layer, and text layer type with the day number

  s_num_label = text_layer_create(GRect(center.x+5, center.y - 10, 18, 20));
  text_layer_set_text(s_num_label, s_num_buffer);
  text_layer_set_background_color(s_num_label, GColorFromRGB(255, 150, 0));
  text_layer_set_text_color(s_num_label, GColorBlack);
  text_layer_set_font(s_num_label, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));

  layer_add_child(s_date_layer, text_layer_get_layer(s_num_label));
	
	//make min layer
	s_min_layer = layer_create(bounds);
 	layer_set_update_proc(s_min_layer, min_update_proc);
  	layer_add_child(window_layer, s_min_layer);
	
	//make hour layer
	s_hour_layer = layer_create(bounds);
 	layer_set_update_proc(s_hour_layer, hour_update_proc);
  	layer_add_child(window_layer, s_hour_layer);
	
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
	APP_LOG(APP_LOG_LEVEL_DEBUG, " tick");
	if(units_changed & MINUTE_UNIT) {
		APP_LOG(APP_LOG_LEVEL_DEBUG, "minute tick");
		layer_mark_dirty(s_min_layer);
	}
	if(units_changed & HOUR_UNIT) {
		APP_LOG(APP_LOG_LEVEL_DEBUG, "hour tick");
		layer_mark_dirty(s_hour_layer);
		// Begin dictionary
  		DictionaryIterator *iter;
  		app_message_outbox_begin(&iter);

  		// Add a key-value pair
  		dict_write_uint8(iter, 0, 0);

  		// Send the message!
  		app_message_outbox_send();
			}
			if(units_changed & DAY_UNIT) {
				layer_mark_dirty(s_date_layer);
	}
}

static void main_window_unload(Window *window) {
	//burn all the evidence
	layer_destroy(s_date_layer);
	text_layer_destroy(s_day_label);
	text_layer_destroy(s_num_label);
	layer_destroy(s_hour_layer);
	layer_destroy(s_min_layer);
	layer_destroy(s_simple_bg_layer);
}



static void deinit() {
	window_destroy(s_main_window);
}



////////////////////////////////////////////////////////////////////////////////////////////
bool comm_is_js_ready() {
  return s_js_ready;
}



static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
	Tuple *ready_tuple = dict_find(iterator, MESSAGE_KEY_JSReady);
  if(ready_tuple) {
    // PebbleKit JS is ready! Safe to send messages
    s_js_ready = true;
		APP_LOG(APP_LOG_LEVEL_INFO, "pebblekit js ready");
  }
	APP_LOG(APP_LOG_LEVEL_INFO, "Recieved message");
	// Read tuples for data
	Tuple *end1_tuple = dict_find(iterator, MESSAGE_KEY_End1);
	Tuple *end2_tuple = dict_find(iterator, MESSAGE_KEY_End2);
	Tuple *end3_tuple = dict_find(iterator, MESSAGE_KEY_End3);
	
	Tuple *start1_tuple = dict_find(iterator, MESSAGE_KEY_Start1);
	Tuple *start2_tuple = dict_find(iterator, MESSAGE_KEY_Start2);
	Tuple *start3_tuple = dict_find(iterator, MESSAGE_KEY_Start3);

	Tuple *name1_tuple = dict_find(iterator, MESSAGE_KEY_Name1);
	Tuple *name2_tuple = dict_find(iterator, MESSAGE_KEY_Name2);
	Tuple *name3_tuple = dict_find(iterator, MESSAGE_KEY_Name3);
		
	// If all data is available, use it
	if(end1_tuple && start1_tuple && name1_tuple) {
		e1_buf = (int)end1_tuple->value->int32;
		s1_buf = (int)start1_tuple->value->int32;
  		snprintf(n1_buf, sizeof(n1_buf), "%s", name1_tuple->value->cstring);
		if(end2_tuple && start2_tuple && name2_tuple) {
			e2_buf = (int)end2_tuple->value->int32;
			s2_buf = (int)start2_tuple->value->int32;
  			snprintf(n2_buf, sizeof(n2_buf), "%s", name2_tuple->value->cstring);
		
			APP_LOG(APP_LOG_LEVEL_INFO, "things are working");
			if(end3_tuple && name3_tuple && start3_tuple) {
				e3_buf = (int)end3_tuple->value->int32;
				s3_buf = (int)start3_tuple->value->int32;
  				snprintf(n3_buf, sizeof(n3_buf), "%s", name3_tuple->value->cstring);
				APP_LOG(APP_LOG_LEVEL_INFO, "things are working");
			}
		}
		
		APP_LOG(APP_LOG_LEVEL_INFO, "things are working");
	}
	time_t now = time(NULL);
	struct tm *tick_time = localtime(&now);
	tick_handler(tick_time, MINUTE_UNIT);
}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}

/////////////////////////////////////////////////////////////////////////////////////////////////
static void init() {
	//create window
	s_main_window = window_create();
	
	window_set_window_handlers(s_main_window, (WindowHandlers) {
		.load = main_window_load,
		.unload = main_window_unload
	});
	
	window_stack_push(s_main_window, true);

	//date buffers
  s_day_buffer[0] = '\0';
  s_num_buffer[0] = '\0';
	s_min_buffer[0] = '\0';
	
	// Register with TickTimerService
	tick_timer_service_subscribe(MINUTE_UNIT | HOUR_UNIT | DAY_UNIT, tick_handler);
	
	// Register callbacks
	app_message_register_inbox_received(inbox_received_callback);
	app_message_register_inbox_dropped(inbox_dropped_callback);
	app_message_register_outbox_failed(outbox_failed_callback);
	app_message_register_outbox_sent(outbox_sent_callback);
	
	// Open AppMessage
	const int inbox_size = 512;
	const int outbox_size = 512;
	app_message_open(inbox_size, outbox_size);
	
	// Begin dictionary
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);

  // Add a key-value pair
  dict_write_uint8(iter, 0, 0);

  // Send the message!
  app_message_outbox_send();
}
int main(void) {
	init();
	app_event_loop();
	deinit();
}