//DECLARACAO DAS BIBLIOTECAS
#include <ESP8266WiFi.h>      // Wi-Fi
#include <PubSubClient.h>     // MQTT
#include <time.h>             // UTC
#include <strings.h>          // String manipulation
#include <FS.h>               // Files

const int trigPin = 12;
const int echoPin = 14;

//define a velocidade do som em cm/uS
#define SOUND_VELOCITY 0.034

long duration;
float distanceCm;

//informações da rede WIFI
const char* ssid = "CSI-Lab"; //SSID da rede WIFI
const char* password =  "In@teLCS&I"; //senha da rede wifi

//informações do broker MQTT
const char* mqttServer = "192.168.66.x";   //servidor
const int mqttPort = 1883;                     //porta
const char* mqttTopicSub = "#";           //tópico que sera assinado
const char *ID = "EspTeste001";  // Name of our device, must be unique

WiFiClient espClient;
PubSubClient client(espClient);

//FUNCAO DE RECEBIMENTO E PROCESSAMENTO DE MENSAGENS VINDAS DO BROKER
void callback(char* topic, byte* payload, unsigned int length){
  //armazena mensagem recebida em uma variavel inteira
  payload[length] = '\0';
  int MSG = atoi((char*)payload);
  String topico = topic;
  
  //MOSTRANDO RECEBIMENTO DA MENSAGEM NA SERIAL
  Serial.println("-----------------------");
  Serial.print("Mensagem no tópico: ");
  Serial.println(topico);
  Serial.print("Mensagem:");
  Serial.print(MSG);
  Serial.println();
  Serial.println("-----------------------");
  Serial.println();
  
}
  
void connect () //FUNCAO DE RECONEXAO COM O BROKER
{
  while (!client.connected()) //enquanto a conexão com o broker não for realizada
  {
    Serial.println("Conectando ao Broker MQTT...");

    if (client.connect(ID)) {
      Serial.println("Conectado");
    }
    else
    {
      Serial.print("falha estado  ");
      Serial.print(client.state());
      delay(2000);
    }
  }
  //subscreve no tópico
  client.subscribe(mqttTopicSub);
}

void setup()
{
  pinMode(trigPin, OUTPUT);        // Seta o trigPin como Output
  pinMode(echoPin, INPUT);         // Seta o echoPin como Input

  Serial.begin(115200); //Velocidade da Serial

  Serial.println("------Conexao WI-FI------");
  Serial.print("Conectando-se na rede: ");
  Serial.println(ssid);
  Serial.println("Aguarde");

  if (WiFi.status() == WL_CONNECTED)
    return;

  WiFi.begin(ssid, password); // Conecta na rede WI-FI

  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }

  Serial.println();
  Serial.print("Conectado com sucesso na rede ");
  Serial.print(ssid);

  //SETTING MQTT SERVER AND CALLBACK
  Serial.print(String(mqttServer)+" "+String(mqttPort));
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
  Serial.print("\nMQTT connecting... \n");
  connect();
}

void loop(){
  client.loop();
  delay(100);

  if (!client.connected())
  {
    connect();

  }

  // Limpa o trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Seta o trigPin para HIGH por 10 micro segundos
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Lê o tempo de resposta obtido pelo sensor
  duration = pulseIn(echoPin, HIGH);
  
  // Calcula a distancia
  distanceCm = duration * SOUND_VELOCITY/2;
  
  // Publica a distancia em cm no mosquitto
  Serial.print("Distancia (cm): ");
  Serial.println(distanceCm);
  char msg[20];
  // Convertendo o float para uma string
  sprintf(msg, "%f", distanceCm);
  client.publish("setInfoSensorHC", msg);

  delay(5000);
}