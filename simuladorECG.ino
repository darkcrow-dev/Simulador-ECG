#include <Wire.h>
#include <LiquidCrystal.h>
#define conversor 0x60

LiquidCrystal lcd(12, 11, 10, 9, 8, 7);

byte corazon[8] = {
  0b00000,
  0b01010,
  0b11111,
  0b11111,
  0b11111,
  0b01110,
  0b00100,
  0b00000
};

float tiempoReposo = 180;
float tiempoP = 120;
float tiempoPQ = 60;
float tiempoQ = 20;
float tiempoR = 40;
float tiempoRS = 40;
float tiempoS = 20;
float tiempoST = 120;
float tiempoT = 200;
float tiempoTU = 40;
float tiempoU = 120;
float tiempoUReposo = 40;

float amplitudP = 0.1;
float amplitudQ = 0.1;
float amplitudR = 0.6;
float amplitudRS = 0.25;
float amplitudS = 0.25;
float amplitudT = 0.2;
float amplitudU = 0.05;

float ecuacion = 0;
unsigned int DAC = 0;
float entrada = 0;

unsigned long cambio = 0;
unsigned long tiempoMuestreo = tiempoReposo + tiempoP + tiempoPQ + tiempoQ + tiempoR + tiempoRS + tiempoS + tiempoST + tiempoT + tiempoTU + tiempoU + tiempoUReposo;
unsigned long tiempoActual = 0;
unsigned long tiempoAnterior = 0;

byte buffer[3];

int boton1 = 0;
int boton2 = 0;
int led = 0;
int contadorPrender = 0;
int contadorVelocidad = 0;
float velocidad = 1;
float latidos = 0;

