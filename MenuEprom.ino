/*
  LiquidCrystal Library - Hello World

 Demonstrates the use a 16x2 LCD display.  The LiquidCrystal
 library works with all LCD displays that are compatible with the
 Hitachi HD44780 driver. There are many of them out there, and you
 can usually tell them by the 16-pin interface.

 This sketch prints "Hello World!" to the LCD
 and shows the time.

  The circuit:
 * LCD RS pin to digital pin 12
 * LCD Enable pin to digital pin 11
 * LCD D4 pin to digital pin 31
 * LCD D5 pin to digital pin 32
 * LCD D6 pin to digital pin 33
 * LCD D7 pin to digital pin 34
 * LCD R/W pin to ground
 * LCD VSS pin to ground
 * LCD VCC pin to 5V
 * 10K resistor:
 * ends to +5V and ground
 * wiper to LCD VO pin (pin 3)
 * 
 * ROJO 35
 * VERDE 36
 * AZUL 37
 


 Library originally added 18 Apr 2008
 by David A. Mellis
 library modified 5 Jul 2009
 by Limor Fried (http://www.ladyada.net)
 example added 9 Jul 2009
 by Tom Igoe
 modified 22 Nov 2010
 by Tom Igoe
 modified 7 Nov 2016
 by Arturo Guadalupi

 This example code is in the public domain.

 http://www.arduino.cc/en/Tutorial/LiquidCrystalHelloWorld



*/

#include <RotaryEncoder.h>
#include <LiquidCrystal.h>
#include <Keypad.h>
#include <EEPROM.h>

//variables defenidas
#define ROJO 35
#define VERDE 36
#define  AZUL 37
#define beta 4090 //the beta of the thermistor
//#define resistance 10 
#define PIN_IN1 38
#define PIN_IN2 39
//#define clk 38
//#define dt  39
#define sw 40
#define led 10
RotaryEncoder encoder(PIN_IN1, PIN_IN2, RotaryEncoder::LatchMode::TWO03);

const int rs = 12, en = 11, d4 = 31, d5 = 32, d6 = 33, d7 = 34;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

const byte ROWS = 4; //four rows
const byte COLS = 3; //three columns
char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};
byte rowPins[ROWS] = {5, 4, 3, 2}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {8, 7, 6}; //connect to the column pinouts of the keypad
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

//vatiables
int soundPin=A2;
String menuOpc[] = {"1.UTempH", "2.UTempL", "3.UmbLuz", "4.UmbSonido", "5.Reset","6.Salir"};

int maxOpc = 6;
int antiguoClk;
int antiguosw;
int count = 0;
int pos = 0;
String menuOpcEsc[] = {"TempH", "TempL", "Luz", "Sonido", "Reset","Salir"};

//funcion de configuracion temperatura
void sensorTemperatura(void);
//int addresTH=0;
char numeroTemp[3]="00";
float temperaturaAlta=25;
char numeroTempL[3]="00";
float temperaturaBaja=18;


//funcion de configuracion luz
void sensorLuz(void);
//int addresL=2;
char numeroLuz[4]="000";
float luzValor=200;

//funcion de configuracion microfono
void sensorMicrofono(void);
///int addresM=3;
char numeroMicro[3]="00";
float sonido=30;
int buttonState = 1;

void setup() {
  Serial.begin(9600);
  lcd.begin(16,2);
  //lcd.backlight();
  pinMode(ROJO, OUTPUT);
  pinMode(AZUL, OUTPUT);
  pinMode(VERDE, OUTPUT);
  digitalWrite(AZUL, HIGH);
  digitalWrite(AZUL, LOW);
  pinMode(led, OUTPUT);
  pinMode(PIN_IN1, INPUT);
  pinMode(PIN_IN2, INPUT);
  pinMode(sw, INPUT);
  antiguoClk = digitalRead(PIN_IN1);
  antiguosw = digitalRead(sw);
  LeerEeprom();
  menu();
  
}

/*
  * Procedimiento que lee en la memoria eprom los umbrales de los sensores<br>
  * <b>post: </b> procedimiento que se encarga de analizar el valor de los 
  * umbrales de y ponerlos en la memoria eeprom
  * 
*/
void LeerEeprom(){
   temperaturaAlta = EEPROM.read(0);
   temperaturaBaja = EEPROM.read(1);
   luzValor = (EEPROM.read(2)*10)+EEPROM.read(3);
   sonido = EEPROM.read(4);
}

