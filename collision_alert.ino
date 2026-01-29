// ---------- PIN DEFINITIONS ----------
#define TRIG1 5
#define ECHO1 18

#define TRIG2 17
#define ECHO2 19

#define LED_RED 25
#define BUZZER  23

// ---------- FILTER & LOGIC SETTINGS ----------
#define SAMPLES 5
#define MIN_CHANGE 2.0
#define MAX_VALID_SPEED 200

#define FAST_SPEED 15.0         // cm/s
#define CRITICAL_DISTANCE 120.0 // cm

// ---------- VARIABLES ----------
float dist1 = 0, dist2 = 0;
float prevDist1 = 0, prevDist2 = 0;
unsigned long prevTime = 0;

// ---------- ALERT SETTINGS ----------
unsigned long lastAlertTime = 0;
const unsigned long ALERT_COOLDOWN = 30000; // 30 seconds

// ---------- FUNCTION: RAW DISTANCE ----------
float rawDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH, 30000);
  if (duration == 0) return -1;

  return duration * 0.034 / 2;
}

// ---------- FUNCTION: STABLE DISTANCE ----------
float getStableDistance(int trigPin, int echoPin) {
  float sum = 0;
  int valid = 0;

  for (int i = 0; i < SAMPLES; i++) {
    float d = rawDistance(trigPin, echoPin);
    if (d > 2 && d < 400) {
      sum += d;
      valid++;
    }
    delay(10);
  }

  if (valid == 0) return -1;
  return sum / valid;
}

// ---------- FUNCTION: SERIAL ALERT ----------
void sendSerialAlert() {
  Serial.println("ALERT: COLLISION_RISK");
}

void setup() {
  Serial.begin(115200);

  pinMode(TRIG1, OUTPUT);
  pinMode(ECHO1, INPUT);

  pinMode(TRIG2, OUTPUT);
  pinMode(ECHO2, INPUT);

  pinMode(LED_RED, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  digitalWrite(LED_RED, LOW);
  digitalWrite(BUZZER, LOW);

  Serial.println("FAST APPROACH DETECTION STARTED");
}

void loop() {
  unsigned long currentTime = millis();
  float deltaTime = (currentTime - prevTime) / 1000.0;

  // Sequential read (avoid cross-talk)
  dist1 = getStableDistance(TRIG1, ECHO1);
  delay(80);
  dist2 = getStableDistance(TRIG2, ECHO2);
  delay(80);

  bool danger = false;

  if (prevTime > 0 && deltaTime > 0) {

    float speed1 = (prevDist1 - dist1) / deltaTime;
    float speed2 = (prevDist2 - dist2) / deltaTime;

    // Noise filtering
    if (abs(dist1 - prevDist1) < MIN_CHANGE) speed1 = 0;
    if (abs(dist2 - prevDist2) < MIN_CHANGE) speed2 = 0;

    if (abs(speed1) > MAX_VALID_SPEED) speed1 = 0;
    if (abs(speed2) > MAX_VALID_SPEED) speed2 = 0;

    Serial.println("--------------------------------");
    Serial.print("S1 Dist: ");
    Serial.print(dist1);
    Serial.print(" cm | Speed: ");
    Serial.println(speed1);

    Serial.print("S2 Dist: ");
    Serial.print(dist2);
    Serial.print(" cm | Speed: ");
    Serial.println(speed2);

    // FAST APPROACH LOGIC
    if (speed1 > FAST_SPEED && dist1 > 0 && dist1 < CRITICAL_DISTANCE) {
      Serial.println("⚠️ SENSOR 1 FAST APPROACH");
      danger = true;
    }

    if (speed2 > FAST_SPEED && dist2 > 0 && dist2 < CRITICAL_DISTANCE) {
      Serial.println("⚠️ SENSOR 2 FAST APPROACH");
      danger = true;
    }
  }

  // OUTPUT CONTROL + TELEGRAM ALERT SIGNAL
  if (danger) {
    digitalWrite(LED_RED, HIGH);
    digitalWrite(BUZZER, HIGH);

    if (millis() - lastAlertTime > ALERT_COOLDOWN) {
      sendSerialAlert();
      lastAlertTime = millis();
    }

  } else {
    digitalWrite(LED_RED, LOW);
    digitalWrite(BUZZER, LOW);
  }

  prevDist1 = dist1;
  prevDist2 = dist2;
  prevTime = currentTime;

  delay(250);
}
