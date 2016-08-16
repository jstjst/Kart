////////////////
//  MEGA32    //
//  16MHz     //
////////////////

//Einstellungen
//Zeit fuer Wechsel Warnung (ms)
#define warnung_delay 100
//LED Multiplexing Umschaltzeit (ms)
#define updateInterval 10

#define NSL PB0   //Ausgang fuer Nebelschlussleuchte
#define DZB PB1   //Ausgang fuer Drehzahlband
#define SEG PB3   //Ausgang fuer Segment Display

//Status-LEDs
#define S_LED_1 PB4
#define S_LED_2 PB5
#define S_LED_3 PB6
#define S_LED_4 PB7

//Drehzahlanzeige
const byte drehzahl[9] = {
  B00000000,
  B00000001,
  B00000011,
  B00000111,
  B00001111,
  B00011111,
  B00111111,
  B01111111,
  B11111111
};

//Warnung
const byte warnung[2] = {
  B10101010,
  B01010101
};

//D,N,R,P  Port C   ABNCPRDE              Port D   FGTSUHKM
const byte d_pc = {B11010011}; const byte d_pd = {B10010001};
const byte n_pc = {B00010110}; const byte n_pd = {B01000110};
const byte r_pc = {B11011100}; const byte r_pd = {B01001100};
const byte p_pc = {B11011000}; const byte p_pd = {B01001100};

//Gang Port C
const byte gang_pc[5] = {
  //ABNCPRDE
  B00110010,   //1
  B11011001,   //2
  B11011011,   //3
  B00011010,   //4
  B11011011    //5
};
//Gang Port D
const byte gang_pd[5] = {
  //FGTSUHKM
  B00000000,   //1
  B11001000,   //2
  B10000000,   //3
  B00001100,   //4
  B10001100    //5
};

void setup()
{
  init_IO();

  //Interrupt Einstellung
  OCR0 = 0xAF;
  TIMSK |= _BV(OCIE0);
}


//Interrupt wird jede ms aufgerufen
SIGNAL(TIMER0_COMPA_vect)
{
  unsigned long currentMillis = millis();
  led(currentMillis, , , , );   //LED Multiplexing
}

//Initialisierung Ein-/Ausgaenge
void init_IO()
{
  //LED Ports (C,D) als Ausgang setzen
  DDRC = B11111111;
  DDRD = B11111111;
  PORTC = B00000000;
  PORTD = B00000000;

  //Transistor fuer Nebelschlussleuchte
  pinMode(NSL, OUTPUT);
  digitalWrite(NSL, LOW);

  //Transistoren fuer LED Ansteuerung
  pinMode(DZB, OUTPUT);
  pinMode(SEG, OUTPUT);
  digitalWrite(DZB, LOW);
  digitalWrite(SEG, LOW);

  //Status-LEDs
  pinMode(S_LED_1, OUTPUT);
  pinMode(S_LED_2, OUTPUT);
  pinMode(S_LED_3, OUTPUT);
  pinMode(S_LED_4, OUTPUT);
  digitalWrite(S_LED_1, HIGH);
  digitalWrite(S_LED_2, HIGH);
  digitalWrite(S_LED_3, HIGH);
  digitalWrite(S_LED_4, HIGH);
}

//Startbild
void start()
{
  //Status-LEDs
  digitalWrite(S_LED_1, LOW);   delay(100);   digitalWrite(S_LED_1, HIGH);
  digitalWrite(S_LED_2, LOW);   delay(100);   digitalWrite(S_LED_2, HIGH);
  digitalWrite(S_LED_3, LOW);   delay(100);   digitalWrite(S_LED_3, HIGH);
  digitalWrite(S_LED_4, LOW);   delay(100);   digitalWrite(S_LED_4, HIGH);

  //DZB
  digitalWrite(DZB, HIGH);
  shift(&PORTC, 8, 100);
  PORTC = B00000000;
  shift(&PORTD, 7, 100);
  PORTD = B00000000;
  digitalWrite(DZB, LOW);

  //SEG
  digitalWrite(SEG, HIGH);
  shift(&PORTC, 8, 100);
  PORTC = B00000000;
  shift(&PORTD, 8, 100);
  PORTD = B00000000;
  igitalWrite(SEG, LOW);
}

//Port durchshiften
void shift(volatile uint8_t *port, int j, int ms)
{
  for (int i = 0; i < j; i++)
  {
    *port = 1 << i;
    delay(ms);
  }
}

void led(unsigned long currentMillis, byte dzb_c, byte dzb_d, byte seg_c, byte seg_d)
{
  static unsigned long lastUpdate;
  static bool state;   //SEG = 0  DZB = 1

  //Wechsel zwischen SEG und DZB ueber state
  if (state == 0 && (currentMillis - lastUpdate) > updateInterval)        //Zeit fuer Update
  {
    lastUpdate = currentMillis;
    state = 1;
    digitalWrite(DZB, LOW);
    digitalWrite(SEG, HIGH);
    PORTC = seg_c;
    PORTD = seg_d;
  }
  else if (state == 1 && (currentMillis - lastUpdate) > updateInterval)   //Zeit fuer Update
  {
    lastUpdate = currentMillis;
    state = 0;
    digitalWrite(SEG, LOW);
    digitalWrite(DZB, HIGH);
    PORTC = dzb_c;
    PORTD = dzb_d;
  }
}


void loop()
{}
