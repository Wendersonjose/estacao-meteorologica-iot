/**
 * ============================================================
 *  CONFIGURAÇÃO - DASHBOARD WEB
 *  ⚠️ ATENÇÃO: Este arquivo contém credenciais sensíveis!
 *  ⚠️ NUNCA faça commit deste arquivo no Git!
 *  ⚠️ Ele está protegido pelo .gitignore
 * ============================================================
 */

/**
 * Configurações do Broker MQTT
 * @constant {Object} MQTT_CONFIG
 */
const MQTT_CONFIG = {
  /**
   * Endereço do broker HiveMQ Cloud
   * @type {string}
   */
  host: 'c3633a3d114d40f4958f2d72c4c7e86f.s1.eu.hivemq.cloud',

  /**
   * Porta WebSocket Secure (WSS)
   * 8884 = MQTT sobre WebSocket com TLS
   * @type {number}
   */
  port: 8884,

  /**
   * Protocolo de conexão
   * @type {string}
   */
  protocol: 'wss',

  /**
   * Credenciais de autenticação
   * @type {Object}
   */
  auth: {
    username: 'gabriel',
    password: 'Gm302278'
  }
};

/**
 * Tópicos MQTT organizados hierarquicamente
 * @constant {Object} TOPICOS
 */
const TOPICOS = {
  /**
   * Leituras dos sensores
   */
  sensores: {
    temperatura: 'clima/uberlandia/centro/temperatura',
    umidade: 'clima/uberlandia/centro/umidade',
    luminosidade: 'clima/uberlandia/centro/luminosidade',
    status: 'clima/uberlandia/centro/status'
  },

  /**
   * Wildcards para subscrição em massa
   */
  wildcards: {
    todasTemperaturas: 'clima/uberlandia/+/temperatura',
    todosAlertas: 'alertas/#'
  }
};

/**
 * Configurações da interface gráfica
 * @constant {Object} UI_CONFIG
 */
const UI_CONFIG = {
  /**
   * Número máximo de pontos no gráfico
   */
  maxPontosGrafico: 30,

  /**
   * Número máximo de mensagens no log
   */
  maxLinhasLog: 50,

  /**
   * Número máximo de alertas visíveis
   */
  maxAlertasVisiveis: 20,

  /**
   * Intervalo de reconexão automática (ms)
   */
  intervaloReconexao: 3000,

  /**
   * Duração da animação do gráfico (ms)
   */
  duracaoAnimacao: 300
};

/**
 * Thresholds para alertas visuais
 * @constant {Object} ALERTAS
 */
const ALERTAS = {
  temperatura: {
    alta: 35.0,
    critica: 40.0
  },
  umidade: {
    baixa: 30.0,
    critica: 20.0
  },
  luminosidade: {
    escuro: 100.0,
    claro: 800.0
  }
};

/**
 * Níveis de QoS (Quality of Service)
 * @constant {Object} QOS
 */
const QOS = {
  LEITURAS: 0,   // At Most Once - para dados frequentes
  CRITICO: 1     // At Least Once - para alertas e status
};

// Congela objetos para evitar modificações acidentais (imutabilidade)
Object.freeze(MQTT_CONFIG);
Object.freeze(TOPICOS);
Object.freeze(UI_CONFIG);
Object.freeze(ALERTAS);
Object.freeze(QOS);
