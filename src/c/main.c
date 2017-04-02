#include <pebble.h>     /* Pebble */
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <math.h>       /* math */

static Window *s_main_window;
static TextLayer *s_time_layer;
static TextLayer *s_time_layerM;
static TextLayer *s_text_layer;
static Layer *shape_layer;
static GFont s_time_font;
static GFont s_text_font;
static GBitmap *s_bitmap;
static BitmapLayer *s_bitmap_layer;

static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);
  // Write the current hours and minutes into a buffer
  static char s_buffer[8];
  static char s_bufferM[8];
  strftime(s_buffer, sizeof(s_buffer), clock_is_24h_style() ? "%H" : "%I", tick_time);
  strftime(s_bufferM, sizeof(s_bufferM), clock_is_24h_style() ? "%M" : "%M", tick_time);
  // Display this time on the TextLayer
  text_layer_set_text(s_time_layer, s_buffer);
  text_layer_set_text(s_time_layerM, s_bufferM);
};

void station_load() {
  // Generate a random number
  srand((unsigned) time(NULL));
  //Random function starts at 0
  //Rand up to the last number in the array
  unsigned int random_number = rand()%21;
  //number of items in the array (last index +1 for 0)
  const char *stations[]={
    "World\nTrade\nCenter",
    "Fulton\nStreet",
    "Bowling\nGreen",
    "42\nStreet",
    "Grand\nCentral",
    "Times\nSquare",
    "14\nStreet",
    "Union\nSquare",
    "42 St-\nBryant\nPark",
    "Park\nPlace",
    "Wall\nStreet",
    "34 St\nHudson\nYards",
    "W4",
    "Jay Street\nMetroTech",
    "Chambers\nStreet",
    "Cortlandt\nStreet",
    "Coney\nIsland",
    "Broadway-\nLafayette\nStreet",
    "34\nPenn\nStation",
    "Bklyn\nBridge",
    "City\nHall",
    "Columbus\nCircle"
  };  
  //Write station stop name to the s_text_layer line
  text_layer_set_text(s_text_layer, stations[random_number]);
};

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
  int minutes = tick_time->tm_min;
  int x = 0;
    if(minutes == 47 || minutes == 15 || minutes == 30 || minutes == 45)
    {
      //In lieu of using seconds and constantly redrawing the screen. Added a switch (x) 
      //If minutes match then change the station and flip the switch off
      if(x == 0){station_load();}
      //Flips the switch off
      x=1;
    };
    //Flips the switch back on during the next minute
      if(minutes == 48 || minutes == 16 || minutes == 31 || minutes == 46)
    {
      x=0;
    };
  //Repeats at 0, 15, 30, 45 preventing constant checking and redrawing of the interface.
  
};

//Bluetooth notification actions
void bt_handler(bool connected) {
  if (connected) {
    //If connected remove notification
    layer_set_hidden(bitmap_layer_get_layer(s_bitmap_layer), true);
  } else {
    //If disconnected add notification
    layer_set_hidden(bitmap_layer_get_layer(s_bitmap_layer), false);
    //Turn on backlight
    light_enable_interaction();
    //Vibrate
    vibes_long_pulse();
  };
};

