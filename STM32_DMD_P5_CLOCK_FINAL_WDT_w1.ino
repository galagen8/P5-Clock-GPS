/*--------------------------------------------------------------------------------------
 Demo for RGB panels

 DMD_STM32a example code for STM32F103xxx board
 ------------------------------------------------------------------------------------- */
#include "DMD_RGB.h"
#include "TinyGPSPlus.h"

// add new template panel
#define RGB64x32plainS8 4,64,32,8,3 // 64x32 1/8

// create GPS odject
TinyGPSPlus gps;

// Fonts includes
#include "st_fonts/InterExtraBold_50.h"
#include "st_fonts/InterExtraBold_19.h"

//Number of panels in x and y axis
#define DISPLAYS_ACROSS 3
#define DISPLAYS_DOWN 3

// Enable of output buffering
// if true, changes only outputs to matrix after
// swapBuffers(true) command
// If dual buffer not enabled, all output draw at matrix directly
// and swapBuffers(true) cimmand do nothing
#define ENABLE_DUAL_BUFFER false

// ==== DMD_RGB pins ====
// mux pins - A, B, C... all mux pins must be selected from same port!
#define DMD_PIN_A PB6
#define DMD_PIN_B PB5
#define DMD_PIN_C PB4
#define DMD_PIN_D PB3
#define DMD_PIN_E PB8

// put all mux pins at list
uint8_t mux_list[] = { DMD_PIN_A , DMD_PIN_B , DMD_PIN_C , DMD_PIN_D , DMD_PIN_E };

// pin OE must be one of PB0 PB1 PA6 PA7
#define DMD_PIN_nOE PB0
#define DMD_PIN_SCLK PB7

// Pins for R0, G0, B0, R1, G1, B1 channels and for clock.
// By default the library uses RGB color order.
// If you need to change this - reorder the R0, G0, B0, R1, G1, B1 pins.
// All this pins also must be selected from same port!
uint8_t custom_rgbpins[] = { PA15, PA0,PA1,PA2,PA3,PA4,PA5 }; // CLK, R0, G0, B0, R1, G1, B1

// Fire up the DMD object as dmd<MATRIX_TYPE, COLOR_DEPTH>
// We use 64x32 matrix with 16 scans and 4bit color:
DMD_RGB <RGB64x32plainS8, COLOR_1BITS> dmd(mux_list, DMD_PIN_nOE, DMD_PIN_SCLK, custom_rgbpins, DISPLAYS_ACROSS, DISPLAYS_DOWN, ENABLE_DUAL_BUFFER);
// Color depth - <COLOR_4BITS_Packed>, <COLOR_4BITS> or <COLOR_1BITS>

// --- Define fonts ----
// DMD.h old style font
DMD_Standard_Font InterExtraBold50_F(InterExtraBold50);
DMD_Standard_Font InterExtraBold19_F(InterExtraBold19);

// fixcount var
uint8_t fixCount = 0;

// basic colors for 1BIT mode
long color_black = 0x0000;     // black color
long color_blue = 0x001F;      // blue color
long color_red = 0xF800;       // red color
long color_green = 0x07E0;     // green color
long color_cyan = 0x07FF;      // cyan color
long color_magenta = 0xF81F;   // white color
long color_yellow = 0xFFE0;    // yellow color
long color_white = 0xFFFF;     // white color

// TIME coordinates variables
int xspace_time = 15;
int yspace_time = 13;

// DATE coordinates variables
int xspace_date = 13;
int yspace_date = 64;

// Time and Date data variables
int rxhours;
int rxmin;
int rxday;
int rxmonth;
int rxyear;

// GMT correction for Asia BKK time
int GMT = 7;

//  real digits for DMD and LCD
int digit1;  // 1st digit for hours
int digit2;  // 2nd digit for hours
int digit3;  // 1st digit for minutes
int digit4;  // 2nd digit for minutes
int digit5;  // 1st digit for date
int digit6;  // 2nd digit for date
int digit7;  // 1st digit for month
int digit8;  // 2nd digit for month
int digit9;  // 1st two digits for year
int digit10; // 2nd two digits for year

// Arduino timer variables
unsigned long previousMillis = 0;  // will store last time when arduino timer was updated
const long interval = 1000;        // arduino timer interval in ms

