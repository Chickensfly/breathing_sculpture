#define SERVO_PIN 13

// Variables for input timing
unsigned long lastInputTime = 0;

// Variables for the "Cycle" mode
bool isCycling = false;
bool cycleForwardPhase = true;
unsigned long lastPhaseTime = 0;
const int phaseDuration = 2500; // 2.5 seconds per phase

// Converts pulse width in ms to 16-bit duty cycle at 50Hz
uint32_t msToDuty(float ms) {
  return (uint32_t)((ms / 20.0f) * 65536);
}

void servoWrite(float ms) {
  ledcWrite(SERVO_PIN, msToDuty(ms));
}

void setup() {
  Serial.begin(115200);

  ledcAttach(SERVO_PIN, 50, 16);

  Serial.println("Servo ready.");
  Serial.println("Commands: f = forward, b = backward, s = stop, c = cycle (2.5s in/out)");
  
  lastInputTime = millis(); 
}

void loop() {
  // 1. Check for and process new Serial inputs
  if (Serial.available()) {
    char cmd = Serial.read();
    
    // Ignore invisible characters
    if (cmd == '\n' || cmd == '\r' || cmd == ' ') {
      return;
    }

    // Calculate time since last input
    // For calculation of time, good indicator for how long to set your cycles
    unsigned long currentTime = millis();
    unsigned long timeElapsed = currentTime - lastInputTime;
    lastInputTime = currentTime;

    if (cmd == 'f') {
      isCycling = false;  // Turn off cycle mode
      servoWrite(1.54);
      Serial.print("Forward | ");
    }
    else if (cmd == 'b') {
      isCycling = false;  // Turn off cycle mode
      servoWrite(1.46);
      Serial.print("Backward | ");
    }
    else if (cmd == 's') {
      isCycling = false;  // Turn off cycle mode
      servoWrite(1.5);
      Serial.print("Stop | ");
    }
    else if (cmd == 'c') {
      isCycling = true;          // Enable cycle mode
      cycleForwardPhase = true;  // Start with forward
      lastPhaseTime = millis();  // Reset the phase timer
      servoWrite(1.54);          // Immediately start moving forward
      Serial.print("Cycle Started | ");
    }
    else {
      Serial.print("Unknown command | ");
    }

    // Print the elapsed time between inputs
    Serial.print("Time since last input: ");
    Serial.print(timeElapsed);
    Serial.println(" ms");
  }

  // 2. Handle the background Cycle logic (if active)
  if (isCycling) {
    // Check if phaseDuration has passed since the last phase started
    if (millis() - lastPhaseTime >= phaseDuration) {
      lastPhaseTime = millis(); // Reset timer for the next phase
      
      // Toggle the direction
      cycleForwardPhase = !cycleForwardPhase; 

      if (cycleForwardPhase) {
        Serial.println("Closing");
        servoWrite(1.62);
      } else {
        Serial.println("Opening");
        servoWrite(1.38);
      }
    }
  }
}