#include <bluefruit.h>

#define ADV_TIMEOUT   60

typedef volatile uint32_t REG32;
#define pREG32 (REG32 *)
#define MAC_ADDRESS_LOW   (*(pREG32 (0x100000a4)))

void setup() {
  // initialize digital pin D3-D8 and the built-in LED as an output.
  pinMode(D3,OUTPUT);
  pinMode(D4,OUTPUT);
  pinMode(D5,OUTPUT);
  pinMode(D6,OUTPUT);
  pinMode(D7,OUTPUT);
  pinMode(D8,OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT); 

  // Init serial communication for debugging
  Serial.begin(115200);
  long start = millis();
  while ( !Serial && (millis() - start < 2000)) delay(10); 

  // start bluetooth
  Bluefruit.begin();
  Bluefruit.setTxPower(4);

  // Set name
  char ble_name[14] = "Snowcamp-XXXX";
  sprintf(&ble_name[9],"%04X",(MAC_ADDRESS_LOW) & 0xFFFF);
  Bluefruit.setName(ble_name);

  startAdv();
}


void startAdv(void)
{   

  // Advertising packet
  Bluefruit.Advertising.clearData();
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.setType(BLE_GAP_ADV_TYPE_NONCONNECTABLE_SCANNABLE_UNDIRECTED);
  Bluefruit.Advertising.addTxPower();
  Bluefruit.Advertising.addName();

  /* Start Advertising
   * - Enable auto advertising if disconnected
   * - Interval:  fast mode = 20 ms, slow mode = 152.5 ms
   * - Timeout for fast mode is 30 seconds
   * - Start(timeout) with timeout = 0 will advertise forever (until connected)
   * 
   * For recommended advertising interval
   * https://developer.apple.com/library/content/qa/qa1931/_index.html
   */
  Bluefruit.Advertising.setStopCallback(adv_stop_callback);
  Bluefruit.Advertising.restartOnDisconnect(true);
  Bluefruit.Advertising.setInterval(32, 244);    // in units of 0.625 ms
  Bluefruit.Advertising.setFastTimeout(30);      // number of seconds in fast mode
  Bluefruit.Advertising.start(ADV_TIMEOUT);      // Stop advertising entirely after ADV_TIMEOUT seconds 
}

/**
 * Callback invoked when advertising is stopped by timeout
 */
void adv_stop_callback(void)
{
  Serial.println("Advertising time passed, advertising will now stop.");
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
