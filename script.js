// ============================================================
// ESTAÇÃO METEOROLÓGICA - DASHBOARD WEB
// Arquitetura: Clean Code + Separação de Responsabilidades
// - config.js: Configurações e credenciais
// - script.js: Lógica de negócio e UI
// ============================================================

// ============================================================
// VALIDAÇÃO DE DEPENDÊNCIAS
// ============================================================

if (typeof MQTT_CONFIG === 'undefined') {
  console.error('❌ ERRO: Arquivo config.js não encontrado!');
  console.error('📋 Copie config.example.js para config.js e configure suas credenciais.');
  alert('Erro: Arquivo de configuração não encontrado. Veja o console para instruções.');
}

// ============================================================
// CONSTANTES DERIVADAS
// ============================================================

/**
 * URL completa do broker MQTT
 * @constant {string}
 */
const BROKER_URL = `${MQTT_CONFIG.protocol}://${MQTT_CONFIG.host}:${MQTT_CONFIG.port}/mqtt`;

// ============================================================
// CONFIGURAÇÃO DO GRÁFICO
// ============================================================

// Quantidade maxima de pontos exibidos no grafico.
const MAX_PONTOS = UI_CONFIG.maxPontosGrafico;

// Lista de horarios exibidos no eixo X.
const labels = [];

// Lista de valores de temperatura.
const tempData = [];

// Lista de valores de umidade.
const umidData = [];

// Lista de valores de luminosidade.
const luxData = [];

// Busca o elemento <canvas> do HTML onde o grafico sera desenhado.
const ctx = document.getElementById('chart').getContext('2d');

// Cria o grafico usando a biblioteca Chart.js.
const chart = new Chart(ctx, {
  // Define que o grafico sera de linha.
  type: 'line',

  // Dados exibidos no grafico.
  data: {
    // Labels sao os horarios das leituras.
    labels,

    // Cada dataset representa uma linha do grafico.
    datasets: [
      {
        // Nome exibido na legenda.
        label: 'Temperatura (°C)',

        // Dados usados pela linha de temperatura.
        data: tempData,

        // Cor da linha.
        borderColor: '#ff4757',

        // Cor do preenchimento abaixo da linha.
        backgroundColor: 'rgba(255, 71, 87, 0.08)',

        // Suaviza a curva da linha.
        tension: 0.4,

        // Tamanho dos pontos no grafico.
        pointRadius: 2,

        // Ativa preenchimento abaixo da linha.
        fill: true
      },
      {
        // Nome exibido na legenda.
        label: 'Umidade (%)',

        // Dados usados pela linha de umidade.
        data: umidData,

        // Cor da linha.
        borderColor: '#00d4ff',

        // Cor do preenchimento abaixo da linha.
        backgroundColor: 'rgba(0, 212, 255, 0.08)',

        // Suaviza a curva da linha.
        tension: 0.4,

        // Tamanho dos pontos no grafico.
        pointRadius: 2,

        // Ativa preenchimento abaixo da linha.
        fill: true
      },
      {
        // Nome exibido na legenda. O valor e dividido por 10 para caber melhor.
        label: 'Lux / 10',

        // Dados usados pela linha de luminosidade.
        data: luxData,

        // Cor da linha.
        borderColor: '#ffd166',

        // Cor do preenchimento abaixo da linha.
        backgroundColor: 'rgba(255, 209, 102, 0.08)',

        // Suaviza a curva da linha.
        tension: 0.4,

        // Tamanho dos pontos no grafico.
        pointRadius: 2,

        // Ativa preenchimento abaixo da linha.
        fill: true
      }
    ]
  },

  // Opcoes visuais e de comportamento do grafico.
  options: {
    // Faz o grafico se adaptar ao tamanho do container.
    responsive: true,

    // Permite controlar a altura pelo CSS.
    maintainAspectRatio: false,

    // Duracao da animacao ao atualizar os dados.
    animation: { duration: 300 },

    // Configuracao dos eixos X e Y.
    scales: {
      // Eixo X mostra os horarios.
      x: {
        // Cor e tamanho dos textos do eixo X.
        ticks: { color: '#64748b', font: { size: 10 } },

        // Cor das linhas de grade do eixo X.
        grid: { color: '#1e2d45' }
      },

      // Eixo Y mostra os valores numericos.
      y: {
        // Cor e tamanho dos textos do eixo Y.
        ticks: { color: '#64748b', font: { size: 10 } },

        // Cor das linhas de grade do eixo Y.
        grid: { color: '#1e2d45' }
      }
    },

    // Configuracao dos plugins do Chart.js.
    plugins: {
      // Estilo da legenda do grafico.
      legend: { labels: { color: '#e2e8f0', font: { size: 11 } } }
    }
  }
});