void setup() {
  lcd.begin(16, 2);
  lcd.createChar(0, corazon);
  lcd.setCursor(0, 0);
  lcd.print(" SIMULADOR ECG ");
  lcd.createChar(0, corazon);
  lcd.setCursor(0, 1);
  lcd.print("       ");
  lcd.setCursor(7, 1);
  lcd.write(byte(0));
  lcd.setCursor(8, 1);
  lcd.print("      ");
  
  Serial.begin(9600);
  Wire.begin();
  
  pinMode(2 ,INPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
}

void loop() {
  boton1 = digitalRead(2);
  boton2 = digitalRead(3);
  led = digitalRead(4);

  if( (boton1 == HIGH) && (boton2 == LOW) && (contadorPrender == 0) && (contadorVelocidad == 0) && ( (led == LOW) || (led == HIGH) ) )
  {
    contadorPrender = 1;
  }
  else if( (boton1 == LOW) && (boton2 == LOW) && (contadorPrender == 1) && (contadorVelocidad == 0) )
  {
    if( led == LOW )
    {
      digitalWrite(4, HIGH);
    }
    else
    {
      digitalWrite(4, LOW);
    }  

    contadorPrender = 0;
  }
  else if( (boton1 == LOW) && (boton2 == HIGH) && (contadorPrender == 0) && (contadorVelocidad == 0) && (led == LOW) )
  {
    contadorVelocidad = 1;
  }
  else if( (boton1 == LOW) && (boton2 == LOW) && (contadorPrender == 0) && (contadorVelocidad == 1) && (led == LOW) )
  {
    contadorVelocidad = 0;
    
    if(velocidad == 1)
    {
      velocidad = 2;
    }
    else if(velocidad == 2)
    {
      velocidad = 0.5;
    }
    else
    {
      velocidad = 1;
    }

    tiempoReposo = 180*velocidad;
    tiempoP = 120*velocidad;
    tiempoPQ = 60*velocidad;
    tiempoQ = 20*velocidad;
    tiempoR = 40*velocidad;
    tiempoRS = 40*velocidad;
    tiempoS = 20*velocidad;
    tiempoST = 120*velocidad;
    tiempoT = 200*velocidad;
    tiempoTU = 40*velocidad;
    tiempoU = 120*velocidad;
    tiempoUReposo = 40*velocidad;

    tiempoMuestreo = tiempoReposo + tiempoP + tiempoPQ + tiempoQ + tiempoR + tiempoRS + tiempoS + tiempoST + tiempoT + tiempoTU + tiempoU + tiempoUReposo;
  }

  if(led == HIGH)
  {
    simulador();

    if(velocidad == 0.5)
    {
      lcd.setCursor(0, 0);
      lcd.print("  TAQUICARDIA  ");
      lcd.setCursor(0, 1);
      lcd.print("     120 BPM    ");
    }
    else if(velocidad == 1)
    {
      lcd.setCursor(0, 0);
      lcd.print("  NORMOCARDIA  ");
      lcd.setCursor(0, 1);
      lcd.print("     60 BPM     ");
    }
    else
    {
      lcd.setCursor(0, 0);
      lcd.print("  BRADICARDIA  ");
      lcd.setCursor(0, 1);
      lcd.print("     30 BPM     ");
    }
  }
  else
  {
    tiempoActual = 0;
    tiempoAnterior = 0;
    ecuacion = ( ( amplitudR )*1023.0 )/5.0;
    lcd.setCursor(0, 0);
    lcd.print(" SIMULADOR ECG ");
    lcd.createChar(0, corazon);
    lcd.setCursor(0, 1);
    lcd.print("       ");
    lcd.setCursor(7, 1);
    lcd.write(byte(0));
    lcd.setCursor(8, 1);
    lcd.print("      ");
  }

  //Serial.println(analogRead(A0));
  Serial.println(ecuacion);
}

void simulador()
{
  tiempoActual = millis();
  cambio = tiempoActual - tiempoAnterior;

  if( cambio <= ( tiempoReposo ) )
    {
      ecuacion = 0;
    }
    else if( cambio <= ( tiempoReposo + tiempoP ) )
    {
      ecuacion = amplitudP + ( ( ( cambio - ( tiempoReposo + tiempoP/2 ) )*( cambio - ( tiempoReposo + tiempoP/2 ) ) )/( -4*( ( ( ( tiempoP )*( tiempoP ) )/( 16*amplitudP ) ) ) ) );
    }
    else if( cambio <= ( tiempoReposo + tiempoP + tiempoPQ ) )
    {
      ecuacion = 0;
    }
    else if( cambio <= ( tiempoReposo + tiempoP + tiempoPQ + tiempoQ ) )
    {
      ecuacion = ( ( - amplitudQ )/( - ( - tiempoQ ) ) )*( cambio - (tiempoReposo + tiempoP + tiempoPQ + tiempoQ) ) - amplitudQ;
    }
    else if( cambio <= ( tiempoReposo + tiempoP + tiempoPQ + tiempoQ + tiempoR ) )
    {
      ecuacion = ( ( amplitudR - ( - amplitudQ ) )/( - ( - tiempoR ) ) )*( cambio - (tiempoReposo + tiempoP + tiempoPQ + tiempoQ + tiempoR) ) + amplitudR;
    }
    else if( cambio <= ( tiempoReposo + tiempoP + tiempoPQ + tiempoQ + tiempoR + tiempoRS ) )
    {
      ecuacion = ( ( - amplitudRS - ( amplitudR ) )/( - ( - tiempoRS ) ) )*( cambio - (tiempoReposo + tiempoP + tiempoPQ + tiempoQ + tiempoR + tiempoRS ) ) - amplitudRS;
    }
    else if( cambio <= ( tiempoReposo + tiempoP + tiempoPQ + tiempoQ + tiempoR + tiempoRS + tiempoS ) )
    {
      ecuacion = ( ( - ( - amplitudS ) )/( - ( - tiempoS ) ) )*( cambio - (tiempoReposo + tiempoP + tiempoPQ + tiempoQ + tiempoR + tiempoRS + tiempoS) );
    }
    else if( cambio <= ( tiempoReposo + tiempoP + tiempoPQ + tiempoQ + tiempoR + tiempoRS + tiempoS + tiempoST ) )
    {
      ecuacion = 0;
    }
    else if( cambio <= ( tiempoReposo + tiempoP + tiempoPQ + tiempoQ + tiempoR + tiempoRS + tiempoS + tiempoST + tiempoT ) )
    {
      ecuacion = amplitudT + ( ( ( cambio - ( tiempoReposo + tiempoP + tiempoPQ + tiempoQ + tiempoR + tiempoRS + tiempoS + tiempoST + tiempoT/2 ) )*( cambio - ( tiempoReposo + tiempoP + tiempoPQ + tiempoQ + tiempoR + tiempoRS + tiempoS + tiempoST + tiempoT/2 ) ) )/( -4*( ( ( ( tiempoT )*( tiempoT ) )/( 16*amplitudT ) ) ) ) );
    }
    else if( cambio <= ( tiempoReposo + tiempoP + tiempoPQ + tiempoQ + tiempoR + tiempoRS + tiempoS + tiempoST + tiempoT + tiempoTU ) )
    {
      ecuacion = 0;
    }
    else if( cambio <= ( tiempoReposo + tiempoP + tiempoPQ + tiempoQ + tiempoR + tiempoRS + tiempoS + tiempoST + tiempoT + tiempoTU + tiempoU ) )
    {
      ecuacion = amplitudU + ( ( ( cambio - ( tiempoReposo + tiempoP + tiempoPQ + tiempoQ + tiempoR + tiempoRS + tiempoS + tiempoST + tiempoT + tiempoTU + tiempoU/2 ) )*( cambio - ( tiempoReposo + tiempoP + tiempoPQ + tiempoQ + tiempoR + tiempoRS + tiempoS + tiempoST + tiempoT + tiempoTU + tiempoU/2 ) ) )/( -4*( ( ( ( tiempoU )*( tiempoU ) )/( 16*amplitudU ) ) ) ) );
    }
    else
    {
      ecuacion = 0;
    }
  
  if( cambio >= tiempoMuestreo )
  {
    tiempoAnterior = tiempoActual;
  }

   ecuacion = ( ( ecuacion + amplitudR )*1023.0 )/5.0;
   buffer[0] = 0b01000000;
   DAC = ecuacion*4;

   buffer[1] = DAC >> 4;
   buffer[2] = DAC << 4;
   
   Wire.beginTransmission(conversor);

   Wire.write(buffer[0]);
   Wire.write(buffer[1]);
   Wire.write(buffer[2]);
   
   Wire.endTransmission();
}

