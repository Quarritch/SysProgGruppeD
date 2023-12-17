#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include "logWert.h"

#define SHM_KEY 1100
#define SHM_SIZE 1024

//Temperaturschwellwerte für die Motorsteuerung
const float room_temp = 21.0;
const float temp_start_cooling = 23.0;
const float temp_increase_cooling = 25.0;
const float temp_stop_cooling = 21.0;

int main() {
    int shmid;
    key_t key = SHM_KEY;
    float *shm, *data;
    int nodecnt = 2;

    //Dateinamen zum kontrollierten Beenden der Prozesse
    const char *filename = "./logs/finish";
    const char *filefinish = "./logs/ctrlfinish";

    //Pointer für die shared memories erstellen
    float *ptrs[nodecnt];

    // Erzeugt einen Shared Memory-Schlüssel
    for(int i=0; i < nodecnt; i++)
    {
        shmid = shmget(key, SHM_SIZE, IPC_CREAT | 0666);
        if (shmid == -1) {
            perror("shmget");
            exit(1);
        }
        
        // Verbindung zum vorhandenen Shared Memory herstellen
        shmid = shmget(key, SHM_SIZE, 0666);
        if (shmid == -1) {
            perror("shmget");
            exit(1);
        }

        key++;
        // Befestigt den Shared Memory
        ptrs[i] = (float *)shmat(shmid, NULL, 0);
    }

    //Variablen zur Steuerung
    bool cooling_on = false;
    float current = 0.0;
    
    while(1)
    {
        sleep(1);

        //Temperatur aus zweitem shared Memory lesen 
        float temp = *ptrs[1];
        if(temp == -1.0) break;

        //Wenn der Motor noch nicht läuft, dann warten, bis der erste Schwellwert überschritten ist
        if(cooling_on == false)
        {
            if (temp >= temp_start_cooling)
            {
                current = temp*0.1;
                cooling_on = true;
            }
        }
        else
        {
            //Bei Überschreitung des zweiten Schwellwerts den Strom erhöhen
            if(temp >= temp_increase_cooling)
            {
                current = temp*0.2;
            }
            else if(temp <= temp_stop_cooling)
            {
                //Bei Unterschreiten unteren Temperatur den Motor ausschalten
                current = 0.0;
                cooling_on = false;
            }
        }

        //aktuellen Stromwert in die Logdatei schreiben
        logWert("control", "write", current);

        // Schreibt Daten in den Shared Memory
        *ptrs[0] = current;
        
        // Versuche, die Datei zu öffnen
        FILE *file = fopen(filename, "r");

        // Überprüfe, ob das Öffnen erfolgreich war
        if (file != NULL) {
            // Schließe die Datei nach der Überprüfung
            fclose(file);
            break;
        }
    }

    //Datei für das Stoppen der anderen Prozesse erstellen
    FILE *newfile = fopen(filefinish, "w");

    fclose(newfile);

    sleep(2);

    key=SHM_KEY;

    for(int i=0; i < nodecnt; i++)
    {
        // Trennt alle Shared Memory
        shmdt(ptrs[i]);
        shmid = shmget(key, SHM_SIZE, 0666);
            if (shmid == -1) {
                perror("shmget");
                exit(1);
            }
        //Entfernt alle Shared Memories
        shmctl(shmid, IPC_RMID, 0);
        key++;
    }

    return 0;
}
