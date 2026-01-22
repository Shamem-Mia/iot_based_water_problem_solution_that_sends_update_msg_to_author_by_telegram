/*
FINAL WATER QUALITY MONITORING SYSTEM
ESP32 + WEB DASHBOARD + TELEGRAM ALERT
NO BLYNK
*/

#include <WiFi.h>
#include <WebServer.h>
#include <HTTPClient.h>

// ================= WIFI =================
const char* wifi_ssid = "Shamem";
const char* wifi_password = "shamem2108018";

// ================= TELEGRAM =================
const char* telegramBotToken = "8032052330:AAHNLgKMRUbbh8idDRv2mUWnnG9o4dkLhYs";
const char* telegramChatID = "5747871936";
const char* telegramAPI = "https://api.telegram.org/bot";

WebServer server(80);

// ================= PIN DEFINITIONS =================
#define TURBIDITY_PIN 34
#define TDS_PIN       33
#define TRIG_PIN      23
#define ECHO_PIN      22

#define RED_LED    25
#define GREEN_LED  26
#define BUZZER_PIN 21

// ================= CONSTANTS =================
#define VREF 3.3
#define ADC_RESOLUTION 4095
#define SAMPLE_COUNT 30
#define TIMEOUT_MICROS 30000
#define MAX_DISTANCE 400

// ================= GLOBAL VARIABLES =================
float turbidityVoltage = 0;
float turbidityNTU = 0;

float tdsVoltage = 0;
float tdsValue = 0;

float waterLevel = 0;
float ultrasonicVoltage = 0;

bool systemAlert = false;

int tdsBuffer[SAMPLE_COUNT];
int tdsIndex = 0;

