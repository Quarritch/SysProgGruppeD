// Eine C-Skript, welches die Infos aus den Prozess in eine Log-Datei schreibt
#ifndef LOGWERT_H
#define LOGWERT_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>


void write2log(float wert, char *pfadLog){

	FILE *file = fopen(pfadLog,"a"); // "a" für "append", um vorhandene Datei zu öffnen oder neue zu erstellen
	if (file == NULL){
		perror("Fehler beim Öffnen der Log-Datei");
		exit(EXIT_FAILURE);
	}

	//Zeitstempel hinzufügen
	time_t t;
	struct tm *tm_info;

	time(&t);
	tm_info = localtime(&t);

	fprintf(file, "[%04d-%02d-%02d %02d:%02d:%02d] %f \n",
		tm_info->tm_year + 1900, tm_info->tm_mon + 1, tm_info->tm_mday,
		tm_info->tm_hour, tm_info->tm_min, tm_info->tm_sec, wert);

	fclose(file);
}

float checkWert(char *pfadLog){
	FILE *file = fopen(pfadLog, "r");
	if (file == NULL) {
		perror("Fehler beim Öffnen der Datei");
	        return 1;
	}

	char line[100]; // Annahme: Maximal 100 Zeichen pro Zeile
    	float wert;
    	// Lese die letzte Zeile
   	while (fgets(line, sizeof(line), file) != NULL) {
		// Extrahiere den Wert (Annahme: Das Datum hat eine feste Länge von 22 Zeichen)
		const char *wertStr = line + 22; // Springe über die ersten 22 Zeichen (Datum)
		sscanf(wertStr, "%f", &wert);
        	// Verarbeite den Wert
	}
    	// Schließe die Datei
    	fclose(file);
    	return wert;

}

float logWert(char *name, char *arg, float value){

	float ruckwert;
	char pfadLog[100] = "./logs/werte/";
	char dateiname[100];
	char action[100];
	float wert = 0.0;

	strcpy(dateiname, name);
	strcpy(action, arg);
	wert = value;


	strcat(dateiname,".log"); //setzt Dateiname und .log zusammen. z.B. input.log
	strcat(pfadLog,dateiname); //setzt zum Pfad die Logdatei hinzu. z.B. ./log/input.log

	if(strcmp(action,"read")==0)
	{
		ruckwert=checkWert(pfadLog);
		return ruckwert;
	}

	else if(strcmp(action,"write")==0)
	{

		if(strcmp(dateiname,"sensor.log")==0||(strcmp(dateiname,"control.log")==0||strcmp(dateiname,"motor.log")==0))
		{

			FILE *file = fopen(pfadLog,"r");
			if(file == NULL){
				file = fopen(pfadLog, "w");

				if(file == NULL){
					perror("Fehler beim Erstellen der Log-Datei");
					exit(EXIT_FAILURE);
				}

				if(strcmp(dateiname, "sensor.log")==0)
				{
					fprintf(file,"Datum\t\t      Temperatur in °C\n");
				}

				if(strcmp(dateiname, "control.log")==0)
				{
					fprintf(file,"Datum\t\t      Strom in A");
				}

				if(strcmp(dateiname, "motor.log")==0)
				{
					fprintf(file,"Datum\t\t      Kühlung die Std in °C/h");
				}

				fclose(file);
			}


		write2log(wert, pfadLog);
		printf("Log-Einträge wurden erstellt und in %s gespeichert.\n",pfadLog);
		return 0;
		}
		else
		{
			perror("Dateiname falsch");
			exit(EXIT_FAILURE);
		}
	}
	else
	{
		perror("Action falsch");
		exit(EXIT_FAILURE);
	}
}
#endif
