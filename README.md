# ☁ Estação Meteorológica Distribuída

> Projeto V2 – N461 Sistemas Multimídia e Distribuídos

Rede de sensores embarcados (ESP32 via Wokwi) que publica leituras de temperatura, umidade e luminosidade por MQTT em nuvem. Uma dashboard web consome os dados em tempo real, exibe gráficos e dispara alertas configuráveis.

---

## 👥 Integrantes

| Nome completo | RA |
|---|---|
| NOME DO INTEGRANTE 1 | RA000001 |
| NOME DO INTEGRANTE 2 | RA000002 |

---

## 🏷 Tema

**Tema 1 — Estação Meteorológica Distribuída**

---

## 🏗 Diagrama de Arquitetura

```
┌─────────────────────────────────────────────────────────────┐
│                        HiveMQ Cloud                         │
│              (broker MQTT – nuvem, porta 8883/8884)         │
└───────────────────┬──────────────────┬──────────────────────┘
                    │ MQTT/TLS          │ MQTT over WSS
          ┌─────────┴──────┐   ┌───────┴──────────┐
          │  ESP32 (Wokwi) │   │  Interface Web   │
          │  PubSubClient  │   │  MQTT.js + Chart │
          │                │   │  (Vercel/Netlify)│
          │ Publica:        │   │                  │
          │ - temperatura  │   │ Assina:          │
          │ - umidade      │   │ - clima/+/+/+    │
          │ - luminosidade │   │ - alertas/#      │
          │ - LWT: status  │   │                  │
          │                │   │ Publica:         │
          │ Assina:        │   │ - alertas/*      │
          │ - alertas/#    │   └──────────────────┘
          └────────────────┘
```

**Ferramentas de teste:**
- MQTT Explorer (desktop) → assina `clima/#` e `alertas/#`
- HiveMQ WebSocket Client (browser) → testes pontuais
- `mosquitto_pub / mosquitto_sub` (linha de comando) → testes de integração

---

## � Configuração do Projeto

Este projeto utiliza **arquitetura limpa** com **separação de responsabilidades**:
- Credenciais e configurações ficam em arquivos **separados** (não versionados no Git)
- Lógica de negócio fica nos arquivos principais (`main.ino`, `script.js`)

### 📁 Arquivos de Configuração

#### **Para o ESP32:**

1. Copie o arquivo template:
   ```bash
   cp config.example.h config.h
   ```

2. Edite `config.h` e preencha suas credenciais:
   ```c
   #define WIFI_SSID "Wokwi-GUEST"
   #define WIFI_PASSWORD ""
   
   #define MQTT_HOST "SEU_BROKER.hivemq.cloud"
   #define MQTT_PORT 8883
   #define MQTT_USER "seu_usuario"
   #define MQTT_PASS "sua_senha"
   ```

#### **Para a Dashboard Web:**

1. Copie o arquivo template:
   ```bash
   cp config.example.js config.js
   ```

2. Edite `config.js` e preencha suas credenciais:
   ```javascript
   const MQTT_CONFIG = {
     host: 'SEU_BROKER.hivemq.cloud',
     port: 8884,
     protocol: 'wss',
     auth: {
       username: 'seu_usuario',
       password: 'sua_senha'
     }
   };
   ```

### 🔒 Segurança

⚠️ **IMPORTANTE:**
- Os arquivos `config.h` e `config.js` estão listados no `.gitignore` e **não devem ser commitados**
- Apenas os arquivos `.example` devem ser versionados no GitHub
- Em ambientes de produção, considere usar variáveis de ambiente ou serviços de gerenciamento de secrets

**Limitações conhecidas:**
- **ESP32:** Firmware compilado contém credenciais. OK para simulação/prototipagem.
- **Dashboard Web:** JavaScript roda no cliente, credenciais são visíveis no source. Para produção, implemente um backend proxy.

---

## �📡 Tópicos MQTT

