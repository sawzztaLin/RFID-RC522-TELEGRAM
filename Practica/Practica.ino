#include <SPI.h>
#include <MFRC522.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>

// Configuración WiFi
const char* ssid = "SSID";
const char* password = "PASSWORD";

// Configuración RFID
#define RST_PIN   22     // GPIO4
#define SS_PIN    5     // GPIO5
MFRC522 mfrc522(SS_PIN, RST_PIN);

// Configuración del servidor y correo
const char* serverURL = "http://IP_LOCAL/api/registrar_acceso";
const char* emailAPI = "http://IP_LOCAL/api/enviar_correo";

void setup() {
  Serial.begin(115200);
  SPI.begin(); // Inicializar bus SPI
  mfrc522.PCD_Init(); 
  
  Serial.print("Conectando a WiFi...");
  WiFi.begin(ssid, password);

  // Configura un tiempo máximo de espera (ej: 20 segundos)
  unsigned long inicio = millis();
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    
    // Si supera el tiempo límite, sale del bucle
    if (millis() - inicio > 20000) {
      Serial.println("\nError: Tiempo de conexión agotado");
      Serial.println("Posibles causas:");
      Serial.println("- SSID/contraseña incorrectos");
      Serial.println("- Router no disponible");
      Serial.println("- Señal WiFi débil");
      break; // Sale del bucle while
    }
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nConectado!");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
  }
}

void loop() {
  // Verificar si hay una tarjeta presente
  if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
    delay(50);
    return;
  }

  // Leer UID de la tarjeta
  String uid = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    uid += String(mfrc522.uid.uidByte[i] < 0x10 ? "0" : "");
    uid += String(mfrc522.uid.uidByte[i], HEX);
  }
  uid.toUpperCase();
  
  Serial.print("Tarjeta detectada - UID: ");
  Serial.println(uid);

  // Registrar acceso en la base de datos
  registrarAcceso(uid);

  // Enviar correo electrónico
  enviarCorreo(uid);

  delay(1000); // Evitar múltiples lecturas
}

void registrarAcceso(String uid) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    
    http.begin(serverURL);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    
    String postData = "uid=" + uid + "&fecha=" + obtenerFechaHora();
    int httpResponseCode = http.POST(postData);
    
    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println("Respuesta del servidor: " + response);
    } else {
      Serial.print("Error en la petición: ");
      Serial.println(httpResponseCode);
    }
    
    http.end();
  }
}

void enviarCorreo(String uid) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    
    http.begin(emailAPI);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    
    String postData = "uid=" + uid + "&fecha=" + obtenerFechaHora();
    int httpResponseCode = http.POST(postData);
    
    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println("Correo enviado: " + response);
    } else {
      Serial.print("Error al enviar correo: ");
      Serial.println(httpResponseCode);
    }
    
    http.end();
  }
}

void testServerConnection() {
  HTTPClient http;
  http.begin("http://192.168.1.72:3000/test");
  int httpCode = http.GET();
  
  Serial.print("Código de respuesta: ");
  Serial.println(httpCode);
  
  if(httpCode > 0) {
    Serial.println(http.getString());
  }
  http.end();
}

// Llama esta función en setup() después de conectar WiFi
String obtenerFechaHora() {
  // Implementar lógica para obtener fecha y hora
  // Puedes usar NTP o RTC si está disponible
  return "2023-11-15 12:00:00"; // Ejemplo
}
