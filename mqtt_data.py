#!/usr/bin/env python3
import paho.mqtt.client as mqtt
import json
import sqlite3
import requests
from datetime import datetime

DB_FILE = "mqtt_data.db"
WEBHOOK = "https://hook.eu1.make.com/jyu9okfy26gbdns7fevtg16hyy65hq54"
TEMP_MAX = 25
HUM_MAX = 60
BROKER = "172.20.10.3"
PORT = 1883

conn = sqlite3.connect(DB_FILE)
c = conn.cursor()
c.execute('''CREATE TABLE IF NOT EXISTS mesures 
             (id INTEGER PRIMARY KEY AUTOINCREMENT,
              timestamp TEXT, 
              topic TEXT, 
              temp REAL, 
              hum REAL, 
              pir INTEGER)''')
conn.commit()

def on_connect(client, userdata, flags, rc):
    print(f"Connecté MQTT (code {rc}) → BDD + Alertes ON")
    client.subscribe("#")

def on_message(client, userdata, msg):
    try:
        data = json.loads(msg.payload.decode())
        temp = data.get('temp', None)
        hum = data.get('hum', None)
        pir = data.get('pir', None)
        
        c.execute("INSERT INTO mesures (timestamp, topic, temp, hum, pir) VALUES (?, ?, ?, ?, ?)",
                  (datetime.now().isoformat(), msg.topic, temp, hum, pir))
        conn.commit()
        
        if temp and temp > TEMP_MAX:
            send_alert("TEMP", temp, TEMP_MAX, msg.topic)
        if hum and hum > HUM_MAX:
            send_alert("HUM", hum, HUM_MAX, msg.topic)
        
        print(f"{msg.topic}: T={temp or '?'}° H={hum or '?'}% P={pir}")
        
    except json.JSONDecodeError:
        print(f"Non-JSON: {msg.topic}")
    except Exception as e:
        print(f"Erreur: {e}")

def send_alert(alerte, valeur, seuil, topic):
    payload = {
        "alerte": alerte,
        "salle": topic.split('/')[-1] if '/' in topic else "salle1",
        "valeur": valeur,
        "seuil": seuil,
        "topic": topic
    }
    resp = requests.post(WEBHOOK, json=payload)
    print(f"{alerte} {valeur}>{seuil} → Make: {resp.status_code}")

client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message

print("MQTT → BDD + Alertes Make.com")
client.connect(BROKER, PORT, 60)
client.loop_forever()
