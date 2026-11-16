#pragma once
#include <stdint.h>
#include <math.h>
#include <string>
#define PI 3.14159265
#define HIGH 1
#define LOW 0
#define OUTPUT 1
#define INPUT_PULLUP 2
#define A0 14
#define F(x) x
#define UNKNOWN 0
#define IRDATA_FLAGS_IS_REPEAT 1
#define DISABLE_LED_FEEDBACK false
typedef bool boolean;
unsigned long millis(); long random(long); long random(long,long); void randomSeed(unsigned long);
void digitalWrite(int,int); void delay(unsigned long);
int analogRead(int); void analogWrite(int,int); void pinMode(int,int); int digitalRead(int);
template<class T> T constrain(T a,T b,T c){return a<b?b:(a>c?c:a);}
struct String: std::string { String(){} String(const char*s):std::string(s){} String(const std::string&s):std::string(s){}
 void trim(){} void toLowerCase(){} bool startsWith(const char*){return true;} String substring(int i)const{return substr(i);} int toInt()const{return 0;} int length()const{return size();} };
struct SerialT{ void begin(long){} void setTimeout(long){} int available(){return 0;} String readStringUntil(char){return "";}
 template<class T> void print(T){} template<class T> void println(T){} void println(){} void println(int,int){} };
extern SerialT Serial;
#define HEX 16
struct IRData{int protocol; int flags; uint8_t command;};
struct IRR{ void begin(int,bool){} bool decode(){return false;} void resume(){} IRData decodedIRData; };
extern IRR IrReceiver;
