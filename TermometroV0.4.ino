
/* Termometro de personas V0.1
 * ==========================================================================================
 * A D V E R T E N C I A!!! 
 * Este codigo y su esquema de conexiones fue preparado por Maker504, 
 * SE PRESENTA COMO ESTA! NO SOMOS RESPONSABLES DE LOS RESULTADOS QUE PUEDA GENERAR
 * y debe ser implementado bajo su propio riesgo por personal capacitado sobre esta material
 * =========================================================================================
 */

//Librerias
#include <Wire.h> //comms
#include <Adafruit_MLX90614.h> //Termico
#include <LiquidCrystal_I2C.h> //Pantalla
#include <DS3231.h> //Reloj
#include <SR04.h> //Ultrasonico
#include "pitches.h" //Buzzer

//Definicion de Pines
#define Echo 11 //Echo del Ultrasonico
#define Trig 10 //Trig del Ultrasonico
#define LP 9 //Salida LED

//Inicializar
Adafruit_MLX90614 mlx = Adafruit_MLX90614(); //Sensor Termico
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE); // Pantalla LCD
DS3231 clock; //Reloj
RTCDateTime dt; //Fecha
SR04 sr04=SR04(Echo,Trig); //Ultrasonico

//Variables
int Espera1=500; //Espera en el loop

//Fecha
String fDia;
String fMes; //Mes en texto
String fAno;
String fHora;
String fMin;
String fSeg;
String fTime;

//Distancia
int Dist; //Distancia del ultrasonico
int DistMin=10; //Distancia minima para detectar al sujeto (mm)
int Presente=0; //Si hay alguien frente al Termometro
int Espera=3000; //Tiempo de espera para verificar sujeto
unsigned long Tiempo=0; //Tiempo que lleva detectado para Millis
int Ahora=0; //Millis en el momento que se inicia

//Temperatura
float TempObj; //Temperatura del sujeto
float TempMax=37.00; //Temperatura maxima permitida
int TpoAlarma=200; //Tiempo de Alarma por alta temperatura

void setup() 
{
  pinMode(LP,OUTPUT); //LED
  mlx.begin(); //Termico

  clock.begin(); //Reloj
  //clock.setDateTime(2020,7,9,20,19,0); //Quitar comentaro y ajustar a la hora deseada en primer run.

  //Sonido inicial
  tone(12,NOTE_C5,3000);
  digitalWrite(LP,HIGH);
  delay(3000);
  digitalWrite(LP,LOW);

//Mensaje inicial
  lcd.begin(20,4);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(" TERMOMETRO / RELOJ");
  lcd.setCursor(0,1);
  lcd.print("AMB: ");
  lcd.setCursor(0,2);
  lcd.print("PER: ");
}

void loop() 
{
//Distancia
Dist=sr04.Distance();
if(Dist>DistMin) //No hay nadie
{
  Presente=0;
  Tiempo=millis();
}
if(Dist<=DistMin && Presente==0) //Llego alguien, tomemos el tiempo
{
  Presente=1;
  Tiempo=millis();
}

if(Presente==1)
{
  if(millis()-Tiempo>Espera) //Se completo el tiempo
  {
    Presente=2;
  }
}

//Actualizar pantalla
lcd.setCursor(12,3);
//Temperaturas
lcd.setCursor(4,1);
lcd.print(mlx.readAmbientTempC());
lcd.setCursor(9,1);
lcd.print("c");

TempObj=mlx.readObjectTempC();
switch(Presente)
{
  case 0: //No hay nadie
  lcd.setCursor(4,2);
  lcd.print("--.--c");
  lcd.setCursor(0,3);
  lcd.print("BUSCANDO...  ");
  break;

  case 1: //Llego alguien
  lcd.setCursor(4,2);
  lcd.print("--.--c");
  lcd.setCursor(0,3);
  lcd.print("LEYENDO...");
  tone(12,NOTE_C5,TpoAlarma);
  break;

  case 2: //Se Completo el tiempo
  lcd.setCursor(4,2);
  lcd.print(TempObj);
  lcd.setCursor(9,2);
  lcd.print("c");
  if(TempObj>TempMax)
  {
    tone(12,NOTE_G5,TpoAlarma);
   lcd.setCursor(0,3);
   lcd.print("TEMP ALTA!!!");
   digitalWrite(LP,HIGH);
   delay(TpoAlarma);
   digitalWrite(LP,LOW);
  }
  else
  {
  lcd.setCursor(0,3);
  lcd.print("NORMAL        ");
  }
  break;
}

//Hora y Fecha
dt=clock.getDateTime();
fDia=String(dt.day);
if(dt.day<10)fDia="0"+fDia; //Para mantener en dos digitos

fAno=String(dt.year-2000);

lcd.setCursor(11,1);
lcd.print(fDia+"/"+ Mes()+"/"+fAno); //Uso de la funcion Mes()

fHora=String(dt.hour);
if(dt.hour<10)fHora="0"+fHora; //Para mantener en dos digitos

fMin=String(dt.minute);
if(dt.minute<10)fMin="0"+fMin; //Para mantener en dos digitos

fSeg=String(dt.second);
if(dt.second<10)fSeg="0"+fSeg; //Para mantener en dos digitos

fTime=fHora+":" + fMin + ":" + fSeg;
lcd.setCursor(11,2);
lcd.print(fTime);

delay(Espera1);

}

/////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
/// F U N C I O N E S 
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////


String Mes()
{
  //Serial.println(dt.month);
  switch (dt.month)
  {
    case 1:
    fMes="Ene";
    break;

    case 2:
    fMes="Feb";
    break;

    case 3:
    fMes="Mar";
    break;

    case 4:
    fMes="Abr";
    break;

    case 5:
    fMes="May";
    break;

    case 6:
    fMes="Jun";
    break;

    case 7:
    fMes="Jul";
    break;

    case 8:
    fMes="Ago";
    break;

    case 9:
    fMes="Sep";
    break;

    case 10:
    fMes="Oct";
    break;

    case 11:
    fMes="Nov";
    break;

    case 12:
    fMes="Dic";
    break;
  }

return fMes;
}
