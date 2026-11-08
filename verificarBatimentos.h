#ifndef VERIFICAR_BATIMENTOS_H
#define VERIFICAR_BATIMENTOS_H

#include "MAX30105.h"
#include "heartRate.h"
#include <Arduino.h>

// Para o maxsensor
extern MAX30105 maxsensor;
extern const byte leituras; 
extern byte rates[]; // Deixamos vazio aqui! O C++ descobre o tamanho no ficheiro principal
extern byte organiza_rates;
extern long Ultimabatida;
extern float bpm;
extern int bpm_medio;

// FALTAVA ISTO: Declarar as variáveis do temporizador usadas no Serial.print
extern unsigned long ultimoenvioSerial;
extern const unsigned long intervaloSerial;

// Outra função para os bpm's
void verificarBatimentos(unsigned long tempoatual){
    // 1. Leitura do sensor  
    long int valorIR = maxsensor.getIR();
    
    // 2. Realiza cálculo apenas se tiver um dedo ali
    if(valorIR > 50000){
        if(checkForBeat(valorIR) == true){
            long delta = millis() - Ultimabatida;
            Ultimabatida = millis();

            bpm = 60 / (delta / 1000.0);

            if (bpm < 255 && bpm > 20){
                // Organizando as leiturinhas
                rates[organiza_rates++] = (byte)bpm;
                organiza_rates %= leituras;

                bpm_medio = 0;
                for (byte i = 0; i < leituras; i++){
                    bpm_medio += rates[i];
                }
                bpm_medio /= leituras;
            }
        }

        // Detectar dedo sem travar tudo
        if(tempoatual - ultimoenvioSerial >= intervaloSerial){
            ultimoenvioSerial = tempoatual;
            Serial.print("Dedo detetado! | BPM Atual: ");
            Serial.println(bpm);
            Serial.print(" | BPM Médio: ");
            Serial.println(bpm_medio);
        }
        
    } else {
        if(tempoatual - ultimoenvioSerial >= intervaloSerial){
            ultimoenvioSerial = tempoatual;
            Serial.println("Sem leitura do coração...");
            bpm_medio = 0; 
        }
    }
}

#endif // VERIFICAR_BATIMENTOS_H
