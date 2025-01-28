#include "main.h"
#include <lvgl.h>
#include "DisplayApp.h"
#include "grl_apis.h"

// #define BOOTING_SCREEN

#ifdef BOOTING_SCREEN
// #define PROGRESS_BAR //BOOTING,
#define BOOT_MSGS
#define ARC_BAR //SPINNER
// #define ANIM_FADE
// #define UBUNTU_DOTS
#define GRL_LOGO
volatile bool data_received = false; // Flag for UART data
lv_obj_t *boot_label; // Label for the booting text
int dot_count = 0;    // Counter for the animated dots

#ifdef PROGRESS_BAR
lv_obj_t *progress_bar;
int progress = 0;
#endif
#ifdef ARC_BAR
int progress = 0;
lv_obj_t *arc;
#endif
#ifdef UBUNTU_DOTS
#define LV_HOR_RES_MAX  320
#define LV_VER_RES_MAX  240
static lv_color_t buf[LV_VER_RES_MAX * 10];
#define DOT_COUNT 6 // Number of dots in the animation
#define RADIUS 40   // Radius of the circular path
#define DOTS_Y_OFFSET 80    // Offset for dots below the text
#define ANIM_DURATION 500 // Duration (ms) for each dot's transition
lv_obj_t *dots[DOT_COUNT]; // Array to store the dot objects
int dot_opacity[DOT_COUNT];   // Array to track opacity of each dot
static lv_style_t dot_style;
bool is_resetting = false;    // Whether the animation is resetting all dots
int current_dot = 0;          // The index of the dot currently transitioning

void animate_dots(void *var, int32_t v) {
    for (int i = 0; i < DOT_COUNT; i++) {
        // Calculate angle for each dot
        int angle = (360 / DOT_COUNT) * i; // Spread dots evenly around the circle
        int x = RADIUS * lv_trigo_sin(angle) / LV_TRIGO_SIN_MAX; // X-coordinate
        int y = RADIUS * lv_trigo_cos(angle) / LV_TRIGO_SIN_MAX; // Y-coordinate
        lv_obj_set_pos(dots[i], 120 + x, 160 - y); // Center circle at (120, 160)

        // Set opacity: Only the active dot transitions, others stay static
        Serial.printf("i %d \t op r : %d\t op e %d\n", i,v,dot_opacity[i]);
        if (i == current_dot) {
            lv_obj_set_style_bg_opa(dots[i], v, LV_PART_MAIN); // Gradually increase opacity
            
        } else if (dot_opacity[i] < LV_OPA_COVER) {
            lv_obj_set_style_bg_opa(dots[i], LV_OPA_30, LV_PART_MAIN); // Keep others dim
        }
    }
}
// Callback when the current animation completes
void anim_ready_cb(lv_anim_t *a) {
    // Mark the current dot as fully transitioned
    dot_opacity[current_dot] = LV_OPA_COVER;

    // Advance to the next dot
    current_dot = (current_dot + 1) % DOT_COUNT;

    // Restart the animation for the next dot
    lv_anim_start(a);
}
void setup_loading_screen() {
    // Create dots
    lv_style_init(&dot_style);
    lv_style_set_bg_opa(&dot_style, LV_OPA_COVER); // Set default full opacity
    for (int i = 0; i < DOT_COUNT; i++) {
        dots[i] = lv_obj_create(lv_scr_act());
        lv_obj_set_size(dots[i], 10, 10); // Dot size
        lv_obj_set_style_radius(dots[i], LV_RADIUS_CIRCLE, LV_PART_MAIN); // Make dots circular
        lv_obj_set_style_bg_color(dots[i], lv_color_hex(0x0000FF), LV_PART_MAIN); // Set dot color (blue)
        lv_obj_set_style_bg_opa(dots[i], LV_OPA_COVER, LV_PART_MAIN);
        lv_obj_clear_flag(dots[i], LV_OBJ_FLAG_CLICKABLE); // Make dots non-clickable
        dot_opacity[i] = LV_OPA_30; // Initialize all dots to low opacity
        lv_obj_add_style(dots[i], &dot_style, LV_PART_MAIN);
    }

    // Create animation
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_exec_cb(&a, animate_dots);
    lv_anim_set_ready_cb(&a, anim_ready_cb);    // Trigger when the animation completes
    lv_anim_set_var(&a, NULL);
    lv_anim_set_values(&a, LV_OPA_30, LV_OPA_COVER); // Fade from low to full opacity
    lv_anim_set_time(&a, ANIM_DURATION);  // Duration for each dot's transition
    
    lv_anim_set_repeat_count(&a,1);
    lv_anim_start(&a);
}
#endif

