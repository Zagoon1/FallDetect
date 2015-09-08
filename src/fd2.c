/*
 * main.c
 * Constructs a Window housing an output TextLayer to show data from 
 * either modes of operation of the accelerometer.
 */

#include <pebble.h>
#include "window2.c" // La ventana 2 es una cuenta atrás 

#define TAP_NOT_DATA false
  
#define KEY 0 // Claves de conexión reloj -> teléfono ( tupla )
#define CAIDA 0
  
// Declaración de variables a usar en el código
  
static int acel; // Valor absoluto del vector aceleración
static int x,y,z; // Variable donde voy a guardar los valores en los 3 ejes del acelerómetro
static int cont=0; // Contador para el data handler
static int i=0; // indice para el upclick handler ( no se usa)
static int sum=0; // unused
static int max=0; // Variables auxiliares para establecer los máximos y mínimos de acel.
static int min=100000000;
static int index;
static int PC=0; //PC = Posible caída
static int CaLib=0; // Caida Libre , flag.
static int impacto=0; // flag
static int normal=0; //flag de comportamiento normal tras caida e impacto.
static int CN=0; // Comportamiento Normal
static int aux=0; //indice auxiliar.

static char char_buffer[1200]; // Vectores auxiliares donde voy guardando los valores de acel en cada ciclo
static char char_buffer1[1200];
static char char_buffer2[1200];
static int int_buffer[1200];


// Definiciones de ventanas y capas de texto del programa
static Window *s_main_window,*window2;
static TextLayer *s_output_layer;
static TextLayer *s_output_layer1;
static TextLayer *s_output_layer2;
static TextLayer *s_output_layer3;
static TextLayer *s_time_layer; // Capa de texto de la parte del código que implementa el reloj

/* Externs de la ventana 2, se definen aquí */
extern void click_config_provider2(void *context);
extern void window_load2(Window *window2);
extern void window_unload2(Window *window2);
extern void window_disappear2(Window *window2);
extern void window_appear2(Window *window2);


/* BLUETOOTH */
// Función manejadora para actuar según se conecte o desconecte el bluetooth.
static void bt_handler(bool connected) {
  // Mostrar el estado actual de la conexión (según la variable connected)
  if (connected) {
    text_layer_set_text(s_output_layer1, "Conectado");
  } else {
    text_layer_set_text(s_output_layer1, "Desconectado");
    vibes_short_pulse(); // En caso de desconexión , el reloj vibra ligeramente a modo de aviso.
    // NOTA : El reloj Pebble tarda mucho más en darse cuenta de una reconexión que de una desconexión
    // Tarda aproximadamente 10 segundos en percatarse de la desconexión.
    
  }
}
/*-----------*/

// RELOJ 00000000000000000000000000000000000000000000000000000
// Parte del código que se encarga de gestionar el reloj y mostrarlo por pantalla.

static void update_time() {
  // Iniciar una estructura de tipo tm
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);

  // Creación de buffer.
  static char buffer[] = "00:00";

  // Escribir la hora/tiempo actual en dicho buffer.
  if(clock_is_24h_style() == true) {
    // Formato 1.
    strftime(buffer, sizeof("00:00"), "%H:%M", tick_time);
  } else {
    // Formato 2.
    strftime(buffer, sizeof("00:00"), "%I:%M", tick_time);
  }

  // Mostrar por pantalla la hora.
  text_layer_set_text(s_time_layer, buffer);
}

// 00000000000000000000000000000000000000000000000000000000000




/* COMUNICACIONES @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ */
// Parte del código que se encarga de las comunicaciones.
// Debido a las características del proyecto, sólo es necesario el envío de datos del reloj al teléfono
// (para avisar de las caídas). Por ello no usamos la función send(), aunque se deja implementada y escrita
// por si se quiere ampliar el proyecto, ya que dicha función ha sido probada en otros sub-proyectos de prueba
// y funciona correctamente.

static void send(int key, int message) {
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);

  dict_write_int(iter, key, &message, sizeof(int), true);

  app_message_outbox_send();
}

static void inbox_received_handler(DictionaryIterator *iterator, void *context) {
// por ahora no recibo nada
}

