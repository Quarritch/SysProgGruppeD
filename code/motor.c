#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>
#include "programme/logWert.h"

#define SHM_KEY 1100
#define SHM_SIZE 1024

int main() {
    int shmid;
    key_t key = SHM_KEY;
    float *shm;

    // Verbindung zum vorhandenen Shared Memory herstellen
    shmid = shmget(key, SHM_SIZE, 0666);
    if (shmid == -1) {
        perror("shmget");
        exit(1);
    }
    // Befestigt den Shared Memory
    shm = (float *)shmat(shmid, NULL, 0);

    // Öffnet eine Pipe zum GNU Plot-Prozess und zu einer Zwischenspeicher Datei
    FILE *gnuplotPipe = NULL,
         *data_TXT = NULL;

    //öffnet GNU plot und Zwischenspeicher
    data_TXT = fopen( "data.txt", "w");
    gnuplotPipe = popen("gnuplot -persist", "w");

    //Fehler bei Erstellen des GNU Plots
    if(gnuplotPipe ==NULL)
    {
        printf("Failed to open Gnupipe!\n");
    }

    //Einstellungen für GNU Plot vornehmen: 
    fprintf(gnuplotPipe, "set term qt persist\n");
    fprintf(gnuplotPipe, "set title \"Cooling rate over time\"\n");   

    //Variable für die X-Achse (Sekunden)
    int counter = 0;

    while(1)
    {
        sleep(1);

        //Kühlrate berechnen
        float cooling_rate = *shm*0.8;

        //Kühlrate in die Logdatei schreiben
        logWert("motor", "write", cooling_rate);

        //Zu plottende Daten (X-Achse=Zeit, Y-Achse=Wert) in den Zwischenspeicher schreiben
        fprintf(data_TXT, "%d %f\n", counter, *shm);
        //Zwischenspeicher flushen, damit nur die neusten Werte da sind
        fflush(data_TXT);

        // Lese den Shared Memory-Wert und schreibe ihn in die Pipe
        fprintf(gnuplotPipe, "plot 'data.txt' using 1:2 with lines lw 8\n");

        // Flushe die Pipe, um die Daten sofort zu zeichnen
        fflush(gnuplotPipe);

        //Sekunde eins hoch zählen
        counter ++;
        
    }

    // Trennt den Shared Memory
    shmdt(shm); 

    //schließt den Zwischenspeicher
    fclose(data_TXT);

    //schließt die GNU Pipe
    fprintf(gnuplotPipe, "e\n");
    fclose(gnuplotPipe);

    return 0;
}
