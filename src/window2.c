#include <pebble.h>

static int count_down=15; // Los segundos de espera ante falso positivo, pasado este tiempo se envía la señal de socorro

static TextLayer *text_layer, *note_layer, *note_layer2; // Las distintas capas de texto

// En esta ventana sí usamos los botones, ya que ante cualquier pulsación se debe anular la cuenta atrás
static void select_click_handler2(ClickRecognizerRef recognizer, void *context);
static void up_click_handler2(ClickRecognizerRef recognizer, void *context);
static void down_click_handler2(ClickRecognizerRef recognizer, void *context);
static void count_down_handler(struct tm *tick_time, TimeUnits units_changed);



static void window_load2(Window *window2) {
  
  // PARTE GRÁFICA DE LA VENTANA 2:
  // La capa de texto que va a ir mostrando los segundos que quedan.
  Layer *window_layer = window_get_root_layer(window2);
  GRect bounds = layer_get_bounds(window_layer);
  //text_layer = text_layer_create((GRect) { .origin = { 0, 72 }});
  text_layer = text_layer_create((GRect) { { 5, 0 }, { bounds.size.w - 2*5, bounds.size.h } });
  text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
  text_layer_set_font(text_layer, fonts_get_system_font(FONT_KEY_ROBOTO_BOLD_SUBSET_49));
  layer_add_child(window_layer, text_layer_get_layer(text_layer));
  text_layer_set_text(text_layer, "15");
  
  // Capa de texto para mostrar el mensaje de "Pulsa un boton"
  note_layer = text_layer_create((GRect) { .origin = { 0, 72 }, .size = { bounds.size.w, 28 } });
  text_layer_set_font(note_layer, fonts_get_system_font(FONT_KEY_ROBOTO_CONDENSED_21));
  text_layer_set_text(note_layer, "Pulsa un boton");
  text_layer_set_text_alignment(note_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(note_layer));
  

  // Capa de texto para mostrar el mensaje de "para parar"
  note_layer2 = text_layer_create((GRect) { .origin = { 0, 100 }, .size = { bounds.size.w, 28 } });
  text_layer_set_font(note_layer2, fonts_get_system_font(FONT_KEY_ROBOTO_CONDENSED_21));
  text_layer_set_text(note_layer2, "para parar");
  text_layer_set_text_alignment(note_layer2, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(note_layer2));

}

// Manejador de ticks, es una función que se dispara cada vez que pasa un segundo, es la que usamos para ir actualizando
// la cuenta atrás.
static void count_down_handler(struct tm *tick_time, TimeUnits units_changed){
  static char buf[] = "12";
  snprintf(buf, sizeof(buf), "%d", count_down--);
  text_layer_set_text(text_layer, buf);
  if(count_down<5){      // Si la cuenta atrás llega a los 5 segundos, se aumenta la presión sobre el usuario al 
    vibes_short_pulse(); // vibrar por cada segundo.
  }
  if(count_down==0){     // Si pasan los 15 segundos sin intervención del usuario, se envía el mensaje.
    //ENVÍO DEL MENSAJE
    // Primero se define un diccionario (con el que me comunico con el teléfono), se le asigna el valor de la tupla
    // correspondiente y se envía. Para enviar primero abrimos la bandeja de salida y después enviamos.
    DictionaryIterator *iter;
    app_message_outbox_begin(&iter);
    Tuplet value = TupletInteger(0,0); // Como sólo hay un tipo de mensaje, la tupla no tiene mucho sentido.
    dict_write_tuplet(iter, &value);
    app_message_outbox_send();
    
    vibes_long_pulse(); // Vibración larga para señalar que se ha enviado el mensaje.
    window_stack_pop(true); // Volvemos a la ventana principal.
  }
}

static void window_appear2(Window *window2){
  tick_timer_service_subscribe(SECOND_UNIT, count_down_handler);
  count_down_handler( NULL, SECOND_UNIT);
  count_down=15;
  vibes_long_pulse();
}

static void window_disappear2(Window *window2){
  tick_timer_service_unsubscribe();
}

static void window_unload2(Window *window2) {
  text_layer_destroy(text_layer);
  text_layer_destroy(note_layer);
  text_layer_destroy(note_layer2);
}

static void click_config_provider2(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler2);
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler2);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler2);
}

// MANEJADORES DE BOTONES
// En caso de que se pulse cualquiera de los 3 botones físicos del reloj, se finaliza la cuenta atrás y se vuelve
// a la pantalla principal.
static void down_click_handler2(ClickRecognizerRef recognizer, void *context){
  window_stack_pop(true);
}

static void up_click_handler2(ClickRecognizerRef recognizer, void *context) {
 window_stack_pop(true);
}

static void select_click_handler2(ClickRecognizerRef recognizer, void *context){
  window_stack_pop(true);
}