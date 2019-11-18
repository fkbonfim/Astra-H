//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
//                                  PROCESSING CAN-MESSAGES                            //
//                All processing and HEXoding of CAN messages is here.                 //
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
void msReceivedMessageProcess(CanMsg *r_msg) {

#ifdef DEBUG
//  char scan[40];
//  sprintf (scan, "\n % d: % 04X # %02X %02X %02X %02X %02X %02X %02X %02X ", millis(),
//           r_msg->ID, r_msg->Data[0], r_msg->Data[1], r_msg->Data[2], r_msg->Data[3],
//           r_msg->Data[4], r_msg->Data[5], r_msg->Data[6], r_msg->Data[7]);
//  UART.println(scan);
printMsg();
#endif

  switch (r_msg->ID)  {

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
//       мои из кондея
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
     case MS_WHEEL_BUTTONS_ID : {// steering wheel buttons 0x206

       debug("steering wheel buttons");
       // setting the flag_blocked flag [01 нажата кнопка] [81 пресет/верхняя на руле] []
       if (r_msg->Data[1] == 0x81) {
         if (r_msg->Data[0] == 0x01) {
           flag_blocked = true;
           digitalWrite(PC13, PC13ON);
           log("Blocking button is pressed");
         } else {
           flag_blocked = false;
           digitalWrite(PC13, PC13OFF);
           UART.println("Blocking button is released");
         }
       }
       break;
     }

     case MS_CLIMATE_CONTROLS_ID : { //climate controls 0x208

       debug("climate controls");
       // check block button pressed
       if (flag_blocked) { // if block pressed, just skip it
         // do nothing
       } else {
         // check if the climate control menu is pressed
         if (
           (r_msg->Data[0] == 0x01) and
           (r_msg->Data[1] == 0x17) and
           (r_msg->Data[2] == 0x00) //?
         ) { // AC triggering script
           log("blocking is NOT pressed");
           log("Running AC triggering script");
           msAcTrigger();
           log("Done.");
           digitalWrite(PC13, PC13OFF);
         }
         //end AC triggering script
       }
       break;
     }

    case MS_CLIMATE_INFO_ID: { // 6C8
        debug("i see MS_CLIMATE_INFO_ID");
        if (r_msg->Data[0] == 0x21 && r_msg->Data[1] == 0x00 && r_msg->Data[6] == 0xB0 && r_msg->Data[7] == 0x24) {
          climaTemperatureDigit1 = r_msg->Data[2];
          climaTemperatureDigit2 = r_msg->Data[4];
          debug("climaTemperatureDigit1=",climaTemperatureDigit1);
          debug("climaTemperatureDigit2=",climaTemperatureDigit2);

        }
        if (r_msg->Data[0] == 0x22 && r_msg->Data[1] == 0x01 && r_msg->Data[2] == 0xE0) {
          climaDirection = r_msg->Data[3] - 0x21;
        }
        if (r_msg->Data[0] == 0x22 && r_msg->Data[4] == 0x25) {
          if (r_msg->Data[5] == 0x01) {
            climaECO = 0x30;
          }
          if (r_msg->Data[5] == 0x03) {
            climaECO = 0x31;
          }
        }
        if (climaECO == 0x30) {
          if (r_msg->Data[0] == 0x23 && r_msg->Data[1] == 0x26) {
            climaSpeed = r_msg->Data [6];
            if (climaSpeed != previous_climaSpeed) {
              UART.println("<climaSpeed:" + String(climaSpeed) + ">");
              previous_climaSpeed = climaSpeed;
            }
          }
        }
        if (climaECO == 0x31) {
          if (r_msg->Data[0] == 0x24 && r_msg->Data[1] == 0x50) {
            climaSpeed = r_msg->Data[3];
            if (climaSpeed != previous_climaSpeed) {
              UART.println("<climaSpeed:" + String(climaSpeed) + ">");
              previous_climaSpeed = climaSpeed;
            }
          }
        }
        if (r_msg->Data[0] == 0x21 && r_msg->Data[1] == 0x00 && r_msg->Data[6] == 0xB0 && r_msg->Data[7] == 0xA3) {
          climaTemperatureDigit1 = r_msg->Data[2];
          climaTemperatureDigit2 = r_msg->Data[4];
          if ((climaTemperatureDigit1 != previous_climaTemperatureDigit1) || (climaTemperatureDigit2 != previous_climaTemperatureDigit2)) {
            UART.println("<CTemp:" + String(climaTemperatureDigit1) + String(climaTemperatureDigit2) + ">");
            previous_climaTemperatureDigit1 = climaTemperatureDigit1;
            previous_climaTemperatureDigit2 = climaTemperatureDigit2;
          }
        }
        if (r_msg->Data[0] == 0x21 && r_msg->Data[1] == 0xE0 && r_msg->Data[3] == 0xA4) {
          climaDirection = r_msg->Data[2] - 0x21;
          if (climaDirection != previous_climaDirection) {
            UART.println("<climaDirection:" + String(climaDirection) + ">");
            previous_climaDirection = climaDirection;
          }
        }
        if (r_msg->Data[0] == 0x10 && r_msg->Data[6] == 0x25) {
          if (r_msg->Data[7] == 0x01) {
            climaECO = 0x30;
          }
          if (r_msg->Data[7] == 0x03) {
            climaECO = 0x31;
          }
          if (climaECO != previous_climaECO) {
            UART.println("<climaECO:" + String(climaECO) + ">");
            previous_climaECO = climaECO;
          }
        }
        if (r_msg->Data[0] == 0x22 && r_msg->Data[1] == 0x50) {
          climaSpeed = r_msg->Data[3];
          if (climaSpeed != previous_climaSpeed) {
            UART.println("<climaSpeed:" + String(climaSpeed) + ">");
            previous_climaSpeed = climaSpeed;
          }
        }
        if ( r_msg->Data[0] == 0x25 && r_msg->Data[1] == 0xA5 && r_msg->Data[2] == 0x02  && r_msg->Data[4] == 0x50 && r_msg->Data[5] == 0x00 && r_msg->Data[6] == 0x41 && r_msg->Data[7] == 0x59) {
          if (r_msg->Data[3] == 0xE0) {
            climaDirection = 0x38;
            climaSpeed = 0x41;
            if (climaDirection != previous_climaDirection) {
              UART.println("<climaDirection:" + String(climaDirection) + ">");
              previous_climaDirection = climaDirection;
            }
            if (climaSpeed != previous_climaSpeed) {
              UART.println("<climaSpeed:" + String(climaSpeed) + ">");
              previous_climaSpeed = climaSpeed;
            }
          }
        }
        break;
      }

    case MS_TEMP_OUT_DOOR_ID: {
        debug("i see MS_TEMP_OUT_DOOR_ID"); //todo
        debug("climaOutTemperature=",climaOutTemperature);
        debug("previous_climaOutTemperature=",previous_climaOutTemperature);
        if ((r_msg->Data[0] == 0x46) ){
          climaOutTemperature = (r_msg->Data[2] / 2) - 40;
            if (climaOutTemperature != previous_climaOutTemperature)) {
                  debug("(r_msg->Data[0] == 0x46)"); // todo
                  debug("climaOutTemperature=",climaOutTemperature);
                  debug("String(climaOutTemperature)=" + String(climaOutTemperature));

                  UART.println("<climaOutTemperature:" + String(climaOutTemperature) + ">");

                  previous_climaOutTemperature = climaOutTemperature;
             }
        }
        break;
      }


//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
//       не мои
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//



#ifndef SKIP_ALIEN

    case MS_WHEEL_BUTTONS_ID: {
        switch (r_msg->Data[1]) {
          case MS_BTN_VOL: {
              if (r_msg->Data[0] == WHEEL_PRESSED) {
                if (r_msg->Data[2] == WHEEL_TURN_UP) {
                  digitalWrite(PB12, LOW); btn = millis();
                  Message_USART = "VOLUME UP";
                }
                if (r_msg->Data[2] == WHEEL_TURN_DOWN) {
                  digitalWrite(PB13, LOW); btn = millis();
                  Message_USART = "VOLUME DOWN";
                }
              }
              break;
            }
          case MS_BTN_NEXT: {
              if ((r_msg->Data[0] == BTN_PRESSED) && (r_msg->Data[2] == BTN_RELEASED)) {
                digitalWrite(PB14, LOW); btn = millis();
                Message_USART = "NEXT TRACK";
              }
              break;
            }
          case MS_BTN_PREV: {
              if ((r_msg->Data[0] == BTN_PRESSED) && (r_msg->Data[2] == BTN_RELEASED)) {
                digitalWrite(PB15, LOW); btn = millis();
                Message_USART = "PREVIOUS TRACK";
              }
              break;
            }
          case MS_BTN_STATION: {
              if (r_msg->Data[0] == BTN_PRESSED) {
                if (( window != 2) && ((r_msg->Data[2]) == BTN_RELEASED)) {
                  //
                  btn = millis();
                }
                if (( window == 2) && ((r_msg->Data[2]) == BTN_RELEASED)) {
                  btn_function(MS_BTN_BC, 0x00);
                  btn = millis();
                }
              }
              break;
            }
          case MS_BTN_MODE: {
              if ((r_msg->Data[0] == BTN_PRESSED) && (r_msg->Data[2] == BTN_RELEASED)) {
                //
                btn = millis();
              }
              break;
            }
          case MS_BTN_LEFT_WHEEL: {
              if (r_msg->Data[0] == BTN_PRESSED) {
                if ((r_msg->Data[2]) == 0x05) {
                  btn_function(MS_BTN_SETTINGS, 0x00);
                  btn = millis();
                }
                if (data2 != r_msg->Data[2])  {
                  data2 = r_msg->Data[2];
                  if (data2 == 0x3C) {
                    btn_function(MS_BTN_SETTINGS, 0x00);
                    btn_function(MS_BTN_SETTINGS, data2);
                    btn = millis();
                    if (test_mode == 0)  {
                      test_mode = 1;
                      Message_USART = "TEST MODE ON";
                    }
                    else if (test_mode == 1 )  {
                      test_mode = 0;
                      Message_USART = "TEST MODE OFF";
                    }
                  }
                }
              }
              break;
            }
        }
        break;
      }

    case MS_ECC_ID: {
        if (r_msg->Data[0] == MS_BATTERY) {
          VOLTAGE = (r_msg->Data[2]);
          if (VOLTAGE != p_VOLTAGE) {
            UART.println("<voltage:" + data_to_str(VOLTAGE, 1) + ">");
            p_VOLTAGE = VOLTAGE;
          }
        }
        if (r_msg->Data[0] == MS_ENGINE_TEMP) {
          T_ENG = (((r_msg->Data[3]) * 256) + (r_msg->Data[4]));
          if (T_ENG != p_T_ENG) {
            UART.println("<t_eng:" + data_to_str(T_ENG, 0) + ">");
            p_T_ENG = T_ENG;
          }
        }
        break;
      }

    case MS_SPEED_RPM_ID: {
        if (r_msg->Data[0] == 0x46) {
          SPEED = ((((r_msg->Data[4]) << 8) + (r_msg->Data[5])) >> 7);
          RPM = ((((r_msg->Data[2]) << 8) + (r_msg->Data[3])) >> 2);
          if ((r_msg->Data[6]) != 0x04) { //задний ход выкл
            REVERSE = 0;
          }
          if ((r_msg->Data[6]) == 0x04) { //задний ход вкл
            REVERSE = 1;
          }
        }
        break;
      }

    case MS_TIME_CLOCK_ID: {
        if (r_msg->Data[0] == 0x46) {
          DAY = ((r_msg->Data[4]) >> 3);
          MONTH = (r_msg->Data[3]);
          YEAR = (r_msg->Data[2]);
        }
        break;
      }

    case MS_RANGE_ID: {
        if (r_msg->Data[0] == 0x46) {
          RANGE = ((((r_msg->Data[2]) << 8) + (r_msg->Data[3])) / 2);
          if (RANGE != p_RANGE) {
            UART.println("<range:" + String(RANGE) + ">");
            p_RANGE = RANGE;
          }
        }
        break;
      }

    case MS_IGNITION_STATE_ID: {
        if ((r_msg->Data[2] ==  MS_IGNITION_NO_KEY) || (r_msg->Data[2] ==  MS_IGNITION_1)) {
          key_acc = 0;
        }
        if ((r_msg->Data[2] ==  MS_IGNITION_KEY_PRESENT) || (r_msg->Data[2] == MS_IGNITION_START)) {
          key_acc = 1;
        }
        break;
      }

    case MS_WINDOW_ID: {
        if (r_msg->Data[1] == 0x0C && r_msg->Data[2] == 0x00 && r_msg->Data[3] == 0xC8) { //main
          window =  1;
        }
        if (r_msg->Data[1] == 0x04 && r_msg->Data[2] == 0x08 && r_msg->Data[3] == 0xC8) { //bc
          window =  2;
        }
        if (((r_msg->Data[1] == 0x0C) || (r_msg->Data[1] == 0x0D)) && r_msg->Data[2] == 0x08 && r_msg->Data[3] == 0xC8) { //settings
          window =  3;
        }
        if (r_msg->Data[1] == 0x0A && r_msg->Data[2] == 0x00 && r_msg->Data[3] == 0xC8) { //climate
          window =  4;
        }
        break;
      }

#endif

  }

}
