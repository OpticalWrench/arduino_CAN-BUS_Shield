#include <SPI.h>
#include <CAN.h>
#include <parallax_LCD_27977.h>
#include <SoftwareSerial.h>

// LCD pin numbers
#define receive_pin_number 3
#define transmit_pin_number 6

// CAN-BUS Shield joystick pin numbers
#define JOYSTICK_UP_PIN A1
#define JOYSTICK_RIGHT_PIN A2
#define JOYSTICK_DOWN_PIN A3
#define JOYSTICK_CLICK_PIN A4
#define JOYSTICK_LEFT_PIN A5

#define CAN_SHIELD_LED_2 8
#define CAN_SHIELD_LED_3 7

const unsigned short can_id = 99;

SoftwareSerial parallax_serial_LCD(receive_pin_number, transmit_pin_number);
byte Tx_buffer[8];
byte Rx_buffer[8];
byte Rx_message_length = 0;
unsigned short frame_id = 0;
unsigned long display_delay = 0;

String LCD_line_one = EMPTY_LINE;
String LCD_line_two = EMPTY_LINE;

String pad_screen_buffer

void setup() {
  pinMode(CAN_SHIELD_LED_2, OUTPUT);
  pinMode(CAN_SHIELD_LED_3, OUTPUT);
  pinMode(JOYSTICK_UP_PIN, INPUT);
  pinMode(JOYSTICK_DOWN_PIN, INPUT);
  pinMode(JOYSTICK_LEFT_PIN, INPUT);
  pinMode(JOYSTICK_RIGHT_PIN, INPUT);
  pinMode(JOYSTICK_CLICK_PIN, INPUT);
  
  digitalWrite(JOYSTICK_UP_PIN, HIGH); // Enable internal pull-up resistor on digital input
  digitalWrite(JOYSTICK_DOWN_PIN, HIGH); // Enable internal pull-up resistor on digital input
  digitalWrite(JOYSTICK_LEFT_PIN, HIGH); // Enable internal pull-up resistor on digital input
  digitalWrite(JOYSTICK_RIGHT_PIN, HIGH); // Enable internal pull-up resistor on digital input
  digitalWrite(JOYSTICK_CLICK_PIN, HIGH); // Enable internal pull-up resistor on digital input
  
  parallax_serial_LCD.begin(19200); // !!! make sure the switches are set to the same baud rate
  parallax_serial_LCD.write(SCREEN_MODE_0);
  parallax_serial_LCD.write(BACKLIGHT_OFF);
  parallax_serial_LCD.write(CLEAR_SCREEN);
  delay(10);
  LCD_line_one = EMPTY_LINE;
  LCD_line_two = EMPTY_LINE;

  CAN.begin();
  CAN.setMode(CONFIGURATION);
  CAN.baudConfig(500);
  CAN.setMode(NORMAL);
  
  // initialize the receive message buffer
  Rx_buffer[0] = 0x00;
  Rx_buffer[1] = 0x00;
  Rx_buffer[2] = 0x00;
  Rx_buffer[3] = 0x00;
  Rx_buffer[4] = 0x00;
  Rx_buffer[5] = 0x00;
  Rx_buffer[6] = 0x00;
  Rx_buffer[7] = 0x00;
  
  // initialize the transmit message buffer
  Tx_buffer[0] = 0x00;
  Tx_buffer[1] = 0x00;
  Tx_buffer[2] = 0x00;
  Tx_buffer[3] = 0x00;
  Tx_buffer[4] = 0x00;
  Tx_buffer[5] = 0x00;
  Tx_buffer[6] = 0x00;
  Tx_buffer[7] = 0x00;
  CAN.load_ff_0((byte) 8, can_id, &Tx_buffer[0]);
}

void loop() {
  Tx_buffer[0] = 0;
  LCD_line_one = EMPTY_LINE;
  
  if (digitalRead(JOYSTICK_CLICK_PIN) == LOW){
    LCD_line_one = "stick is CLICKED";
    LCD_line_two = EMPTY_LINE;
    Tx_buffer[0] = 8;
  }
  if (digitalRead(JOYSTICK_UP_PIN) == LOW){
    LCD_line_one = "stick is UP";
    Tx_buffer[0] = 1;
  }
  if (digitalRead(JOYSTICK_DOWN_PIN) == LOW){
    LCD_line_one = "stick is DOWN";
    Tx_buffer[0] = 3;
  }
  if (digitalRead(JOYSTICK_LEFT_PIN) == LOW){
    LCD_line_one = "stick is LEFT";
    Tx_buffer[0] = 4;
  }
  if (digitalRead(JOYSTICK_RIGHT_PIN) == LOW){
    LCD_line_one = "stick is RIGHT";
    Tx_buffer[0] = 2;
  }
  
  CAN.load_ff_0((byte) 8, can_id, &Tx_buffer[0]);
  if(Tx_buffer[0] != 0){
    CAN.send_0();
    delay(1000);
  }

  if((CAN.readStatus() & 0x01) == 0x01){
    CAN.readDATA_ff_0(&Rx_message_length,Rx_buffer,&frame_id);
    LCD_line_two = EMPTY_LINE;
    LCD_line_two = "Rx msg id:";
    LCD_line_two += frame_id;
  }
  if((CAN.readStatus() & 0x02) == 0x02){
    CAN.readDATA_ff_1(&Rx_message_length,Rx_buffer,&frame_id);
    LCD_line_two = "caught 0x02";
  }

  parallax_serial_LCD.write(MOVE_CURSOR_0_0);
  parallax_serial_LCD.print(LCD_line_one);
  parallax_serial_LCD.write(MOVE_CURSOR_1_0);
  parallax_serial_LCD.print(LCD_line_two);
  
}
