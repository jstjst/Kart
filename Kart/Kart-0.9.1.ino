////////////////
//  MEGA32    //
//  16MHz     //
////////////////

//Einstellungen
  //LED Multiplexing Umschaltzeit (ms)
    #define updateInterval 10

  //Zeiten(ms)
    //Startbild
      #define T_SB_STATUS   50
      #define T_SB_FUELLEN  50
      #define T_SB_WARNUNG  50
      #define T_SB_LEEREN   50
      #define T_SB_ALLE     50
      #define T_SB_ZEICHEN  50
      #define T_SB_START    50

  //Ausgänge
    #define NSL PB0   //Ausgang fuer Nebelschlussleuchte (PB0)
    #define DZB PB1   //Ausgang fuer Drehzahlband (PB1)
    #define SEG PB3  //Ausgang fuer Segment Display (PB3)

  //Status-LEDs
    #define S_LED_1 PB4
    #define S_LED_2 PB5
    #define S_LED_3 PB6
    #define S_LED_4 PB7

  //Gaenge
    #define DN PA4    //D/N
    #define NR PA5    //N/R

  //Bremsen
    #define B PA7     //Bremse
    #define HB PA6    //Handbremse

  //Drehzahlanzeige
    const word drehzahl[16] = {
      0b0000000000000000,
      0b0000000000000001,
      0b0000000000000011,
      0b0000000000000111,
      0b0000000000001111,
      0b0000000000011111,
      0b0000000000111111,
      0b0000000001111111,
      0b0000000011111111,
      0b0000000111111111,
      0b0000001111111111,
      0b0000011111111111,
      0b0000111111111111,
      0b0001111111111111,
      0b0011111111111111,
      0b0111111111111111
    };

  //Warnung
    const word warnung = 0b0101010101010101;

  //Segment Zeichen
    const word gang_let_seg[5] = {
      //FGTSUHKMABNCPRDE
        0b1001000111010011,   //D
        0b0100011000010110,   //N
        0b0100110011011100,   //R
        0b0100110011011000,   //P
        0b0110010000010110    //W
    };

  //Segmnet Zahlen
    const word gang_seg[5] = {
      //FGTSUHKMABNCPRDE
        0b0000000000110010,   //1
        0b1100100011011001,   //2
        0b1000000011011011,   //3
        0b0000110000011010,   //4
        0b1000110011001011    //5
    };


//Beschreibung
  void setup()
    {
      init_IO();
      start();

      //Interrupt Einstellung
        OCR0 = 0xAF;
        TIMSK |= _BV(OCIE0);
    }


//Interrupt wird jede ms aufgerufen
  SIGNAL(TIMER0_COMPA_vect)
  {
    unsigned long currentMillis = millis();
    //int seg = gang(digitalRead(DN), digitalRead(NR), digitalRead(HB));
    //led(currentMillis, , );   //LED Multiplexing
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
      digitalWrite(NSL, HIGH);

    //Transistoren fuer LED Ansteuerung
      pinMode(DZB, OUTPUT);
      pinMode(SEG, OUTPUT);
      digitalWrite(DZB, HIGH);
      digitalWrite(SEG, HIGH);

    //Status-LEDs
      pinMode(S_LED_1, OUTPUT);
      pinMode(S_LED_2, OUTPUT);
      pinMode(S_LED_3, OUTPUT);
      pinMode(S_LED_4, OUTPUT);
      digitalWrite(S_LED_1, HIGH);
      digitalWrite(S_LED_2, HIGH);
      digitalWrite(S_LED_3, HIGH);
      digitalWrite(S_LED_4, HIGH);

    //Gaenge
      pinMode(DN, INPUT);
      pinMode(NR, INPUT);

    //Bremsen
      pinMode(B, INPUT);
      pinMode(HB, INPUT);
  }