// ============================================================
// FUNCAO PARA ATUALIZAR O GRAFICO
// ============================================================

// Recebe temperatura, umidade e luminosidade e adiciona um novo ponto no grafico.
function adicionarPonto(temp, umid, lux) {
  // Pega a hora atual formatada no padrao brasileiro.
  const hora = new Date().toLocaleTimeString('pt-BR', {
    // Mostra duas casas para hora.
    hour: '2-digit',

    // Mostra duas casas para minuto.
    minute: '2-digit',

    // Mostra duas casas para segundo.
    second: '2-digit'
  });

  // Se o grafico ja tem o limite de pontos, remove o ponto mais antigo.
  if (labels.length >= MAX_PONTOS) {
    // Remove o horario mais antigo.
    labels.shift();

    // Remove a temperatura mais antiga.
    tempData.shift();

    // Remove a umidade mais antiga.
    umidData.shift();

    // Remove a luminosidade mais antiga.
    luxData.shift();
  }

  // Adiciona o horario da nova leitura.
  labels.push(hora);

  // Adiciona a nova temperatura.
  tempData.push(temp);

  // Adiciona a nova umidade.
  umidData.push(umid);

  // Adiciona a luminosidade dividida por 10 para caber melhor na escala.
  luxData.push(lux / 10);

  // Atualiza o grafico na tela.
  chart.update();
}

// ============================================================
// LOG E ALERTAS DA INTERFACE
// ============================================================

// Guarda a ultima leitura de cada sensor antes de montar um ponto completo no grafico.
const ultimosValores = { temp: null, umid: null, lux: null };

// Escreve uma mensagem no painel de log da dashboard.
function log(msg, tipo = 'info') {
  // Busca o elemento de log no HTML.
  const logEl = document.getElementById('log');

  // Pega a hora atual.
  const hora = new Date().toLocaleTimeString('pt-BR');

  // Cria uma tag <span> para representar uma linha do log.
  const span = document.createElement('span');

  // Define a classe visual da mensagem: info, warn ou err.
  span.className = tipo;

  // Define o texto mostrado no log.
  span.textContent = `[${hora}] ${msg}`;

  // Insere a nova mensagem no topo do log.
  logEl.prepend(span);

  // Mantem no maximo as mensagens configuradas para nao pesar a pagina.
  if (logEl.children.length > UI_CONFIG.maxLinhasLog) {
    // Remove a mensagem mais antiga.
    logEl.removeChild(logEl.lastChild);
  }
}

// Adiciona uma mensagem de alerta na lista de alertas recebidos.
function adicionarAlerta(topico, mensagem) {
  // Busca a lista <ul> onde os alertas aparecem.
  const lista = document.getElementById('alert-list');

  // Verifica se ainda existe a mensagem "Nenhum alerta ainda".
  const vazio = lista.querySelector('.empty');

  // Remove a mensagem vazia quando chega o primeiro alerta real.
  if (vazio) {
    vazio.remove();
  }

  // Pega a hora atual.
  const hora = new Date().toLocaleTimeString('pt-BR');

  // Cria um item <li> para o alerta.
  const li = document.createElement('li');

  // Define o texto do alerta com horario, topico e mensagem.
  li.textContent = `[${hora}] ${topico}: ${mensagem}`;

  // Coloca o alerta novo no topo da lista.
  lista.prepend(li);

  // Mantem no maximo os alertas configurados.
  if (lista.children.length > UI_CONFIG.maxAlertasVisiveis) {
    // Remove o alerta mais antigo.
    lista.removeChild(lista.lastChild);
  }
}

// ============================================================
// CONEXÃO MQTT VIA WEBSOCKET
// ============================================================

/**
 * Gera um identificador único para este cliente web
 * @returns {string} ID único no formato 'web-XXXXXXXX'
 */
function gerarClientId() {
  const random = Math.random().toString(16).substring(2, 10);
  return `web-${random}`;
}

// Identificador único deste cliente
const clientId = gerarClientId();

/**
 * Cliente MQTT configurado para comunicação WebSocket segura
 * Utiliza credenciais e configurações do arquivo config.js
 */
