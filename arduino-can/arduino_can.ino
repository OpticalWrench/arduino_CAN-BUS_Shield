#include <SPI.h>
#include <CAN.h>
#include <parallax_LCD_27977.h>
#include <SoftwareSerial.h>

// LCD pin numbers
#define LCD_SERIAL_RECEIVE_PIN 3
#define LCD_SERIAL_TRANSMIT_PIN 6

// CAN-BUS Shield joystick pin numbers
#define JOYSTICK_UP_PIN A1
#define JOYSTICK_RIGHT_PIN A2
#define JOYSTICK_DOWN_PIN A3
#define JOYSTICK_CLICK_PIN A4
#define JOYSTICK_LEFT_PIN A5

// CAN-BUS Shield LED pin numbers
#define CAN_SHIELD_LED_2_PIN 8
#define CAN_SHIELD_LED_3_PIN 7

// SoftwareSerial object used to interact with the LCD
SoftwareSerial parallax_serial_LCD(LCD_SERIAL_RECEIVE_PIN, LCD_SERIAL_TRANSMIT_PIN);

byte can_transmit_buffer[8]; // storage area for CAN messages to be transmitted
byte can_receive_buffer[8]; // storage area for received CAN messages
unsigned short transmit_message_id = 99; // CAN message ID used in messages sent to the CAN bus from this program
byte received_message_length = 0; // length of received message (number of bytes)
unsigned short received_message_id = 0; // CAN message ID from received message
unsigned long display_delay_duration = 5000; // time in milliseconds to keep a received CAN message and info on LCD screen before clearing
unsigned long message_received_time = 0; // millis() at time of receiving a CAN message

// EMPTY_LINE is declared in the LCD header file
String LCD_line_one = EMPTY_LINE; // LCD dsiplay buffer top line
String LCD_line_two = EMPTY_LINE; // LCD display buffer bottom line

void setup() {
  
  // CAN-BUS Shield I/O pin assignments
  pinMode(CAN_SHIELD_LED_2_PIN, OUTPUT);
  pinMode(CAN_SHIELD_LED_3_PIN, OUTPUT);
  
    // REMEMBER: The CAN-BUS Shield joystick uses true-LOW logic.
  // When the pin is read as LOW, then that direction or click is active.
  pinMode(JOYSTICK_UP_PIN, INPUT_PULLUP);
  pinMode(JOYSTICK_DOWN_PIN, INPUT_PULLUP);
  pinMode(JOYSTICK_LEFT_PIN, INPUT_PULLUP);
  pinMode(JOYSTICK_RIGHT_PIN, INPUT_PULLUP);
  pinMode(JOYSTICK_CLICK_PIN, INPUT_PULLUP);
  
  // setup the LCD screen
  parallax_serial_LCD.begin(19200); // make sure the switches are set to the same baud rate
  parallax_serial_LCD.write(SCREEN_MODE_0);
  parallax_serial_LCD.write(BACKLIGHT_OFF);
  parallax_serial_LCD.write(CLEAR_SCREEN);
  delay(10);
  
  // initialize the LCD buffers
  LCD_line_one = EMPTY_LINE;
  LCD_line_two = EMPTY_LINE;

  // initialize and setup the CAN object
  CAN.begin();
  CAN.baudConfig(500);
  CAN.setMode(NORMAL);
  
  // initialize the receive message buffer
  can_receive_buffer[0] = 0x00;
  can_receive_buffer[1] = 0x00;
  can_receive_buffer[2] = 0x00;
  can_receive_buffer[3] = 0x00;
  can_receive_buffer[4] = 0x00;
  can_receive_buffer[5] = 0x00;
  can_receive_buffer[6] = 0x00;
  can_receive_buffer[7] = 0x00;
  
  // initialize the transmit message buffer
  can_transmit_buffer[0] = 0x00;
  can_transmit_buffer[1] = 0x00;
  can_transmit_buffer[2] = 0x00;
  can_transmit_buffer[3] = 0x00;
  can_transmit_buffer[4] = 0x00;
  can_transmit_buffer[5] = 0x00;
  can_transmit_buffer[6] = 0x00;
  can_transmit_buffer[7] = 0x00;
  
  //  initialize the CAN-BUS Shield transmit buffer to all zeros to be sure there's no garbage in it.
  CAN.load_ff_0((byte) 8, transmit_message_id, &can_transmit_buffer[0]);
}

