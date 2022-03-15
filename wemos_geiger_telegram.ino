
/////////////////////////////////////////////////
///////////////Bibliotheken//////////////////////
/////////////////////////////////////////////////

//Bibliothek für Geigerzähler
#include <SPI.h>
//Bibliothek für WiFi
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <WiFiUdp.h>
//Bibliothek für Telegram Bot
#include <UniversalTelegramBot.h>
//Bibliothek für Zeitserver
#include <NTPClient.h>


/////////////////////////////////////////////////
////////////////Variablen////////////////////////
/////////////////////////////////////////////////

//Variablen für WiFi//
const char* ssid = " ";       //Wifi SSID
const char* password = " ";   //Wifi Passwort

//Geigerzähler Definitionen//
//Logging Zeitspanne in Millisekunden, empfohlener Wert zwischen 15000-60000.
#define LOG_PERIOD 15000
//Maximale logging Zeitspanne ohne diesen Sketch zu modifizeren.
#define MAX_PERIOD 60000

//Variablen für Geigerzähler Zähler//
unsigned long counts;
float cpm;
float usv;
int usvcount;
float usvmeridian;
unsigned int multiplier;
unsigned long previousMillis;
unsigned int sev;
unsigned long currentMillis;
const byte interruptPin = 14;
volatile byte interruptCounter = 0;
int numberOfInterrupts = 0;

//Variablen für Telegram//
#define BOT_TOKEN " "               //Bot Token hier eingeben. Ohne 'bot' Abschnitt.
#define CHAT_ID " "                 //Chat ID angeben. Es können weitere Chat IDs definiert werden.
bool disable_notification = true;
#define standort "Stadt, Bundesland"                //Standort Angeben falls gewünscht.
unsigned long botMillis;
int initialnachricht = 0;
int botnachricht1 = 0;
int botnachricht2 = 0;
int botnachricht3 = 0;
int botnachricht4 = 0;

//Variablen für Zeitserver//
String formattedDate;
String timeStamp;

/////////////////////////////////////////////////
////////////////Voreinstellungen/////////////////
/////////////////////////////////////////////////


//Wifi Einstellungen//
X509List cert(TELEGRAM_CERTIFICATE_ROOT);
WiFiClientSecure client;

//Zeitserver Einstellungen//
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

//Telegram Bot Einstellungen//
UniversalTelegramBot bot(BOT_TOKEN, client);

//Geigerzähler Einstellungen//
void ICACHE_RAM_ATTR tube_impulse();

void tube_impulse(){       
  counts++;
}

/////////////////////////////////////////////////
///////////////////Setup/////////////////////////
/////////////////////////////////////////////////

void setup(){

  Serial.begin(115200);

  //Zeit synchronisieren//
  configTime(0,0, "pool.ntp.org");


  //Root Zertifikat für Telegram Bot hinzufügen//
  client.setTrustAnchors(&cert);

  
  //Setzen der Geigerzähler Werte//
  counts = 0;
  cpm = 0;
  multiplier = MAX_PERIOD / LOG_PERIOD;      
  attachInterrupt(14, tube_impulse, FALLING);

  // Verbindung zum WiFi aufbauen//
  Serial.print("Verbindung zum Wifi wird aufgebaut: ");
  Serial.println(ssid);
    
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("");
  Serial.println("WiFi verbunden");
  Serial.print("IP Adresse: ");
  Serial.println(WiFi.localIP());

  client.setInsecure();

  //Verbindung zum Zeitserver
  timeClient.begin();
  timeClient.setTimeOffset(3600);
  timeClient.update();

  formattedDate = timeClient.getFormattedTime();
  int splitT = formattedDate.indexOf("T");
  timeStamp = formattedDate.substring(splitT+1, formattedDate.length()-1);
  String stunde = timeStamp;
  Serial.println(timeStamp);
  
  if (stunde < "07:00:00"){
    botnachricht4 = botnachricht4 + 1;
  }
  if (stunde > "07:00:01" && timeStamp < "12:00:00"){
    botnachricht1 = botnachricht1 + 1;
  }
  if (stunde > "12:00:01" && timeStamp < "17:00:00"){
    botnachricht2 = botnachricht2 + 1;
  }
  if (stunde > "17:00:01" && timeStamp < "22:00:00"){
    botnachricht3 = botnachricht3 + 1;
  }
  if (stunde > "22:00:01"){
    botnachricht4 = botnachricht4 + 1;
  }

  
  //Verbindungsbestätigung des Bots//
  bot.sendMessage(CHAT_ID, "Bot ist online.", "", 0, disable_notification);
  delay(500);

}

