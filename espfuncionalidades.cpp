#include <Wire.h>
#include "BluetoothSerial.h"
#include "Adafruit_MPU6050.h"
#include "Adafruit_Sensor.h"
#include "MAX30105.h"
#include "heartRate.h"
#include "verificarQueda.h"
#include "verificarBatimentos.h"

// LEMBRA DE DEIXAR EXATAMENTE NESSA ORDEM PELO AMOR DE DEUS

// definindo os pinos
#define I2C_SDA 4  // CONECTAR NO PINO D4
#define I2C_SCL 5  // CONECTA NO PINO D1  
#define buzzer 13  // PINO D7 --- esse se tu quiser trocar de pino evita apenas os pinos D3 e D8

// (Se for usar o ESP32 real, as linhas de RX e TX não são necessárias pois o Bluetooth é nativo.
// Se for usar o ESP12-E com módulo, teria de usar a SoftwareSerial como discutimos antes).

Adafruit_MPU6050 mpu;
MAX30105 maxsensor;
BluetoothSerial SerialBT; // O objeto do bluetooth

// Variáveis da Queda
bool quedapossivel = false;
unsigned long timeincioqueda = 0, ultimoenvioSerial = 0;
const unsigned long timermaxqueda = 15000; // Voltamos para 15s como combinado
const unsigned long intervaloSerial = 1000;
const float limitegiro = 3.5; // Ajuste para 3.5 para evitar falsos positivos de inclinação natural

// Para o maxsensor
const byte leituras = 4; // Quantas leituras para fazer a média.
byte rates[leituras]; // Vetor para guardar as últimas leituras
byte organiza_rates = 0; // Vai guarda a pos onde o rates[leituras] está
long Ultimabatida = 0; // Guarda a ultima batida do heart
float bpm;
int bpm_medio = 0; // a media do batimento

void setup(){
    // Começando do começo
    Serial.begin(115200);
    pinMode(buzzer, OUTPUT);
    digitalWrite(buzzer, LOW);

    // Parte do bluetooth
    if(!SerialBT.begin("Bengala_Inteligente")){
        Serial.println("Falha ao conectar o bluetooth");
        while(1){
            delay(10);
        }
    }
    Serial.println("Bluetooth conectado!"); 

    // conectando mpu e o max no código
    Wire.begin(I2C_SDA, I2C_SCL);
    Serial.println("Inicializando sensores");
    
    // Incializando MPU
    if (!mpu.begin()){ // esse '!' significa q n conecto
        Serial.println("Falha ao incializar o mpu!");
        while(1){ // essa parte serve para travar o código e impedir que rode com o erro
            delay(10); 
        }
    }
    Serial.println("mpu connected!");
    mpu.setAccelerometerRange(MPU6050_RANGE_4_G); // Aceleraçao máxima
    mpu.setGyroRange(MPU6050_RANGE_500_DEG); // Decta giros rápidos
    mpu.setFilterBandwidth(MPU6050_BAND_21_HZ); // Impedir que ruídos atrapalhem
    
    // incializar o max agora (FALTAVA O WIRE E A VELOCIDADE AQUI)
     if(!maxsensor.begin(Wire, 100000)){
        Serial.println("Falha no max!");
        while(1){
            delay(10);
        }
    }
    Serial.println("Max conectado!");
    maxsensor.setup(); // inicializa o chip e aplica config padrão
    maxsensor.setPulseAmplitudeRed(0x0A); // Define corrente led vermelho
    maxsensor.setPulseAmplitudeIR(0x0A); // Define Led infravermelho 
}

void enviarDadosBluetooth() {
    String json = "{";
    
    // 1. Frequência Cardíaca (ATUALIZADO PARA bpm_medio)
    json += "\"frequencia_cardiaca\": " + String(bpm_medio) + ", ";
    
    // 2. Se a bengala caiu
    if (quedapossivel) {
        json += "\"bengala_caiu\": true";
    } else {
        json += "\"bengala_caiu\": false";
    }
    
    json += "}"; // Fecha o JSON

    // Imprime no Monitor Serial para testar
    Serial.println(json);
    
    // O que vai para o telemóvel/site
    SerialBT.println(json);
}

void loop(){
    // Creating milis()
    unsigned long tempoatual = millis();

    // Criando um espaço para alocar os dados do MPU:
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);
    
    //Checar se caiu
    //verificarQueda(a, g, tempoatual);
    
    //Verificar frequencia cardiaca
    //verificarBatimentos(tempoatual);

    // Envia os dados JSON via USB e Bluetooth a cada 1 segundo
    /*if(tempoatual - ultimoenvioSerial >= intervaloSerial){
        ultimoenvioSerial = tempoatual;
        enviarDadosBluetooth();
    }*/

     if(tempoatual-ultimoenvioSerial>=intervaloSerial){
        ultimoenvioSerial=tempoatual;
        Serial.println("Leitura do infravermelho");
        Serial.println( maxsensor.getIR());
        Serial.println("--------------------------------");
        Serial.println("EIXO X:");
        Serial.println(a.acceleration.x);
        Serial.println(g.gyro.x);
        Serial.println("--------------------------------");
        Serial.println("EIXO Y:");
        Serial.println(a.acceleration.y);
        Serial.println(g.gyro.y);
        Serial.println("--------------------------------");
        Serial.println("EIXO Z:");
        Serial.println(a.acceleration.z);
        Serial.println(g.gyro.z); 
}
