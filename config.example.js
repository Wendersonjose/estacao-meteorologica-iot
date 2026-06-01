/**
 * ============================================================
 *  TEMPLATE DE CONFIGURAÇÃO - DASHBOARD WEB
 *  📋 INSTRUÇÕES:
 *  1. Copie este arquivo e renomeie para "config.js"
 *  2. Preencha suas credenciais reais
 *  3. NUNCA faça commit do arquivo "config.js"
 * ============================================================
 */

const MQTT_CONFIG = {
  host: 'seu-broker.hivemq.cloud',  // Seu broker HiveMQ
  port: 8884,                        // Porta WSS padrão
  protocol: 'wss',                   // WebSocket Secure
  auth: {
    username: 'seu_usuario',         // Seu usuário
    password: 'sua_senha'            // Sua senha
  }
};

const TOPICOS = {
  sensores: {
    temperatura: 'clima/uberlandia/centro/temperatura',
    umidade: 'clima/uberlandia/centro/umidade',
    luminosidade: 'clima/uberlandia/centro/luminosidade',
    status: 'clima/uberlandia/centro/status'
  },
  wildcards: {
    todasTemperaturas: 'clima/uberlandia/+/temperatura',
    todosAlertas: 'alertas/#'
  }
};

const UI_CONFIG = {
  maxPontosGrafico: 30,
  maxLinhasLog: 50,
  maxAlertasVisiveis: 20,
  intervaloReconexao: 3000,
  duracaoAnimacao: 300
};

const ALERTAS = {
  temperatura: { alta: 35.0, critica: 40.0 },
  umidade: { baixa: 30.0, critica: 20.0 },
  luminosidade: { escuro: 100.0, claro: 800.0 }
};

const QOS = {
  LEITURAS: 0,
  CRITICO: 1
};

Object.freeze(MQTT_CONFIG);
Object.freeze(TOPICOS);
Object.freeze(UI_CONFIG);
Object.freeze(ALERTAS);
Object.freeze(QOS);
