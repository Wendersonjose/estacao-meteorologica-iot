/*
 * ============================================================
 *  TEMPLATE DE CONFIGURAÇÃO - ESP32
 *  📋 INSTRUÇÕES:
 *  1. Copie este arquivo e renomeie para "config.h"
 *  2. Preencha suas credenciais reais
 *  3. NUNCA faça commit do arquivo "config.h"
 * ============================================================
 */

#ifndef CONFIG_H
#define CONFIG_H

// ─────────────────────────────────────────────────────────────
// CONFIGURAÇÕES DE REDE
// ─────────────────────────────────────────────────────────────

#define WIFI_SSID "Wokwi-GUEST"        // Nome da sua rede WiFi
#define WIFI_PASSWORD ""                // Senha da sua rede WiFi

// ─────────────────────────────────────────────────────────────
// BROKER MQTT
// ─────────────────────────────────────────────────────────────

#define MQTT_HOST "seu-broker.hivemq.cloud"  // Seu broker HiveMQ
#define MQTT_PORT 8883                       // Porta TLS padrão
#define MQTT_USER "seu_usuario"              // Seu usuário
#define MQTT_PASS "sua_senha"                // Sua senha
#define CLIENT_ID "esp32-estacao-01"         // ID único do dispositivo

// ─────────────────────────────────────────────────────────────
// TOPOLOGIA MQTT - TÓPICOS
// ─────────────────────────────────────────────────────────────

#define TOPICO_TEMP   "clima/uberlandia/centro/temperatura"
#define TOPICO_UMID   "clima/uberlandia/centro/umidade"
#define TOPICO_LUX    "clima/uberlandia/centro/luminosidade"
#define TOPICO_STATUS "clima/uberlandia/centro/status"
#define TOPICO_ALERTA "alertas/#"

// ─────────────────────────────────────────────────────────────
// MAPEAMENTO DE HARDWARE
// ─────────────────────────────────────────────────────────────

#define PIN_SENSOR_TEMPERATURA  34
#define PIN_SENSOR_UMIDADE      35
#define PIN_SENSOR_LUMINOSIDADE 32

// ─────────────────────────────────────────────────────────────
// PARÂMETROS DE OPERAÇÃO
// ─────────────────────────────────────────────────────────────

#define INTERVALO_PUBLICACAO 3000
#define TIMEOUT_RECONEXAO 5000
#define SERIAL_BAUD_RATE 115200

// ─────────────────────────────────────────────────────────────
// THRESHOLDS DE ALERTA
// ─────────────────────────────────────────────────────────────

#define ALERTA_TEMP_ALTA 35.0
#define ALERTA_UMID_BAIXA 30.0

// ─────────────────────────────────────────────────────────────
// NÍVEIS DE QoS
// ─────────────────────────────────────────────────────────────

#define QOS_LEITURAS 0
#define QOS_CRITICO 1

#endif // CONFIG_H