// ================= HTML PAGE =================
const char* htmlPage = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <title>Water Quality Monitoring System</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
        * {
            box-sizing: border-box;
            margin: 0;
            padding: 0;
        }
        
        body {
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            min-height: 100vh;
            padding: 20px;
        }
        
        .container {
            max-width: 1200px;
            margin: 0 auto;
            background: rgba(255, 255, 255, 0.95);
            border-radius: 20px;
            padding: 30px;
            box-shadow: 0 15px 35px rgba(0, 0, 0, 0.2);
        }
        
        header {
            text-align: center;
            margin-bottom: 30px;
        }
        
        h1 {
            color: #2c3e50;
            font-size: 2.5em;
            margin-bottom: 10px;
        }
        
        .subtitle {
            color: #7f8c8d;
            font-size: 1.1em;
        }
        
        .status-container {
            display: flex;
            justify-content: center;
            margin-bottom: 30px;
        }
        
        .status-box {
            padding: 20px 40px;
            border-radius: 15px;
            font-size: 1.2em;
            font-weight: bold;
            text-align: center;
            display: flex;
            align-items: center;
            gap: 10px;
        }
        
        .status-normal {
            background: linear-gradient(135deg, #43e97b 0%, #38f9d7 100%);
            color: #ffffff;
            box-shadow: 0 5px 15px rgba(67, 233, 123, 0.3);
        }
        
        .status-alert {
            background: linear-gradient(135deg, #ff6b6b 0%, #ee5a52 100%);
            color: #ffffff;
            box-shadow: 0 5px 15px rgba(255, 107, 107, 0.3);
            animation: pulse 2s infinite;
        }
        
        @keyframes pulse {
            0% { transform: scale(1); }
            50% { transform: scale(1.02); }
            100% { transform: scale(1); }
        }
        
        .sensors-grid {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(300px, 1fr));
            gap: 25px;
            margin-bottom: 30px;
        }
        
        .sensor-card {
            background: white;
            border-radius: 15px;
            padding: 25px;
            box-shadow: 0 8px 25px rgba(0, 0, 0, 0.1);
            transition: transform 0.3s ease, box-shadow 0.3s ease;
            border-left: 5px solid #667eea;
        }
        
        .sensor-card:hover {
            transform: translateY(-5px);
            box-shadow: 0 12px 30px rgba(0, 0, 0, 0.15);
        }
        
        .sensor-header {
            display: flex;
            align-items: center;
            gap: 15px;
            margin-bottom: 20px;
        }
        
        .sensor-icon {
            font-size: 2em;
            color: #667eea;
        }
        
        .sensor-title {
            font-size: 1.3em;
            color: #2c3e50;
            font-weight: 600;
        }
        
        .sensor-value {
            font-size: 2.5em;
            font-weight: bold;
            color: #2c3e50;
            margin-bottom: 10px;
            text-align: center;
        }
        
        .sensor-unit {
            color: #7f8c8d;
            font-size: 1.1em;
            text-align: center;
        }
        
        .sensor-details {
            margin-top: 15px;
            padding-top: 15px;
            border-top: 1px solid #eee;
            color: #666;
            font-size: 0.9em;
        }
        
        .footer {
            text-align: center;
            margin-top: 40px;
            padding-top: 20px;
            border-top: 1px solid #eee;
            color: #7f8c8d;
            font-size: 0.9em;
        }
        
        .data-time {
            background: #f8f9fa;
            padding: 10px 20px;
            border-radius: 10px;
            display: inline-block;
            margin-top: 10px;
            color: #667eea;
            font-weight: bold;
        }
        
        @media (max-width: 768px) {
            .container {
                padding: 20px;
            }
            
            .sensors-grid {
                grid-template-columns: 1fr;
            }
            
            .sensor-card {
                padding: 20px;
            }
        }
    </style>
</head>
<body>
    <div class="container">
        <header>
            <h1>🌊 Water Quality Monitoring System</h1>
            <p class="subtitle">Real-time monitoring of water quality parameters</p>
        </header>
        
        <div class="status-container">
            <div id="statusBox" class="status-box status-normal">
                <span id="statusIcon">✅</span>
                <span id="statusText">System Normal</span>
            </div>
        </div>
        
        <div class="sensors-grid">
            <div class="sensor-card">
                <div class="sensor-header">
                    <div class="sensor-icon">💧</div>
                    <div class="sensor-title">Turbidity</div>
                </div>
                <div class="sensor-value" id="ntu">0.00</div>
                <div class="sensor-unit">NTU</div>
                <div class="sensor-details">
                    Voltage: <span id="tv">0.000</span> V
                </div>
            </div>
            
            <div class="sensor-card">
                <div class="sensor-header">
                    <div class="sensor-icon">📊</div>
                    <div class="sensor-title">TDS Value</div>
                </div>
                <div class="sensor-value" id="tds">0</div>
                <div class="sensor-unit">ppm</div>
                <div class="sensor-details">
                    Voltage: <span id="tdsv">0.000</span> V
                </div>
            </div>
            
            <div class="sensor-card">
                <div class="sensor-header">
                    <div class="sensor-icon">📏</div>
                    <div class="sensor-title">Water Level</div>
                </div>
                <div class="sensor-value" id="wl">0.0</div>
                <div class="sensor-unit">cm</div>
                <div class="sensor-details">
                    Ultrasonic Voltage: <span id="uv">0.000</span> V
                </div>
            </div>
        </div>
        
        <div class="footer">
            <p>ESP32 Water Quality Monitoring System | Real-time Data</p>
            <div class="data-time">
                Last Updated: <span id="lastUpdate">--:--:--</span>
            </div>
        </div>
    </div>
    
    <script>
        function updateData() {
            fetch("/data")
                .then(response => response.json())
                .then(data => {
                    // Update sensor values
                    document.getElementById("ntu").textContent = data.turbidity.toFixed(2);
                    document.getElementById("tv").textContent = data.turbidity_voltage.toFixed(3);
                    document.getElementById("tds").textContent = Math.round(data.tds);
                    document.getElementById("tdsv").textContent = data.tds_voltage.toFixed(3);
                    document.getElementById("wl").textContent = data.water_level.toFixed(1);
                    document.getElementById("uv").textContent = data.ultrasonic_voltage.toFixed(3);
                    
                    // Update status
                    const statusBox = document.getElementById("statusBox");
                    const statusIcon = document.getElementById("statusIcon");
                    const statusText = document.getElementById("statusText");
                    
                    if (data.system_alert) {
                        statusBox.className = "status-box status-alert";
                        statusIcon.textContent = "🚨";
                        statusText.textContent = "SYSTEM ALERT - Check Parameters";
                    } else {
                        statusBox.className = "status-box status-normal";
                        statusIcon.textContent = "✅";
                        statusText.textContent = "System Normal - All Parameters OK";
                    }
                    
                    // Update timestamp
                    const now = new Date();
                    const timeString = now.toLocaleTimeString();
                    document.getElementById("lastUpdate").textContent = timeString;
                })
                .catch(error => {
                    console.error('Error fetching data:', error);
                });
        }
        
        // Update data every 2 seconds
        setInterval(updateData, 2000);
        
        // Initial update
        document.addEventListener('DOMContentLoaded', updateData);
    </script>
</body>
</html>
)rawliteral";

