📟 Sistema de Emergência com ESP32 – WhatsApp, Telegram e Alerta Local

<div align="center">
![ESPE32](https://img.shields.io/badge/ESP32-E7352C?style=for-the-badge&logo=espressif&logoColor=white)
https://img.shields.io/badge/Arduino-00979D?style=for-the-badge&logo=Arduino&logoColor=white
https://img.shields.io/badge/WhatsApp-25D366?style=for-the-badge&logo=whatsapp&logoColor=white
https://img.shields.io/badge/Telegram-2CA5E0?style=for-the-badge&logo=telegram&logoColor=white

</div>

Este projeto implementa um sistema de emergência usando um ESP32, onde três botões físicos executam ações diferentes: enviar alerta pelo WhatsApp, enviar alerta pelo Telegram ou ativar um alerta local.
O dispositivo é ideal para monitoramento, segurança residencial ou assistência a pessoas com necessidade de botão de pânico.

✨ Funcionalidades
🔘 Botão 1 – WhatsApp

Envia mensagem usando CallMeBot API

Acende LED específico

Executa um padrão sonoro

Possui tempo de espera (cooldown) para evitar spam

🔘 Botão 2 – Telegram

Envia alerta via Telegram Bot API

LED próprio

Buzzer com som diferente

Cooldown automático

🔘 Botão 3 – Alerta Local

Acende LED dedicado

Ativa o buzzer local 3 vezes

Não envia nada pela internet

🛠️ Componentes Utilizados

ESP32 DevKit

3 Botões

3 LEDs

Resistores 220Ω

Buzzer ativo ou passivo

Jumpers + protoboard

🔌 Mapeamento dos Pinos
Função	Pino
Botão WhatsApp	22
Botão Telegram	4
Botão Local	5
LED WhatsApp	19
LED Telegram	18
LED Local	21
Buzzer	23
📡 Como o Sistema Funciona

Usa debounce por software para evitar falsos acionamentos

Conecta ao Wi-Fi

Envia mensagens via:

HTTPS para Telegram (com certificado SSL carregado no ESP32)

API CallMeBot para WhatsApp

Cada botão tem:

LED próprio

Som exclusivo

Função isolada

Cooldown entre acionamentos evita mensagens repetidas

⚙️ Configuração Necessária

Editar no código:

const char* ssid = "SEU_WIFI";
const char* password = "SENHA";

String phoneNumber = "5511XXXXXXXXX"; // CallMeBot
String callmebotApiKey = "SUA_KEY";

#define BOT_TOKEN "SEU_TOKEN"
#define CHAT_ID "SEU_CHAT_ID"

▶️ Como Fazer o Upload

Instale as placas ESP32 na Arduino IDE

Instale as bibliotecas:

UniversalTelegramBot

ArduinoJson

Selecione a porta COM correta

Clique em Upload

📑 Sobre o Código

O firmware contém:

Funções separadas para cada botão

Envio HTTP/HTTPS com WiFiClientSecure

Função personalizada de URL encoding

Certificado SSL para o Telegram

Sincronização via NTP

Mensagens de debug detalhadas no Serial Monitor

Estrutura limpa e legível

<div align="center">
Desenvolvido para salvar vidas através da tecnologia

https://img.shields.io/badge/GitHub-Reposit%C3%B3rio-181717?style=for-the-badge&logo=github

</div>
