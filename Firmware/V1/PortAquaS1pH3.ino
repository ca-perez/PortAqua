// Simple I2C test for ebay 128x64 oled.
// Use smaller faster AvrI2c class in place of Wire.
// Edit AVRI2C_FASTMODE in SSD1306Ascii.h to change the default I2C frequency.
//
#include "Adafruit_TCS34725.h"
#include "SSD1306Ascii.h"
#include "SSD1306AsciiAvrI2c.h"
// 0X3C+SA0 - 0x3C or 0x3D
#define I2C_ADDRESS 0x3C
// Define proper RST_PIN if required.
#define RST_PIN -1
#define led 5
#define nextbtn 4
#define backbtn 2
#define sen0 A1
#define sen1 A0
#define sen2 A2
#define sen3 A3
#define error 5
//TIRILLA LIMPIA
//Marcador 1
#define ref1R 102.44
#define ref1G 125.3
#define ref1B 68.6
//Marcador 2
#define ref2R 104.38
#define ref2G 129.25
#define ref2B 61.5
//Marcador 3
#define ref3R 112.58
#define ref3G 118.32
#define ref3B 63.9
//Marcador 4
#define ref4R 109.43
#define ref4G 123.68
#define ref4B 65.2
//Tirilla 10.0
//MarcadorA
# define Ref100Ar  92.42
# define Ref100Ag  97.57
# define Ref100Ab  47.68
//MarcadorB
# define Ref100Br  56.95
# define Ref100Bg  95.23
# define Ref100Bb  88.25
//MarcadorC
# define Ref100Cr 102.00
# define Ref100Cg  85.50
# define Ref100Cb  52.72
//MarcadorD
# define Ref100Dr  91.91
# define Ref100Dg  94.89
# define Ref100Db  50.24
//Tirilla 7.1
//MarcadorA
# define Ref71Ar  89.20
# define Ref71Ag  98.70
# define Ref71Ab  48.52
//MarcadorB
# define Ref71Br  71.30
# define Ref71Bg 106.08
# define Ref71Bb  60.85
//MarcadorC
# define Ref71Cr  96.58
# define Ref71Cg  90.30
# define Ref71Cb  48.27
//MarcadorD
# define Ref71Dr  93.28
# define Ref71Dg  93.92
# define Ref71Db  47.99
//Tirilla 4.0
//MarcadorA
# define Ref40Ar  91.75
# define Ref40Ag  93.02
# define Ref40Ab  55.37
//MarcadorB
# define Ref40Br  98.91
# define Ref40Bg  95.99
# define Ref40Bb  41.38
//MarcadorC
# define Ref40Cr 102.00
# define Ref40Cg  88.14
# define Ref40Cb  44.33
//MarcadorD
# define Ref40Dr  96.54
# define Ref40Dg  92.96
# define Ref40Db  45.81
//calcurves mx+b
//s0
# define s0m -8.27613
# define s0b 5271.87806
//s1
# define s1m -10.63029
# define s1b 6837.3208
//s2
# define s2m -3.58559
# define s2b 3236.30268
//s3
# define s3m -4.73005
# define s3b 4993.03211

# define debug true 