/////////////////////////////////////////////////
//////////////////////Loop///////////////////////
/////////////////////////////////////////////////

void loop(){

  //Geiger Zähler//
  currentMillis = millis();
  if(currentMillis - previousMillis > LOG_PERIOD){
    previousMillis = currentMillis;
    cpm = counts * multiplier;
    usv = cpm / 151;
    counts = 0;
    usvcount = usvcount + 1;
    usvmeridian = usvmeridian + usv;
  }
  
  //Zeitabgleich
  timeClient.update();
  formattedDate = timeClient.getFormattedTime();
  int splitT = formattedDate.indexOf("T");
  timeStamp = formattedDate.substring(splitT+1, formattedDate.length()-1);
  String stunde = timeStamp;

  //Telegram Bot//
  //Initiale Nachricht//
  if (initialnachricht == 0 && (millis() - botMillis) > 20000){
    bot.sendMessage(CHAT_ID, "Der aktuelle Wert in " +String(standort) + " beträgt " +String(usv) + " µSv/h", "", 0, disable_notification);
    initialnachricht = initialnachricht + 1;
  }

  //Nachricht für extreme Grenzwert Überschreitung//
  if (usv > 110.0 && (millis() - botMillis) > 300000) {
    usvmeridian = usvmeridian / usvcount;
    bot.sendMessage(CHAT_ID, "Ein Wert von " +String(usvmeridian) + " µSv/h wurde in " +String(standort) + " gemessen!  Nächste Messung erfolgt in 5 Minuten.", "");
    botMillis = millis();
    usvcount = 0;
    usvmeridian = 0;
  }

  //Nachricht für Jahresgrenzwert Überschreitung//
  if (usv > 3.0 && (millis() - botMillis) > 1800000) {
    usvmeridian = usvmeridian / usvcount;
    bot.sendMessage(CHAT_ID,"Der jährliche Strahlengrenzwert wurde überschritten. Ein Wert von " +String(usvmeridian) + " µSv/h wurde in " +String(standort) + " gemessen. Nächste Messung erfolgt in 15 Minuten.","");
    botMillis = millis();
    usvcount = 0;
    usvmeridian = 0;
  }

  //Standard Nachrichten zu bestimmten Zeiten
  if ( stunde == "07:00:0" && botnachricht1 == 0 ) {
    botMillis = millis();
    usvmeridian = usvmeridian / usvcount;
    bot.sendMessage(CHAT_ID, "Der aktuelle Wert in " +String(standort)+ " beträgt " +String(usvmeridian) + " µSv/h", "", 0 , disable_notification);
    usvcount = 0;
    usvmeridian = 0;
    botnachricht1 = botnachricht1 + 1;
    botnachricht4 = botnachricht4 -1;
  }

  if ( stunde == "12:00:0" && botnachricht2 == 0 ) {
    botMillis = millis();
    usvmeridian = usvmeridian / usvcount;
    bot.sendMessage(CHAT_ID, "Der aktuelle Wert in " +String(standort)+ " beträgt " +String(usvmeridian) + " µSv/h", "", 0 , disable_notification);
    usvcount = 0;
    usvmeridian = 0;
    botnachricht2 = botnachricht2 + 1;
    botnachricht1 = botnachricht1 - 1;
  }

  if ( stunde == "17:00:0" && botnachricht3 == 0) {
    botMillis = millis();
    usvmeridian = usvmeridian / usvcount;
    bot.sendMessage(CHAT_ID, "Der aktuelle Wert in " +String(standort) + " beträgt " +String(usvmeridian) + " µSv/h", "", 0 , disable_notification);
    usvcount = 0;
    usvmeridian = 0;
    botnachricht3 = botnachricht3 + 1;
    botnachricht2 = botnachricht2 - 1;
  }

  if ( stunde == "22:00:0" && botnachricht4 == 0 ){
    botMillis = millis();
    usvmeridian = usvmeridian / usvcount;
    bot.sendMessage(CHAT_ID, "Der aktuelle Wert in " +String(standort)+ " beträgt " +String(usvmeridian) + " µSv/h", "", 0 , disable_notification);
    usvcount = 0;
    usvmeridian = 0;
    botnachricht4 = botnachricht4 + 1;
    botnachricht3 = botnachricht3 -1;
  }

}
