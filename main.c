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
            sql = "CREATE TABLE IF NOT EXISTS UTENTI(USERNAME TEXT PRIMARY KEY, PASSWORD TEXT NOT NULL,NOME TEXT NOT NULL, COGNOME TEXT NOT NULL, TIPO TEXT NOT NULL, PUNTI INT NOT NULL);"
                  "CREATE TABLE IF NOT EXISTS AEREOPORTO(NAEROPO INTEGER PRIMARY KEY AUTOINCREMENT, CODAERO TEXT NOT NULL, CITTA TEXT);"
                  "CREATE TABLE IF NOT EXISTS TRATTE(TRATTA INTEGER PRIMARY KEY AUTOINCREMENT, AEROPART TEXT NOT NULL, AERODEST TEXT NOT NULL, KM INT NOT NULL);"
                  "CREATE TABLE IF NOT EXISTS PRENOTATO(NPRENOTAZIONE INTEGER PRIMARY KEY AUTOINCREMENT, USERNAME TEXT NOT NULL, AEROPART TEXT NOT NULL, AERODEST TEXT NOT NULL) ;";
            rc = sqlite3_exec(db, sql, 0, 0, &zErrMsg);
            if (rc != SQLITE_OK) {
                printf("SQL error: %s\n", zErrMsg);
                sqlite3_free(zErrMsg);
                sqlite3_close(db);
                exit(-1);
            }
        }
    }
}

void EffettuaRegistrazione(char username[], char nome[], char cognome[], char password[]){
    char *sql;
    int rc;
    sqlite3_stmt *stmt;
    sql = "INSERT INTO UTENTI(USERNAME, PASSWORD ,NOME , COGNOME, TIPO) VALUES(?1, ?2, ?3, ?4, 'Utente', 0);";
    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, password, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, nome, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, cognome, -1, SQLITE_STATIC);
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        printf("Exit");
        sqlite3_close(db);
        exit(-1);
    }
    sqlite3_finalize(stmt);
    printf("Registrazione effettuata correttamente!\n");
}

int EffettuaAccesso(char username[], char password[]){
    char *sql;
    int rc;
    sqlite3_stmt *stmt;
    sql = "SELECT TIPO FROM UTENTI WHERE USERNAME = ?1 AND PASSWORD = ?2;";
    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, password, -1, SQLITE_STATIC);
    rc = sqlite3_step(stmt);
    if(rc == SQLITE_ROW) {
        if(strcmp(sqlite3_column_text(stmt, 0),"Utente") == 0)
            return 1;
        else if(strcmp(sqlite3_column_text(stmt, 0),"Admin") == 0)
            return 2;
    }
    return 0;
}

int main() {
    char username[MAX];
    char nome[MAX], cognome[MAX];
    char password[33];
    int scelta = 1;

    creaDatabase();
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
                EffettuaRegistrazione(username, nome, cognome, password);
                //salvataggio nel db
                break;
            case 2:
                do{
                    //Effettuare il login e controllo se utente o amministratore
                    printf("Inserisci username:\n");
                    gets(username);
                    printf("Inserisci password:\n");
                    gets(password);
                    if(EffettuaAccesso(username, password) == 0){
                        printf("username/password errate!Riprova.\n");
                    } else if(EffettuaAccesso(username, password) == 1) {
                        int sceltaUtente = 1;
                        printf("%s ha effettuato correttamente l'accesso!\n", username);
                        //poter scegliere e visualizzare i voli
                        do {
                            if ((scelta > 0) && (scelta < 4)) {
                                printf("Scegli una delle seguenti funzioni, inserendo il numero della funzione:\n");
                                printf("1. Visualizza voli.\n");
                                printf("2. Prenota un volo.\n");
                                printf("3. Visualizza prenotazioni.\n");
                                printf("4. Effettuare il Logout.\n");
                            } else {
                                printf("Riprova! Hai inserito un numero non valido.\n");
                            }
                            scanf("%d", &sceltaUtente);
                            getchar();
                            switch (scelta) {
                                case 1:
                                    //visualizzare tutti i voli
                                    break;
                                case 2:
                                    //Possibilita di prenotare un volo un altro switch all'interno di questo case con possibilita di tornare indietro
                                    //Oppure scegliere gli algoritmi, piu economico, piu veloce
                                    break;
                                case 3:
                                    //visualizzare tutte le prenotazioni effettuate
                                    break;
                            }
                        }while(scelta != 4);
                    }else if(EffettuaAccesso(username, password) == 2){
                        int sceltaAdmin = 1;
                        printf("L'admin %s ha effettuato correttamente l'accesso!\n", username);
                        do {
                            if ((scelta > 0) && (scelta < 5)) {
                                printf("Scegli una delle seguenti funzioni, inserendo il numero della funzione:\n");
                                printf("1. Visualizza voli.\n");
                                printf("2. Aggiungere nuovi aereporti.\n");
                                printf("3. Aggiungere nuovo volo.\n");
                                printf("4. Rimuovere un volo.\n");
                                printf("5. Effettuare il Logout.\n");
                            } else {
                                printf("Riprova! Hai inserito un numero non valido.\n");
                            }
                            scanf("%d", &sceltaAdmin);
                            getchar();
                            switch (scelta) {
                                case 1:
                                    //visualizzare voli
                                    break;
                                case 2:
                                    //Aggiungere nuovo nodo con funzione creaNodo
                                    break;
                                case 3:
                                    //Aggiungere nuovo arco con funzione Aggiungi
                                    break;
                                case 4:
                                    //Rimuovere un arco con funzione rimuovi
                                    break;
                            }
                        }while(scelta != 5);
                        //poter inserire o rimuovere tratte o voli con scali
                    }
                }while(EffettuaAccesso(username, password) == 0);
                break;

        }
    }while (scelta != 3);
    //saveToDatabase(&Lista, P);
    sqlite3_close(db);
}