int mode = 0;
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);
SSD1306AsciiAvrI2c oled;
//------------------------------------------------------------------------------
void setup() {
  pinMode(led, OUTPUT);
  pinMode(nextbtn, INPUT);
  pinMode(backbtn, INPUT);
  digitalWrite(led, LOW);

  Serial.begin(9600);

  if (tcs.begin()) {
    Serial.println("Found sensor");
  } else {
    Serial.println("No TCSfound");
    while (1); // halt!
  }

#if RST_PIN >= 0
  oled.begin(&Adafruit128x64, I2C_ADDRESS, RST_PIN);
#else // RST_PIN >= 0
  oled.begin(&Adafruit128x64, I2C_ADDRESS);
#endif // RST_PIN >= 0
  // Call oled.setI2cClock(frequency) to change from the default frequency.
  oled.setFont(Adafruit5x7);
}
//------------------------------------------------------------------------------
void loop() {
gloop:
  bool btnst = false;
  float red, green, blue;
  if (mode == 0) {
    oled.clear();
    oled.set2X();
    oled.println("PortAqua");
    oled.set1X();
    oled.println("Version 1.11 Ser#1NB\n(c)Uniandes 2021\n2/12/21\n");
    printOpt("Iniciar", "Acerca de ...");
    waitfor(1, -1);
  } else if (mode == -1) {
    oled.clear();
    oled.set2X();
    oled.println("PortAqua");
    oled.set1X();
    oled.println("Desarollado por C. A.\nPerez-Lopez, A. Avila\n, J. A. Perez-Taborda\nen Uniandes con apoyo\neconomico de IEEE-EDS\nPressione (+) o (-).");
    waitforRes(0);
    delay(500);
  }else if (mode == 1) {
    oled.clear();
    oled.set2X();
    oled.println("Modo Medida");
    oled.set1X();
    printOpt("Conductividad", "pH");
    waitfor(2, 3);
    delay(500);
  } else if (mode == 2) {
    oled.clear();
    oled.set2X();
    oled.println("Conduct...");
    oled.set1X();
    oled.println("Sumerja la sonda has-\nta que el protector\nplastico contacte con\n la muestra de agua.");
    printOptMV();
    btnst = waitfor(2, 1);
    if (btnst) goto gloop;
    printmidiendo("Conductividad");
    int Sensor0 = analogRead(sen0) - 512;
    int Sensor1 = analogRead(sen1) - 512;
    int Sensor2 = analogRead(sen2) - 512;
    int Sensor3 = analogRead(sen3) - 512;
    oled.clear();
    //oled.println(Sensor0+512);
    //oled.println(Sensor1+512);
    //oled.println(Sensor2+512);
    //oled.println(Sensor3+512);
    //waitfor(2, 2);
    float resvalue = 0;
    if (Sensor0 + 512 > 1010) {
      printconderror("resistiva");
    }
    else if (Sensor3 + 512 < 100) {
      printconderror("conductiva");
    } else {
      if (abs(Sensor0) < abs(Sensor1) || abs(Sensor0) < abs(Sensor2) || abs(Sensor0) < abs(Sensor3)) {
        //10k
        resvalue =  s0m*(Sensor0 + 512.00)+s0b ;
        printres(resvalue/2);
      }
      else if (abs(Sensor1) < abs(Sensor2) || abs(Sensor1) < abs(Sensor3)) {
        //1k
        resvalue =  s1m*(Sensor1 + 512.00)+s1b ;
        printres(resvalue/2);
      }
      else if (abs(Sensor2) < abs(Sensor3)) {
        //330
        resvalue =  s2m*(Sensor2 + 512.00)+s2b ;
        printres(resvalue/2);
      }
      else {
        //68k
        resvalue =  s3m*(Sensor3 + 512.00)+s3b ;
        printres(resvalue/2);
      }
    }
    waitfor(2, 1);
  }
  else if (mode == 3) {
    oled.clear();
    printpH();
    printOpt("Prueba", "Medicion");
    waitfor(4,5);
    delay(500);
    if (mode == 4) {
      oled.clear();
      printpH();
      oled.println("pH Verificacion\nInserte una tirilla\nnueva hasta la 1ra\nmarca");
      printOptMV();
      btnst = waitfor(4,3);
      if (btnst) goto gloop;
      digitalWrite(5, HIGH);
      printmidiendo("1er marcador");
      tcs.getRGB(&red, &green, &blue);
      digitalWrite(5, LOW);
      oled.clear();
      printpH();
      float delta1[3];
      delta1[0] = red - ref1R;
      delta1[1] = green - ref1G;
      delta1[2] = blue - ref1B;
      checkError(delta1, red, green, blue, error);
      correcto();
      oled.println("mueva la tirilla a la\n2da marca");
      printOptMV();
      btnst = waitfor(4,3);
      if (btnst) goto gloop;
      digitalWrite(5, HIGH);
      printmidiendo("2do marcador");
      tcs.getRGB(&red, &green, &blue);
      digitalWrite(5, LOW);
      oled.clear();
      printpH();
      float delta2[3];
      delta2[0] = red - ref2R;
      delta2[1] = green - ref2G;
      delta2[2] = blue - ref2B;
      checkError(delta2, red, green, blue, error);
      correcto();
      oled.println("mueva la tirilla a la\n3ra marca");
      printOptMV();
      btnst = waitfor(4,3);
      if (btnst) goto gloop;
      digitalWrite(5, HIGH);
      printmidiendo("3er marcador");
      tcs.getRGB(&red, &green, &blue);
      digitalWrite(5, LOW);
      oled.clear();      
      printpH();
      float delta3[3];
      delta3[0] = red - ref3R;
      delta3[1] = green - ref3G;
      delta3[2] = blue - ref3B;
      checkError(delta3, red, green, blue, error);
      correcto();
      oled.println("mueva la tirilla a la\n4ta marca");
      printOptMV();
      btnst = waitfor(4,3);
      if (btnst) goto gloop;
      digitalWrite(5, HIGH);
      printmidiendo("4to marcador");
      tcs.getRGB(&red, &green, &blue);
      digitalWrite(5, LOW);
      oled.clear();
      float delta4[3];
      delta4[0] = red - ref4R;
      delta4[1] = green - ref4G;
      delta4[2] = blue - ref4B;
      checkError(delta4, red, green, blue, error);
      printpH();
      correcto();
      oled.println("La tirilla esta en\nbuen estado");
      printOptCV();
      waitfor(0, 3);
    }
    else if (mode == 5) {
      oled.clear();
      oled.println("Tome la tirilla\ny sumerjala en el\nliquido a medir por\n2 segundos");
      printOptCV();
      waitfor(5, 3);
      oled.clear();
      printpH();
      movetirilla("1ra");
      printOptMV();
      waitfor(5, 3);
      digitalWrite(5, HIGH);
      printmidiendo("1er marcador");
      float rgb1[3];
      tcs.getRGB(&rgb1[0], &rgb1[1], &rgb1[2]);
      digitalWrite(5, LOW);
      oled.clear();
      printpH();
      correcto();
      movetirilla("2da");
      printOptMV();
      waitfor(5, 3);
      digitalWrite(5, HIGH);
      printmidiendo("2do marcador");
      float rgb2[3];
      tcs.getRGB(&rgb2[0], &rgb2[1], &rgb2[2]);
      digitalWrite(5, LOW);
      oled.clear();
      printpH();
      correcto();
      movetirilla("3ra");
      printOptMV();
      waitfor(5, 3);
      digitalWrite(5, HIGH);
      printmidiendo("3er marcador");
      float rgb3[3];
      tcs.getRGB(&rgb3[0], &rgb3[1], &rgb3[2]);
      digitalWrite(5, LOW);
      oled.clear();
      printpH();
      correcto();
      movetirilla("4ta");
      printOptMV();
      waitfor(5, 3);
      digitalWrite(5, HIGH);
      printmidiendo("4to marcador");
      float rgb4[3];
      tcs.getRGB(&rgb4[0], &rgb4[1], &rgb4[2]);
      digitalWrite(5, LOW);
      oled.clear();
      correcto();
      oled.println("Medicion finalizada");
      if (debug){
        oled.print(rgb1[0]);
        oled.print(" ");
        oled.print(rgb1[1]);
        oled.print(" ");
        oled.println(rgb1[2]);
        oled.print(rgb2[0]);
        oled.print(" ");
        oled.print(rgb2[1]);
        oled.print(" ");
        oled.println(rgb2[2]);
        oled.print(rgb3[0]);
        oled.print(" ");
        oled.print(rgb3[1]);
        oled.print(" ");
        oled.println(rgb3[2]);
        oled.print(rgb4[0]);
        oled.print(" ");
        oled.print(rgb4[1]);
        oled.print(" ");
        oled.println(rgb4[2]);
      }
      
      printOptCV();
      waitfor(5, 3);
      oled.clear();
      float dst[5];
      float pHvalues[5] = {4.0, 7.1, 10};
      dst[0] = distance(rgb1, Ref40Ar, Ref40Ag, Ref40Ab) + distance(rgb2, Ref40Br, Ref40Bg, Ref40Bb) + distance(rgb3, Ref40Cr, Ref40Cg, Ref40Cb) + distance(rgb4, Ref40Dr, Ref40Dg, Ref40Db);
      dst[1] = distance(rgb1, Ref71Ar, Ref71Ag, Ref71Ab) + distance(rgb2, Ref71Br, Ref71Bg, Ref71Bb) + distance(rgb3, Ref71Cr, Ref71Cg, Ref71Cb) + distance(rgb4, Ref71Dr, Ref71Dg, Ref71Db);
      dst[2] = distance(rgb1, Ref100Ar, Ref100Ag, Ref100Ab) + distance(rgb2, Ref100Br, Ref100Bg, Ref100Bb) + distance(rgb3, Ref100Cr, Ref100Cg, Ref100Cb) + distance(rgb4, Ref100Dr, Ref100Dg, Ref100Db);
      float pH;
      if (dst[0]<dst[1] && dst[0]<dst[2]) {
        pH = (dst[0]/(dst[0]+dst[1])) * (pHvalues[1]-pHvalues[0]) + pHvalues[0];
      } else if (dst[1]>dst[0] && dst[1]<dst[2]) {
        if (dst[0]>dst[2]){
          pH = (dst[1]/(dst[1]+dst[2])) * (pHvalues[2]-pHvalues[1]) + pHvalues[1];
        } else {
          pH = (dst[0]/(dst[0]+dst[1])) * (pHvalues[1]-pHvalues[0]) + pHvalues[0];
        }
      } else if (dst[2]>dst[0] && dst[2]>dst[1]) {
        pH = (dst[1]/(dst[1]+dst[2])) * (pHvalues[2]-pHvalues[1]) + pHvalues[1];
      } else {
        pH = 20;
      }
      if (pH<15){
        oled.set2X();
        oled.print("pH = ");
        oled.println(pH);     
        oled.set1X();   
      } else {
        oled.set2X();
        oled.println("Error"); 
        oled.set1X();
        oled.println("Fuera de rango");
        oled.print("pH");
        if (dst[0]<dst[2]){
          oled.print("<4");
        }
          
        else {
          oled.print(">10");
        }
      }
      waitfor(0,0);
    }
  }
}

