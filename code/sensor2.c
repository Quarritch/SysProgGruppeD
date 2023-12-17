#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <math.h>
#include <time.h>
#include "./programme/logWert.h"        //Einbinden der Bibliothek "logWert.h"

#define SHM_KEY 1101            //Schlüssel der Shared Memory
#define SHM_SIZE 1024           //Größe der Shared Memory


//Funktion zur Erzeugung einer Rechteckfunktion alle 5 Sekunden
float impGen() {        

    int coeff = 20;             //Grundtemperatur
    float amplitudenvarianz;    //Variation der Amplitude
    float rechteck;             //Ausgabe

    // Startzeitpunkt
    time_t startzeit = time(NULL);
    while (1)
    {
        if (time(NULL) - startzeit >= 5) {      //Alle 5 Sekunden

            amplitudenvarianz = ((float)rand() / RAND_MAX) * 6;     //Zufälliger Floatwert zwischen 0 und 6
            rechteck = amplitudenvarianz + coeff;       //Temperaturwert
                            
            startzeit = time(NULL);             //Reset Startzeit
            return rechteck;
        }
    }
}

int cnt = 0;            //Iterationen unendlich

//Funktion zur Erzeugung eines Sinussignals
float sineGen() {    

    float coeff = 23;           //Basistemperatur
    int dauer = 100;            //Zeitdauer
    float amplitude = 4;        //Amplitude
    float frequenz = 2;         //Frequenz

    time_t startzeit = time(NULL);

    while (1)
    {
        if (time(NULL) - startzeit >=1) {          //Alle 1 Sekunden

            float t = (float)cnt / dauer;               //Zeit t generieren mit Laufvariabalen
            float sinus = coeff + amplitude * sin(2.0 * M_PI * frequenz * t);   //Erzeugung Sinus -> 20°C+-sinus
            cnt++;          

            if(cnt > dauer) {         //Zeitdauer zurücksetzen
                cnt = 0;
            }
            return sinus;
        }
    }
    
}

int main(int argc, char *argv[]) {
    int shmid;                 //Rückgabe Shared Memory -> 0 bedeutet vorhanden
    key_t key = SHM_KEY;       //Schlüssel des Shared Memory Segments
    float *shm, *data;         //Wert in der Shared Memory

    shmid = shmget(key, SHM_SIZE, 0666);        // Verbindung zum vorhandenen Shared Memory herstellen (alle Rechte)        
    if (shmid == -1) {              //Errormeldung, Shared Memory nicht erzeugt
        perror("sensor: shmget");
        exit(1);
    }

    shm = (float *)shmat(shmid, NULL, 0);   //Befestigt den Shared Memory

    while(1)
    {
        float temp;                 //Temperatur
        
        if(!strcmp(argv[1], "sinus"))     //Auswahl Sinusfunktion
        {
            temp = sineGen();
        }
        else if(!strcmp(argv[1], "rechteck"))       //Auswahl Rechteckfunktion
        {
            temp = impGen();
        }
        else
        {
            perror("Argumente fehlerhaft!");        //Errormeldung Falsche Argumente
            return 2;
        }

        if(temp == -1.0) break;       //Aus unendliche Schleife raus
            
        
        *shm = temp;          //Schreiben der Temperatur in den Shared Memory

        logWert("sensor", "write", temp);           //Loggen der Temperatur
    }
    
    shmdt(shm);         //Trennt den Shared Memory

    return 0;
}
