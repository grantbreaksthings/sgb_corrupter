//Define stuff for Arduino register functions
#define CLR(x,y) (x&=(~(1<<y)))
#define SET(x,y) (x|=(1<<y))

//Pin connected to ST_CP of 74HC595, green wire
#define latch_pin 2
//Pin connected to SH_CP of 74HC595, yellow wire
#define clock_pin 3
//Pin connected to DS of 74HC595, blue wire
#define data_pin 4

//Pin connected to WRAM Data Corruption Button
#define wram_data_button 19
//Pin connected to WRAM Address Corruption Button
#define wram_addr_button 18
//Pin connected to Cartridge Data Corruption Button
#define cart_data_button 16
//Pin connected to Cartridge Address Corruption Button
#define cart_addr_button 17

//Pin connected to WRAM Data Corruption Status LED
#define wram_data_led 12
//Pin connected to WRAM Address Corruption Status LED
#define wram_addr_led 7
//Pin connected to Cartridge Data Corruption Status LED
#define cart_data_led 6
//Pin connected to Cartridge Address Corruption Status LED
#define cart_addr_led 5

//Pin connected to the analog output pin of the VRAM Address Corruption Potentiometer
#define vram_addr_pot 0
//Pin connected to the analog output pin of the VRAM Data Corruption Potentiometer
#define vram_data_pot 1

//Define how long in milleseconds to wait between WRAM and Cart corruptions
#define wram_cart_corrupt_delay 75

void setup() {
  //Set shift register output pins
  pinMode(latch_pin, OUTPUT);
  pinMode(clock_pin, OUTPUT);
  pinMode(data_pin, OUTPUT);

  //Set inputs for WRAM and Cartridge corruption buttons
  pinMode(wram_data_button, INPUT);
  pinMode(wram_addr_button, INPUT);
  pinMode(cart_data_button, INPUT);
  pinMode(cart_addr_button, INPUT);

  //Set outputs for WRAM and Cartridge status LEDs
  pinMode(wram_data_led, OUTPUT);
  pinMode(wram_addr_led, OUTPUT);
  pinMode(cart_data_led, OUTPUT);
  pinMode(cart_addr_led, OUTPUT);
  
  Serial.begin(9600);
}

void loop() {
  corrupt_vram(analogRead(vram_addr_pot), analogRead(vram_data_pot));
  corrupt_cart_wram(digitalRead(wram_data_button), digitalRead(wram_addr_button), digitalRead(cart_data_button), digitalRead(cart_addr_button));
}

//Communicate with the shift registers to pull the various VRAM bus lines low
void corrupt_vram(int addr_pot, int data_pot){
  
  //Read the data from the address and data potentiometers and convert the reading to a number between 0 and 1
  //When this byte is written to the shift registers, it will pull the corresponding VRAM bus lines to low.
  int rand_addr = (addr_pot/8);
  int rand_data = (data_pot/8);
  
  //Bitmask the byte to be sent to the shift registers and prevent the 4th bit from being set high. When this 
  //bit is set to high it generates a bunch of parallel lines on the SGB screen that just persist and break the game.
  rand_data = rand_data&0b11110111;
  rand_addr = rand_addr&0b11110111;
  
  //Write the byte out to the shift registers
  digitalWrite(latch_pin, LOW);
  Serial.println(rand_addr);
  shiftOut(data_pin, clock_pin, LSBFIRST, rand_addr);  
  //digitalWrite(latch_pin, HIGH);
  //digitalWrite(latch_pin, LOW);
  Serial.println(rand_data);
  shiftOut(data_pin, clock_pin, LSBFIRST, rand_data);  
  digitalWrite(latch_pin, HIGH);
}

//Scan through the various button inputs on the corrupter and see if any of them are being pressed. This would be much 
//better served with an interrupt but I ran out of interruptable pins and I don't feel like resoldering everything.
void corrupt_cart_wram(bool wram_data_button_status, bool wram_addr_button_status, bool cart_data_button_status, bool cart_addr_button_status)
{
  //Check to see if a button is pressed, flash the corresponding status LED, and use corrupt_line(int port) to perform the corruption.
  if(wram_data_button_status)
  {
    digitalWrite(wram_data_led, HIGH);
    corrupt_line(2);
    delay(wram_cart_corrupt_delay);
    digitalWrite(wram_data_led, LOW);
  }

  if(wram_addr_button_status)
  {
    digitalWrite(wram_addr_led, HIGH);
    corrupt_line(1);
    delay(wram_cart_corrupt_delay);
    digitalWrite(wram_addr_led, LOW);
  }

  if(cart_data_button_status)
  {
    digitalWrite(cart_data_led, HIGH);
    corrupt_line(3);
    delay(wram_cart_corrupt_delay);
    digitalWrite(cart_data_led, LOW);
  }

  if(cart_addr_button_status)
  {
    digitalWrite(cart_addr_led, HIGH);
    corrupt_line(0);
    delay(wram_cart_corrupt_delay);
    digitalWrite(cart_addr_led, LOW);
  }
}

//This function actually does the corrupting on the WRAM and Cartridge busses. Instead of using more conventional functions like digitalWrite() I had
//to implement this by setting registers and writing some basic assembly code. This is due to timing issues with the Arduino and the SGB. The SGB
//runs at about 4 Mhz and the Arduino runs at about 16 Mhz. If corruptions are performed for longer than 1 CPU cycle on the SGB, it is very likely
//to cause an instant crash. Functions like digitalWrite() take about 50 Arduino cycles to complete, which means that the corruption will be active for
//about 12 SGB cycles. Setting a port register only takes about 2 Arduino cycles. Combining this with some nops written in assmbly allows the Arduino
//to perform corruptions must faster and minimize crashes.
void corrupt_line(int port)
{
  //Set the port register to high. This basically does the same thing as digitalWrite(pin, HIGH)
  SET(PORTB, port);
  
  //Since there aren't functions to delay for nanoseconds, just tell the Arduino to wait for 5 CPU cycles. I found that waiting for 5 cycles produced
  //the best corruptions
  __asm__("nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t");

  //Set the port register to low. This basically does the same thing as digitalWrite(pin, LOW)
  CLR(PORTB, port);
}

