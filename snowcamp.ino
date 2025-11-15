void setup() {
  // put your setup code here, to run once:
  pinMode(D3,OUTPUT);
  pinMode(D4,OUTPUT);
  pinMode(D5,OUTPUT);
  pinMode(D6,OUTPUT);
  pinMode(D7,OUTPUT);
  pinMode(D8,OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT); 
}


void loop() {
  static int cnt = 0;
  digitalWrite(D3,LOW);
  digitalWrite(D4,LOW);
  digitalWrite(D5,LOW);
  digitalWrite(D6,LOW);
  digitalWrite(D7,LOW);
  digitalWrite(D8,LOW);
  switch(cnt) {
    case 0 : digitalWrite(D3,HIGH);break;
    case 1 : digitalWrite(D4,HIGH);break;
    case 2 : digitalWrite(D5,HIGH);break;
    case 3 : digitalWrite(D6,HIGH);break;
    case 4 : digitalWrite(D7,HIGH);break;
    case 5 : digitalWrite(D8,HIGH);break;
    default: break;
  }
  digitalWrite(LED_BUILTIN,(cnt&1)?HIGH:LOW);
  cnt = ( cnt + 1 ) % 6;
  delay(500);
}