#ifdef BOOT_MSGS
const char *loading_messages[] = {
    "Initializing Modules...",
    "Connecting to Systems...",
    "Loading User Interface...",
    "Almost Ready!",
    "Booting..."
};
int current_message = 0;
lv_obj_t *boot_msgs_label;
#endif

void boot_time_uart_task(void *pvParameters) {
  char string_buffer[24]; // Buffer to hold received strings
  size_t string_len = 0;               // Current length of the string
  
  while (1) 
  {
    lv_timer_handler(); /* let the GUI do its work */
#ifdef PROGRESS_BAR
        lv_bar_set_value(progress_bar, progress, LV_ANIM_ON);
        progress += 5; // Increment progress
        vTaskDelay(500 / portTICK_PERIOD_MS); // Update every 500ms
        if(progress >=100)
          progress = 0;
#endif

#ifdef ARC_BAR
        lv_arc_set_value(arc, progress);
        progress += 4; // Increment progress
        if(progress >=100)
          progress = 0;
#endif

#ifdef BOOT_MSGS
        lv_label_set_text(boot_msgs_label, loading_messages[current_message]);
        current_message = (current_message + 1) % (sizeof(loading_messages) / sizeof(loading_messages[0]));
#endif
        vTaskDelay(1000 / portTICK_PERIOD_MS); // Update every 1000ms

#ifdef BOOTING
    // Update the label text with animated dots
      char buffer[32];
      sprintf(buffer, "Booting%s", std::string(dot_count, '.').c_str());
      lv_label_set_text(boot_label, buffer);
      dot_count = (dot_count + 1) % 4; // Cycle through 0, 1, 2, 3 dots
      vTaskDelay(500 / portTICK_PERIOD_MS); // Update every 500ms
#endif
    if(Serial.available())
    {
      char c = Serial.read(); // Read a single character
      // Serial.println(c);
      if (string_len < 24 - 1) {
        if (c == '0') { //change this to new line
          // End of string
          string_buffer[string_len] = '\0'; // Null-terminate the string
          string_len = 0;                  // Reset for the next string
          // if(strcmp(string_buffer,"booted") == 0)
          {
            data_received = true;            // Flag to trigger working panel
            // Serial.println("Received string: " + String(string_buffer)); // Debug output
            vTaskDelete(NULL); // Delete this task
          }
        } else {
          // Append the character to the string buffer
          string_buffer[string_len++] = c;
        }
      } else {
        // Handle overflow
        Serial.println("Buffer overflow, clearing buffer.");
        string_len = 0;
      }
    }
  }
}
#endif //BOOTING_SCREEN

void grl_init(){
  memset(panel_obj_p, 0, sizeof(panel_obj_t));

  // Assign the addresses of the static instances to the pointers
  panel_obj_p->over_view_tab_s.sys_info_panel = &sys_info_panel_instance;
  panel_obj_p->over_view_tab_s.port_sts_panel = &port_sts_panel_instance;
  panel_obj_p->log_tab_s.log_panel = &log_panel_instance;
  panel_obj_p->port_ctrl_tab_s.msg_sts_panel = &msg_panel_instance;

  // Initialize the nested structures
  memset(panel_obj_p->over_view_tab_s.sys_info_panel, 0, sizeof(sys_info_panel_t));
  memset(panel_obj_p->over_view_tab_s.port_sts_panel, 0, sizeof(port_sts_panel_t));
  memset(panel_obj_p->log_tab_s.log_panel, 0, sizeof(log_panel_t));
  memset(panel_obj_p->port_ctrl_tab_s.msg_sts_panel, 0, sizeof(msg_sts_panel_t));

  init_get_cmds_fp();
  init_set_cmd_fp();
  // setup_styles();
}

