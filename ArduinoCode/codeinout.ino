const int trigPinInterieur = A3; // Capteur intérieur
const int echoPinInterieur = A2;
const int trigPinExterieur = A1; // Capteur extérieur
const int echoPinExterieur = A0;

float distanceInterieur, distanceExterieur;
int presence = 0; // Variable pour compter les passages

// Variables pour le timing
unsigned long dernierPassageExterieur = 0;
unsigned long dernierPassageInterieur = 0;
const unsigned long delaiMax = 1000; // 1 seconde en ms

void setup() {
  Serial.begin(9600);
  pinMode(trigPinInterieur, OUTPUT);
  pinMode(echoPinInterieur, INPUT);
  pinMode(trigPinExterieur, OUTPUT);
  pinMode(echoPinExterieur, INPUT);
}

float mesureDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  float duration = pulseIn(echoPin, HIGH);
  return duration / 58.2; // Distance en cm
}

void loop() {
  distanceInterieur = mesureDistance(trigPinInterieur, echoPinInterieur);
  distanceExterieur = mesureDistance(trigPinExterieur, echoPinExterieur);

  // Détection passage extérieur -> intérieur (+1)
  if (distanceExterieur < 60) {
    dernierPassageExterieur = millis(); // Enregistre le moment de détection
  }

  if (distanceInterieur < 60 && 
      dernierPassageExterieur > 0 && 
      (millis() - dernierPassageExterieur) < delaiMax) {
    presence++;
    Serial.print("Passage entrant. Presence: ");
    Serial.println(presence);
    dernierPassageExterieur = 0; // Réinitialise pour éviter les doubles comptages
  }

  // Détection passage intérieur -> extérieur (-1)
  if (distanceInterieur < 60) {
    dernierPassageInterieur = millis();
  }

  if (distanceExterieur < 60 && 
      dernierPassageInterieur > 0 && 
      (millis() - dernierPassageInterieur) < delaiMax) {
    presence--;
    Serial.print("Passage sortant. Presence: ");
    Serial.println(presence);
    dernierPassageInterieur = 0;
  }

  // Affichage des distances pour débogage
  Serial.print("Interieur: ");
  Serial.print(distanceInterieur);
  Serial.print(" cm | Exterieur: ");
  Serial.print(distanceExterieur);
  Serial.print(" cm | Presence: ");
  Serial.println(presence);

  delay(100); // Petite pause pour stabiliser
}