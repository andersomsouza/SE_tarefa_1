Sistema desenvolvido para a disciplina de Sistemas Embarcados.

## Servidor
### Especificação

* Arquitetura: Arm (Raspberry Pi Zero W)
* Apache2 com PHP 7.0 + FastCGI
* Redis
* MySQL
* Mosquitto (MQTT Broker)

## Clientes
### Especificação

* Plataformas: Arduino ou ESP8266
* [arduino-mqtt](https://github.com/256dpi/arduino-mqtt)

*\* Nas plataformas ESP8266 foi usada o [core-arduino](https://github.com/esp8266/Arduino), para facil portabilidade do código.*

## Como funciona?
O protocolo utilizado foi o [MQTT v3.1](https://pt.wikipedia.org/wiki/MQTT).
O usuário pode consumir o serviço de acesso as plataformas de quatro maneiras:

* Via Socket na porta 1883 (Protocolo MQTT)
* Via WebSocket na porta 1884  (Protocolo MQTT)
* Via API Restful (Ainda não realizada)
* Via WebServer (Parcialmente)

Para tirar melhor proveito no acesso às informações foram usados duas estruturas de banco de dados, o MySQL para registro dos históricos e Redis para registro da ultima leitura (O registro é feito através de uma thread em PHP).

### Especificação tópicos para MQTT
URI:  `/(arduino|esp8266)/(id)/(topico)`

| Parametro          |                Descrição                      |
|--------------------|-----------------------------------------------|
|`(arduino\|esp8266)` | Selecionar um desses dependendo da plataforma |
| `(id)`             | Especificar id unico para a plataforma (0...n)|
| `(topico)`         | Topico especifico \*                          |

\* Alguns tópicos definido foram alive e led.
`alive` é o tópico em que a plataforma publica o número de segundos de programa rodando.
`led` é o registro específico para o usuário interagir e acionar o led embutido da plataforma.

### via Socket ou Websocket 
O consumo é realizado de forma autenticada através de aplicativos e/ou bibliotecas que implementem o protocolo MQTT.

### via API RestFUL
Essa API será responsável pelas consultas de histórico ao banco de dados e também retornar o ultimo valor enviado pelas plataformas.

### via WebServer
Na porta 80 da Raspberry fica hospedado um servidor web,onde na página principal é possível realizar o login e acompanhar algumas variavéis diretamente das plataformas (Conecta ao servidor através de WebSocket com a implementação da biblioteca [MQTT.js](https://github.com/mqttjs))

#### Estrutura do banco de dados (MySQL)

`Tabela: MQTT`

| Campo        | Tipo            |
|--------------|-----------------|
| id           | int(6) unsigned |
| topico       | text            |
| payload      | longtext        |
| data_criacao | timestamp       |