static void shape_update_proc(Layer *this_layer, GContext *ctx) {
  int posH;
  int posL;
  #if defined(PBL_ROUND)
  posH = 134;
  posL = 36;
  #else
  posH = 147;
  posL = 17;
  #endif
  
  APP_LOG(APP_LOG_LEVEL_DEBUG, "posL: %d", posL);

  //Get the current time as a struct
  time_t rawtime; 
  time (&rawtime); 
  struct tm *tm_struct = localtime(&rawtime);
  //Break down the time into each digit so we can use
  //those digits to assign colors below
  int hour = tm_struct->tm_hour; //Get the hours
  if(clock_is_24h_style()){}
  else{
    if (hour > 12){hour = hour - 12;}
    else if(hour ==0){hour = 12;}
  };//Convert to 12hr
  
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Hour is: %d", hour);
  #if defined(PBL_COLOR) 
  //Check if we're in color
  //First digit of hour is always 1 or 0 so we skip that
  //We'll just use an if statement below to determine if hours is more than 10
  int hourTwo = hour % 10; // Get the second digit of the hours
  int minOne = round(tm_struct->tm_min/10); //Get the first digit of minutes
  int minTwo = tm_struct->tm_min % 10; // Get the second digit of minutes using mod to remove the first
  //Let's define the colors
  // https://developer.pebble.com/guides/tools-and-resources/color-picker/
  GColor watchcolor[]={
    GColorBlack, // All 0s
    GColorRed, //1 Train
    GColorRed, //2 Train
    GColorRed, //3 Train
    GColorJaegerGreen, //4 train
    GColorJaegerGreen, //5 Train
    GColorJaegerGreen, //6 Train
    GColorPurple, //7 Train
    GColorVividCerulean, //8 Train
    GColorRed, //9 Train
  };
  //Overline
  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_fill_rect(ctx, GRect(0, 25, 155, 3), 0, GCornerNone);
  //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  //01 hour circle
  GPoint centerH = GPoint(posL, posH);
  if (hour < 10){
    //Since we're making 0 black draw an outer white circle first
    GPoint outerH = GPoint(posL, posH);
    graphics_context_set_fill_color(ctx, GColorWhite);
    graphics_fill_circle(ctx, outerH, 17);
    
    graphics_context_set_fill_color(ctx, watchcolor[0]);
    graphics_fill_circle(ctx, centerH, 16);
  }
  else{
    graphics_context_set_fill_color(ctx, watchcolor[1]);
    graphics_fill_circle(ctx, centerH, 17);
  };
  //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  //10 hour circle 
  if (hourTwo == 0){
    //Since we're making 0 black draw an outer white circle first
    GPoint outerHH = GPoint(posL + 36, posH);
    graphics_context_set_fill_color(ctx, GColorWhite);
    graphics_fill_circle(ctx, outerHH, 17);
    
    GPoint centerHH = GPoint(posL + 36, posH);
    graphics_context_set_fill_color(ctx, watchcolor[0]);
    graphics_fill_circle(ctx, centerHH, 16);
  }
  else{
    GPoint centerHH = GPoint(posL + 36, posH);
    graphics_context_set_fill_color(ctx, watchcolor[hourTwo]);
    graphics_fill_circle(ctx, centerHH, 17);
  }; 
  //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  //10 Minute Circle and fill circle
  if (minOne == 0){
    //Since we're making 0 black draw an outer white circle first
    GPoint outerM = GPoint(posL + 72, posH);
    graphics_context_set_fill_color(ctx, GColorWhite);
    graphics_fill_circle(ctx, outerM, 17);
    
    GPoint centerM = GPoint(posL + 72, posH);
    graphics_context_set_fill_color(ctx, watchcolor[0]);
    graphics_fill_circle(ctx, centerM, 16);
  }
  else{
    GPoint centerM = GPoint(posL + 72, posH);
    graphics_context_set_fill_color(ctx, watchcolor[minOne]);
    graphics_fill_circle(ctx, centerM, 17);
  };
  //01 minute circle and fill circle  
  if (minTwo == 0){
    //Since we're making 0 black draw an outer white circle first
    GPoint outerMM = GPoint(posL + 108, posH);
    graphics_context_set_fill_color(ctx, GColorWhite);
    graphics_fill_circle(ctx, outerMM, 17);
    
    GPoint centerMM = GPoint(posL + 108, posH);
    graphics_context_set_fill_color(ctx, watchcolor[0]);
    graphics_fill_circle(ctx, centerMM, 16);
  }
  else{
    GPoint centerMM = GPoint(posL + 108, posH);
    graphics_context_set_fill_color(ctx, watchcolor[minTwo]);
    graphics_fill_circle(ctx, centerMM, 17);
  };
  //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#else
    //Color for 1000
    GPoint outerH = GPoint(17, (posH));
    graphics_context_set_fill_color(ctx, GColorWhite);
    graphics_fill_circle(ctx, outerH, 17);
  
    //Color for 0100
    GPoint outerHH = GPoint(53, (posH));
    graphics_context_set_fill_color(ctx, GColorWhite);
    graphics_fill_circle(ctx, outerHH, 17);
  
    //Color for 0010
    GPoint outerM = GPoint(89, (posH));
    GPoint innerM = GPoint(89, (posH));
    graphics_context_set_fill_color(ctx, GColorWhite);
    graphics_fill_circle(ctx, outerM, 17);
    graphics_context_set_fill_color(ctx, GColorBlack);
    graphics_fill_circle(ctx, innerM, 16);
  
    //Color for 0001
    GPoint outerMM = GPoint(125, (posH));
    GPoint innerMM = GPoint(125, (posH));
    graphics_context_set_fill_color(ctx, GColorWhite);
    graphics_fill_circle(ctx, outerMM, 17);
    graphics_context_set_fill_color(ctx, GColorBlack);
    graphics_fill_circle(ctx, innerMM, 16);
#endif
};

