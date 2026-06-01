/*
 * ============================================================
 *  N461 – Estação Meteorológica Distribuída
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

// ─── OBJETOS GLOBAIS ──────────────────────────────────────────
WiFiClientSecure espClient;
PubSubClient     mqttClient(espClient);

// ─── CONTROLE DE TEMPO ────────────────────────────────────────
unsigned long ultimaPublicacao = 0;

// ═════════════════════════════════════════════════════════════
// HANDLERS DE EVENTOS MQTT
// ═════════════════════════════════════════════════════════════

/**
 * Callback executado ao receber mensagens MQTT
 * @param topico Nome do tópico que recebeu a mensagem
 * @param payload Conteúdo da mensagem (bytes)
 * @param tamanho Tamanho do payload em bytes
 */
void callbackMQTT(char* topico, byte* payload, unsigned int tamanho) {
  String mensagem = "";
  m═════════════════════════════════════════════════════════════
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
  Serial.print("✓ (mensagem);
}

// ─── CONEXÃO WIFI ─────────────────────────────────────────────
v**
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
      delay(TIMEOUT_RECONEXAO
       * Assina alertas com wildcard '#' para receber QUALQUER
       * subtópico de alertas (alertas/temperatura, alertas/umidade, etc.)
       * QoS 1: comandos de alerta não devem ser perdidos.
       */
      mqttClient.subscribe(TOPICO_ALERTA, 1);
      Serial.println("Inscrito em: " + String(TOPICO_ALERTA));

    } else {
      Serial.print(" falhou! rc=");
      Serial.print(mqttClient.state());
      Serial.println(". Tentando novamente em 5s...");
   ═════════════════════════════════════════════════════════════
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
 */═════════════════════════════════════════════════════════════
// INICIALIZAÇÃO DO SISTEMA
// ═════════════════════════════════════════════════════════════

/**
 * Configura o sistema e estabelece conexões
 * Executado uma vez ao ligar o ESP32
 */
void setup() {
  // Inicializa comunicação serial
  Serial.begin(SERIAL_BAUD_RATE);
  delay(500);
  
  Serial.println("\n╔════════════════════════════════════════╗");
  Serial.println("║  Estação Meteorológica N461           ║");
  Serial.println("║  Sistema Distribuído IoT              ║");
  Serial.println("╚════════════════════════════════════════╝\n");

  // Camada de Rede: Conecta WiFi
  conectarWiFi();

  // Camada de Segurança: Configura TLS
  /═════════════════════════════════════════════════════════════
// LOOP PRINCIPAL - CICLO DE EXECUÇÃO
// ═════════════════════════════════════════════════════════════

/**
 * Verifica threshold de temperatura
 * @param temperatura Valor atual da temperatura
 */
void verificarAlertaTemperatura(float temperatura) {
  if (temperatura > ALERTA_TEMP_ALTA) {
    String payload = "ALTA:" + String(temperatura, 1) + "C";
    mqttClient.publish("alertas/temperatura", payload.c_str(), false);
    
    Serial.print("⚠ [ALERTA] Temperatura crítica: ");
    Serial.print(temperatura, 1);
    Serial.println("°C");
  }
}

/**
 * Verifica threshold de umidade
 * @param umidade Valor atual da umidade
 */
void verificarAlertaUmidade(float umidade) {
  if (umidade < ALERTA_UMID_BAIXA) {
    String payload = "BAIXA:" + String(umidade, 1) + "%";
    mqttClient.publish("alertas/umidade", payload.c_str(), false);
    
    Serial.print("⚠ [ALERTA] Umidade baixa: ");
    Serial.print(umidade, 1);
    Serial.println("%");
  }
}

/**
 * Publica leituras dos sensores via MQTT
 */
void publicarLeituras() {
  // Leitura dos sensores (camada de hardware)
  const float temp = lerTemperatura();
  const float umid = lerUmidade();
  const float lux  = lerLuminosidade();

  char buffer[16];

  /*
   * QoS 0 (At Most Once) para leituras
   * Justificativa: Dados chegam a cada 3s; perda eventual tolerável
   * Retained false: Dados antigos não fazem sentido para séries temporais
   */
  
  // Publica temperatura
  dtostrf(temp, 5, 1, buffer);
  mqttClient.publish(TOPICO_TEMP, buffer, false);

  // Publica umidade
  dtostrf(umid, 5, 1, buffer);
  mqttClient.publish(TOPICO_UMID, buffer, false);

  // Publica luminosidade
  dtostrf(lux, 7, 1, buffer);
  mqttClient.publish(TOPICO_LUX, buffer, false);

  // Log estruturado
  Serial.printf("📊 [PUBLICADO] Temp: %.1f°C | Umid: %.1f%% | Lux: %.1f lux\n",
                temp, umid, lux);

  // Verifica condições de alerta
  verificarAlertaTemperatura(temp);
  verificarAlertaUmidade(umid);
}

/**
 * Loop principal executado continuamente
 * Padrão: Event Loop com polling de tempo
 */
void loop() {
  // Garante conexão MQTT ativa (auto-heal)
  if (!mqttClient.connected()) {
    conectarMQTT();
  }
  
  // Processa mensagens MQTT recebidas
  mqttClient.loop();

  // Controle de tempo não-bloqueante
  const unsigned long agora = millis();
  const unsigned long tempoDecorrido = agora - ultimaPublicacao;
  
  if (tempoDecorrido >= INTERVALO_PUBLICACAO) {
    ultimaPublicacao = agora;
    publicarLeituras();loat lux  = lerLuminosidade();

    char buf[16];

    /*
     * Publicações de leituras em QoS 0 (at most once)
     * Justificativa: leituras chegam a cada 3s; perda eventual de
     * um pacote não é crítica pois o próximo chegará em breve.
     * retained=false (dados antigos não fazem sentido para sensores).
     */
    dtostrf(temp, 5, 1, buf);
    mqttClient.publish(TOPICO_TEMP, buf);

    dtostrf(umid, 5, 1, buf);
    mqttClient.publish(TOPICO_UMID, buf);

    dtostrf(lux, 7, 1, buf);
    mqttClient.publish(TOPICO_LUX, buf);

    Serial.printf("[PUBLICADO] Temp: %.1f°C | Umid: %.1f%% | Lux: %.1f lux\n",
                  temp, umid, lux);

    // Alerta de temperatura alta – QoS 1 (não pode se perder)
    if (temp > 35.0) {
      String alerta = "ALTA:" + String(temp, 1) + "C";
      mqttClient.publish("alertas/temperatura", alerta.c_str());
      Serial.println("[ALERTA] Temperatura acima de 35°C!");
    }

    // Alerta de umidade baixa – QoS 1
    if (umid < 30.0) {
      String alerta = "BAIXA:" + String(umid, 1) + "%";
      mqttClient.publish("alertas/umidade", alerta.c_str());
      Serial.println("[ALERTA] Umidade abaixo de 30%!");
    }
  }
}
