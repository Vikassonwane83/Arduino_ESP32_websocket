#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

const char* ssid = "V1AG";
const char* password =  "00000000";

bool ledState = 0;
const int ledPin = 2;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
AsyncWebSocketClient * client;
AsyncWebSocketClient * globalClient = NULL;

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>ESP Web Server</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="icon" href="data:,">
  <style>
  html {
    font-family: Arial, Helvetica, sans-serif;
    text-align: center;
  }
  h1 {
    font-size: 1.8rem;
    color: white;
  }
  h2{
    font-size: 1.5rem;
    font-weight: bold;
    color: #143642;
  }
  .topnav {
    overflow: hidden;
    background-color: #143642;
  }
  body {
    margin: 0;
  }
  .content {
    padding: 30px;
    max-width: 600px;
    margin: 0 auto;
  }
  .card {
    background-color: #F8F7F9;;
    box-shadow: 2px 2px 12px 1px rgba(140,140,140,.5);
    padding-top:10px;
    padding-bottom:20px;
  }
  .button {
    padding: 15px 50px;
    font-size: 24px;
    text-align: center;
    outline: none;
    color: #fff;
    background-color: #0f8b8d;
    border: none;
    border-radius: 5px;
    -webkit-touch-callout: none;
    -webkit-user-select: none;
    -khtml-user-select: none;
    -moz-user-select: none;
    -ms-user-select: none;
    user-select: none;
    -webkit-tap-highlight-color: rgba(0,0,0,0);
   }
   /*.button:hover {background-color: #0f8b8d}*/
   .button:active {
     background-color: #0f8b8d;
     box-shadow: 2 2px #CDCDCD;
     transform: translateY(2px);
   }
   .state {
     font-size: 1.5rem;
     color:#8c8c8c;
     font-weight: bold;
   }
  </style>
<title>ESP Web Server</title>
<meta name="viewport" content="width=device-width, initial-scale=1">
<link rel="icon" href="data:,">
</head>
<body onload="initWebSocket()">
  <div class="topnav">
    <h1>ESP WebSocket Server</h1>
  </div>
  <div class="content">
    <div class="card">
      <h2>Output - GPIO 2</h2>
      <p class="state">state: <span id="state">%STATE%</span></p>
      <p><button id="button" class="button">Toggle</button></p>
      
      <input type="text" name="Input" id="input_text">
      <button type="submit" value="Submit" id="Submit_mes" onclick="SendData()">Send</button>
     

    </div>
  </div>
<script>
  var gateway = `ws://192.168.43.214/ws`;
  var websocket;
  <!--window.addEventListener('load', onLoad); -->
  function initWebSocket() {
    console.log('Trying to open a WebSocket connection...');
    websocket = new WebSocket(gateway);
    websocket.onopen    = onwebsiteOpen;
    websocket.onclose   = onClose;
    websocket.onmessage = onMessage;
  }
  
  function onwebsiteOpen(event) {
    console.log('Connection opened');
  }
  function onClose(event) {
    console.log('Connection closed');
    setTimeout(initWebSocket, 5000);
    
  }
  function onMessage(event) {
    var server_message = event.data;
    console.log(server_message);
    websocket.send(server_message);
  }
  function onLoad(event) {
    initWebSocket();
    initButton();
  }
  function initButton() {
    document.getElementById('Submit_mes').addEventListener('click', toggle);
  }
  function toggle(){
    websocket.send(onMessage());
  }
  function Submit_btn(){
   var x =  document.getElementById("input_text");
   x[0].submit();
    }
   function SendData(){
    var textTosend = document.getElementById("input_text").value;
    websocket.send(textTosend);
    }
</script>
</body>
</html>
)rawliteral";

void notifyClients() {  
  Serial.println("--> notifyClients method");  
  ws.textAll(Serial_monitor());
//  send_data_client();
  
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  Serial.println("--> handleWebSocketMessage method");
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  String handlewsMessage = (char*)data;
  
  Serial.println(handlewsMessage);
  notifyClients();
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
             void *arg, uint8_t *data, size_t len) {
 Serial.println("--> on Event method");
  switch (type) {
    case WS_EVT_CONNECT:
    Serial.println("6");
    globalClient = client;
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      break;
    case WS_EVT_DISCONNECT:
    globalClient = NULL;
    Serial.println("7");
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      break;
    case WS_EVT_DATA:
      Serial.println("8");
      handleWebSocketMessage(arg, data, len);
      break;
    case WS_EVT_PONG:
      
    case WS_EVT_ERROR:
      break;
  }
}


void initWebSocket() {
  Serial.println("--> initws method");
  Serial.println("8-");
  ws.onEvent(onEvent);
  Serial.println("9");
  server.addHandler(&ws);
}

String Serial_monitor(){
  String str;
  if(Serial.available()) {
  str = Serial.readString();
  Serial.println(str);
  ws.textAll(str);
  }
  return str;
}
//String send_data_client(){
//  String Serial_m = Serial_monitor();
//  ws.textAll(Serial_m);
//  int len = Serial_m.length();
//  char char_array[len + 1];
//  strcpy(char_array, Serial_m.c_str());
//
//  std::vector<uint8_t> myVector(Serial_m.begin(), Serial_m.end());
//  uint8_t *p = &myVector[0];
//  int n = myVector.size();
//  client->text((char*)char_array);
//  client->text((uint8_t*)p, (size_t)n); 
//  }

String processor(const String& var){
  Serial.println("--> processor method");
  Serial.println(var);
  Serial.println("10");
  if(var == "STATE"){
    if (ledState){
      Serial.println("11");
      return "Vicky";
    }
    else{
      Serial.println("12");
      Serial_monitor();
      return "Vikas";
    }
  }
  Serial.println("13");
  return String();
}

void setup(){
  // Serial port for debugging purposes
  Serial.println("--> setup method before begin");
  Serial.begin(115200);

  Serial.println("--> setup method after begin");
  pinMode(ledPin, OUTPUT);
  Serial.println("--> After pinmode method");
  digitalWrite(ledPin, LOW);
  Serial.println("--> After digitalWrite method");
  
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);    
    Serial.println("Connecting to WiFi..");
  }

  // Print ESP Local IP Address
  Serial.println("before print IP ");
  Serial.println(WiFi.localIP());

  Serial.println("before initWebSocket method call");
  initWebSocket();
  Serial.println("after initWebSocket method call");

  // Route for root / web page
  
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    Serial.println("in server.on method");
    request->send_P(200, "text/html", index_html, processor);
  });

  // Start server
//  send_data_client();
  Serial.println("before server.begin");
  server.begin();
}

void loop() {
  ws.cleanupClients();
  Serial_monitor();
  if(globalClient != NULL && globalClient->status() == WS_CONNECTED){
     globalClient->text(Serial_monitor());
   }
  digitalWrite(ledPin, ledState);
}