static void main_window_load(Window *window) {
  // Get information about the Window and set background
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  window_set_background_color(s_main_window, GColorBlack);
  
  //Define circle layer
  shape_layer = layer_create(GRect(0, 0, bounds.size.w, bounds.size.h));
  //Add all layers to the app window
  layer_add_child(window_layer, shape_layer);
  #if defined(PBL_ROUND)
  s_time_layer = text_layer_create(GRect(-28, 113, bounds.size.w, 50));//Create hour text
  s_time_layerM = text_layer_create(GRect(44, 113, bounds.size.w, 50));//Create minute text
  s_text_layer = text_layer_create(GRect(30, 23, bounds.size.w, 100));//Create station stop text
  #else
  s_time_layer = text_layer_create(GRect(-28, 126, bounds.size.w, 50));//Create hour text
  s_time_layerM = text_layer_create(GRect(44, 126, bounds.size.w, 50));//Create minute text
  s_text_layer = text_layer_create(GRect(3, 23, bounds.size.w, 100));//Create station stop text
  #endif
  // Create the BitmapLayer
  s_bitmap_layer = bitmap_layer_create(GRect(126, 5, 15, 15));
  //Set time text attributes
  #if defined(PBL_COLOR)
    text_layer_set_text_color(s_time_layerM, GColorWhite);
    text_layer_set_text_color(s_time_layer, GColorWhite);
  #else
    text_layer_set_text_color(s_time_layerM, GColorWhite);
    text_layer_set_text_color(s_time_layer, GColorBlack);
  #endif
  
  text_layer_set_text_alignment(s_time_layerM, GTextAlignmentCenter);
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  //Set station stop text attributes
  text_layer_set_text_color(s_text_layer, GColorWhite);
  text_layer_set_text_alignment(s_text_layer, GTextAlignmentLeft);
  //Load custom resources (same font in two sizes)
  s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_HELVETICA_33));
  s_text_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_HELVETICA_28));
  s_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BT_ICON_WHITE);
  //Apply custom fonts
  text_layer_set_font(s_time_layer, s_time_font);
  text_layer_set_font(s_time_layerM, s_time_font);
  text_layer_set_font(s_text_layer, s_text_font);
  //Set the bitmap
  bitmap_layer_set_bitmap(s_bitmap_layer, s_bitmap);
  layer_add_child(window_layer, text_layer_get_layer(s_time_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_time_layerM));
  layer_add_child(window_layer, text_layer_get_layer(s_text_layer));
  layer_add_child(window_layer, bitmap_layer_get_layer(s_bitmap_layer));
  //Randomly generate the first station
  station_load();
};


static void main_window_unload(Window *window) {
  //Destroy layers on window unload
  layer_destroy(shape_layer);
  text_layer_destroy(s_time_layer);
  text_layer_destroy(s_time_layerM);
  text_layer_destroy(s_text_layer);
  bitmap_layer_destroy(s_bitmap_layer);
};


static void init(void) {
  // Create main Window element and assign to pointer
  s_main_window = window_create();
  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  }); 
  // Subscribe to get connection events
  bluetooth_connection_service_subscribe(bt_handler);
  //Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true); 
  
  //Set default properties
  layer_set_hidden(bitmap_layer_get_layer(s_bitmap_layer), true);  //Set BT icon to hidden
  text_layer_set_background_color(s_time_layer, GColorClear);  //Used to set default background
    text_layer_set_background_color(s_time_layerM, GColorClear);  //Used to set default background
  text_layer_set_background_color(s_text_layer, GColorClear);  //Used to set default background
  layer_set_update_proc(shape_layer, shape_update_proc);  //Draw all of the shapes on the shape layer
  
  // Make sure the time is displayed from the start
  update_time();  
  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
};

static void deinit() {
  // Destroy Window
   window_destroy(s_main_window);
};

int main(void) {
  init();
  app_event_loop();
  deinit();
};