/*
  * Procedimiento que resetea los umbrales <br>
  * <b>post: </b> procedimiento que se encarga de poner los umbrales 
  * de los sensores con los valores predeterminados
  * 
*/
void reset(){
       EEPROM.write(0, 25); 
       EEPROM.put(1, 18); 
       EEPROM.put(2, 300/10); 
       EEPROM.write(3, 300%10);
       EEPROM.write(4, 30); 
}


/*
  * Procedimiento que guardar los valores de los umbrales<br>
  * <b>post: </b> procedimiento que se encarga de poner los umbrales 
  * de los sensores con los valores dados por el usuario
  * 
*/
void guardarEeprom(){
       EEPROM.write(0, temperaturaAlta);   
       EEPROM.write(1, temperaturaBaja); 
       EEPROM.write(2, luzValor/10); 
       EEPROM.write(3, (int)luzValor%10);
       EEPROM.write(4, sonido); 
}
void loop() {
  int state_sw = digitalRead(sw);
  encoderMenu();
  char key = keypad.getKey();
  if (key){
    if((key- '0')==count+1 ||(key- '0')==count+2)
      run_option(key);

 /*
  * Procedimiento que ejecuta el menu<br>
  * <b>post: </b> procedimiento que se encarga de ejecutar 
  * la opcion seleccionada por el usuario
  * @parametri char opc
*/

void run_option(char opc){
  switch (opc){
  case '1':
    TemperaturaHigh();
    break;
  case '2':
    TemperaturaLow();
    break;
  case '3':
    LuzVar();
    break;
  case '4':
  SonidoVar();
    break;
  case '5':
    lcd.setCursor(0,0);
    lcd.print(menuOpcEsc[4]);
    lcd.print("               ");
    reset();
    LeerEeprom();
    lcd.setCursor(0,1);
    lcd.print("                  ");
    delay(2000);
    break;
  case '6':
    lcd.setCursor(0,0);
    lcd.print(menuOpcEsc[5]);
    lcd.print("               ");
    //guardarEeprom();
    lcd.setCursor(0,1);
    lcd.print("                  ");
    delay(2000);
    break;
  }
  menu();  
}



 /*
  * Procedimiento que acomoda el umbral alto de temperatura<br>
  * <b>post: </b> procedimiento que se encarga de leer el nuevo valor
  * del umbral alto de temperatua
*/
void TemperaturaHigh(){
  char caracter='x';
  lcd.setCursor(0, 1);
  lcd.println("                        ");
  while(caracter!='*'){
    lcd.setCursor(0,0);
    lcd.print(menuOpcEsc[0]);
    lcd.print(":");
    lcd.print(temperaturaAlta);
    lcd.print("c");
    lcd.println("                        ");
    char key = keypad.getKey();
    if (key){
      if(key=='#'){
        if(temperaturaBaja<numer(numeroTemp,pos))
          temperaturaAlta=numer(numeroTemp,pos);
        lcd.setCursor(0, 1);
      lcd.println("                        ");
      pos=0;
      }else if(key=='*')
        caracter='*';
      else{
        numeroTemp[pos] = key;
        lcd.setCursor(pos++, 1);
        lcd.print(key);
        lcd.print("                  ");
        
      }
    }
    if(pos == 3)
      pos=0;
  }
  pos=0;
}
 /*
  * Procedimiento que acomoda el umbral bajo de temperatura<br>
  * <b>post: </b> procedimiento que se encarga de leer el nuevo valor
  * del umbral bajo de temperatua
*/
void TemperaturaLow(){
  char caracter='x';
  lcd.setCursor(0, 1);
  lcd.println("                        ");
  while(caracter!='*'){    
    lcd.setCursor(0,0);
    lcd.print(menuOpcEsc[1]);
    lcd.print(":");
    lcd.print(temperaturaBaja);
    lcd.print("c");
    lcd.println("                        ");
    encoderValor();
    char key = keypad.getKey();
    lcd.setCursor(0, 1);
    lcd.print(encoderValor());
    
    if (key){
      if(key=='#'){
        if(entero(numeroTempL,pos)<temperaturaAlta && entero(numeroTempL,pos)!=0)
          temperaturaBaja=entero(numeroTempL,pos);
        lcd.setCursor(0, 1);
      lcd.println("                        ");
        pos=0;
      }else if(key=='*'){
        caracter='*';
      }else{
        numeroTemp[pos] = key;
        lcd.setCursor(pos++, 1);
        lcd.print(key);
        lcd.print("                  ");
        
      }
        
       /*else{
        
        numeroTempL[pos] = key;
        
        //lcd.print("                       ");
        */
      //}
    }
    if(pos == 3)
      pos=0;
  }
  pos=0;
}

 /*
  * Procedimiento que acomoda el umbral alto de luz <br>
  * <b>post: </b> procedimiento que se encarga de leer el nuevo valor
  * del umbral de luz
*/
void LuzVar(){
  char caracter='x';
  lcd.setCursor(0, 1);
  lcd.println("                        ");
  while(caracter!='*'){
    lcd.setCursor(0,0);
    lcd.print(menuOpcEsc[2]);
    lcd.print(":");
    lcd.print(luzValor);
    lcd.print("p");
    lcd.println("                        ");
    char key = keypad.getKey();
    if (key){
      if(key=='#'){
        if(entero(numeroLuz,pos)>0)
          luzValor=entero(numeroLuz,pos);
        lcd.setCursor(0, 1);
      lcd.println("                        ");
        pos=0;
      }
      else if(key=='*')
        caracter='*';
      else{
        numeroLuz[pos] = key;
        lcd.setCursor(pos++, 1);
        lcd.print(key);
        lcd.print("                       ");
      }
    }
    if(pos == 4)
      pos=0;
  }
  pos=0;
}

 /*
  * Procedimiento que acomoda el umbral alto de sonido<br>
  * <b>post: </b> procedimiento que se encarga de leer el nuevo valor
  * del umbral alto de sonido
*/
void SonidoVar(){
  char caracter='x';
  lcd.setCursor(0, 1);
  lcd.println("                        ");
  while(caracter!='*'){
    lcd.setCursor(0,0);
    lcd.print(menuOpcEsc[3]);
    lcd.print(":");
    lcd.print(sonido);
    lcd.println("                        ");
    char key = keypad.getKey();
    if (key){
      if(key=='#'){
        if(entero(numeroMicro,pos)>0)
          sonido=entero(numeroMicro,pos);
        lcd.setCursor(0, 1);
      lcd.println("                        ");
        pos=0;
      }else if(key=='*')
        caracter='*';
      else{
        //
        numeroMicro[pos] = key;
        lcd.setCursor(pos++, 1);
        lcd.print(key);
        lcd.print("                       ");
      }
    }
    if(pos == 3)
      pos=0;
    }
  pos=0;
}
 /*
  * Procedimiento que tranformar un caracter en numero<br>
  * <b>post: </b> procedimiento que se encarga convertir un numero que
  * se encuentra en formato de caracter a un formato float
  * @parametro char caracter[],int tam
*/
float entero(char caracter[],int tam){//convierte una cadena de caracteres en numero
    float Entero=0;
    for(int i=0;i<tam;i++){
        Entero+= caracter[i] - '0';
        if(i<tam-1)
            Entero*=10;
    }
    return Entero;
}

/*
  * Procedimiento que se encarga de mover el menu<br>
  * <b>post: </b> procedimiento que se encarga de mover el 
  * menu cada dos opciones y las imprmie en el LCD, validando que no se salga del 
  * rango de opciones maximas
*/
void encoderMenu(){
  int state_clk = digitalRead(PIN_IN1);
  int state_dt = digitalRead(PIN_IN2);
  
  if(antiguoClk == HIGH && state_clk == LOW){
    if(state_dt == LOW){
      count--;
    }else{
      count++;
    }
    
    if(count < 0) count = maxOpc-2;
    else if(count > maxOpc-2) count = 0;
    
    menu();
  }

  delay(5);
  antiguoClk = state_clk;
}



/*
  * Funcion que se encarga de retornar el valor del encoder<br>
  * <b>post: </b> funcion que detecta en que valro se encuentra 
  * el encodes para retornar este valor
*/
char* encoderValor(){
  char* varion = "";
  /*int state_clk = digitalRead(clk);
  int state_dt = digitalRead(dt);
  
  int valor = 0;
  
  if(antiguoClk == HIGH && state_clk == LOW){
    if(state_dt == LOW){
      valor--;
    }else{
      valor++;
    }  
   // if(valor < 0) valor = 0;
    return valor;*/
  
  
  static int pos = temperaturaBaja;
  encoder.tick();

  int newPos = encoder.getPosition();
  if (pos != newPos) {
     
    Serial.println(newPos);
    
    pos = newPos;
  } 
  
  varion = pos;
  
  delay(5);
  return varion;
 // antiguoClk = state_clk;
}

/*
  * Procedimiento que se encarga de mostrar el menu<br>
  * <b>post: </b> procedimiento que se encarga de mostrar el 
  * menu cada dos opciones y las imprmie en el LCD, validando que no se salga del 
  * rango de opciones maximas
*/
void menu(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(menuOpc[count]);
    
  lcd.setCursor(0,1);
  lcd.print(menuOpc[count+1]);
}


/*
  * Procedimiento que se encarga de mostrar la temperatura<br>
  * <b>post: </b> procedimiento que se encarga de leer y mostrar la temperatura
*/
void sensorTemperatura(void){
  lcd.setCursor(0, 0);
  lcd.print("Temperatura                ");
   //read thermistor value
long a =1023 - analogRead(A0);
//the calculating formula of temperature
float tempC = beta /(log((1025.0*10/a-10) / 10)+beta/298.0)-273.0;
Serial.print("Temperatura: ");
Serial.println(tempC);
lcd.setCursor(0, 1);
      lcd.print(tempC);
      lcd.println("C               ");
    if(tempC>temperaturaAlta){
      encenderRojo();
    }else if(tempC>temperaturaBaja){
      encenderAzul();
      
    }else{
      encenderVerde();
      
    }
    
}/*
  * Procedimiento que se encarga de mostrar la Luz<br>
  * <b>post: </b> procedimiento que se encarga de leer y mostrar la Luz
*/
void sensorLuz(void){
  lcd.setCursor(0, 0);
  lcd.print("Luz                             ");
  int luz=analogRead(A1);
Serial.print("Luz: ");
  Serial.println(luz);
  lcd.setCursor(0, 1);
  lcd.print(luz);
  lcd.println("p            ");
    if(luz>luzValor){
      encenderVerde();
    }else{
      encenderAzul();
      
      }
}
/*
  * Procedimiento que se encarga de mostrar el sonido<br>
  * <b>post: </b> procedimiento que se encarga de leer y mostrar la sonido
*/
void sensorMicrofono(void){
  int value = analogRead(soundPin);
  Serial.println(value);
  if(value > sonido){
    LED_RGB(125,33,129);
    delay(500);
  }
  else{
    apagarLeds();
  }
}

/*
  * Procedimiento que se encarga de encender led<br>
  * <b>post: </b> procedimiento que se encarga de identificar
  * el valor de los parametros para saber que led enceder
  * @parametro int r,int g,int b
*/
void LED_RGB(int r,int g,int b){
  analogWrite(ROJO,r);
  analogWrite(VERDE,g);
  analogWrite(AZUL,b);
}


/*
  * Procedimientos que se encarga de encender leds <br>
  * <b>post: </b> procedimiento que se encarga de de apagar los 
  * demas leds y solo encender el que se necesita
*/
void encenderVerde(){
    digitalWrite(VERDE, HIGH);
    digitalWrite(ROJO, LOW);
    digitalWrite(AZUL, LOW);
}

void encenderAzul(){
    digitalWrite(VERDE, LOW);
    digitalWrite(ROJO, LOW);
    digitalWrite(AZUL, HIGH);
}

void encenderRojo(){
    digitalWrite(VERDE, LOW);
    digitalWrite(ROJO, HIGH);
    digitalWrite(AZUL, LOW);
}

void apagarLeds(){

    digitalWrite(VERDE, LOW);
    digitalWrite(ROJO, LOW);
    digitalWrite(AZUL, LOW);
  }
