void setup() {
  Serial.begin(115200);
}

void loop() {
  while(Serial.available()){
    char value = Serial.read();
    if (value == '1')
    {
      digitalWrite(10, HIGH);
    }
    else if (value == '0')
    {
      digitalWrite(10, LOW);
    }
  }
}
