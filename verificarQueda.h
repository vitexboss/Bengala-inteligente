#ifndef VERIFICAR_QUEDA_H
#define VERIFICAR_QUEDA_H

#include "Adafruit_Sensor.h"
#include <Arduino.h> 
#include "Adafruit_MPU6050.h"

extern bool quedapossivel;
extern unsigned long timeincioqueda;
extern const unsigned long timermaxqueda;
extern const float limitegiro;

// Proteção para o ficheiro reconhecer qual é o pino do buzzer
#ifndef buzzer
#define buzzer 4 //Que muda dependendo de onde ele vai estar
#endif

// Função para a parte da queda em si
void verificarQueda(sensors_event_t &a, sensors_event_t &g, unsigned long tempoatual){
    // Passo 1: Calcular a rotação e verificar a pos! 
    // Calcular a velocidade angular(rad/s) usando hipotenusa 3d
    float giroTotal = sqrt(pow(g.gyro.x, 2) + pow(g.gyro.y, 2) + pow(g.gyro.z, 2));
    
    // Deixar uma pos fixa para deitada;
    bool dorminhoca = (abs(a.acceleration.y) < 1);

    // Passo 2: Detectar se caiu e iniciar contagem caso TRUE;
    if(giroTotal >= limitegiro && dorminhoca && !quedapossivel){ 
        quedapossivel = true;
        timeincioqueda = tempoatual;
        Serial.println("Possivel Queda identificada, iniciando contagem regressiva...");
    }
    
    // Passo 3: Enquanto ela estiver caída;
    if(quedapossivel){
        if(dorminhoca){ // Continua caida?
            digitalWrite(buzzer, HIGH);

             if(tempoatual - timeincioqueda >= timermaxqueda){
                // Aqui sera para enviar para o bluetooth a queda
                Serial.println("15s passados, enviando sinal");
            }
        } else {
            // Bengala voltou para posição correta e reseta tudo!
            quedapossivel = false;
            digitalWrite(buzzer, LOW);
            Serial.println("Bengala de volta ao normal, timer resetado");
        }  
    }
} // <--- AQUI ESTÁ A CHAVETA QUE FALTAVA PARA FECHAR A FUNÇÃO!

#endif // VERIFICAR_QUEDA_H
