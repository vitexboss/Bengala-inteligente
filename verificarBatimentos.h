#ifndef VERIFICAR_BATIMENTOS_H
#define VERIFICAR_BATIMENTOS_H

#include "MAX30105.h"
#include "heartRate.h"
#include <Arduino.h>
//Para o maxsensor
extern MAX30105 maxsensor;
extern const byte leituras; //Quantas leituras para fazer a média. -- Byte quarda valores de 0 a 255 positivos, ocupa menos espaço
extern byte rates[leituras];//Vetor para guardar as últimas leituras
extern byte organiza_rates;//Vai guarda a pos onde o rates[leituras] está para organização;
extern long Ultimabatida;//Guarda a ultima batida do heart
extern float bpm;
extern int bpm_medio; //a media do batimento


//Outra função para os bpm's
void verificarBatimentos(unsigned long tempoatual){
    //1. Leitura do sensor  
   long int valorIR = maxsensor.getIR();
    //2. Realiza cálculo apenas se tiver um dedo ali
    if(valorIR> 50000){
        if(checkForBeat(valorIR) == true){
            long delta= millis() - Ultimabatida;
            Ultimabatida=millis();

            bpm=60/(delta/1000.0);

            if (bpm < 255 && bpm > 20){
                //Organizando as leiturinhas
                rates[organiza_rates++] = (byte)bpm;
                organiza_rates%=leituras;

                bpm_medio=0;
                for (byte i=0; i<leituras; i++){
                    bpm_medio+=rates[i];
                }
                bpm_medio/=leituras;
            }
        }

        //Detectar dedo
        if(tempoatual - ultimoenvioSerial >= intervaloSerial){
            ultimoenvioSerial = tempoatual; // Note que isto vai usar o mesmo timer da queda
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
