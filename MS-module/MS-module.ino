/*
   Uses STM32duino with Phono patch. Must add 33 and 95 CAN speeds
*/
#include <HardwareCAN.h>
#include "includes/defines.h"
/////// === Настройки модуля! === ///////

// не компилировать чужой код
#define SKIP_ALIEN

// Choose output serial port
#define UART Serial2

// Choose CAN pins
#define CAN_GPIO_PINS_MS CAN_GPIO_PA11_PA12
//#define CAN_GPIO_PINS_LS CAN_GPIO_PB8_PB9

#define CAN_SEND_TIMEOUT 800

// Uncomment to enable 'debug()' messages output
#define DEBUG

// Uncomment to enable 'log()' messages output
#define LOG

#define PC13ON 0
#define PC13OFF 1
#define DELAY 200


/****************************************** global variables ******************************************/
char CTemp1 = 0;
char CTemp2 = 0;
char CNapr;
char CSpeed;
char CEco;
char COutT;
char p_CTemp1;
char p_CTemp2;
char p_CNapr;
char p_CSpeed;
char p_CEco;

volatile bool flag_blocked;
// Instanciation of CAN interface
HardwareCAN canBus(CAN1_BASE);
CanMsg msg ;
CanMsg *r_msg;

byte activeBus = 0;
enum Activebus {
  LS_BUS = 1,
  MS_BUS = 2
};

void setup()
{
  UART.begin(115200); // output to A2 pin
  UART.println("Hello World!");
  UART.println("Starting \"1-button-compressor switch plus\" v21 2018-11-22");
  debug("checking debug level");
  log("checking log level");


  pinMode(PC13, OUTPUT); // LED
  digitalWrite(PC13, PC13ON);
  delay(50);// to see the LED
  log("Initialize the CAN module ...");

  msCANSetup();        // Initialize the CAN module
  log("Initialization MS CAN ON");
  flag_blocked = false;
  log("flag_blocked is set to " + flag_blocked);
  wakeUpBus();
  delay(100);
  msWakeUpScreen(); // only MS
  delay(100);

#ifdef CAN_GPIO_PINS_LS
  lsCANSetup();        // Initialize the CAN module
  log("Initialization LS CAN ON");
  wakeUpBus();
  lsBeep(2);
  lsPanelCheck();
  playWithEcn();
  msCANSetup();
#endif
  
  log("end set up");
  digitalWrite(PC13, PC13OFF);
}

void loop()
{
  //  debug("loop");
  /*
    while (canBus.available() > 0)
    { CAN_message_process(canBus.recv());
      canBus.free();
    }
  */

  while ( ( r_msg = canBus.recv() ) != NULL ) {
    msReceivedMessageProcess(r_msg); // processing the incoming message
    canBus.free();
  } // close while
}
// close void loop
