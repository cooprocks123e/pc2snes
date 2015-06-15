// pins 8-13
#define S_DATA  8
#define S_LATCH 9
#define S_CLOCK 10

#define LED
//#define DEBUGLED

#define checkLatch (PINB&(1<<(S_LATCH-8)))
#define checkClock (PINB&(1<<(S_CLOCK-8)))
inline void writeData(byte x){ if(x) PORTB|=(1<<(S_DATA-8)); else PORTB&=~(1<<(S_DATA-8)); }
inline void writeLED(byte x){ if(x) PORTB|=(1<<(13-8)); else PORTB&=~(1<<(13-8)); }

byte bits[16];
byte i,im8;
byte tmp;
#ifdef LED
byte led;
#endif

void setup() {
  Serial.begin(115200);
  
  pinMode(S_DATA,OUTPUT);
  pinMode(S_LATCH,INPUT_PULLUP);
  pinMode(S_CLOCK,INPUT_PULLUP);
  digitalWrite(S_DATA,LOW);
  
  #ifdef LED
  led=0;
  pinMode(13,OUTPUT);
  #endif
  
  for(i=0;i<16;i++){
    bits[i]=0;
  }
}

void loop() {
  Serial.print("N");
  while(Serial.read()!='r'){}
  while(!Serial.available()){}
  Serial.read();
  for(i=0;i<16;i++){
    im8=i%8;
    if(im8==0){
      while(!Serial.available()){}
      tmp=Serial.read();
    }
    bits[i]=!(tmp&(1<<im8));
  }
  
  
  #ifdef DEBUGLED
  writeLED(1);
  delay(1000);
  
  for(i=0;i<16;i++){
    writeLED(bits[i]);
    delay(250);
    writeLED(0);
    delay(250);
  }
  #else
  #ifdef LED
  if (led) {
    led=0;writeLED(0);
  } else {
    led=1;writeLED(1);
  }
  #endif
  #endif
  
  i=0;
  
  noInterrupts();
  
  while(!checkLatch);
  writeData(1);
  while(checkLatch);
  for(;i<16;i++){
    writeData(bits[i]);
    while(checkClock);  //wait for falling
    while(!checkClock); //wait for rising
  }
  
  interrupts();
}