static void inbox_dropped_handler(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

static void outbox_failed_handler(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

static void outbox_sent_handler(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}

/* @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ */



// Manejadores de pulsación de botones. Hay uno para la pulsación y otro para la liberación del botón. Sólo se activan
// si han pasado 700milisegundos. Si se activan se pasa a la pantalla de cuenta atrás. Los dos primeros manejadores 
// servían para mostrar por pantalla los valores de acel, paso por paso. Se dejan por si se desean utilizar.

/*static void up_long_click_handler(ClickRecognizerRef recognizer, void *context) { // MANEJADOR DE CLICK (no se usa)
  accel_data_service_unsubscribe();
  
  
  snprintf(char_buffer,sizeof(char_buffer),"Cont: %d\n %d",i,int_buffer[i]);
  text_layer_set_text(s_output_layer,char_buffer);
  snprintf(char_buffer2,sizeof(char_buffer2),"Max: %d\n Min: %d",max,min);
  text_layer_set_text(s_output_layer2,char_buffer2);
  i=i+1;
  
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) { // MANEJADOR DE CLICK (no se usa)
  i=i-1;
  
  
  snprintf(char_buffer,sizeof(char_buffer),"Cont: %d\n %d",i,int_buffer[i]);
  text_layer_set_text(s_output_layer,char_buffer);
  snprintf(char_buffer2,sizeof(char_buffer2),"Max: %d\n Min: %d",max,min);
  text_layer_set_text(s_output_layer2,char_buffer2);
  
}*/

void up_long_click_handler(ClickRecognizerRef recognizer, void *context) {
  
 // Window *window = (Window *)context;
}

void up_long_click_release_handler(ClickRecognizerRef recognizer, void *context) {
  
  //Window *window = (Window *)context;
  window_stack_push(window2, true/*animated*/); // Paso a la ventana de la cuenta atrás.
}

void down_long_click_handler(ClickRecognizerRef recognizer, void *context) {
  
 // Window *window = (Window *)context;
}

void down_long_click_release_handler(ClickRecognizerRef recognizer, void *context) {
  
  //Window *window = (Window *)context;
  window_stack_push(window2, true/*animated*/); // Paso a la ventana de la cuenta atrás.
}

void select_long_click_handler(ClickRecognizerRef recognizer, void *context) {
  
 // Window *window = (Window *)context;
}

void select_long_click_release_handler(ClickRecognizerRef recognizer, void *context) {
  
  //Window *window = (Window *)context;
  window_stack_push(window2, true/*animated*/); // Paso a la ventana de la cuenta atrás.
}


static void click_config_provider(void *context) {
  // Register the ClickHandlers
  window_long_click_subscribe(BUTTON_ID_UP, 700, up_long_click_handler, up_long_click_release_handler);
  window_long_click_subscribe(BUTTON_ID_DOWN, 700, down_long_click_handler, down_long_click_release_handler);
  window_long_click_subscribe(BUTTON_ID_SELECT, 700, select_long_click_handler, select_long_click_release_handler);
}

// MANEJADOR DE DATOS DE ACELEROMETRO
// Se encarga de recibir los datos del acelerómetro cada ciclo y de procersarlos. Debido a la complejidad de la 
// programación concurrente en el sistema Pebble ( hay que recordar que tiene un procesador bastante limitado)
// el mismo manejador de datos es el que los procesa y aplica el algoritmo de selección de caídas.

static void data_handler(AccelData *data, uint32_t num_samples) { // data_handler está ya definido, es así
  // Definiciones : Guardamos cada componente en una de las variables ( estas variables son globales ).
  x=data[0].x;
  y=data[0].y;
  z=data[0].z;
  
  acel=x*x+y*y+z*z; // acel es el mçodulo de la aceleración.
  int_buffer[cont]=acel; // Guardo dicho módulo en un vector, este paso es importante para ir viendo la evolución de
                         // los valores del acelerómetro.
  
  
  // Mostrar por pantalla los datos : Esta parte se ha comentado puesto que sólo servía en el periodo de investigación
  // Básicamente mostraba por pantalla los valores de x, y, z, un contador de los ciclos del acelerómetro y el valor de
  // acel. Se usaba para ver en tiempo real qué iba marcando el sensor. Mediante los botones se podía ver paso por paso
  // los valores de cada ciclo, para estudiar con mayor detenimiento qué estaba pasando.
  
  
  /*snprintf(char_buffer, sizeof(char_buffer),
             "Cont x y z\n %d %d %d %d",
    cont,x,y,z
    );
  text_layer_set_text(s_output_layer,char_buffer);
  
  snprintf(char_buffer2,sizeof(char_buffer2),"   %d",acel);
  text_layer_set_text(s_output_layer2,char_buffer2);*/
  
  
  // Almacenamos los valores máximo y mínimo del vector de acel.
  if(int_buffer[cont]>max){
    max=int_buffer[cont];
  }
  if(int_buffer[cont]<min){
    min=int_buffer[cont];
  }
  
  
  // Algoritmo de selección de caídas: Deben darse 4 condiciones consecutivas para determinar caída
  // Comprtamiento normal -> Caída libre -> Impacto -> Comportamiento normal
  // El funcionamiento y la base de este algoritmo deben estar descritos con detalle en otras partes de este proyecto.
  // Los valores umbrales de cada paso están sacados de muchas y diversas pruebas con el acelerómetro y
  // pueden ajustarse según la sensibilidad que se quiera ( cada persona tiene una movilidad distinta ).
  
  
  // CAIDA LIBRE
  // Se tiene que dar un valor pequeño de acel, seguido de al menos 3 ciclos o iteraciones con valores muy pequeños
  // Cuando se supera el límite inferior de 200k lo que hacemos es observar si se trata de una posible caída o 
  // sencillamente es un valor pequeño.
  if((acel<200000)){
    if(PC==0){PC=1;} // Una iteración menor de 200k -> Empezamos a ver si estamos en una posible caída o no.
     if(acel<40000){ // Al menos 3 iters menores que 40k.
       PC=PC+1;
     }
    if(PC==4){ //Se confirma caída libre.
      CaLib=1; // Activamos el flag de caída libre.
    }
  }
  else{PC=0;} // Si se trataba sencillamente de un valor bajo de acel, reseteamos el flag de posible caída y volvemos
              // a funcionamiento normal.
  
  // IMPACTO
  if(CaLib==1){ // Si estoy en caida libre confirmada, espero al IMPACTO
    if(acel>10000000){impacto=1;} // Valor de impacto medio, sacado de las pruebas con el reloj. Es ajustable.
  }
  // COMPORTAMIENTO NORMAL (post caida e impacto)
  // Lo que hacemos es ver si en las próximos 4 ciclos estamos entre los valores límites de comportamiento normal.
  
  if(impacto==1){
    aux=aux+1;
    if(aux>4){
      if(acel<1200000){
        if(acel>800000){
          normal=normal+1;
        }
        else{normal=0;}
      }
      else{normal=0;}
    }
    if(normal>4){
      // Una vez detectada la caída reseteamos todos los flags y mandamos la ventana de la cuenta atrás (window2).
      CN=1;
      //accel_data_service_unsubscribe(); 
      PC=0;
      CaLib=0;
      impacto=0;
      normal=0;
      CN=0;
      aux=0;
      window_stack_push(window2, true/*animated*/); // Paso a la ventana de la cuenta atrás.
    }
  }
  
  // Mostrar por pantalla el estado actual:
  // Muestra el valor de 3 de los flags del algoritmo para saber en qué estado estamos
  
  snprintf(char_buffer1,sizeof(char_buffer1),"CL: %d\n Impacto: %d\n CN: %d",CaLib,impacto,CN);
  text_layer_set_text(s_output_layer3,char_buffer1);
  
  // Actualizar contador:
  cont=cont+1;
  
  /*if(cont>400){   // Esto se usaba para recopilar información, ahora no sirve para nada.
    cont=0;
    accel_data_service_unsubscribe();
  }*/
  
}

static void tap_handler(AccelAxisType axis, int32_t direction) { // No se usa pero hay que ponerlo.
  
}

static void main_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect window_bounds = layer_get_bounds(window_layer);

  // Los 3 siguientes párrafos se encargaban de la creación de 3 capas de gráficos y texto para ver los valores de 
  // las componentes y el módulo de la aceleración. Se usaba en la investigación del acelerómetro. Lo dejo comentado
  // por si puede resultar útil.
  
  s_output_layer = text_layer_create(GRect(5, 0, 200, 200));
  text_layer_set_font(s_output_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24));
  //text_layer_set_text(s_output_layer, "No data yet.");
  //text_layer_set_overflow_mode(s_output_layer, GTextOverflowModeWordWrap);
  layer_add_child(window_layer, text_layer_get_layer(s_output_layer));
  
 s_output_layer2 = text_layer_create(GRect(5, 60, window_bounds.size.w - 10, window_bounds.size.h));
  text_layer_set_font(s_output_layer2, fonts_get_system_font(FONT_KEY_GOTHIC_24));
  //text_layer_set_text(s_output_layer2, "Probando .");
  //text_layer_set_overflow_mode(s_output_layer2, GTextOverflowModeWordWrap);
  layer_add_child(window_layer, text_layer_get_layer(s_output_layer2));
  
  s_output_layer3 = text_layer_create(GRect(5, 100, window_bounds.size.w - 10, window_bounds.size.h));
  text_layer_set_font(s_output_layer3, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  //text_layer_set_text(s_output_layer3, "Probando .");
  //text_layer_set_overflow_mode(s_output_layer3, GTextOverflowModeWordWrap);
  layer_add_child(window_layer, text_layer_get_layer(s_output_layer3));
  
  
  // Capa de texto para el bluetooth
  s_output_layer1 = text_layer_create(GRect(0, 0, 150, 25));
  text_layer_set_text_alignment(s_output_layer1, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(s_output_layer1));

  // Manejador de bluetooth que nos enseña el estado actual
  bt_handler(bluetooth_connection_service_peek());
  
  // Capa de texto para el reloj
  s_time_layer = text_layer_create(GRect(0, 55, 144, 50));
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorBlack);
  text_layer_set_text(s_time_layer, "00:00");
  text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));
  
  // LLamada al reloj, lo ponemos en main_window_load para que el tiempo aparezca desde el principio.
  update_time();
  
}