void loop() {
  
  // clear/empty the transmit buffer
  can_transmit_buffer[0] = 0x00;
  can_transmit_buffer[1] = 0x00;
  can_transmit_buffer[2] = 0x00;
  can_transmit_buffer[3] = 0x00;
  can_transmit_buffer[4] = 0x00;
  can_transmit_buffer[5] = 0x00;
  can_transmit_buffer[6] = 0x00;
  can_transmit_buffer[7] = 0x00;
  
  // clear/empty the receive message buffer
  can_receive_buffer[0] = 0x00;
  can_receive_buffer[1] = 0x00;
  can_receive_buffer[2] = 0x00;
  can_receive_buffer[3] = 0x00;
  can_receive_buffer[4] = 0x00;
  can_receive_buffer[5] = 0x00;
  can_receive_buffer[6] = 0x00;
  can_receive_buffer[7] = 0x00;
  
  // clear the LCD buffers after the "keep it visible" delay time has transpired
  if((millis() - message_received_time) >= display_delay_duration){
    LCD_line_one = EMPTY_LINE;
    LCD_line_two = EMPTY_LINE;
  }
  
  // read all of the joystick digital inputs
  if (digitalRead(JOYSTICK_CLICK_PIN) == LOW){
    LCD_line_one = "stick is CLICKED";
    can_transmit_buffer[0] = 8;
  }
  if (digitalRead(JOYSTICK_UP_PIN) == LOW){
    LCD_line_one = "stick is UP";
    can_transmit_buffer[0] = 1;
  }
  if (digitalRead(JOYSTICK_DOWN_PIN) == LOW){
    LCD_line_one = "stick is DOWN";
    can_transmit_buffer[0] = 3;
  }
  if (digitalRead(JOYSTICK_LEFT_PIN) == LOW){
    LCD_line_one = "stick is LEFT";
    can_transmit_buffer[0] = 4;
  }
  if (digitalRead(JOYSTICK_RIGHT_PIN) == LOW){
    LCD_line_one = "stick is RIGHT";
    can_transmit_buffer[0] = 2;
  }
  
  // if a joystick input was detected, then send a message to the CAN bus
  if(can_transmit_buffer[0] != 0){
    CAN.load_ff_0((byte) 8, transmit_message_id, &can_transmit_buffer[0]);
    CAN.send_0();
    delay(1000); // transmit the joystic position CAN message only once every second
  }

  // Check for received messages in the CAN-BUS Shield buffer ZERO
  // if there is data in the CAN Shield buffer, then copy it to can_receive_buffer
  if((CAN.readStatus() & 0x01) == 0x01 || (CAN.readStatus() & 0x02) == 0x02){
    message_received_time = millis();
    CAN.readDATA_ff_0(&received_message_length, can_receive_buffer, &received_message_id);
    
    LCD_line_one = EMPTY_LINE;
    LCD_line_two = EMPTY_LINE;

    // assemble line one of the LCD
    LCD_line_one = "id ";
    LCD_line_one += received_message_id;
    LCD_line_one += "  len ";
    LCD_line_one += received_message_length;
    
    parallax_serial_LCD.write(MOVE_CURSOR_0_0); // move to top row, all the way left
    parallax_serial_LCD.print(LCD_line_one); // write line one to LCD screen
    
    display_entire_received_CAN_message();

  }

  //  write the LCD buffers to the LCD screen
  parallax_serial_LCD.write(MOVE_CURSOR_0_0); // move to top row, all the way left
  parallax_serial_LCD.print(LCD_line_one); // write line one to LCD screen
  parallax_serial_LCD.write(MOVE_CURSOR_1_0); // move to bottom row, all the way left
  parallax_serial_LCD.print(LCD_line_two); // write line two to LCD screen
  
}

void display_entire_received_CAN_message(void){
  
  unsigned long byte_display_time = 1500;
  
  // assemble line two of the LCD
  LCD_line_two = EMPTY_LINE;
  LCD_line_two = "B0: ";
  LCD_line_two += can_receive_buffer[0];
  LCD_line_two += "  B1: ";
  LCD_line_two += can_receive_buffer[1];   
  parallax_serial_LCD.write(MOVE_CURSOR_1_0); // move to bottom row, all the way left
  parallax_serial_LCD.print(LCD_line_two); // write line two to LCD screen
  delay(byte_display_time);
  
  LCD_line_two = EMPTY_LINE;
  LCD_line_two = "B2: ";
  LCD_line_two += can_receive_buffer[2];
  LCD_line_two += "  B3: ";
  LCD_line_two += can_receive_buffer[3];   
  parallax_serial_LCD.write(MOVE_CURSOR_1_0); // move to bottom row, all the way left
  parallax_serial_LCD.print(LCD_line_two); // write line two to LCD screen
  delay(byte_display_time);
  
  LCD_line_two = EMPTY_LINE;
  LCD_line_two = "B4: ";
  LCD_line_two += can_receive_buffer[4];
  LCD_line_two += "  B5: ";
  LCD_line_two += can_receive_buffer[5];   
  parallax_serial_LCD.write(MOVE_CURSOR_1_0); // move to bottom row, all the way left
  parallax_serial_LCD.print(LCD_line_two); // write line two to LCD screen
  delay(byte_display_time);
  
  LCD_line_two = EMPTY_LINE;
  LCD_line_two = "B6: ";
  LCD_line_two += can_receive_buffer[6];
  LCD_line_two += "  B7: ";
  LCD_line_two += can_receive_buffer[7];   
  parallax_serial_LCD.write(MOVE_CURSOR_1_0); // move to bottom row, all the way left
  parallax_serial_LCD.print(LCD_line_two); // write line two to LCD screen
  delay(byte_display_time);
  
  return;
}
