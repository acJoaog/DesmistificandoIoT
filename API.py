#Código exemplo de conexão ao databse e broker mqtt

import paho.mqtt.client as mqtt #pip install paho-mqtt==1.6.1
import mysql.connector #pip instal mysql-connector-python

cnx = mysql.connector.connect(host="192.168.0.122",user="root",password="root",database="iot",) 

# Função para conexão
def on_connect(client, userdata, flags, rc):
    print("Conectado - Codigo de resultado: "+str(rc))

    # Indique o tópico a ser assinado - "#" se inscreve em todos
    client.subscribe("#")

#função onde recebe mensagens 
def on_message(client, userdata, msg):
    try:
        cursor = cnx.cursor()
        print(msg.topic+" "+str(msg.payload.decode()))
        lista = msg.topic.split("/")
				
        if lista[0] == "getInfosHC":
            query = "SELECT Valor FROM SensorHC;"
            cursor.execute(query)
            result = cursor.fetchone()
            for row in result:
                print(row)

        if lista[0] == "setInfoSensorHC":
            query = f"INSERT INTO SensorHC (Valor) values ({msg.payload.decode()});"
            cursor.execute(query)
            cnx.commit()
            print("Valor salvo no banco de dados com sucesso!")

    except Exception as e:
        print(e)

client = mqtt.Client()

client.on_connect = on_connect
client.on_message = on_message
		
try:
    client.connect("192.168.0.122", 1883, 60) #Mude o hostname para o IP do servidor
except:
    print("Não foi possivel conectar ao MQTT...")
    print("Encerrando...")

try:
    client.loop_forever()
except KeyboardInterrupt:  #precionar Crtl + C para salir
    print("Encerrando...")