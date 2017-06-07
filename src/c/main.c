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
void drawTimeCircle(int timeDiv, int posL, int posH, int Offset, GContext *ctx, GColor *watchcolor);

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
    "South\nFerry",
    "Jay Street\nMetroTech",
    "Chambers\nStreet",
    "Cortlandt\nStreet",
    "Coney\nIsland",
    "Broadway-\nLafayette\nStreet",
    "34\nPenn\nStation",
    "Bklyn\nBridge",
    "City\nHall",
    "Columbus\nCircle",
    "B'Way\nJunction"
  };  
  // Generate a random number
  srand((unsigned) time(NULL));
  //Random function starts at 0
  //Rand up to the last number in the array
  int len = sizeof(stations) / sizeof(stations[0])-1;
  unsigned int random_number = rand()%len;
  //number of items in the array (last index +1 for 0)
  //Write station stop name to the s_text_layer line
  text_layer_set_text(s_text_layer, stations[random_number]);
};

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
  int minutes = tick_time->tm_min;
  int x = 0;
    if(minutes == 15 || minutes == 30 || minutes == 45 || minutes == 0)
    {
      //In lieu of using seconds and constantly redrawing the screen. Added a switch (x) 
      //If minutes match then change the station and flip the switch off
      if(x == 0){station_load();}
      //Flips the switch off
      //***This also prevents a loop from occurring while checking what minute it is***/
      //There is probably a better way to do this
      x=1;
    };
    //Flips the switch back on during the next minute
      if(minutes == 16 || minutes == 31 || minutes == 46 || minutes == 1)
    {
      x=0;
    };  
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
    #if PBL_DISPLAY_WIDTH == 200
      posH = 168;
      posL = 45;
    #else
    #endif
  #endif

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
  //APP_LOG(APP_LOG_LEVEL_DEBUG, "Hour is: %d", hour); // This gets called three times per launch
  //Overline
  graphics_context_set_fill_color(ctx, GColorWhite);
  #if PBL_DISPLAY_HEIGHT == 228
  graphics_fill_rect(ctx, GRect(0, 35, 200, 3), 0, GCornerNone);
  #else
  graphics_fill_rect(ctx, GRect(0, 25, 200, 3), 0, GCornerNone);
  #endif
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
    GColorLightGray,
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
  //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  //01 hour circle
  hour = floor(hour/10);
  //APP_LOG(APP_LOG_LEVEL_DEBUG, "Hour is: %d", hour); // This gets called three times per launch
  drawTimeCircle(hour, posL, posH, 0, ctx, watchcolor);
  //10 hour circle 
  drawTimeCircle(hourTwo, posL, posH, 36, ctx, watchcolor);
  //10 Minute Circle and fill circle
  drawTimeCircle(minOne, posL, posH, 72, ctx, watchcolor);
  //01 minute circle and fill circle  
  drawTimeCircle(minTwo, posL, posH, 108, ctx, watchcolor);
  //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#else
  GColor watchcolor[]={
    GColorBlack,
    GColorWhite
  };
    //Color for 1000
    drawTimeCircle(1, posL, posH, 0, ctx, watchcolor);
    //Color for 0100
    drawTimeCircle(1, posL, posH, 36, ctx, watchcolor);
    //Color for 0010
    drawTimeCircle(0, posL, posH, 72, ctx, watchcolor);
    //Color for 0001
    drawTimeCircle(0, posL, posH, 108, ctx, watchcolor);
#endif
};

//Function to draw colored circles for 0111 - does not draw for first digit yet
void drawTimeCircle(int timeDiv, int posL, int posH, int Offset, GContext *ctx, GColor *watchcolor){
    GPoint innerCircle = GPoint(posL + Offset, posH);
    graphics_context_set_fill_color(ctx, watchcolor[timeDiv]);
    graphics_fill_circle(ctx, innerCircle, 17);
  
    #if defined(PBL_BW)
      if (timeDiv == 0){
        //Since we're making 0 black draw an outer white circle first
        GPoint outerCircle = GPoint(posL + Offset, posH);
        graphics_context_set_fill_color(ctx, GColorWhite);
        graphics_fill_circle(ctx, outerCircle, 17);
          
        GPoint innerCircle = GPoint(posL + Offset, posH);
        graphics_context_set_fill_color(ctx, GColorBlack);
        graphics_fill_circle(ctx, innerCircle, 16);
      }
    #endif
}

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
    #if defined(PBL_PLATFORM_EMERY)
    s_time_layer = text_layer_create(GRect(-28, 146, bounds.size.w, 50));//Create hour text
    s_time_layerM = text_layer_create(GRect(44, 146, bounds.size.w, 50));//Create minute text
    s_text_layer = text_layer_create(GRect(27, 40, bounds.size.w, 100));//Create station stop text
    #else
    s_time_layer = text_layer_create(GRect(-28, 126, bounds.size.w, 50));//Create hour text
    s_time_layerM = text_layer_create(GRect(44, 126, bounds.size.w, 50));//Create minute text
    s_text_layer = text_layer_create(GRect(3, 23, bounds.size.w, 100));//Create station stop text
    #endif
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
  //Show the Window on the watch, with animated=false
  window_stack_push(s_main_window, false); 
  
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