// ================= TELEGRAM FUNCTIONS =================
String urlEncode(String s) {
  String r="";
  for (char c : s) {
    if (isalnum(c)) r+=c;
    else if (c==' ') r+='+';
    else { r+='%'; r+=String(c,HEX); }
  }
  return r;
}

void sendTelegram(String msg){
  if(WiFi.status()!=WL_CONNECTED) return;
  HTTPClient http;
  String url=String(telegramAPI)+telegramBotToken+
  "/sendMessage?chat_id="+telegramChatID+"&text="+urlEncode(msg);
  http.begin(url);
  http.GET();
  http.end();
}

// ================= MEDIAN FILTER (FIXED) =================
int median(int a[], int n) {
  int b[n];
  for(int i=0;i<n;i++) b[i]=a[i];

  for(int i=0;i<n-1;i++){
    for(int j=i+1;j<n;j++){
      if(b[i]>b[j]){
        int temp=b[i];
        b[i]=b[j];
        b[j]=temp;
      }
    }
  }
  return b[n/2];
}

// ================= SENSOR FUNCTIONS =================
float measureTurbidity(){
  long sum = 0;
  for(int i = 0; i < 10; i++){
    sum += analogRead(TURBIDITY_PIN);
    delay(2);
  }

  turbidityVoltage = (sum / 10.0) * (VREF / ADC_RESOLUTION);

  if(turbidityVoltage > 2.16){
    turbidityNTU = 10;
  } else {
    turbidityNTU = 10 + (2.16 - turbidityVoltage) / 0.001;
  }

  if(turbidityNTU < 10) turbidityNTU = 10;   // safety lower limit
  return turbidityNTU;
}


float measureTDS(){
  tdsBuffer[tdsIndex++]=analogRead(TDS_PIN);
  if(tdsIndex>=SAMPLE_COUNT) tdsIndex=0;

  int m=median(tdsBuffer,SAMPLE_COUNT);
  tdsVoltage=m*(VREF/ADC_RESOLUTION);
  tdsValue=300*(tdsVoltage/2.5);
  return tdsValue;
}

float measureWaterLevel(){
  digitalWrite(TRIG_PIN,LOW); delayMicroseconds(2);
  digitalWrite(TRIG_PIN,HIGH); delayMicroseconds(10);
  digitalWrite(TRIG_PIN,LOW);

  long d=pulseIn(ECHO_PIN,HIGH,TIMEOUT_MICROS);
  ultrasonicVoltage=(d/(float)TIMEOUT_MICROS)*3.3;
  if(ultrasonicVoltage>3.3) ultrasonicVoltage=3.3;

  if(d==0) return 0;
  float cm=(d/2.0)/29.1;
  if(cm<1||cm>MAX_DISTANCE) return 0;
  return cm;
}

// ================= ALERT LOGIC =================
void checkAlert(){
  bool turbAlert = turbidityVoltage < 2.10;
  bool levelAlert = waterLevel > 10;

  if(turbAlert || levelAlert){
    digitalWrite(RED_LED,HIGH);
    digitalWrite(GREEN_LED,LOW);
    digitalWrite(BUZZER_PIN,HIGH);

    String msg = "🚨 WATER ALERT\n\n";

    // ---------- TURBIDITY ALERT MESSAGES ----------
    if(turbidityVoltage < 2.10 && turbidityVoltage > 2.00){
      msg += "⚠️ Water is slightly dirty.\n"
             "Check the water and take steps.\n\n";
    }
    else if(turbidityVoltage <= 2.00 && turbidityVoltage > 1.50){
      msg += "⚠️ Water is polluted.\n"
             "Students are suffering from bathing and freshness issues.\n"
             "Take steps to clean it.\n"
             "You can ON the motor to fill the tank.\n\n";
    }
    else if(turbidityVoltage <= 1.50){
      msg += "🚨 Water is too dirty.\n"
             "Take steps to clean it.\n"
             "Otherwise students will get sick due to water pollution.\n"
             "EMERGENCY CASE!\n"
             "ON the motor RIGHT NOW.\n\n";
    }

    // ---------- WATER LEVEL ALERT MESSAGES ----------
    if(waterLevel > 10 && waterLevel <= 13){
      msg += "⚠️ Water level is slightly low.\n"
             "You should ON the motor of Block A.\n\n";
    }
    else if(waterLevel > 13 && waterLevel <= 16){
      msg += "🚨 Water level is very low.\n"
             "ON the motor of Block A RIGHT NOW.\n\n";
    }
    else if(waterLevel > 16){
      msg += "🚨 EMERGENCY CASE!\n"
             "Water level is too low.\n"
             "Water may be polluted and students are suffering from lack of water.\n"
             "ON the motor IMMEDIATELY.\n\n";
    }

    sendTelegram(msg);
    systemAlert = true;
  }
  else{
    digitalWrite(RED_LED,LOW);
    digitalWrite(GREEN_LED,HIGH);
    digitalWrite(BUZZER_PIN,LOW);
    systemAlert = false;
  }
}


