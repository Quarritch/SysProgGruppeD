// Eine C-Skript, welches die Infos aus den Prozess in eine Log-Datei schreibt und bei Bedarf auslesen kann
#ifndef LOGWERT_H
#define LOGWERT_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>



//Funktion zum Schreiben von Logs in eine Datei
void write2log(float wert, char *pfadLog){

	FILE *file = fopen(pfadLog,"a"); 	//"a" für "append", um vorhandene Datei zu öffnen und Text anzuhängen oder neue zu erstellen, der Pfadlog entspricht dem Pfad mit dem Dateinamen ../logs/dateiname.log
	if (file == NULL){			//Errormeldung, wenn Dateiöffnen nicht funktioniert hat
		perror("Fehler beim Öffnen der Log-Datei");
		exit(EXIT_FAILURE);
	}

	//Zeitstempel hinzufügen
	time_t t;			
	struct tm *tm_info;

	time(&t);
	tm_info = localtime(&t);

	//Schreiben von Datum und Wert
	fprintf(file, "[%04d-%02d-%02d %02d:%02d:%02d] %f \n",
		tm_info->tm_year + 1900, tm_info->tm_mon + 1, tm_info->tm_mday,
		tm_info->tm_hour, tm_info->tm_min, tm_info->tm_sec, wert);

	fclose(file);	//Schließen der Datei
}



//Funktion zum Lesen aus einer Logdatei (nur den Wert)
float checkWert(char *pfadLog){
	FILE *file = fopen(pfadLog, "r")		//"r" für "read", um vorhandene Datei zu öffnen und zu lesen, der Pfadlog entspricht dem Pfad mit dem Dateinamen ../logs/dateiname.log
	if (file == NULL) {				//Errormeldung, wenn Dateiöffnen nicht funktioniert hat
		perror("Fehler beim Öffnen der Datei");
	        return 1;
	}

	char line[100]; //Annahme: Maximal 100 Zeichen pro Zeile
    	float wert;	//Deklaration Wert
    	
   	while (fgets(line, sizeof(line), file) != NULL) {	//Lese die letzte Zeile
		const char *wertStr = line + 22; 	//Springe über die ersten 22 Zeichen (Datum)
		sscanf(wertStr, "%f", &wert);		//Wert lesen
        }
    	
    	fclose(file); 	//Schließen der Datei
    	return wert;

}

float logWert(char *name, char *arg, float value){

	float ruckwert;					//Rückgabewert
	char pfadLog[100] = "./logs/werte/";		//Pfad
	char dateiname[100];				//Dateiname
	char action[100];				//Aktion
	float wert = 0.0;				//Deklaration Wert zum Schreiben

	strcpy(dateiname, name);			//Übertragen des Inputs
	strcpy(action, arg);				//Übertragen des Inputs
	wert = value;


	strcat(dateiname,".log"); 	//setzt Dateiname und .log zusammen. z.B. input.log
	strcat(pfadLog,dateiname); 	//setzt zum Pfad die Logdatei hinzu. z.B. ../logs/werte/input.log

	if(strcmp(action,"read")==0)		//Abfrage, ob Aktion "read" ist
	{
		ruckwert=checkWert(pfadLog);		//Aus Datei lesen
		return ruckwert;		//Rückgabewert an Aufruf der Funktion
	}

	else if(strcmp(action,"write")==0)	//Abfrage, ob Aktion "write" ist
	{

		if(strcmp(dateiname,"sensor.log")==0||(strcmp(dateiname,"control.log")==0||strcmp(dateiname,"motor.log")==0))		//Abfrage, ob dateiname "sensor.log", "control.log", "motor.log" ist
		{

			FILE *file = fopen(pfadLog,"r");		//"r" für "read", um vorhandene Datei zu öffnen und zu lesen, der Pfadlog entspricht dem Pfad mit dem Dateinamen ../logs/dateiname.log
			if(file == NULL){
				file = fopen(pfadLog, "w");		//"w" für "write", um in vorhanden Datei zu schreiben, der Pfadlog entspricht dem Pfad mit dem Dateinamen ../logs/dateiname.log

				if(file == NULL){			
					perror("Fehler beim Erstellen der Log-Datei");
					exit(EXIT_FAILURE);
				}

				if(strcmp(dateiname, "sensor.log")==0)	//Abfrage, ob Dateiname "sensor.log"
				{
					fprintf(file,"Datum\t\t      Temperatur in °C\n");	//Schreiben eines Headers
				}

				if(strcmp(dateiname, "control.log")==0)		
				{
					fprintf(file,"Datum\t\t      Strom in A");	
				}

				if(strcmp(dateiname, "motor.log")==0)		
				{
					fprintf(file,"Datum\t\t      Kühlung die Std in °C/h");		
				}

				fclose(file);		//Schließen der Datei
			}


		write2log(wert, pfadLog);	//Aufruf zum Schreiben von Funktionswerten in pfadLog
		return 0;			
		}
		else
		{
			perror("Dateiname falsch");	//Errormeldung, wenn falsche Dateinamen angegeben wurden
			exit(EXIT_FAILURE);
		}
	}
	else
	{
		perror("Action falsch");		//Errormeldung, wenn falsche Aktion angegeben wurde
		exit(EXIT_FAILURE);
	}
}
#endif
