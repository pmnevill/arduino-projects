int button1 = 2;
int button1State;
int button1LastState = HIGH;
unsigned long button1LastDebounceTime;

int button2 = 3;
int button2State;
int button2LastState = HIGH;
unsigned long button2LastDebounceTime;

int pin0 = 6;
int pin1 = 5;
int pin2 = 4;

int hits = 0;

unsigned long debounceDelay = 50;

unsigned long idleDelay = 0;
unsigned long idleLastTime;

unsigned long zAxisIdleDelay = 50;
unsigned long zAxisIdleLastTime;
int zAxis;

boolean pupilBlink = false;
int pupilBrightness = 255;
boolean pupilFade = false;
unsigned long pupilBlinkLastTime;
int pupilBlinkDelay = 2;

boolean pupilDie = false;
unsigned long pupilDieLastTime;
int pupilDieDelay = 3000;

int soundDelay = 0;
unsigned long soundLastTime;

boolean teleport = false;
unsigned long teleportLastTime;

void setup() {
  Serial.begin(9600);

  pinMode(button1, INPUT_PULLUP);
  pinMode(button2, INPUT_PULLUP);

  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);

  digitalWrite(13, HIGH);
  delay(1000);
  digitalWrite(13, LOW);
}

void loop() {

  if ((millis() - pupilBlinkLastTime) > pupilBlinkDelay) {
    pupilBlinkLastTime = millis();
    if (pupilBlink) {
      if (pupilBrightness >= 255) {
        pupilFade = true;
      } else if (pupilBrightness <= 150) {
        pupilFade = false;
      }
      int fadeJump = random(0, 10);
      if (pupilFade) {
        if (pupilBrightness - fadeJump >= 0) {
          pupilBrightness = pupilBrightness - fadeJump;
        } else {
          pupilBrightness = 0;
        }
      } else {
        if (pupilBrightness + fadeJump <= 255) {
          pupilBrightness = pupilBrightness + fadeJump;
        } else {
          pupilBrightness = 255;
        }
      }
    } else if (pupilDie) {
      if ((pupilBrightness - 2) >= 0) {
        pupilBrightness = pupilBrightness - 2;
      }
    } else if (teleport && (millis() - teleportLastTime) < soundDelay + 1000) {
      pupilBrightness = 0;
    } else {
      if ((millis() - pupilDieLastTime) > pupilDieDelay && pupilBrightness < 255) {
        pupilBrightness++;
      }
    }
  }
  
  analogWrite(10, pupilBrightness);
  analogWrite(11, pupilBrightness/6);

  if (soundDelay == 0) {
    digitalWrite(pin0, LOW);
    digitalWrite(pin1, LOW);
    digitalWrite(pin2, LOW);
    zAxis = analogRead(A0);
    if (zAxis > 250) {
      hit();
    }

    if (!pupilDie && (millis() - idleLastTime) > idleDelay) {
      idleDelay = random(20000, 50000);
      idleLastTime = millis();
      digitalWrite(pin0, HIGH);
      digitalWrite(pin1, HIGH);
      soundLastTime = millis();
      soundDelay = 500;
    }

    boolean button1Pushed = registerButton(button1, button1State, button1LastState, button1LastDebounceTime);
    boolean button2Pushed = registerButton(button2, button2State, button2LastState, button2LastDebounceTime);

    if (button1Pushed) {
      //Stare
      pupilBlink = true;
      digitalWrite(pin0, HIGH);
      soundLastTime = millis();
      soundDelay = 15000;
    }
    else if (button2Pushed) {
      //Teleport
      digitalWrite(pin1, HIGH);
      soundLastTime = millis();
      teleport = true;
      teleportLastTime = millis();
      soundDelay = 500;
    }
  }

  if (soundDelay != 0 && (millis() - soundLastTime) > soundDelay) {
    soundDelay = 0;
    pupilBlink = false;
    pupilDie = false;
  }

}

boolean registerButton(int &buttonPin, int &buttonState, int &lastButtonState, unsigned long &lastDebounceTime) {
  boolean response = false;

  int reading = digitalRead(buttonPin);

  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;
      if (buttonState == LOW) {
        response = true;
      }
    }
  }

  lastButtonState = reading;
  return response;
}

void hit() {
  soundLastTime = millis();
  if (hits == 3) {
    //Die
    pupilDie = true;
    pupilDieLastTime = millis();
    digitalWrite(pin0, HIGH);
    digitalWrite(pin2, HIGH);
    soundDelay = 1000;
    hits = 0;
  }
  else {
    pupilBlink = true;
    digitalWrite(pin2, HIGH);
    soundDelay = 500;
    hits++;
  }
}
