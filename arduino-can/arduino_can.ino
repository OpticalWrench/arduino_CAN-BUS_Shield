#include <SPI.h>
#include <CAN.h>
#include <parallax_LCD_27977.h>
#include <SoftwareSerial.h>

// LCD pin numbers
#define receive_pin_number 3
#define transmit_pin_number 6

// CAN-BUS Shield joystick pin numbers
#define STICK_UP A1
#define STICK_RIGHT A2
#define STICK_DOWN A3
#define STICK_CLICK A4
#define STICK_LEFT A5

#define CAN_SHIELD_LED_2 8
#define CAN_SHIELD_LED_3 7

const unsigned short can_id = 99;

SoftwareSerial parallax_serial_LCD(receive_pin_number, transmit_pin_number);
can_message testdata;

String LCD_line_one = EMPTY_LINE;
String LCD_line_two = EMPTY_LINE;

void setup() {
  pinMode(CAN_SHIELD_LED_2, OUTPUT);
  pinMode(CAN_SHIELD_LED_3, OUTPUT);
  pinMode(STICK_UP,INPUT);
  pinMode(STICK_DOWN,INPUT);
  pinMode(STICK_LEFT,INPUT);
  pinMode(STICK_RIGHT,INPUT);
  pinMode(STICK_CLICK,INPUT);
  
  digitalWrite(STICK_UP, HIGH); // Enable internal pull-up resistor on digital input
  digitalWrite(STICK_DOWN, HIGH); // Enable internal pull-up resistor on digital input
  digitalWrite(STICK_LEFT, HIGH); // Enable internal pull-up resistor on digital input
  digitalWrite(STICK_RIGHT, HIGH); // Enable internal pull-up resistor on digital input
  digitalWrite(STICK_CLICK, HIGH); // Enable internal pull-up resistor on digital input
  
  parallax_serial_LCD.begin(19200); // !!! make sure the switches are set to the same baud rate
  parallax_serial_LCD.write(SCREEN_MODE_0);
  parallax_serial_LCD.write(BACKLIGHT_OFF);
  parallax_serial_LCD.write(CLEAR_SCREEN);
  delay(10);
  CAN.begin();
  CAN.baudConfig(500);
  CAN.setMode(NORMAL);
  
  // initialize the message buffer
  testdata.msg_byte[0] = 0;
  testdata.msg_byte[1] = 0;
  testdata.msg_byte[2] = 0;
  testdata.msg_byte[3] = 0;
  testdata.msg_byte[4] = 0;
  testdata.msg_byte[5] = 0;
  testdata.msg_byte[6] = 0;
  testdata.msg_byte[7] = 0;
  CAN.load_ff_0((byte) 8, can_id, &testdata.msg_byte[0]);
}

void loop() {
  
  LCD_line_one = EMPTY_LINE;
  LCD_line_two = EMPTY_LINE;
  testdata.msg_byte[0] = 0;
  
  if (digitalRead(STICK_CLICK) == LOW){
    LCD_line_one = "stick is CLICKED";
    testdata.msg_byte[0] = 8;
  }
  if (digitalRead(STICK_UP) == LOW){
    LCD_line_one = "stick is UP";
    testdata.msg_byte[0] = 1;
  }
  if (digitalRead(STICK_DOWN) == LOW){
    LCD_line_one = "stick is DOWN";
    testdata.msg_byte[0] = 3;
  }
  if (digitalRead(STICK_LEFT) == LOW){
    LCD_line_one = "stick is LEFT";
    testdata.msg_byte[0] = 4;
  }
  if (digitalRead(STICK_RIGHT) == LOW){
    LCD_line_one = "stick is RIGHT";
    testdata.msg_byte[0] = 2;
  }
  
  CAN.load_ff_0((byte) 8, can_id, &testdata.msg_byte[0]);
  if(testdata.msg_byte[0] != 0){
    CAN.send_0();
    delay(1000);
  }

  parallax_serial_LCD.write(MOVE_CURSOR_0_0);
  parallax_serial_LCD.print(LCD_line_one);
  parallax_serial_LCD.write(MOVE_CURSOR_1_0);
  parallax_serial_LCD.print(LCD_line_two);
  
}