void(* resetFunc) (void) = 0;

bool waitfor(int plusmode, int minusmode) {
  bool ret = false;
  while (true) {
    if (digitalRead(nextbtn) == 0) {
      mode = plusmode;
      break;
    }
    if (digitalRead(backbtn) == 0) {
      mode = minusmode;
      ret = true;
      break;
    }
  }
  delay(500);
  return ret;
}

bool waitforRes(int plusmode) {
  bool ret = false;
  while (true) {
    if (digitalRead(nextbtn) == 0) {
      mode = plusmode;
      break;
    }
    if (digitalRead(backbtn) == 0) {
      resetFunc();
    }
  }
  delay(500);
  return ret;
}

void checkError(float FDelta[], float Fred, float Fgreen, float Fblue, float FError) {
  if (abs(FDelta[0]) > FError || abs(FDelta[1]) > FError || abs(FDelta[2]) > FError) {
    oled.set2X();
    oled.println("Error");
    oled.set1X();
    oled.println("Revise o cambie la\ntirilla de pH.");
    oled.print("RGB=");
    oled.print(Fred);
    oled.print(",");
    oled.print(Fgreen);
    oled.print(",");
    oled.println(Fblue);
    oled.print("DEL=");
    oled.print(FDelta[0]);
    oled.print(",");
    oled.print(FDelta[1]);
    oled.print(",");
    oled.print(FDelta[2]);
    delay(500);
  }
}

