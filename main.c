#include <stdio.h>
#include <stdlib.h>
#include "sqlite3.h"
#include "graph.h"
#include <string.h>
#define MAX 100
sqlite3 *db;

//funzione per vedere se esiste la tabella e quindi il database
int tableExists() {
    sqlite3_stmt *stmt;
    char *sql;
    sql = "SELECT count(*) FROM sqlite_master WHERE type='table' AND name='AEREOPORTI';";
    int rc;
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if(rc != SQLITE_OK) {
        printf("Exit");
        sqlite3_close(db);
        exit(-1);
    }
    int TablePresent = 0;
    while ( (rc = sqlite3_step(stmt)) == SQLITE_ROW){
        if (sqlite3_column_int(stmt, 0) >= 1) {
            TablePresent = 1;
            break;
        }
    }
    sqlite3_finalize(stmt);
    return TablePresent;
}

//funzione per inizializzare le strutture dati
/*void initStruct() {
    sqlite3_stmt *stmt;
    int rc;
    int index = 0;

    sqlite3_prepare_v2(db, "select * from LIBRI ORDER BY ID ASC;", -1, &stmt, NULL);
    while((rc = sqlite3_step(stmt)) == SQLITE_ROW)
        inserisciCoda(L, (char*)sqlite3_column_text(stmt, 1), (char *)sqlite3_column_text(stmt, 2), sqlite3_column_int(stmt, 0), sqlite3_column_int(stmt, 3), 1, -1);
    sqlite3_finalize(stmt);


    sqlite3_prepare_v2(db, "select COUNT(ID) from RICHIESTE GROUP BY ID;", -1, &stmt, NULL);
    while((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        index++;
    }
    sqlite3_finalize(stmt);
    if (index > 0) {
        if (P == NULL) {
            printf("ERRORE: impossibile allocare memoria per il grafo\n");
            sqlite3_close(db);
            exit(-1);
        } else {
            P->q = (Queue **) malloc(index * sizeof(Queue *));
            if ((P->q == NULL) && (index > 0)) {
                printf("ERRORE: impossibile allocare memoria per la lista del grafo\n");
                free(P);
                P = NULL;
            } else {
                P->n = index;
                for (int i = 0; i < index; i++)
                    P->q[i] = NULL;
            }
        }
    } else {
        P->n = index;
        P->q = NULL;
    }

    index = -1;
    int libro = -1;
    char matricola[10];

    sqlite3_prepare_v2(db, "select * from RICHIESTE AS R INNER JOIN LIBRI AS L ON R.ID = L.ID ORDER BY R.ID, R.NRICHIESTA ASC;", -1, &stmt, NULL);
    while((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        strcpy(matricola, sqlite3_column_text(stmt, 1));
        if (libro != -1 && libro == sqlite3_column_int(stmt, 2)) {
            Enqueue(P, index, matricola, 0);
        }
        else{
            if (L) {
                index++;
                libro = sqlite3_column_int(stmt, 2);
                ModificaIndice(L, libro, index);
                Enqueue(P, index, matricola, 0);
            }
        }
    }
    sqlite3_finalize(stmt);
}
*/
//funzione che crea il database se non trova il file .db
void creaDatabase(){
    char *zErrMsg = 0;
    int rc;
    char *sql;

    sql = "SELECT count(*) FROM sqlite_master WHERE type='table' AND name='LIBRI';";
    rc = sqlite3_open("Aereoporto.db", &db);
    if(rc) {
        printf("Impossibile aprire il database: %s\n", sqlite3_errmsg(db));
        exit(-1);
    }
    rc = sqlite3_exec(db, sql, 0, 0, &zErrMsg);
    if( rc != SQLITE_OK ){
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        sqlite3_close(db);
        exit(-1);
    } else {
        if (tableExists() == 1) {
            //initStruct(Lista,  P);
        }
        else {
            //creazione tabelle
            //sql = "CREATE TABLE IF NOT EXISTS UTENTI( CF TEXT PRIMARY KEY NOT NULL, PASSWORD TEXT NOT NULL,NOME TEXT NOT NULL, COGNOME TEXT NOT NULL, TIPO TEXT NOT NULL);"
                  "CREATE TABLE IF NOT EXISTS AEREOPORTO( CODAEREO TEXT PRIMARY KEY, CITTA TEXT);"
                  "CREATE TABLE IF NOT EXISTS TRATTE( TRATTA INT PRIMARY KEY AUTOINCREMENT, AEREOPART TEXT NOT NULL, AEREODEST TEXT NOT NULL, KM INT NOT NULL;"
                  "CREATE TABLE IF NOT EXISTS PRENOTATO(NPRENOTAZIONE INTEGER PRIMARY KEY AUTOINCREMENT, USERNAME TEXT NOT NULL, AEREOPART TEXT NOT NULL, AEREODEST TEXT NOT NULL) ;";
            rc = sqlite3_exec(db, sql, 0, 0, &zErrMsg);
            if (rc != SQLITE_OK) {
                printf("SQL error: %s\n", zErrMsg);
                sqlite3_free(zErrMsg);
                sqlite3_close(db);
                exit(-1);
            } else {
                printf("Tabella creata con successo!\n");
            }
        }
    }
}

int main() {
    char username[MAX];
    char nome[MAX], cognome[MAX];
    char password[33];
    int scelta = 1;
    printf("***************************************************************\nBENVENUTO IN AIRITALY\n\n");
    do {
        if ((scelta > 0) && (scelta < 3)) {
            printf("Scegli una delle seguenti funzioni, inserendo il numero della funzione:\n");
            printf("1. Registra un nuovo utente\n");
            printf("2. Effettuare il Login.\n");
            printf("3. Uscire dal programma.\n");
        } else {
            printf("Riprova! Hai inserito un numero non valido.\n");
        }
        scanf("%d", &scelta);
        getchar();
        switch (scelta) {
            case 1:
                printf("Inserisci il tuo nome:\n");
                gets(nome);

                printf("Inserisci il tuo cognome:\n");
                gets(cognome);

                printf("Scegli il tuo username:\n");
                gets(username);

                printf("Scegli una password(da 4 a 32 caratteri):\n");
                do {
                    gets(password);
                    if(strlen(password)<4 || strlen(password)>32)
                        printf("La password non rispetta la lunghezza!\nReinserisci una password che rispetta la lunghezza:\n");
                }while (strlen(password)<4 || strlen(password)>32);
                printf("URrra\n");
                //salvataggio nel db
                break;
            case 2:
                //Effettuare il login e controllo se utente o amministratore
                // Utente:
                // poter scegliere e visualizzare i voli
                //Admin:
                //poter inserire o rimuovere tratte o voli con scali
                break;

        }
    }while (scelta != 3);
    //saveToDatabase(&Lista, P);
    sqlite3_close(db);
}