const client = mqtt.connect(BROKER_URL, {
  clientId: clientId,
  username: MQTT_CONFIG.auth.username,
  password: MQTT_CONFIG.auth.password,
  clean: true,  // Sessão limpa: não reaproveita inscrições antigas
  reconnectPeriod: UI_CONFIG.intervaloReconexao
});

// Evento disparado quando a dashboard conecta ao broker.
client.on('connect', () => {
  // Muda a bolinha de status para online.
  document.getElementById('status-dot').className = 'online';

  // Atualiza o texto do status com o endereco do broker.
  document.getElementById('status-text').textContent = 'conectado · ' + MQTT_CONFIG.host;

  // Registra no log que a conexao foi feita.
  log(`Conectado ao broker (${clientId})`, 'info');

  /*
   * INSCRIÇÕES MQTT COM QoS DIFERENCIADO
   * 
   * QoS 0 (At Most Once): Para leituras frequentes de sensores
   * - Novas leituras chegam a cada 3s, perda eventual é tolerável
   * 
   * QoS 1 (At Least Once): Para mensagens críticas
   * - Status do dispositivo (online/offline)
   * - Alertas de condições extremas
   */
  
  // Sensores: QoS 0 (leituras frequentes)
  client.subscribe(TOPICOS.sensores.temperatura, { qos: QOS.LEITURAS });
  client.subscribe(TOPICOS.sensores.umidade, { qos: QOS.LEITURAS });
  client.subscribe(TOPICOS.sensores.luminosidade, { qos: QOS.LEITURAS });

  // Status: QoS 1 (disponibilidade é crítica)
  client.subscribe(TOPICOS.sensores.status, { qos: QOS.CRITICO });

  // Wildcards: Demonstração de padrões avançados
  client.subscribe(TOPICOS.wildcards.bairros, { qos: QOS.LEITURAS });
  client.subscribe(TOPICOS.wildcards.todosAlertas, { qos: QOS.CRITICO });

  // Mostra no log quais topicos foram assinados.
  log('Inscrições: temperatura, umidade, luminosidade, status, alertas', 'info');
});

// Evento disparado quando o cliente desconecta do broker.
client.on('disconnect', () => {
  // Muda a bolinha de status para offline.
  document.getElementById('status-dot').className = 'offline';

  // Atualiza o texto do status.
  document.getElementById('status-text').textContent = 'desconectado';

  // Registra a desconexao no log.
  log('Desconectado do broker', 'err');
});

// Evento disparado quando ocorre erro na conexao MQTT.
client.on('error', (err) => {
  // Mostra a mensagem de erro no log.
  log('Erro MQTT: ' + err.message, 'err');
});

// Evento disparado enquanto a biblioteca tenta reconectar.
client.on('reconnect', () => {
  // Informa ao usuario que a reconexao esta em andamento.
  log('Reconectando...', 'warn');
});

// ============================================================
// RECEBIMENTO DE MENSAGENS MQTT
// ============================================================

/**
 * Handler principal de mensagens MQTT
 * Processa temperatura, umidade, luminosidade, status e alertas
 */