void printmidiendo(String medicion) {
  oled.clear();
  oled.set2X();
  oled.println("Midiendo");
  oled.set1X();
  oled.print(medicion);
  delay(500);
  oled.print(" .");
  delay(500);
  oled.print(".");
  delay(500);
  oled.print(".");
  delay(500);
}

void printconderror(String tipo) {
  oled.set2X();
  oled.println("Error");
  oled.set1X();
  oled.println("Fuera de rango\nMuestra es muy");
  oled.println(tipo);
}

void printres(float resvalue) {
  oled.set2X();
  oled.println("Ok");
  oled.set1X();
  oled.print("C= ");
  oled.print(resvalue);
  oled.println(" uS/cm");
}
void printOpt(String plus, String minus) {
  oled.print("(+) ");
  oled.println(plus);
  oled.print("(-) ");
  oled.print(minus);
}
void printOptCV() {
  printOpt("Continuar", "Volver");
}
void printOptMV() {
  printOpt("Medir", "Volver");
}
void movetirilla(String marca){
  oled.println("mueva la tirilla a la");
  oled.print(marca);
  oled.println(" marca");
}
void correcto() {
  oled.println("Correcto!");
}
float distance(float meas[], float Refr, float Refg, float Refb){
  float r = sqrt(pow((meas[0]-Refr),2)+pow((meas[1]-Refg),2)+pow((meas[2]- Refb),2));
  return r;  
}

void printpH(){
    oled.set2X();
    oled.println("pH");
    oled.set1X();
}