//  ***************************************************
//  WatchDog Timer (WDT) example for STM32F10* BluePill
//  Formula I used:
//  RL = (Time[2000ms] * LSI_Clock[32k]) : (4 * 2^PR[3] * 1000) - 1 ;
//  Time - WDT Timeout in ms;
//  LSI_Clock - for BluePill (STM32F103C8) here is crystal 32,768 Hertz. I approxed this value to 32,000;
//  PR - Prescaler, refer to #define lines below. If the prescaler "4" is chosen, use "IWDG_PRE_64" in func "iwdg_init(prescaler, y)";
//  RL - computed value, which needed be set inside "reload" variable in func "iwdg_init(x, reload)";
//  NOTE: RL value CANNOT be more than 4095 !!!
//  P.S. As I see, WDT Timeout has became not exactly 2 sec, it's less - it's about 1600-1800ms.

#define LIBMAPPLE_CORE //comment it for HAL based core

#define IWDG_PR_DIV_4 0x0
#define IWDG_PR_DIV_8 0x1
#define IWDG_PR_DIV_16 0x2
#define IWDG_PR_DIV_32 0x3
#define IWDG_PR_DIV_64 0x4
#define IWDG_PR_DIV_128 0x5
#define IWDG_PR_DIV_256 0x6

typedef enum iwdg_prescaler 
{
  IWDG_PRE_4 = IWDG_PR_DIV_4,     /**< Divide by 4 */
  IWDG_PRE_8 = IWDG_PR_DIV_8,     /**< Divide by 8 */
  IWDG_PRE_16 = IWDG_PR_DIV_16,   /**< Divide by 16 */
  IWDG_PRE_32 = IWDG_PR_DIV_32,   /**< Divide by 32 */
  IWDG_PRE_64 = IWDG_PR_DIV_64,   /**< Divide by 64 */
  IWDG_PRE_128 = IWDG_PR_DIV_128, /**< Divide by 128 */
  IWDG_PRE_256 = IWDG_PR_DIV_256  /**< Divide by 256 */
} iwdg_prescaler;

#if defined(LIBMAPPLE_CORE)
typedef struct iwdg_reg_map {
  volatile uint32_t KR;  /**< Key register. */
  volatile uint32_t PR;  /**< Prescaler register. */
  volatile uint32_t RLR; /**< Reload register. */
  volatile uint32_t SR;  /**< Status register */
} iwdg_reg_map;

#define IWDG ((struct iwdg_reg_map *)0x40003000)
#endif

//  func to feed WatchDog: just reset WDT timer, must be in Arduino loop
void iwdg_feed(void)
{ 
  IWDG->KR = 0xAAAA; 
}

//  set WDT timer(timeout time)
void iwdg_init(iwdg_prescaler prescaler, uint16_t reload)
{
  IWDG->KR = 0x5555;
  IWDG->PR = prescaler;
  IWDG->RLR = reload;
  IWDG->KR = 0xCCCC;
  IWDG->KR = 0xAAAA;
}
//  **********************
//  **********************
//  end of WDT description


/*--------------------------------------------------------------------------------------
  setup
  Called by the Arduino architecture before the main loop begins
--------------------------------------------------------------------------------------*/

void setup(void)
{
  //  initialize DWT timer and set the timeout
  iwdg_init(IWDG_PRE_64, 999);

  // uncomment line below if you using St-Link with STM32F103xx boards
  enableDebugPorts();
  
  // initialize DMD objects
	dmd.init();

  // set connection scheme
  dmd.setConnectScheme(CONNECT_NORMAL);
  
  // set the brightness
  dmd.setBrightness(50);

  // clear screen
  dmd.clearScreen(true);

  // enable Serial UART transmission
  Serial1.begin(9600);  //  UART for NEO6M_GPS_Module
  Serial.begin(115200); //  STM32 UART RX(PA10) TX(PA9)

  // Just a message:
  Serial1.println("WDC was set at 2 seconds.");


}

/*--------------------------------------------------------------------------------------
  loop
  Arduino architecture main loop
--------------------------------------------------------------------------------------*/