static void main_window_unload(Window *window) {
  // Destruir la capa de texto.
  text_layer_destroy(s_output_layer);
}

// Manejador de "ticks", llama a nuestro propio manejador de tiempo.
static void tick_handler(struct tm *tick_time, TimeUnits units_changed) { 
  update_time();
}

static void init() {
  // Hay que registrar todas las llamdas a funciones:
  app_message_register_inbox_received(inbox_received_handler);
  app_message_register_inbox_dropped(inbox_dropped_handler);
  app_message_register_outbox_failed(outbox_failed_handler);
  app_message_register_outbox_sent(outbox_sent_handler);

  // Abrir AppMessage
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
  
  // Creación de main_window
  // init() y deinit() tienen siempre esta estructura, que está dada por el sistema pebble. A la hora de programar
  // en esta plataforma es mejor crearse una plantilla con estas funciones, así como main window load y unload.
  
  s_main_window = window_create();
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });
  
  // Create window2, no significa que aparezca, solo asigno handlers y demás
  window2 = window_create();
  window_set_click_config_provider(window2, click_config_provider2);
  window_set_window_handlers(window2, (WindowHandlers) {
	  .load = window_load2,
    .appear=window_appear2,
    .disappear=window_disappear2,
    .unload = window_unload2,
  });
  
  window_stack_push(s_main_window, true);

  // Elección del servicio de acelerómetro que quiero usar, nosotros usamos el data service, que nos va dando los valores
  // en los 3 ejes cada ciclo. El tap service te da una respuesta cada vez que se agita el reloj.
  if (TAP_NOT_DATA) {
    // Tap service
    accel_tap_service_subscribe(tap_handler);
  } else {
    // Data service
    int num_samples = 1; // Sólo quiero un sample por vez
    accel_data_service_subscribe(num_samples, data_handler);
    
    window_set_click_config_provider(s_main_window, click_config_provider);

    // Frecuencia de muestreo. Todas las pruebas se han hecho con esta frecuencia y el algoritmo está basado en esta
    // frecuencia, por tanto no se debería cambiar nunca de 10Hz.
    accel_service_set_sampling_rate(ACCEL_SAMPLING_10HZ);
    
     // Suscribirse al servicio de bluetooth
  bluetooth_connection_service_subscribe(bt_handler);
    
    // Suscribirse al servicio de tiempo.
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
    
  }
}

static void deinit() {
  // Destruir ventana principal.
  window_destroy(s_main_window);

  if (TAP_NOT_DATA) {
    accel_tap_service_unsubscribe();
  } else {
    accel_data_service_unsubscribe();
  }
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
