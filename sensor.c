#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <math.h>
#include <time.h>

#define SHM_KEY 1101
#define SHM_SIZE 1024

float impGen() {

    int coeff = 20;
    float amplitudenvarianz;
    float rechteck;

    // Startzeitpunkt
    time_t startzeit = time(NULL);
    while (1)
    {
        if (time(NULL) - startzeit >= 5) {

            amplitudenvarianz = ((float)rand() / RAND_MAX) * 6;
            rechteck = amplitudenvarianz + coeff;
                            
            printf ("%f\n", rechteck);
                            
            startzeit = time(NULL);
            return rechteck;
        }
    }
}

int cnt = 0;

float sineGen(){

    float coeff = 23;
    int dauer = 100;
    float amplitude = 4;
    float frequenz = 2;

    time_t startzeit = time(NULL);

    while (1)
    {
        if (time(NULL) - startzeit >=1) {

            float t = (float)cnt / dauer;
            float sinus = coeff + amplitude * sin(2.0 * M_PI * frequenz * t);
            cnt++;

            if(cnt > dauer) {
                cnt = 0;
            }

            printf ("%f\n", sinus);

            return sinus;
        }
    }
    
}

int main(int argc, char *argv[]) {
    int shmid;
    key_t key = SHM_KEY;
    float *shm, *data;

    // Verbindung zum vorhandenen Shared Memory herstellen
    shmid = shmget(key, SHM_SIZE, 0666);
    if (shmid == -1) {
        perror("shmget");
        exit(1);
    }

    // Befestigt den Shared Memory
    shm = (float *)shmat(shmid, NULL, 0);

    printf("%d\n",key);

    while(1)
    {
        float temp;
        
        if(!strcmp(argv[1], "sinus"))
        {
            temp = sineGen();
        }
        else if(!strcmp(argv[1], "rechteck"))
        {
            temp = impGen();
        }
        else
        {
            perror("Argumente fehlerhaft!");
            return 2;
        }

        if(temp == -1.0) break;
            
        // Schreibt Daten in den Shared Memory
        *shm = temp;   
    }
    // Trennt den Shared Memory
    shmdt(shm);

    return 0;
}