//Startbild
  void start()
  {
    { //Status        
        //Status-LEDs
          digitalWrite(S_LED_1, LOW);   delay(T_SB_STATUS);   digitalWrite(S_LED_1, HIGH);
          digitalWrite(S_LED_2, LOW);   delay(T_SB_STATUS);   digitalWrite(S_LED_2, HIGH);
          digitalWrite(S_LED_3, LOW);   delay(T_SB_STATUS);   digitalWrite(S_LED_3, HIGH);
          digitalWrite(S_LED_4, LOW);   delay(T_SB_STATUS);   digitalWrite(S_LED_4, HIGH);

        //DZB
          digitalWrite(DZB, LOW);
          shift_left(&PORTC, 1, 8, T_SB_STATUS);
          shift_left(&PORTD, 1, 7, T_SB_STATUS);
          digitalWrite(DZB, HIGH);

        //SEG
          digitalWrite(SEG, LOW);
          shift_left(&PORTC, 1, 8, T_SB_STATUS);
          shift_left(&PORTD, 1, 8, T_SB_STATUS);
          digitalWrite(SEG, HIGH);
    }
    
    { //Drehzahl
        digitalWrite(DZB, LOW);
    
       { //auffuellen
            for (int i = 0; i < 16; i++)
            {
              PORTC = lowByte(drehzahl[i]);
              PORTD = highByte(drehzahl[i]);
              delay(T_SB_FUELLEN);
            }
        }
    
        { //warnung
            PORTC = lowByte(warnung);
            PORTD = highByte(warnung);
            delay(T_SB_WARNUNG);
            PORTC = ~lowByte(warnung);
            PORTD = ~highByte(warnung);
            delay(T_SB_WARNUNG);
        }
   
        { //leeren
            for (int i = 15; i > 0; i--)
            {
              PORTC = lowByte(drehzahl[i]);
              PORTD = highByte(drehzahl[i]);
              delay(T_SB_LEEREN);

            //Entfernt zur Fehlerbehebung:
            /*PORTC = 0b11111111;
              shift_right(&PORTD, 0b01111111, 7, T_SB_LEEREN);
              shift_right(&PORTC, 0b11111111, 8, T_SB_LEEREN);
            */
            }
        }
    
        digitalWrite(DZB, HIGH);
    }
  
    { //Gaenge
        digitalWrite(SEG, LOW);
          { //alle
              PORTC = 0b11111111;
              PORTD = 0b11111111;
              delay(T_SB_ALLE);
          }
    
    { //Zeichen
        for (int i = 0; i < 5; i++)
          {
            PORTC = lowByte(gang_let_seg[i]);
            PORTD = highByte(gang_let_seg[i]);
            delay(T_SB_ZEICHEN);
          }

        for (int i = 0; i < 5; i++)
          {
            PORTC = lowByte(gang_seg[i]);
            PORTD = highByte(gang_seg[i]);
            delay(T_SB_ZEICHEN);
          }
    }
    
    { //aus
          PORTC = 0b00000000;
          PORTD = 0b00000000;
    }
      
    digitalWrite(SEG, HIGH);
  }
  
  delay(T_SB_START);   //alles aus
}

//Beschreibung
  int gang(bool dn, bool nr, bool hb)
  {
    //return: D=0 N=1 R=2 P=3 W=4

      if (dn == 0 && nr == 1 && hb == 1)
      {
        return 0;
      }

      else if (dn == 1 && nr == 0 && hb == 1)
      {
        return 2;
      }
  
      else if (dn == 0 && nr == 0 && hb == 1)
      {
        return 1;
      }
  
      else if ((dn == 1 && nr == 1 && hb == 0) || (dn == 0 && nr == 0 && hb == 0))
      {
        return 3;
      }
  
      else if ((dn == 1 && nr == 0 && hb == 0) || (dn == 0 && nr == 1 && hb == 0))
      {
        return 4;
      }
  }

//Port durchshiften
  void shift_left(volatile uint8_t *port, uint8_t mask, int j, int ms)
  {
    for (int i = 0; i < j; i++)
    {
      *port = mask << i;
      delay(ms);
    }
    
    *port = 0b00000000;
  }

  void shift_right(volatile uint8_t *port, uint8_t mask, int j, int ms)
  {
    for (int i = 0; i < j; i++)
    {
      *port = mask << i;
      delay(ms);
    }
    
    *port = 0b00000000;
  }

//Bescheibung
  void led(unsigned long currentMillis, word dzb, word seg)
  {
    static unsigned long lastUpdate;
    static bool state;   //SEG = 0  DZB = 1

  //Wechsel zwischen SEG und DZB ueber state
    if (state == 0 && (currentMillis - lastUpdate) > updateInterval)        //Zeit fuer Update  SEG
    {
      lastUpdate = currentMillis;
      state = 1;
      digitalWrite(DZB, HIGH);
      digitalWrite(SEG, LOW);
      PORTC = lowByte(seg);
      PORTD = highByte(seg);
    }
    
    else if (state == 1 && (currentMillis - lastUpdate) > updateInterval)   //Zeit fuer Update  DZB
    {
      lastUpdate = currentMillis;
      state = 0;
      digitalWrite(SEG, HIGH);
      digitalWrite(DZB, LOW);
      PORTC = lowByte(dzb);
      PORTD = highByte(dzb);
    }
  }

//Schleife (leer)
  void loop()
  {}
