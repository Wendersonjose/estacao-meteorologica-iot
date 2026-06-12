/*
 * ============================================================
 *   – Estação Meteorológica Distribuída
 *  Dispositivo Embarcado: ESP32 (simulado no Wokwi)
 *  
 *  Arquitetura: Separação de responsabilidades
 *  - config.h: Configurações e credenciais
 *  - main.ino: Lógica de negócio
 *  
 *  Sensores simulados via potenciômetros:
 *    IO34 → Temperatura (0–50 °C)
 *    IO35 → Umidade     (0–100 %)
 *    IO32 → Luminosidade(0–1000 lux)
 * ============================================================
 */

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include "config.h"  // Importa todas as configurações

// ═════════════════════════════════════════════════════════════
// OBJETOS GLOBAIS
// ═════════════════════════════════════════════════════════════

WiFiClientSecure espClient;
PubSubClient mqttClient(espClient);

// Temporizador para publicação periódica
unsigned long ultimaPublicacao = 0;

// ═════════════════════════════════════════════════════════════
// CALLBACK MQTT
// ═════════════════════════════════════════════════════════════

/**
 * Processa mensagens recebidas via MQTT
 * @param topico Nome do tópico que recebeu a mensagem
 * @param payload Conteúdo da mensagem (bytes)
 * @param tamanho Tamanho do payload em bytes
 */
void callbackMQTT(char* topico, byte* payload, unsigned int tamanho) {
  String mensagem = "";
  for (unsigned int i = 0; i < tamanho; i++) {
    mensagem += (char)payload[i];
  }
  
  Serial.print("📩 [");
  Serial.print(topico);
  Serial.print("] ");
  Serial.println(mensagem);
}

// ═════════════════════════════════════════════════════════════
// GERENCIAMENTO DE CONEXÕES
// ═════════════════════════════════════════════════════════════

/**
 * Estabelece conexão WiFi
 * Bloqueia até conectar com sucesso
 */
void conectarWiFi() {
  Serial.print("Conectando ao WiFi [");
  Serial.print(WIFI_SSID);
  Serial.print("]");
  
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println();
  Serial.print("✓ WiFi conectado! IP: ");
  Serial.println(WiFi.localIP());
}

/**
 * Estabelece conexão com o broker MQTT
 * Inclui LWT (Last Will Testament) para detecção de desconexão
 * Reconecta automaticamente em caso de falha
 */
void conectarMQTT() {
  while (!mqttClient.connected()) {
    Serial.print("Conectando ao broker MQTT [");
    Serial.print(MQTT_HOST);
    Serial.print("]...");

    /*
     * LWT (Last Will and Testament) – QoS 1, retained=true
     * Arquitetura: Padrão Observer para detecção de falhas
     * Se o ESP32 perder a conexão abruptamente, o broker publica
     * "offline" automaticamente, notificando todos os observadores.
     * 
     * QoS 1: Garante que a mensagem de status chegue ao menos uma vez
     * Retained: Novos clientes veem o último status imediatamente
     */
    bool conectado = mqttClient.connect(
      CLIENT_ID,
      MQTT_USER,
      MQTT_PASS,
      TOPICO_STATUS,      // Tópico do LWT
      QOS_CRITICO,        // QoS 1
      true,               // Retained
      "offline"           // Mensagem do LWT
    );

    if (conectado) {
      Serial.println(" ✓ conectado!");

      // Anuncia que está online (retained message)
      mqttClient.publish(TOPICO_STATUS, "online", true);

      // Subscreve em alertas usando wildcard '#'
      mqttClient.subscribe(TOPICO_ALERTA, QOS_CRITICO);
      Serial.println("✓ Inscrito em: " + String(TOPICO_ALERTA));

    } else {
      Serial.print(" ✗ falhou! Código: ");
      Serial.print(mqttClient.state());
      Serial.print(". Tentando novamente em ");
      Serial.print(TIMEOUT_RECONEXAO / 1000);
      Serial.println("s...");
      delay(TIMEOUT_RECONEXAO);
    }
  }
}

// ═════════════════════════════════════════════════════════════
// CAMADA DE ACESSO A HARDWARE - SENSORES
// ═════════════════════════════════════════════════════════════

/**
 * Lê o sensor de temperatura
 * @return Temperatura em graus Celsius (0.0 a 50.0)
 */
float lerTemperatura() {
  const int raw = analogRead(PIN_SENSOR_TEMPERATURA);
  return map(raw, 0, 4095, 0, 500) / 10.0; // Conversão ADC → °C
}

/**
 * Lê o sensor de umidade
 * @return Umidade relativa em porcentagem (0.0 a 100.0)
 */
float lerUmidade() {
  const int raw = analogRead(PIN_SENSOR_UMIDADE);
  return map(raw, 0, 4095, 0, 1000) / 10.0; // Conversão ADC → %
}

/**
 * Lê o sensor de luminosidade
 * @return Intensidade luminosa em lux (0 a 1000)
 */
int lerLuminosidade() {
  const int raw = analogRead(PIN_SENSOR_LUMINOSIDADE);
  return map(raw, 0, 4095, 0, 1000); // Conversão ADC → lux
}