void grl_timer_init(){
	timer_1 = xTimerCreate("tmr_smf", pdMS_TO_TICKS(500), false, 0, Timer0_ISR);
}

void create_tasks(){
  xTaskCreate(process_task, "UI Process Task", 10000, NULL, process_task_priority, &xHandle1);
  xTaskCreate(uart_read_task, "Read Task", 10000, NULL, read_task_priority, &xHandle2);
  xTaskCreate(uart_write_task, "Write Task", 10000, NULL, write_task_priority, &xHandle3);
}

#ifdef BOOTING_SCREEN
void grl_boot_screen_init(){
  Serial.begin(115200);
  Wire.begin(19, 20);
  dht20.begin();
  //IO
  pinMode(38, OUTPUT);
  digitalWrite(38, LOW);
  
  grl_init();
  grl_display_init();

#ifdef TFT_BL
  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH);
#endif
// Mount SPIFFS
    // if (!SPIFFS.begin(true)) {
    //     Serial.println("Failed to mount SPIFFS");
    //     return;
    // }
    
    // Set the background color
    // lv_obj_set_style_bg_color(lv_scr_act(), lv_color_hex(0x001F3F), LV_PART_MAIN); // Navy Blue
    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_hex(0xFFFFFF), LV_PART_MAIN); 
    lv_obj_set_style_bg_opa(lv_scr_act(), LV_OPA_COVER, LV_PART_MAIN);


#if 0
    // Check if the GIF exists
    if (!SPIFFS.exists("/123.gif")) {
        Serial.println("grl_500.gif not found in SPIFFS");
        return;
    }

    lv_obj_t *gif = lv_gif_create(lv_scr_act());
    lv_gif_set_src(gif, "/123.gif"); // Replace with your GIF file path
    lv_obj_align(gif, LV_ALIGN_CENTER, 0, 0); // Center the GIF
    lv_obj_set_size(gif, LV_HOR_RES, LV_VER_RES);  // Scale the GIF to fit the screen
    lv_obj_set_style_opa(gif, LV_OPA_COVER, LV_PART_MAIN);
#endif
#ifdef GRL_LOGO
    // Create an image object for the background
    lv_obj_t *bg_image = lv_img_create(lv_scr_act());
    LV_IMG_DECLARE(grllogocrop);
    lv_img_set_src(bg_image, &grllogocrop); // Use your image variable
    lv_obj_align(bg_image, LV_ALIGN_CENTER, 0, 0); // Center the image
    lv_img_set_zoom(bg_image, 200); // Scale the image to 2x
    lv_obj_align(bg_image, LV_ALIGN_CENTER, 0, -80); // Center the image and move it slightly up
#endif
#if 0
   // Add a heading
    lv_obj_t *heading = lv_label_create(lv_scr_act());
    lv_label_set_text(heading, "Driving Connectivity & Charging");
    lv_obj_set_style_text_color(heading, lv_color_hex(0x000000), LV_PART_MAIN); // Black text
    lv_obj_set_style_text_font(heading, &lv_font_montserrat_24, LV_PART_MAIN); // Medium font size
    lv_obj_align_to(heading, bg_image, LV_ALIGN_OUT_BOTTOM_MID, 0, 5); // Position below the logo

    // Add a two-line sentence
    lv_obj_t *sentence = lv_label_create(lv_scr_act());
    lv_label_set_text(sentence, "GRL is a global leader in engineering services and test automation solutions for\ndigital connectivity and charging technologies");
    lv_obj_set_style_text_color(sentence, lv_color_hex(0x000000), LV_PART_MAIN); // Black text
    lv_obj_set_style_text_align(sentence, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN); // Center align text
    lv_obj_set_style_text_font(sentence, &lv_font_montserrat_16, LV_PART_MAIN); // Small font size
    lv_obj_align_to(sentence, heading, LV_ALIGN_OUT_BOTTOM_MID, 0, 5); // Position below the heading
#endif

#ifdef PROGRESS_BAR
  progress_bar = lv_bar_create(lv_scr_act());
  lv_obj_set_size(progress_bar, 200, 20); // Width and height
  lv_obj_align(progress_bar, LV_ALIGN_CENTER, 0, 80); // Adjust position
  lv_bar_set_range(progress_bar, 0, 100);
