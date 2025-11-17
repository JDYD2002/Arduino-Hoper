#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>

// =======================
// WiFi
// =======================
const char* ssid = "";  // Nome wifi
const char* password = ""; // Senha wifi

// =======================
// Pinos
// =======================
#define BOTAO_WPP 22
#define BOTAO_TELEGRAM 4
#define BOTAO_LOCAL 5
#define LED_WPP 19
#define LED_TELEGRAM 18
#define LED_LOCAL 21
#define BUZZER 23

// =======================
// CallMeBot (WhatsApp)
// =======================
String phoneNumber = "";  // sem '+', telefone
String callmebotApiKey = ""; // APi 

// =======================
// Telegram
// =======================
#define BOT_TOKEN "" 
#define CHAT_ID "" 

WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);

// =======================
// Controle
// =======================
unsigned long lastSentWpp = 0;
unsigned long lastSentTel = 0;
const unsigned long cooldownWpp = 30000;
const unsigned long cooldownTel = 30000;
const unsigned long debounceDelay = 50;

// Estado dos botões
bool ultimoBotaoWpp = HIGH;
bool ultimoBotaoTel = HIGH;
bool ultimoBotaoLocal = HIGH;
bool estadoEstavelWpp = HIGH;
bool estadoEstavelTel = HIGH;
bool estadoEstavelLocal = HIGH;
unsigned long ultimoDebounceWpp = 0;
unsigned long ultimoDebounceTel = 0;
unsigned long ultimoDebounceLocal = 0;

// =======================
// Setup
// =======================
void setup() {
  Serial.begin(9600);
  delay(1000);
  Serial.println("\n=========================");
  Serial.println("🚀 Iniciando sistema de emergência (ESP32)");
  Serial.println("=========================");

  pinMode(BOTAO_WPP, INPUT_PULLUP);
  pinMode(BOTAO_TELEGRAM, INPUT_PULLUP);
  pinMode(BOTAO_LOCAL, INPUT_PULLUP);
  pinMode(LED_WPP, OUTPUT);
  pinMode(LED_TELEGRAM, OUTPUT);
  pinMode(LED_LOCAL, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  digitalWrite(LED_WPP, LOW);
  digitalWrite(LED_TELEGRAM, LOW);
  digitalWrite(LED_LOCAL, LOW);
  digitalWrite(BUZZER, LOW);

  // WiFi
  Serial.print("Conectando ao WiFi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\n✅ WiFi conectado!");
  Serial.print("📶 IP: ");
  Serial.println(WiFi.localIP());

  secured_client.setCACert(TELEGRAM_CERTIFICATE_ROOT);
  configTime(0, 0, "pool.ntp.org");
  time_t now = time(nullptr);
  while (now < 24 * 3600) {
    delay(100);
    now = time(nullptr);
  }

  Serial.println("⏱️ Hora sincronizada para SSL.");
  bot.sendMessage(CHAT_ID, "🤖 Bot de emergência (ESP32) iniciado com sucesso!", "");
  Serial.println("✅ Bot Telegram conectado e pronto.");
  Serial.println("=========================\n");
}

// =======================
// Loop principal
// =======================
void loop() {
  int leituraWpp = digitalRead(BOTAO_WPP);
  int leituraTel = digitalRead(BOTAO_TELEGRAM);
  int leituraLocal = digitalRead(BOTAO_LOCAL);

  verificarBotao(leituraWpp, ultimoBotaoWpp, ultimoDebounceWpp, estadoEstavelWpp, LED_WPP, 1);
  verificarBotao(leituraTel, ultimoBotaoTel, ultimoDebounceTel, estadoEstavelTel, LED_TELEGRAM, 2);
  verificarBotao(leituraLocal, ultimoBotaoLocal, ultimoDebounceLocal, estadoEstavelLocal, LED_LOCAL, 3);

  delay(50);
}

// =======================
// Função: verificar botões
// =======================
void verificarBotao(int leitura, bool &ultimoEstado, unsigned long &ultimoDebounce,
                    bool &estadoEstavel, int led, int tipoBotao) {

  if (leitura != ultimoEstado) {
    ultimoDebounce = millis();
  }

  if ((millis() - ultimoDebounce) > debounceDelay) {
    if (leitura != estadoEstavel) {
      estadoEstavel = leitura;

      if (estadoEstavel == LOW) { // pressionado
        // 🔆 Acende o LED do botão
        digitalWrite(led, HIGH);

        // 🔊 Sons diferentes para cada botão
        if (tipoBotao == 1) { // WhatsApp
          Serial.println("📤 Enviando mensagem via WhatsApp...");
          tone(BUZZER, 800, 250);
          enviarWhatsApp("🚨 Emergência detectada! Preciso de ajuda imediata!");
        } 
        else if (tipoBotao == 2) { // Telegram
          Serial.println("📤 Enviando mensagem via Telegram...");
          tone(BUZZER, 1000, 350);
          enviarTelegram("⚠️ Botão de emergência pressionado!");
        }
        else if (tipoBotao == 3) { // Alerta local
          Serial.println("📢 Alerta local ativado!");
          for (int i = 0; i < 3; i++) {
            tone(BUZZER, 600, 150);
            digitalWrite(led, HIGH);
            delay(150);
            digitalWrite(led, LOW);
            delay(150);
          }
        }

        delay(500);
        digitalWrite(led, LOW);
        noTone(BUZZER);
      }
    }
  }
  ultimoEstado = leitura;
}

// =======================
// Enviar WhatsApp
// =======================
void enviarWhatsApp(const String &msg) {
  unsigned long now = millis();
  if (now - lastSentWpp < cooldownWpp) {
    Serial.println("⚠️ WPP: cooldown ativo, ignorando envio.");
    return;
  }

  String encoded = urlencode(msg);
  String url = "https://api.callmebot.com/whatsapp.php?phone=" + phoneNumber +
               "&text=" + encoded + "&apikey=" + callmebotApiKey;

  HTTPClient http;
  http.begin(url);
  int code = http.GET();

  if (code > 0) {
    Serial.printf("✅ WPP HTTP code: %d\n", code);
    Serial.println(http.getString());
  } else {
    Serial.printf("❌ WPP HTTP erro: %s\n", http.errorToString(code).c_str());
  }

  http.end();

  if (code == 200) {
    lastSentWpp = now;
    Serial.println("📱 WhatsApp enviado com sucesso!\n");
  }
}

// =======================
// Enviar Telegram
// =======================
void enviarTelegram(const String &msg) {
  unsigned long now = millis();
  if (now - lastSentTel < cooldownTel) {
    Serial.println("⚠️ Telegram: cooldown ativo, ignorando envio.");
    return;
  }

  if (bot.sendMessage(CHAT_ID, msg, "")) {
    Serial.println("✅ Telegram enviado com sucesso!\n");
    lastSentTel = now;
  } else {
    Serial.println("❌ Telegram: falha ao enviar mensagem!\n");
  }
}

// =======================
// URL Encode
// =======================
String urlencode(const String &str) {
  String encoded = "";
  char c;
  for (int i = 0; i < str.length(); i++) {
    c = str.charAt(i);
    if (isalnum(c)) encoded += c;
    else {
      encoded += '%';
      char code0 = (c >> 4) & 0xF;
      char code1 = c & 0xF;
      encoded += char(code0 + (code0 > 9 ? 'A' - 10 : '0'));
      encoded += char(code1 + (code1 > 9 ? 'A' - 10 : '0'));
    }
  }
  return encoded;
}