// ═════════════════════════════════════════════════════════════
// LÓGICA DE NEGÓCIO - DETECÇÃO DE ALERTAS
// ═════════════════════════════════════════════════════════════

/**
 * Verifica se a temperatura está em nível crítico
 * @param temperatura Valor atual da temperatura
 */
void verificarAlertaTemperatura(const float temperatura) {
  if (temperatura > ALERTA_TEMP_ALTA) {
    const String mensagem = "⚠️ Temperatura crítica: " + String(temperatura, 1) + "°C";
    mqttClient.publish("alertas/temperatura", mensagem.c_str(), QOS_CRITICO);
    Serial.println(mensagem);
  }
}

/**
 * Verifica se a umidade está em nível crítico
 * @param umidade Valor atual da umidade
 */
void verificarAlertaUmidade(const float umidade) {
  if (umidade < ALERTA_UMID_BAIXA) {
    const String mensagem = "⚠️ Umidade crítica: " + String(umidade, 1) + "%";
    mqttClient.publish("alertas/umidade", mensagem.c_str(), QOS_CRITICO);
    Serial.println(mensagem);
  }
}

// ═════════════════════════════════════════════════════════════
// CAMADA DE PUBLICAÇÃO
// ═════════════════════════════════════════════════════════════

/**
 * Lê todos os sensores, verifica alertas e publica no MQTT
 * Utiliza QoS 0 para leituras periódicas (otimizado para alta frequência)
 * Utiliza QoS 1 para alertas críticos (garante entrega)
 */
void publicarLeituras() {
  // Leitura dos sensores
  const float temperatura = lerTemperatura();
  const float umidade = lerUmidade();
  const int luminosidade = lerLuminosidade();

  // Conversão para strings
  char bufferTemp[8];
  char bufferUmid[8];
  char bufferLux[8];
  
  dtostrf(temperatura, 4, 1, bufferTemp);
  dtostrf(umidade, 4, 1, bufferUmid);
  itoa(luminosidade, bufferLux, 10);

  // Publicação via MQTT (QoS 0 para dados periódicos)
  mqttClient.publish(TOPICO_TEMP, bufferTemp, QOS_LEITURAS);
  mqttClient.publish(TOPICO_UMID, bufferUmid, QOS_LEITURAS);
  mqttClient.publish(TOPICO_LUX, bufferLux, QOS_LEITURAS);

  // Log no serial
  Serial.println("──────────────────────────────");
  Serial.print("🌡️  Temperatura: ");
  Serial.print(temperatura, 1);
  Serial.println(" °C");
  
  Serial.print("💧 Umidade: ");
  Serial.print(umidade, 1);
  Serial.println(" %");
  
  Serial.print("☀️  Luminosidade: ");
  Serial.print(luminosidade);
  Serial.println(" lux");

  // Verificação de alertas (QoS 1 se houver alerta)
  verificarAlertaTemperatura(temperatura);
  verificarAlertaUmidade(umidade);
}

// ═════════════════════════════════════════════════════════════
// INICIALIZAÇÃO DO SISTEMA
// ═════════════════════════════════════════════════════════════

/**
 * Configura o sistema e estabelece conexões
 * Executado uma vez ao ligar o ESP32
 */
void setup() {
  // Inicializa comunicação serial
  Serial.begin(SERIAL_BAUD_RATE);
  Serial.println("\n╔════════════════════════════════════════╗");
  Serial.println("║   – Estação Meteorológica IoT    ║");
  Serial.println("║  ESP32 + Sensores + MQTT + HiveMQ     ║");
  Serial.println("╚════════════════════════════════════════╝\n");

  // Configuração dos pinos analógicos
  pinMode(PIN_SENSOR_TEMPERATURA, INPUT);
  pinMode(PIN_SENSOR_UMIDADE, INPUT);
  pinMode(PIN_SENSOR_LUMINOSIDADE, INPUT);

  // Conexão WiFi
  conectarWiFi();

  // Configuração do cliente MQTT
  espClient.setInsecure();  // Desabilita validação SSL (apenas para simulação)
  mqttClient.setServer(MQTT_HOST, MQTT_PORT);
  mqttClient.setCallback(callbackMQTT);

  // Conexão MQTT
  conectarMQTT();

  Serial.println("\n🚀 Sistema inicializado com sucesso!");
  Serial.println("════════════════════════════════════════\n");
}

// ═════════════════════════════════════════════════════════════
// LOOP PRINCIPAL
// ═════════════════════════════════════════════════════════════

/**
 * Loop principal do ESP32
 * Mantém conexões ativas e publica leituras periodicamente
 */
void loop() {
  // Reconecta se necessário
  if (!mqttClient.connected()) {
    conectarMQTT();
  }

  // Processa mensagens MQTT recebidas
  mqttClient.loop();

  // Publica leituras periodicamente
  const unsigned long agora = millis();
  if (agora - ultimaPublicacao >= INTERVALO_PUBLICACAO) {
    ultimaPublicacao = agora;
    publicarLeituras();
  }
}