| Tópico | Direção | QoS | Retained | Descrição |
|---|---|---|---|---|
| `clima/uberlandia/centro/temperatura` | ESP32 → Broker | 0 | Não | Leitura de temperatura (°C) a cada 3s |
| `clima/uberlandia/centro/umidade` | ESP32 → Broker | 0 | Não | Leitura de umidade relativa (%) |
| `clima/uberlandia/centro/luminosidade` | ESP32 → Broker | 0 | Não | Leitura de luminosidade (lux) |
| `clima/uberlandia/centro/status` | ESP32 → Broker | 1 | **Sim** | Status do dispositivo: "online" / "offline" (LWT) |
| `alertas/temperatura` | ESP32 / Web → Broker | 1 | Não | Alerta quando temperatura > 35 °C |
| `alertas/umidade` | ESP32 / Web → Broker | 1 | Não | Alerta quando umidade < 30% |
| `clima/uberlandia/+/temperatura` | (wildcard) | 0 | — | Web assina: todos os sensores de temperatura da cidade |
| `alertas/#` | (wildcard) | 1 | — | Web e ESP32 assinam: todos os alertas |

### Justificativa dos níveis de QoS

| QoS | Onde foi usado | Justificativa |
|---|---|---|
| **0** (at most once) | Leituras dos sensores | São publicadas a cada 3s; a perda eventual de um pacote é aceitável, pois o próximo chegará logo. Menor overhead de rede. |
| **1** (at least once) | Status (LWT), alertas, comandos da interface | Não podem ser perdidos. O LWT precisa chegar ao broker para marcar o dispositivo como offline. Alertas e comandos são eventos únicos e críticos. |
| **2** (exactly once) | Não utilizado | Overhead alto; não justificado neste cenário. |

### Recursos avançados utilizados

- **LWT (Last Will and Testament):** ao se conectar, o ESP32 registra a mensagem `"offline"` no tópico `clima/uberlandia/centro/status` com `retained=true`. Se a conexão cair abruptamente, o broker publica a mensagem automaticamente.
- **Retained messages:** o tópico de status mantém o último valor conhecido. Novos clientes que se conectam ao broker já veem imediatamente se o sensor está online ou offline, sem precisar aguardar a próxima publicação.
- **Wildcards:** a interface web assina `clima/uberlandia/+/temperatura` (wildcard `+` para qualquer bairro) e `alertas/#` (wildcard `#` para todos os subtipos de alerta).

---

## 🛠 Tecnologias e Bibliotecas