client.on('message', (topico, payload) => {
  const mensagem = payload.toString();
  const hora = new Date().toLocaleTimeString('pt-BR');

  // Log de todas as mensagens recebidas
  log(`${topico} → ${mensagem}`, 'info');

  // ──────────────────────────────────────────────────────────
  // TEMPERATURA
  // ──────────────────────────────────────────────────────────
  if (topico === TOPICOS.sensores.temperatura) {
    const valor = parseFloat(mensagem);
    
    // Atualiza interface
    document.getElementById('val-temp').innerHTML = 
      valor.toFixed(1) + '<span class="unit">°C</span>';
    document.getElementById('ts-temp').textContent = `Atualizado às ${hora}`;
    
    // Verifica alerta
    if (valor > ALERTAS.temperatura.alta) {
      log(`⚠ Temperatura crítica: ${valor.toFixed(1)}°C`, 'warn');
    }
    
    // Armazena para gráfico
    ultimosValores.temp = valor;
    verificarGrafico();
    return;
  }

  // ──────────────────────────────────────────────────────────
  // UMIDADE
  // ──────────────────────────────────────────────────────────
  if (topico === TOPICOS.sensores.umidade) {
    const valor = parseFloat(mensagem);
    
    // Atualiza interface
    document.getElementById('val-umid').innerHTML = 
      valor.toFixed(1) + '<span class="unit">%</span>';
    document.getElementById('ts-umid').textContent = `Atualizado às ${hora}`;
    
    // Verifica alerta
    if (valor < ALERTAS.umidade.baixa) {
      log(`⚠ Umidade baixa: ${valor.toFixed(1)}%`, 'warn');
    }
    
    // Armazena para gráfico
    ultimosValores.umid = valor;
    verificarGrafico();
    return;
  }

  // ──────────────────────────────────────────────────────────
  // LUMINOSIDADE
  // ──────────────────────────────────────────────────────────
  if (topico === TOPICOS.sensores.luminosidade) {
    const valor = parseFloat(mensagem);
    
    // Atualiza interface
    document.getElementById('val-lux').innerHTML = 
      valor.toFixed(0) + '<span class="unit">lux</span>';
    document.getElementById('ts-lux').textContent = `Atualizado às ${hora}`;
    
    // Armazena para gráfico
    ultimosValores.lux = valor;
    verificarGrafico();
    return;
  }

  // ──────────────────────────────────────────────────────────
  // ALERTAS
  // ──────────────────────────────────────────────────────────
  if (topico.startsWith('alertas/')) {
    adicionarAlerta(topico, mensagem);
    log(`ALERTA: ${topico} → ${mensagem}`, 'warn');
    return;
  }

  // ──────────────────────────────────────────────────────────
  // STATUS DO DISPOSITIVO
  // ──────────────────────────────────────────────────────────
  if (topico === TOPICOS.sensores.status) {
    const tipo = mensagem === 'online' ? 'info' : 'warn';
    log(`Status do sensor: ${mensagem}`, tipo);
  }
});

// ============================================================
// CONTROLE DE ATUALIZACAO DO GRAFICO
// ============================================================

// Guarda o horario em milissegundos do ultimo ponto adicionado no grafico.
let ultimoPonto = 0;

// Verifica se as tres leituras ja chegaram antes de atualizar o grafico.
function verificarGrafico() {
  // Extrai os ultimos valores recebidos de temperatura, umidade e luminosidade.
  const { temp, umid, lux } = ultimosValores;

  // Pega o horario atual em milissegundos.
  const agora = Date.now();

  // So adiciona ponto se todos os valores existirem e se passou cerca de 2,5 segundos.
  if (temp !== null && umid !== null && lux !== null && agora - ultimoPonto > 2500) {
    // Adiciona o conjunto de leituras no grafico.
    adicionarPonto(temp, umid, lux);

    // Atualiza o controle de tempo do ultimo ponto.
    ultimoPonto = agora;
  }
}

// ============================================================
// PUBLICACAO MANUAL DE MENSAGENS
// ============================================================

// Busca o select onde o usuario escolhe o topico de publicacao.
const selectTopic = document.getElementById('pub-topic');

// Busca o input usado quando o usuario quer digitar um topico customizado.
const inputCustom = document.getElementById('pub-topic-custom');

// Evento disparado quando o usuario troca a opcao do select.
selectTopic.addEventListener('change', () => {
  // Mostra o campo customizado apenas se a opcao "custom" estiver selecionada.
  inputCustom.style.display = selectTopic.value === 'custom' ? 'block' : 'none';
});

// Funcao chamada pelo botao PUBLICAR no HTML.
function publicar() {
  // Verifica se o cliente MQTT esta conectado antes de tentar publicar.
  if (!client.connected) {
    // Mostra erro no log se nao houver conexao.
    log('Não conectado ao broker!', 'err');

    // Interrompe a funcao.
    return;
  }

  // Define o topico: usa o campo customizado ou a opcao escolhida no select.
  const topico = selectTopic.value === 'custom'
    ? inputCustom.value.trim()
    : selectTopic.value;

  // Pega a mensagem digitada pelo usuario.
  const mensagem = document.getElementById('pub-msg').value.trim();

  // Verifica se topico e mensagem foram preenchidos.
  if (!topico || !mensagem) {
    // Mostra aviso se algum campo estiver vazio.
    log('Preencha tópico e mensagem!', 'warn');

    // Interrompe a funcao.
    return;
  }

  // Publica a mensagem no broker com QoS 1.
  client.publish(topico, mensagem, { qos: 1 }, (err) => {
    // Se o broker retornar erro, mostra no log.
    if (err) {
      // Registra falha de publicacao.
      log('Erro ao publicar: ' + err.message, 'err');
    } else {
      // Registra sucesso de publicacao.
      log(`Publicado em "${topico}": ${mensagem}`, 'info');
    }
  });
}
