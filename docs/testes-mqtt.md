# 🧪 Testes Documentados — Requisito 3.2

Evidências dos testes da comunicação MQTT realizados em **11/06/2026**, em uma máquina Windows 11 com **Mosquitto 2.1.2** instalado localmente, contra o broker **HiveMQ Cloud** (TLS, porta 8883).

> Credenciais ocultadas nos logs por segurança (`-u <usuario> -P <senha>`).

---

## 1. Teste com `mosquitto_sub` — assinatura com wildcard `#` (cliente local → broker na nuvem)

Comando executado:

```powershell
& "C:\Program Files\mosquitto\mosquitto_sub.exe" `
  -h c3633a3d114d40f4958f2d72c4c7e86f.s1.eu.hivemq.cloud -p 8883 `
  --cafile .\isrgrootx1.pem -u <usuario> -P <senha> `
  -t "clima/#" -v -W 15
```

Saída (com o ESP32 desligado — recebemos apenas a **retained message** do LWT, comprovando que o broker guarda o último status conhecido):

```text
clima/uberlandia/centro/status offline
```

✅ Demonstra: conexão TLS com o broker na nuvem, wildcard `#`, retained message do LWT.

---

## 2. Teste com `mosquitto_pub` + `mosquitto_sub` — publicação QoS 1 de alerta

Assinante em `alertas/#` rodando em paralelo enquanto publicamos um alerta com QoS 1:

```powershell
# Terminal 1 (assinante)
mosquitto_sub.exe -h c3633a3d114d40f4958f2d72c4c7e86f.s1.eu.hivemq.cloud -p 8883 `
  --cafile .\isrgrootx1.pem -u <usuario> -P <senha> -t "alertas/#" -v

# Terminal 2 (publicador)
mosquitto_pub.exe -h c3633a3d114d40f4958f2d72c4c7e86f.s1.eu.hivemq.cloud -p 8883 `
  --cafile .\isrgrootx1.pem -u <usuario> -P <senha> `
  -t "alertas/temperatura" -m "TESTE-CLI: 45.0C (mosquitto_pub QoS1)" -q 1
```

Saída do assinante:

```text
alertas/temperatura TESTE-CLI: 45.0C (mosquitto_pub QoS1)
```

✅ Demonstra: publicação e entrega com QoS 1 no tópico de alertas, via linha de comando.

---

## 3. Ponte (bridge): broker Mosquitto LOCAL ↔ HiveMQ Cloud

Configuração do broker local com bridge (ver [`mosquitto-bridge.example.conf`](../mosquitto-bridge.example.conf)):

```conf
listener 1884 localhost
allow_anonymous true

connection ponte-hivemq-cloud
address c3633a3d114d40f4958f2d72c4c7e86f.s1.eu.hivemq.cloud:8883
remote_clientid ponte-local-01
bridge_protocol_version mqttv311
remote_username <usuario>
remote_password <senha>
bridge_cafile isrgrootx1.pem
bridge_tls_version tlsv1.2

topic clima/# both 0
topic alertas/# both 1
```

Trecho do log do Mosquitto local mostrando a ponte conectada e trafegando mensagens:

```text
1781225246: mosquitto version 2.1.2 starting
1781225246: Config loaded from mosquitto-bridge.conf.
1781225246: Opening ipv4 listen socket on port 1884.
1781225246: Bridge local.ponte-local-01 doing local SUBSCRIBE on topic clima/#
1781225246: Bridge local.ponte-local-01 doing local SUBSCRIBE on topic alertas/#
1781225246: Connecting bridge ponte-hivemq-cloud (c3633a3d114d40f4958f2d72c4c7e86f.s1.eu.hivemq.cloud:8883)
1781225360: Received PUBLISH from auto-07B19CF0... ('clima/uberlandia/centro/temperatura', ... (4 bytes))
1781225360: Sending PUBLISH to local.ponte-local-01 ('clima/uberlandia/centro/temperatura', ... (4 bytes))
1781225420: Sending PINGREQ to local.ponte-local-01
1781225420: Received PINGRESP from local.ponte-local-01
```

**Teste de ponta a ponta:** publicamos no broker **local** e recebemos no assinante conectado à **nuvem**:

```powershell
# Assinante conectado ao HiveMQ Cloud (nuvem)
mosquitto_sub.exe -h c3633a3d114d40f4958f2d72c4c7e86f.s1.eu.hivemq.cloud -p 8883 `
  --cafile .\isrgrootx1.pem -u <usuario> -P <senha> -t "clima/#" -v

# Publicador conectado ao broker LOCAL (localhost:1884, sem TLS)
mosquitto_pub.exe -h localhost -p 1884 `
  -t "clima/uberlandia/centro/temperatura" -m "TESTE-PONTE-LOCAL-29.9"
```

Saída do assinante na **nuvem**:

```text
clima/uberlandia/centro/status offline
clima/uberlandia/centro/temperatura TESTE-PONTE-LOCAL-29.9
```

✅ Demonstra: broker local Mosquitto conectado por **bridge** ao broker na nuvem — mensagem publicada localmente atravessou a ponte e foi entregue na nuvem (e o retained da nuvem foi replicado para o broker local).

---

## Observações / dificuldades

- O `--tls-use-os-certs` do Mosquitto no Windows falhou na validação do certificado do HiveMQ (`certificate verify failed`). Solução: baixar o certificado raiz **ISRG Root X1** da Let's Encrypt e passar via `--cafile`.
- Nas primeiras tentativas a ponte desconectava logo após o `CONNECT`. A conexão estabilizou após fixar `remote_clientid` e `bridge_protocol_version mqttv311` — o HiveMQ Cloud rejeita reconexões rápidas com client ID instável.
- A porta padrão 1883 já estava ocupada pelo serviço do Mosquitto instalado no Windows; o broker de teste usou a porta **1884**.