| Camada | Tecnologia |
|---|---|
| Dispositivo embarcado | ESP32 (Wokwi), Arduino Framework |
| Biblioteca MQTT embarcada | [PubSubClient 2.8](https://github.com/knolleary/pubsubclient) |
| Broker MQTT | [HiveMQ Cloud](https://www.hivemq.com/mqtt-cloud-broker/) (gratuito) |
| Interface web | HTML5 / CSS3 / JavaScript puro |
| Cliente MQTT no browser | [MQTT.js 5.0](https://github.com/mqttjs/MQTT.js) via CDN |
| Gráficos | [Chart.js 4.4](https://www.chartjs.org/) |
| Deploy | Vercel / Netlify (gratuito) |
| Versionamento | Git + GitHub |

---

## ⚡ Instalação e Execução Local

### Pré-requisitos

- [Node.js](https://nodejs.org) (apenas para servidor local simples, opcional)
- Conta no [HiveMQ Cloud](https://console.hivemq.cloud/) (gratuita)
- [Wokwi](https://wokwi.com/) (browser, gratuito)
- [MQTT Explorer](https://mqtt-explorer.com/) (para testes)
- Mosquitto instalado (`sudo apt install mosquitto-clients` no Linux)

### 1. Configurar o broker HiveMQ Cloud

1. Acesse [console.hivemq.cloud](https://console.hivemq.cloud/) e crie um cluster gratuito.
2. Vá em **Access Management** e crie um usuário/senha.
3. Anote o **hostname** do cluster (ex.: `abc123.s1.eu.hivemq.cloud`).

### 2. Configurar o ESP32 (Wokwi)

1. Acesse [wokwi.com](https://wokwi.com) e crie um novo projeto ESP32.
2. Cole o conteúdo de `main.ino` no editor.
3. Cole o conteúdo de `diagram.json` no diagrama.
4. **Edite as linhas marcadas com `← EDITE`** em `main.ino`:
   ```cpp
   const char* MQTT_HOST = "SEU_BROKER.s1.eu.hivemq.cloud";
   const char* MQTT_USER = "SEU_USUARIO";
   const char* MQTT_PASS = "SUA_SENHA";
   ```
5. Clique em **▶ Run** e gire os potenciômetros para simular leituras.

### 3. Configurar a interface web

1. Abra `index.html` em qualquer editor e edite as linhas marcadas:
   ```javascript
   const BROKER_HOST = 'SEU_BROKER.s1.eu.hivemq.cloud';
   const MQTT_USER   = 'SEU_USUARIO';
   const MQTT_PASS   = 'SUA_SENHA';
   ```
2. Abra o arquivo diretamente no browser **ou** suba em um servidor local:
   ```bash
   npx serve .
   # acesse http://localhost:3000
   ```

### 4. Testar com MQTT Explorer

```
Host: SEU_BROKER.s1.eu.hivemq.cloud
Port: 8883  (TLS)
Username: SEU_USUARIO
Password: SUA_SENHA
```

Assine `clima/#` para ver todas as leituras e `alertas/#` para alertas.

### 5. Testar com mosquitto_sub / mosquitto_pub

```bash
# Assinar todas as leituras (wildcard '#')
mosquitto_sub -h SEU_BROKER.s1.eu.hivemq.cloud -p 8883 \
  --cafile /etc/ssl/certs/ca-certificates.crt \
  -u SEU_USUARIO -P SUA_SENHA \
  -t "clima/#" -v

# Publicar um alerta manualmente (QoS 1)
mosquitto_pub -h SEU_BROKER.s1.eu.hivemq.cloud -p 8883 \
  --cafile /etc/ssl/certs/ca-certificates.crt \
  -u SEU_USUARIO -P SUA_SENHA \
  -t "alertas/temperatura" -m "TESTE:45.0C" -q 1
```

---

## 🚀 Deploy

A interface web é um único arquivo estático (`index.html`) e pode ser publicada em qualquer plataforma gratuita:

**Vercel (recomendado):**
```bash
npm install -g vercel
vercel deploy
```

**Netlify:** arraste a pasta do projeto para [app.netlify.com/drop](https://app.netlify.com/drop).

**Resultado:** a aplicação ficará acessível em uma URL pública (ex.: `https://estacao-n461.vercel.app`).

---

## 📸 Capturas de Tela

> Adicione screenshots aqui após executar o projeto:
> - Dashboard web com gráficos
> - Serial Monitor do Wokwi mostrando publicações
> - MQTT Explorer mostrando os tópicos
> - Teste via mosquitto_pub / mosquitto_sub

---

## 🔗 Links

- **Aplicação hospedada:** https://SEU-LINK.vercel.app
- **Projeto Wokwi:** https://wokwi.com/projects/SEU-ID
- **HiveMQ WebSocket Client (teste):** https://www.hivemq.com/demos/websocket-client/

---

## 📋 Dificuldades Enfrentadas

> Preencha durante o desenvolvimento:
> - Configuração do TLS no ESP32 com WiFiClientSecure
> - Ajuste do timeout de reconexão do PubSubClient
> - etc.

---

## 📜 Créditos

- [PubSubClient](https://github.com/knolleary/pubsubclient) – Nick O'Leary (MIT)
- [MQTT.js](https://github.com/mqttjs/MQTT.js) – Adam Rudd (MIT)
- [Chart.js](https://www.chartjs.org/) – Chart.js Contributors (MIT)