#endif //PROGRESS_BAR

#ifdef BOOT_MSGS
  boot_msgs_label = lv_label_create(lv_scr_act());
  lv_label_set_text(boot_msgs_label, "Starting...");
  lv_obj_set_style_text_color(boot_msgs_label, lv_color_black(), LV_PART_MAIN);
  lv_obj_set_style_text_font(boot_msgs_label, &lv_font_montserrat_28, LV_PART_MAIN); // Large font size
  lv_obj_set_style_text_align(boot_msgs_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
  lv_obj_align(boot_msgs_label, LV_ALIGN_CENTER, 0, 200); // Adjust position
#endif

#ifdef SPINNER
  lv_obj_t *spinner = lv_spinner_create(lv_scr_act(), 10000, 120); // 1000ms for a full spin
  lv_obj_set_size(spinner, 80, 80); // Adjust size
  lv_obj_align(spinner, LV_ALIGN_CENTER, 0, 90); // Adjust position
#endif

#ifdef ARC_BAR
  arc = lv_arc_create(lv_scr_act());
  lv_obj_set_size(arc, 100, 100); // Adjust size
  lv_obj_align(arc, LV_ALIGN_CENTER, 0, 90);
  lv_arc_set_range(arc, 0, 100);

  // Set the spinner color to blue
  // lv_obj_set_style_arc_color(arc, lv_color_hex(0x0000FF), LV_PART_INDICATOR); // Blue arc
  // lv_obj_set_style_arc_width(arc, 10, LV_PART_INDICATOR); // Adjust the arc width
#endif

#ifdef BOOTING
    // Create a label for "Booting..." text
    boot_label = lv_label_create(lv_scr_act());
    lv_label_set_text(boot_label, "Booting...");
    lv_obj_set_style_text_color(boot_label, lv_color_black(), LV_PART_MAIN);
    lv_obj_set_style_text_font(boot_label, &lv_font_montserrat_28, LV_PART_MAIN); // Large font size
    lv_obj_set_style_text_align(boot_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    // lv_obj_align(boot_label, LV_ALIGN_CENTER, 0, 0); // Center the label
    lv_obj_align_to(boot_label, bg_image, LV_ALIGN_OUT_BOTTOM_MID, 0, 120); // Position below the image
#endif

#ifdef UBUNTU_DOTS
  setup_loading_screen();
#endif
  // Serial.println("booting screen booted\n");
// #endif
  xTaskCreate(boot_time_uart_task, "boot Task", 10000, NULL, boot_task_priority, &xbootHandle);
}
#endif //BOOTING_SCREEN

void grl_working_screen_init()
{

#ifndef BOOTING_SCREEN
  Serial.begin(115200);
  Wire.begin(19, 20);
  dht20.begin();
  //IO
  pinMode(38, OUTPUT);
  digitalWrite(38, LOW);

#ifdef TFT_BL
  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH);
#endif

#endif//BOOTING_SCREEN

  grl_init();

#ifndef BOOTING_SCREEN
  grl_display_init(); //This function shall be called only after grl_init() ie., afer initializing respective structures
#endif
  grl_timer_init();

  // Create mutex before starting tasks that shall be used for protecting uart writes
  uart_write_mutex = xSemaphoreCreateMutex();

  /***Queues creating for handling Rx and Tx events */
  _tx_queue = xQueueCreate(TX_QUEUE_ELEMENTS_MAX, sizeof(_tx_queue_struct_t));
  if (!_tx_queue) {
    Serial.println("TX Queue Error\n");
    return;
  }
  _rx_queue = xQueueCreate(RX_QUEUE_ELEMENTS_MAX, sizeof(_rx_queue_struct_t));
  if (!_tx_queue) {
    Serial.println("RX Queue Error\n");
    return;
  } 
  create_tasks();
}

void setup()
{
#ifdef BOOTING_SCREEN
  grl_boot_screen_init();
#else
  grl_working_screen_init();
#endif//BOOTING_SCREEN
}

void loop()
{
#ifdef BOOTING_SCREEN 
  if (data_received) {
      data_received = false; // Reset the flag
      grl_working_screen_init(); // Transition to the working panel
  }
#endif//BOOTING_SCREEN
}
