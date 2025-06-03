#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Configuration des capteurs
const int trigPinInterieur = A3;
const int echoPinInterieur = A2;
const int trigPinExterieur = A1;
const int echoPinExterieur = A0;

// Variables globales
float distanceInterieur, distanceExterieur;
int presence = 0;
unsigned long dernierPassageExterieur = 0;
unsigned long dernierPassageInterieur = 0;
const unsigned long delaiMax = 1200; // 1,2 s

void setup() {
  // Initialisation série
  Serial.begin(9600);
  
  // Initialisation capteurs
  pinMode(trigPinInterieur, OUTPUT);
  pinMode(echoPinInterieur, INPUT);
  pinMode(trigPinExterieur, OUTPUT);
  pinMode(echoPinExterieur, INPUT);
  
  // Initialisation OLED
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("Erreur OLED"));
    for(;;);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("Systeme pret");
  display.display();
  delay(1000);
}

float mesureDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  float duration = pulseIn(echoPin, HIGH);
  return duration / 58.2;
}

void updateDisplay() {
  display.clearDisplay();
  
  // Affichage des distances
  display.setCursor(0,0);
  display.print("Int: ");
  display.print(distanceInterieur, 0);
  display.print("cm");
  
  display.setCursor(64,0);
  display.print("Ext: ");
  display.print(distanceExterieur, 0);
  display.print("cm");

  // Affichage de la présence
  display.setCursor(0,20);
  display.setTextSize(2);
  display.print("Presence: ");
  display.println(presence);
  display.setTextSize(1);

  // Ajout d'une ligne de séparation
  display.drawLine(0, 18, 128, 18, WHITE);
  
  display.display();
}

void loop() {
  // Mesure des distances
  distanceInterieur = mesureDistance(trigPinInterieur, echoPinInterieur);
  distanceExterieur = mesureDistance(trigPinExterieur, echoPinExterieur);

  // Logique de détection de présence
  if (distanceExterieur < 40) {
    dernierPassageExterieur = millis();
  }

  if (distanceInterieur < 40 && dernierPassageExterieur > 0 && 
      (millis() - dernierPassageExterieur) < delaiMax) {
    presence++;
    dernierPassageExterieur = 0;
  }

  if (distanceInterieur < 40) {
    dernierPassageInterieur = millis();
  }

  if (distanceExterieur < 40 && dernierPassageInterieur > 0 && 
      (millis() - dernierPassageInterieur) < delaiMax) {
    presence--;
    dernierPassageInterieur = 0;
  }

  // Mise à jour de l'affichage
  updateDisplay();

  // Interface série
  if (Serial.available() > 0) {
    if (Serial.read() == 'p') {
      Serial.print("Presence=");
      Serial.println(presence);
    }
    while (Serial.available() > 0) Serial.read();
  }

  delay(200);
}