void loop(void)
{
  iwdg_feed();  // reset WDT Timer
  Serial1.println("Loop goes on...");

  while (Serial1.available() > 0) 
  {
    gps.encode(Serial1.read());
    if (++fixCount >= 250) 
    {

      //  send time to Serial
      if (gps.time.isValid())
      {
      Serial.print(gps.time.hour());
      Serial.print(":");
      Serial.print(gps.time.minute());
      fixCount = 0;
      }

      gps.time; //  create GPS time object
      gps.date; //  create GPS date object

      rxhours = gps.time.hour() + GMT;  //  getting current hour + GMT hour correction
      rxmin = gps.time.minute();        //  getting current minute
      rxday = gps.date.day();           //  getting current day
      rxmonth = gps.date.month();       //  getting current month
      rxyear = gps.date.year();         //  getting current year

      //  GMT corrections
      if(rxhours > 23)
      {
          rxhours = rxhours - 24;

          if(rxmonth == 4 || rxmonth == 6 || rxmonth == 9 || rxmonth == 11)
          {
              if(rxday > 29)
              {
                  rxday = 1;
                  rxmonth = rxmonth + 1;
              }
              else
              {
                  rxday = rxday + 1;
              }
          }
          else if(rxmonth == 1 || rxmonth == 3 || rxmonth == 5 || rxmonth == 7 || rxmonth == 8 || rxmonth == 10 || rxmonth == 12)
          {
              if(rxday > 30)
              {
                  if(rxmonth == 12)
                  {
                      rxday = 1;
                      rxmonth = 1;
                      rxyear = rxyear + 1;
                  }
                  else
                  {
                      rxday = 1;
                      rxmonth = rxmonth + 1;
                  }
              }
              else
              {
                  rxday = rxday + 1;
              }
          }
          else
          {
              if(rxyear % 4 == 0)
              {
                  if(rxday > 28)
                  {
                      rxday = 1;
                      rxmonth = rxmonth + 1;
                  }
                  else
                  {
                      rxday = rxday + 1;
                  }
              }
              else
              {
                  if(rxday > 27)
                  {
                      rxday = 1;
                      rxmonth = rxmonth + 1;
                  }
                  else
                  {
                      rxday = rxday + 1;
                  }
              }
          }
      } //  end of GMT corrections 

      //  making hours here:
      if(rxhours < 10) 
      {
        digit1 = 0;
        digit2 = rxhours;
      }
      else if (rxhours > 19) 
      {
        digit1 = 2;
        digit2 = rxhours - 20;
      }
      else 
      {
        digit1 = 1;
        digit2 = rxhours - 10;
      }

      //making minutes
      if (rxmin < 10) 
      {        // here was < 9, needed to set < 10
        digit3 = 0;
        digit4 = rxmin;
      }
      else if (rxmin < 20) 
      {
        digit3 = 1;
        digit4 = rxmin - 10;
      }
      else if (rxmin < 30) 
      {
        digit3 = 2;
        digit4 = rxmin - 20;
      }
      else if (rxmin < 40) 
      {
        digit3 = 3;
        digit4 = rxmin - 30;
      }
      else if (rxmin < 50) 
      {
        digit3 = 4;
        digit4 = rxmin - 40;
      }
      else 
      {
        digit3 = 5;
        digit4 = rxmin - 50;
      }

      //making date
      if (rxday < 10) 
      {        // here was < 9, needed to set < 10
        digit5 = 0;
        digit6 = rxday;
      }
      else if (rxday < 20) 
      {
        digit5 = 1;
        digit6 = rxday - 10;
      }
      else if (rxday < 30) 
      {
        digit5 = 2;
        digit6 = rxday - 20;
      }
      else if (rxday < 40) 
      {
        digit5 = 3;
        digit6 = rxday - 30;
      }
      else if (rxday < 50) 
      {
        digit5 = 4;
        digit6 = rxday - 40;
      }
      else if (rxday < 60) 
      {
        digit5 = 5;
        digit6 = rxday - 50;
      }
      else if (rxday < 70) 
      {
        digit5 = 6;
        digit6 = rxday - 60;
      }
      else if (rxday < 80) 
      {
        digit5 = 7;
        digit6 = rxday - 70;
      }
      else if (rxday < 90) 
      {
        digit5 = 8;
        digit6 = rxday - 80;
      }

      //making month
      if(rxmonth < 10)
      {       // here was < 9, needed to set < 10
        digit7 = 0;
        digit8 = rxmonth;
      }
      else 
      {
        digit7 = 1;
        digit8 = rxmonth - 10;
      }

      //making year
      rxyear = rxyear - 2000;     // here was <9, needed to set <10
      if (rxyear < 10)
      {
        digit9 = 0;
        digit10 = rxyear;
      }
      else if (rxyear < 20)
      {
        digit9 = 1;
        digit10 = rxyear - 10;
      }
      else if (rxyear < 30)
      {
        digit9 = 2;
        digit10 = rxyear - 20;
      }
      else if (rxyear < 40)
      {
        digit9 = 3;
        digit10 = rxyear - 30;
      }
      else if (rxyear < 50)
      {
        digit9 = 4;
        digit10 = rxyear - 40;
      }
      else if (rxyear < 60)
      {
        digit9 = 5;
        digit10 = rxyear - 50;
      }
      else if (rxyear < 70)
      {
        digit9 = 6;
        digit10 = rxyear - 60;
      }
      else if (rxyear < 80)
      {
        digit9 = 7;
        digit10 = rxyear - 70;
      }
      else if (rxyear < 90)
      {
        digit9 = 8;
        digit10 = rxyear - 80;
      }

      // int to char conversion
      char chdigit1 = digit1 + '0'; //  char for hour
      char chdigit2 = digit2 + '0'; //  char for hour
      char chdigit3 = digit3 + '0'; //  char for min
      char chdigit4 = digit4 + '0'; //  char for min
      char chdigit5 = digit5 + '0'; //  char for day
      char chdigit6 = digit6 + '0'; //  char for day
      char chdigit7 = digit7 + '0'; //  char for month
      char chdigit8 = digit8 + '0'; //  char for month
      char chdigit9 = digit9 + '0'; //  char for year
      char chdigit10 = digit10 + '0'; //  char for year

      // top time frame 00:00
      dmd.selectFont(&InterExtraBold50_F);
      dmd.drawChar(0 + xspace_time, yspace_time, chdigit1, color_white, 0);    //  "1" digit
      dmd.drawChar(36 + xspace_time, yspace_time, chdigit2, color_white, 0);   //  "6" digit
      dmd.fillCircle(80 + xspace_time, 12 + yspace_time, 2, color_white);    //  "upper dot"
      dmd.fillCircle(80 + xspace_time, 27 + yspace_time, 2, color_white);    //  "lower dot"
      dmd.drawChar(91 + xspace_time, yspace_time, chdigit3, color_white, 0);  //  "3" digit
      dmd.drawChar(128 + xspace_time, yspace_time, chdigit4, color_white, 0);  //  "5" digit

      // bottom date frame 00.00.0000
      dmd.selectFont(&InterExtraBold19_F);
      dmd.drawChar(0 + xspace_date, yspace_date, chdigit5, color_white, 0);   //  "0" digit
      dmd.drawChar(18 + xspace_date, yspace_date, chdigit6, color_white, 0);  //  "9" digit
      dmd.fillRect(40 + xspace_date, 16 + yspace_date, 3, 3, color_white);
      dmd.drawChar(43 + xspace_date, yspace_date, chdigit7, color_white, 0);   //  "0" digit
      dmd.drawChar(63 + xspace_date, yspace_date, chdigit8, color_white, 0);   //  "8" digit
      dmd.fillRect(83 + xspace_date, 16 + yspace_date, 3, 3, color_white);
      dmd.drawChar(88 + xspace_date, yspace_date, 50, color_white, 0);   //  "2" digit
      dmd.drawChar(108 + xspace_date, yspace_date, 48, color_white, 0);   //  "0" digit
      dmd.drawChar(128 + xspace_date, yspace_date, chdigit9, color_white, 0);   //  "2" digit
      dmd.drawChar(148 + xspace_date, yspace_date, chdigit10, color_white, 0);   //  "4" digit
      //---------------------------------------------------------------------------------------
      // END OF EXECUTABLE CODE

    } // timer for MCU when to read serial from GPS

  } // if serial avaliable, we read Serial input from GPS
    
} // arduino loop bracket   
    

    

  





