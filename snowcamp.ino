#include <bluefruit.h>

#define ADV_TIMEOUT   1 // change adv every 5 seconds

typedef volatile uint32_t REG32;
#define pREG32 (REG32 *)
#define MAC_ADDRESS_LOW   (*(pREG32 (0x100000a4)))
#define MAX_DEVICES 64
#define MAX_TOPICS 32

/**
 * Select the Topics you want to share as positive or negative
 * by removing the comment.
 */
 const char topics[][8] = {
  "+JAVA",
  //"-JAVA",
  //"+PYTHON",
  //"-PYTHON",
  //"+JSCRIP",
  //"-JSCRIP",
  //"+CPLUS",
  //"-CPLUS",
  //"+RUBY",
  //"-RUBY",
  //"+SWIFT",
  //"-SWIFT",
  //"+KOTLIN",
  //"-KOTLIN",
  //"+GO",
  //"-GO",
  //"+RUST",
  //"-RUST",
  //"+HTML",
  //"-HTML",
  //"+CSS",
  //"-CSS",
  //"+SQL",
  //"-SQL",
  //"+NODE",
  //"-NODE",
  //"+ANGULR",
  //"-ANGULR",
  //"+REACT",
  //"-REACT",
  //"+VUE",
  //"-VUE",
  //"+DART",
  //"-DART",
  //"+FLUTTR",
  //"-FLUTTR",
  //"+COBOL",
  //"-COBOL",
  //"+C",
  //"-C",
  //"+PHP",
  "-PHP",
  "+PERL",
  //"-PERL",
  //"+CSHARP",
  //"-CSHARP",
  //"+UNITY",
  //"-UNITY",
  "+ARDUIN",
  //"-ARDUIN",
 };
 
#define NUM_TOPICS (sizeof(topics)/8)

_Static_assert(NUM_TOPICS <= MAX_TOPICS,"Too many topics selected");
uint8_t topic_i; // topic index

/**
 * The system will maintain the advertising from the others and matching with signal strength.
 * The objective is to apply the matching calculation when the device are really close.
 * Memory allocation is static, we will store only MAX_DEVICES devices.
 */
 typedef struct {
    boolean initialized; 
    uint32_t lastUpdate;
    uint16_t peerId;
    int8_t lastRssi;
    uint8_t matchCount;
    uint8_t detractorCount;
    char topics[MAX_TOPICS][7];
 } peer_t;

 peer_t peers[MAX_DEVICES];


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

  // Set name, setName just associate the buffer with the right size
  // the buffer update is not appreciated by the lib so we will just
  // update the content later. The size will be fixed to 15 chars
  topic_i = 0;
  startAdv();
  startScan();
}

// ===========================================================
// Advertizing
// ===========================================================

/**
 * Start Advertising
 */
void startAdv(void)
{   
  static char ble_name[15];

  // Setup device name
  sprintf(ble_name,"M&G%04X%-7.7s",(MAC_ADDRESS_LOW) & 0xFFFF, topics[topic_i]);
  Serial.printf("Name : %s (%d)\r\n",ble_name,strlen(ble_name));

  // Clean
  Bluefruit.Advertising.stop();
  Bluefruit.Advertising.clearData();
  Bluefruit.ScanResponse.clearData();
  Bluefruit.setName(ble_name);

  // Advertising packet
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.setType(BLE_GAP_ADV_TYPE_NONCONNECTABLE_SCANNABLE_UNDIRECTED);
  Bluefruit.Advertising.addTxPower();
  //Bluefruit.Advertising.addName();
  Bluefruit.ScanResponse.addName();

  Bluefruit.Advertising.setStopCallback(adv_stop_callback);
  Bluefruit.Advertising.restartOnDisconnect(true);
  Bluefruit.Advertising.setInterval(32, 244);    // in units of 0.625 ms
  Bluefruit.Advertising.setFastTimeout(ADV_TIMEOUT);      // number of seconds in fast mode
  Bluefruit.Advertising.start(ADV_TIMEOUT);               // Stop advertising entirely after ADV_TIMEOUT seconds 
}

/**
 * Callback invoked when advertising is stopped by timeout
 */
void adv_stop_callback(void)
{
  // got to next topic
 // Bluefruit.Advertising.stop();
  Serial.printf("end\r\n");
  topic_i = (topic_i + 1) % NUM_TOPICS;
  startAdv();
}

// ===========================================================
// Scanning
// ===========================================================

/**
 * Start scan
 */
void startScan(void) {
  Bluefruit.Scanner.setRxCallback(scan_callback);
  Bluefruit.Scanner.restartOnDisconnect(true);
  Bluefruit.Scanner.filterRssi(-80);
  Bluefruit.Scanner.setInterval(160, 80);       // in units of 0.625 ms
  Bluefruit.Scanner.useActiveScan(true);        // Request scan response data
  Bluefruit.Scanner.start(0);                  // 0 = Don't stop scanning after n seconds
}

/**
  * Scan callback
  */
void scan_callback(ble_gap_evt_adv_report_t* report)
{
  // Get the name & filter one M&G
  const char name[32] = {0};
  if (Bluefruit.Scanner.parseReportByType(
    report,
    BLE_GAP_AD_TYPE_COMPLETE_LOCAL_NAME,
    (uint8_t *)name, sizeof(name)
  )) {
    if ( strncmp(name,"M&G", 3) == 0 && strlen(name) >= 14) {
      char adr[5]; char top[8];
      bcopy(&name[3],adr,4);adr[4]=0;
      bcopy(&name[7],top,7);top[7]=0; 
      Serial.printf("Rx %s %s with rssi %d\r\n",adr,top,report->rssi);
      // il faut eliminer les autoscan
    } //else if ( name[0] == 'M' ) Serial.printf("from (%s)\r\n",name);
  } 
  Bluefruit.Scanner.resume();
}


void loop() {

  // Manage bootloader switch for over_the_cable fw update (type ! on serial console)
  while ( Serial && Serial.available() ) if (Serial.read()=='!') enterSerialDfu();



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