// ================= SERIAL MONITOR OUTPUT =================
void printToSerialMonitor() {
  Serial.println("==========================================");
  Serial.println("        WATER QUALITY MONITORING         ");
  Serial.println("==========================================");
  Serial.println("TURBIDITY SENSOR:");
  Serial.printf("  Voltage: %.3f V\n", turbidityVoltage);
  Serial.printf("  NTU: %.2f\n", turbidityNTU);
  Serial.println();
  
  Serial.println("TDS SENSOR:");
  Serial.printf("  Voltage: %.3f V\n", tdsVoltage);
  Serial.printf("  TDS: %.0f ppm\n", tdsValue);
  Serial.println();
  
  Serial.println("ULTRASONIC SENSOR:");
  Serial.printf("  Distance: %.1f cm\n", waterLevel);
  Serial.printf("  Voltage: %.3f V\n", ultrasonicVoltage);
  Serial.println();
  
  Serial.println("SYSTEM STATUS:");
  Serial.printf("  Alert Status: %s\n", systemAlert ? "ALERT 🚨" : "NORMAL ✅");
  Serial.printf("  WiFi Status: %s\n", WiFi.status() == WL_CONNECTED ? "Connected" : "Disconnected");
  Serial.printf("  IP Address: %s\n", WiFi.localIP().toString().c_str());
  Serial.println("==========================================");
  Serial.println();
}

// ================= SETUP =================
void setup(){
  Serial.begin(115200);
  Serial.println();
  Serial.println("Initializing Water Quality Monitoring System...");
  Serial.println("==========================================");

  pinMode(TRIG_PIN,OUTPUT);
  pinMode(ECHO_PIN,INPUT);
  pinMode(RED_LED,OUTPUT);
  pinMode(GREEN_LED,OUTPUT);
  pinMode(BUZZER_PIN,OUTPUT);

  analogReadResolution(12);
  analogSetPinAttenuation(TURBIDITY_PIN,ADC_11db);
  analogSetPinAttenuation(TDS_PIN,ADC_11db);

  Serial.print("Connecting to WiFi: ");
  Serial.println(wifi_ssid);
  WiFi.begin(wifi_ssid,wifi_password);
  
  int attempts = 0;
  while(WiFi.status()!=WL_CONNECTED && attempts < 20){
    delay(500);
    Serial.print(".");
    attempts++;
  }
  Serial.println();
  
  if(WiFi.status()==WL_CONNECTED){
    Serial.println("WiFi Connected!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("Failed to connect to WiFi!");
  }

  server.on("/",[](){
    server.send(200,"text/html",htmlPage);
  });

  server.on("/data",[](){
    // Measure all sensors
    turbidityNTU = measureTurbidity();
    tdsValue = measureTDS();
    waterLevel = measureWaterLevel();
    checkAlert();
    
    // Print to serial monitor
    static unsigned long lastPrint = 0;
    if(millis() - lastPrint > 2000) { // Print every 2 seconds
      printToSerialMonitor();
      lastPrint = millis();
    }

    // Prepare JSON response for web
    String json="{";
    json+="\"turbidity\":"+String(turbidityNTU,2)+",";
    json+="\"turbidity_voltage\":"+String(turbidityVoltage,3)+",";
    json+="\"water_level\":"+String(waterLevel,1)+",";
    json+="\"ultrasonic_voltage\":"+String(ultrasonicVoltage,3)+",";
    json+="\"tds\":"+String(tdsValue,0)+",";
    json+="\"tds_voltage\":"+String(tdsVoltage,3)+",";
    json+="\"system_alert\":"+(systemAlert?String("true"):String("false"));
    json+="}";
    server.send(200,"application/json",json);
  });

  server.begin();
  Serial.println("HTTP server started");
  
  sendTelegram("✅ Water Quality Monitor Started");
  Serial.println("Initialization complete!");
  Serial.println("==========================================");
}

// ================= LOOP =================
void loop(){
  server.handleClient();
  delay(10);
}