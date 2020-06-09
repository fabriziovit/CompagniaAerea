#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "sqlite3.h"
#include "graph.h"
#define MAX 100
sqlite3 *db;



//funzione per vedere se esiste la tabella e quindi il database
int tableExists() {
    sqlite3_stmt *stmt;
    char *sql;
    sql = "SELECT count(*) FROM sqlite_master WHERE type='table' AND name='AEROPORTO';";
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
void initStruct(Graph *G, aeroporto *L) {
    sqlite3_stmt *stmt;
    int rc;
    int index = 0;

    sqlite3_prepare_v2(db, "select * from AEROPORTO ORDER BY NAEROPO ASC;", -1, &stmt, NULL);
    while((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        inserisciCoda(L, sqlite3_column_int(stmt, 0), (char*)sqlite3_column_text(stmt, 1), (char*)sqlite3_column_text(stmt, 2), 0);
        index++;
    }
    sqlite3_finalize(stmt);

    if (index > 0) {
        if (G == NULL) {
            printf("ERRORE: impossibile allocare memoria per il grafo\n");
            sqlite3_close(db);
            exit(-1);
        } else {
            G->adj = (struct Edge **) (Edge **) malloc(index * sizeof(Edge *));
            if ((G->adj == NULL) && (index > 0)) {
                printf("ERRORE: impossibile allocare memoria per la lista del grafo\n");
                free(G);
                G = NULL;
            } else {
                G->n = index;
                for (int i = 0; i < index; i++)
                    G->adj[i] = NULL;
            }
        }
    } else {
        G->n = index;
        G->adj = NULL;
    }
    char codicePartenza[10];
    char cittadestinazione[100];
    char codiceDestinazione[10];
    int dbindex;
    int ind;

    sqlite3_prepare_v2(db, "select * from TRATTE;", -1, &stmt, NULL);
    while((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        strcpy(codicePartenza, (char*)sqlite3_column_text(stmt, 1));
        ind = trovaArray(*L, codicePartenza);
        strcpy(codiceDestinazione, (char*)sqlite3_column_text(stmt, 2));
        dbindex = trovaArray(*L, codiceDestinazione);
        strcpy(cittadestinazione, trovaCitta(*L, codiceDestinazione));
        Aggiungi(G, cittadestinazione, codiceDestinazione, sqlite3_column_int(stmt, 3), 0, dbindex, ind-1);
    }
    sqlite3_finalize(stmt);
}


//funzione che crea il database se non trova il file .db
void creaDatabase(Graph *G , aeroporto *L){
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
            initStruct(G , L);
        }
        else {
            //creazione tabelle
            sql = "CREATE TABLE IF NOT EXISTS UTENTI(USERNAME TEXT PRIMARY KEY, PASSWORD TEXT NOT NULL,NOME TEXT NOT NULL, COGNOME TEXT NOT NULL, TIPO TEXT NOT NULL, PUNTI INT NOT NULL);"
                  "CREATE TABLE IF NOT EXISTS AEROPORTO(NAEROPO INTEGER PRIMARY KEY AUTOINCREMENT, CODAERO TEXT NOT NULL, CITTA TEXT);"
                  "CREATE TABLE IF NOT EXISTS TRATTE(TRATTA INTEGER PRIMARY KEY AUTOINCREMENT, AEROPART TEXT NOT NULL, AERODEST TEXT NOT NULL, KM INT NOT NULL);"
                  "CREATE TABLE IF NOT EXISTS PRENOTATO(NPRENOTAZIONE INTEGER PRIMARY KEY AUTOINCREMENT, USERNAME TEXT NOT NULL, AEROPART TEXT NOT NULL, AERODEST TEXT NOT NULL, PREZZO DOUBLE NOT NULL);";
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

void AggiungiPrenotazioniDB(char partenza[100], char destinazione[100], char utente[100]){
    char *sql;
    int rc;
    sqlite3_stmt *stmt;

    sql = "INSERT INTO PRENOTATO(USERNAME, AEROPART, AERODEST) VALUES(?1, ?2, ?3)";
    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    sqlite3_bind_text(stmt, 1, utente, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, partenza, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, destinazione, -1, SQLITE_STATIC);
    rc = sqlite3_step(stmt);
    if(rc != SQLITE_DONE){
        printf("Errore durante l'inserimento \n");
        sqlite3_close(db);
        exit(-1);
    }
    sqlite3_finalize(stmt);
    printf("\nPrenotazione confermata \n");
}

//funzione per salvare le modifiche avvenute nelle strutture dati e salvare i cambiamenti nel database
void saveToDatabase(aeroporto *L, Graph *G) {
    char *sql;
    int rc;
    sqlite3_stmt *stmt;
    aeroporto temp = *L;


    if (!temp) {
        printf("Salvataggio non effettuato, la lista e' vuota\n");
    } else {
        while (temp!=NULL){
            if(temp->inserito == 1){
                //insert
                sql="INSERT INTO AEROPORTO(CODAERO, CITTA) VALUES(?1,?2);";
                sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
                sqlite3_bind_text(stmt, 1, temp->codiceAeroporto, -1, SQLITE_STATIC);
                sqlite3_bind_text(stmt, 2, temp->citta, -1, SQLITE_STATIC);

                rc = sqlite3_step(stmt);
                if (rc != SQLITE_DONE) {
                    printf("ERROR inserting data: %s\n", sqlite3_errmsg(db));
                    sqlite3_close(db);
                    exit(-1);
                }
                sqlite3_finalize(stmt);
            }
            temp = temp->next;
        }
    }

    if (!G) {
        printf("Salvataggio non effettuato, non ci sono richieste in pending\n");
    } else {
        Edge *e;
        int i;
        char aeroportoPartenza[10];
        for (i = 0; i < G->n; i++) {
            e = (Edge *) G->adj[i];
            strcpy(aeroportoPartenza, trovaCodice(*L, i+1));
            while (e!=NULL) {
                if(e->inserito == 1) {
                    sql = "INSERT INTO TRATTE(AEROPART, AERODEST, KM) VALUES(?1,?2,?3);";
                    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
                    sqlite3_bind_text(stmt, 1, aeroportoPartenza, -1, SQLITE_STATIC);
                    sqlite3_bind_text(stmt, 2, e->codice, -1, SQLITE_STATIC);
                    sqlite3_bind_int(stmt, 3, e->km);
                    rc = sqlite3_step(stmt);
                    if (rc != SQLITE_DONE) {
                        printf("ERRORE: %s\n", sqlite3_errmsg(db));
                        sqlite3_close(db);
                        exit(-1);
                    }
                    sqlite3_finalize(stmt);
                }
                e = e->next;
            }
        }
        free(e);
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

void rimuoviTratta(char *codicePartenza, char *codiceDestinazione, int indice, Graph *G){
    char *sql;
    int rc;
    sqlite3_stmt *stmt;
    sql = "DELETE FROM TRATTE WHERE AEROPART = ?1 AND AERODEST = ?2;";
    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    sqlite3_bind_text(stmt, 1, codicePartenza, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, codiceDestinazione, -1, SQLITE_STATIC);
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        printf("Exit");
        sqlite3_close(db);
        exit(-1);
    }
    sqlite3_finalize(stmt);
    if(sqlite3_changes(db) != 0){
        Rimuovi(G, indice, codiceDestinazione);
        printf("La tratta che va dall'aeroporto %s a quello %s e` stata correttamente cancellata.\n\n", codicePartenza, codiceDestinazione);
    } else{
        printf("Gli aeroport inseriti non hanno una tratta, controlla di aver inserito i dati corretti!\n\n");
    }
}

void visualizzaPrenotazioni(char *username){
    char *sql;
    int rc;
    sqlite3_stmt *stmt;
    sql = "SELECT * FROM PRENOTATO WHERE USERNAME = ?1;";
    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);
    printf("Prenotazioni effettuate:\n");
    while((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        printf("Volo da %s a %s. Prezzo pagato: %.2f\n", (char*)sqlite3_column_text(stmt, 2), (char*)sqlite3_column_text(stmt, 3), sqlite3_column_double(stmt, 4));
    }
    printf("\n");
    sqlite3_finalize(stmt);
}

char *MetaGettonata(char *codicePartenza){
    char *sql;
    int rc;
    sqlite3_stmt *stmt;
    sql = "SELECT COUNT(*) as count, p.AERODEST from prenotato as p WHERE p.AEROPART = ?1 GROUP BY p.AERODEST ORDER BY count DESC;";
    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    sqlite3_bind_text(stmt, 1, codicePartenza, -1, SQLITE_STATIC);
    if((rc = sqlite3_step(stmt)) == SQLITE_ROW){
        return (char*)sqlite3_column_text(stmt, 1);
    }
    return NULL;
}

int getPunti(char *username){
    char *sql;
    int rc;
    sqlite3_stmt *stmt;
    sql = "SELECT PUNTI FROM UTENTI WHERE USERNAME = ?1;";
    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);
    if((rc = sqlite3_step(stmt)) == SQLITE_ROW){
        return sqlite3_column_int(stmt, 0);
    }
    return 0;
}

void salvaPrenotazione(char *username, char *codicePartenza, char *codiceDestinazione, double prezzototale){
    char *sql;
    int rc;
    sqlite3_stmt *stmt;
    sql = "INSERT INTO PRENOTATO(USERNAME, AEROPART , AERODEST, PREZZO) VALUES(?1, ?2, ?3, ?4);";
    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, codicePartenza, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, codiceDestinazione, -1, SQLITE_STATIC);
    sqlite3_bind_double(stmt, 4, prezzototale);
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        printf("Exit");
        sqlite3_close(db);
        exit(-1);
    }
    sqlite3_finalize(stmt);
    printf("Prenotazione effettuata correttamente!\n");
}

void AggiornaPunti(char *username, int punti){
    char *sql;
    int rc;
    sqlite3_stmt *stmt;
    sql = "UPDATE UTENTI SET PUNTI = ?1 WHERE USERNAME = ?2;";
    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    sqlite3_bind_int(stmt, 1,punti);
    sqlite3_bind_text(stmt, 2, username, -1, SQLITE_STATIC);
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        printf("Exit");
        sqlite3_close(db);
        exit(-1);
    }
    sqlite3_finalize(stmt);
}



int main() {
    char username[MAX];
    char nome[MAX], cognome[MAX];
    char password[33];
    int scelta = 1;
    Graph *G;
    aeroporto L=NULL;

    G = (Graph *)malloc(sizeof(Graph));
    creaDatabase(G , &L);

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
                //Effettuare il login e controllo se utente o amministratore
                printf("Inserisci username:\n");
                gets(username);
                printf("Inserisci password:\n");
                gets(password);
                if(EffettuaAccesso(username, password) == 0){
                    printf("username/password errate!Riprova.\n");
                } else if(EffettuaAccesso(username, password) == 1) {
                    int sceltaUtente = 1;
                    int sceltaPrenotazione = 1;
                    int sceltaTipo = 1;
                    char cittaPartenza[100];
                    char cittaDestinazione[100];
                    char codicePartenza[10];
                    char codiceDestinazione[10];
                    int indicePartenza;
                    double prezzototale;
                    char continuaPrenotazione;
                    int punti;
                    int km;
                    double sconto;
                    printf("%s ha effettuato correttamente l'accesso!\n", username);
                    //poter scegliere e visualizzare i voli
                    do {
                        if ((sceltaUtente > 0) && (sceltaUtente < 4)) {
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
                        switch (sceltaUtente) {
                            case 1:
                                //Visualizzare i voli
                                stampaVoli(G, L);
                                break;
                            case 2:
                                do {
                                    if ((sceltaPrenotazione > 0) && (sceltaPrenotazione < 3)) {
                                        printf("Scegli una delle seguenti funzioni, inserendo il numero della funzione:\n");
                                        printf("1. Inserisci andata e Ritorno.\n");
                                        printf("2. Inserisci solo andata.\n");
                                        printf("3. Torna indietro.\n");
                                    } else {
                                        printf("Riprova! Hai inserito un numero non valido.\n");
                                    }
                                }while(sceltaPrenotazione <=0 || sceltaPrenotazione > 3);
                                if(sceltaPrenotazione != 3) {
                                    scanf("%d", &sceltaPrenotazione);
                                    getchar();
                                    switch (sceltaPrenotazione) {
                                        case 1:
                                            printf("Inserisci la citta` di partenza:\n");
                                            gets(cittaPartenza);
                                            printf("Inserisci la citta` di destinazione:\n");
                                            gets(cittaDestinazione);
                                            do {
                                                if ((sceltaTipo > 0) && (sceltaTipo < 3)) {
                                                    printf("Scegli una delle seguenti funzioni, inserendo il numero della funzione:\n");
                                                    printf("1. Tratta piu` economica.\n");
                                                    printf("2. Tratta casuale.\n");
                                                    printf("3. Annulla prenotazione.\n");
                                                } else {
                                                    printf("Riprova! Hai inserito un numero non valido.\n");
                                                }
                                            } while ((sceltaTipo <= 0) || (sceltaTipo > 3));
                                            scanf("%d", &sceltaTipo);
                                            getchar();
                                            if (sceltaTipo != 3) {
                                                switch (sceltaTipo) {
                                                    case 1:
                                                        if(trovaCodiceDaCitta(L, cittaPartenza) != NULL) {
                                                            strcpy(codicePartenza, trovaCodiceDaCitta(L, cittaPartenza));
                                                            indicePartenza = trovaArray(L, codicePartenza) - 1;
                                                            if (haTratta(G, indicePartenza) == 1) {

                                                            } else
                                                                printf("La citta` di partenza indicata non ha tratte!\n");
                                                        } else
                                                            printf("Non esiste nessun aeroporto nella citta` indicata! Riprova inserendo i dati corretti.\n");
                                                        //Tratta economica/breve
                                                        //Specie di carrello con utilizzo punti bonus
                                                        //salvataggio nel db
                                                        break;
                                                    case 2:
                                                        //Tratta qualsiasi
                                                        //Specie di carrello con utilizzo punti bonus
                                                        //salvataggio nel db
                                                        break;
                                                }
                                            }
                                            sceltaTipo = 1;
                                            break;
                                        case 2:
                                            printf("Inserisci la citta` di partenza:\n");
                                            gets(cittaPartenza);
                                            do {
                                                if ((sceltaTipo > 0) && (sceltaTipo < 3)) {
                                                    printf("Scegli una delle seguenti funzioni, inserendo il numero della funzione:\n");
                                                    printf("1. Tratta piu` economica.\n");
                                                    printf("2. Meta` piu` gettonata.\n");
                                                    printf("3. Annulla prenotazione.\n");
                                                } else {
                                                    printf("Riprova! Hai inserito un numero non valido.\n");
                                                }
                                            } while ((sceltaTipo <= 0) || (sceltaTipo > 3));
                                            if (sceltaTipo != 3) {
                                                scanf("%d", &sceltaTipo);
                                                getchar();
                                                switch (sceltaTipo) {
                                                    case 1:
                                                        if(trovaCodiceDaCitta(L, cittaPartenza) != NULL){
                                                            strcpy(codicePartenza, trovaCodiceDaCitta(L,cittaPartenza));
                                                            indicePartenza = trovaArray(L, codicePartenza)-1;
                                                            if(haTratta(G, indicePartenza) == 1) {
                                                                strcpy(codiceDestinazione,TrovaMinKm(G, indicePartenza, L));
                                                                strcpy(cittaDestinazione,trovaCitta(L, codiceDestinazione));
                                                                printf("La citta` piu gettonata al momento e` %s vuoi continuare la prenotazione? y/n\n",cittaDestinazione);
                                                                scanf("%c", &continuaPrenotazione);
                                                                getchar();
                                                                if (continuaPrenotazione == 'y') {
                                                                    km = getKm(G, codiceDestinazione, indicePartenza);
                                                                    prezzototale = km * 2.5;
                                                                    punti = getPunti(username);
                                                                    if (punti >= 150) {
                                                                        printf("Punti disponibili: %d.\nVuoi usare i tuoi punti per ricevere un sconto? y/n\n",
                                                                               punti);
                                                                        scanf("%c", &continuaPrenotazione);
                                                                        getchar();
                                                                        if (continuaPrenotazione == 'y') {
                                                                            if (punti >= 150 && punti < 300) {
                                                                                //10% di sconto
                                                                                sconto = 0.1 * prezzototale;
                                                                                prezzototale -= sconto;
                                                                                punti = punti - 150;
                                                                                //diminuzione dei punti di 150
                                                                                printf("Nuovo prezzo con sconto: %.2f",
                                                                                       prezzototale);
                                                                            } else if (punti >= 300 && punti < 450) {
                                                                                //20% di sconto
                                                                                sconto = 0.2 * prezzototale;
                                                                                prezzototale -= sconto;
                                                                                punti = punti - 300;
                                                                                //diminuzione dei punti di 300
                                                                                printf("Nuovo prezzo con sconto: %.2f",
                                                                                       prezzototale);
                                                                            } else if (punti >= 450 && punti < 600) {
                                                                                //30% di sconto
                                                                                sconto = 0.3 * prezzototale;
                                                                                prezzototale -= sconto;
                                                                                punti = punti - 450;
                                                                                //diminuzione dei punti di 450
                                                                                printf("Nuovo prezzo con sconto: %.2f",
                                                                                       prezzototale);
                                                                            } else if (punti >= 600 && punti < 750) {
                                                                                //40% di sconto
                                                                                sconto = 0.4 * prezzototale;
                                                                                prezzototale -= sconto;
                                                                                punti = punti - 600;
                                                                                //diminuzione dei punti di 600
                                                                                printf("Nuovo prezzo con sconto: %.2f",
                                                                                       prezzototale);
                                                                            } else if (punti >= 750) {
                                                                                //50% di sconto
                                                                                sconto = 0.5 * prezzototale;
                                                                                prezzototale -= sconto;
                                                                                punti = punti - 750;
                                                                                //diminuzione dei punti di 750
                                                                                printf("Nuovo prezzo con sconto: %.2f",
                                                                                       prezzototale);
                                                                            }
                                                                        }
                                                                    } else {
                                                                        printf("Non disponi di abbastanza punti per ricevere uno sconto.\nPrezzo totale: %.2f\n",
                                                                               prezzototale);
                                                                    }
                                                                    int puntitotali = punti;
                                                                    salvaPrenotazione(username, codicePartenza,
                                                                                      codiceDestinazione, prezzototale);
                                                                    punti = (km / 15);
                                                                    puntitotali += punti;
                                                                    printf("Punti aggiunti all'account: %d\nPunti totali disponibili: %d\n",
                                                                           punti, puntitotali);
                                                                    AggiornaPunti(username, puntitotali);
                                                                } else
                                                                    printf("Prenotazione annullata. Tornerai al menu` delle prenotazioni.\n");
                                                            }else
                                                                printf("La citta` di partenza indicata non ha tratte!\n");
                                                        }else{
                                                            printf("Non esiste nessun aeroporto nella citta` indicata! Riprova inserendo i dati corretti.\n");
                                                        }
                                                        break;
                                                    case 2:
                                                        //Meta piu gettonata
                                                        if(trovaCodiceDaCitta(L, cittaPartenza) != NULL){
                                                            strcpy(codicePartenza, trovaCodiceDaCitta(L,cittaPartenza));
                                                            if(MetaGettonata(codicePartenza) != NULL) {
                                                                strcpy(codiceDestinazione, MetaGettonata(codicePartenza));
                                                                strcpy(cittaDestinazione, trovaCitta(L, codiceDestinazione));
                                                                printf("La citta` piu gettonata al momento e` %s vuoi continuare la prenotazione? y/n\n", cittaDestinazione);
                                                                scanf("%c", &continuaPrenotazione);
                                                                getchar();
                                                                if(continuaPrenotazione == 'y'){
                                                                    indicePartenza = trovaArray(L, codicePartenza) -1;
                                                                    km = getKm(G, codiceDestinazione, indicePartenza);
                                                                    prezzototale = km * 2.5;
                                                                    punti = getPunti(username);
                                                                    if(punti>=150){
                                                                        printf("Punti disponibili: %d.\nVuoi usare i tuoi punti per ricevere un sconto? y/n\n", punti);
                                                                        scanf("%c", &continuaPrenotazione);
                                                                        getchar();
                                                                        if(continuaPrenotazione == 'y'){
                                                                            if(punti >= 150 && punti < 300){
                                                                                //10% di sconto
                                                                                sconto = 0.1 * prezzototale;
                                                                                prezzototale -= sconto;
                                                                                punti = punti - 150;
                                                                                //diminuzione dei punti di 150
                                                                                printf("Nuovo prezzo con sconto: %.2f", prezzototale);
                                                                            }else if(punti >=300 && punti <450){
                                                                                //20% di sconto
                                                                                sconto = 0.2 * prezzototale;
                                                                                prezzototale -= sconto;
                                                                                punti = punti - 300;
                                                                                //diminuzione dei punti di 300
                                                                                printf("Nuovo prezzo con sconto: %.2f", prezzototale);
                                                                            } else if(punti >=450 && punti <600){
                                                                                //30% di sconto
                                                                                sconto = 0.3 * prezzototale;
                                                                                prezzototale -= sconto;
                                                                                punti = punti - 450;
                                                                                //diminuzione dei punti di 450
                                                                                printf("Nuovo prezzo con sconto: %.2f", prezzototale);
                                                                            } else if(punti >=600 && punti <750){
                                                                                //40% di sconto
                                                                                sconto = 0.4 * prezzototale;
                                                                                prezzototale -= sconto;
                                                                                punti = punti - 600;
                                                                                //diminuzione dei punti di 600
                                                                                printf("Nuovo prezzo con sconto: %.2f", prezzototale);
                                                                            } else if(punti >= 750){
                                                                                //50% di sconto
                                                                                sconto = 0.5 * prezzototale;
                                                                                prezzototale -= sconto;
                                                                                punti = punti - 750;
                                                                                //diminuzione dei punti di 750
                                                                                printf("Nuovo prezzo con sconto: %.2f", prezzototale);
                                                                            }
                                                                        }
                                                                    } else{
                                                                        printf("Non disponi di abbastanza punti per ricevere uno sconto.\nPrezzo totale: %.2f\n", prezzototale);
                                                                    }
                                                                    int puntitotali  = punti;
                                                                    salvaPrenotazione(username, codicePartenza, codiceDestinazione, prezzototale);
                                                                    punti = (km / 15);
                                                                    puntitotali += punti;
                                                                    printf("Punti aggiunti all'account: %d\nPunti totali disponibili: %d\n", punti, puntitotali);
                                                                    AggiornaPunti(username, puntitotali);
                                                                } else
                                                                    printf("Prenotazione annullata. Tornerai al menu` delle prenotazioni.\n");
                                                            } else
                                                                printf("Non esiste nessuna meta gettonata per la citta inserita!\n");
                                                        }else {
                                                            printf("Non esiste nessun aeroporto nella citta` indicata! Oppure non esiste nessuna meta gettonata per la citta inserita.\n");
                                                        }
                                                        break;
                                                }
                                            }
                                            sceltaTipo = 1;
                                            break;
                                    }
                                }
                                sceltaPrenotazione = 1;
                                break;
                            case 3:
                                //visualizzare tutte le prenotazioni effettuate
                                visualizzaPrenotazioni(username);
                                break;
                        }
                    }while(sceltaUtente != 4);
                }else if(EffettuaAccesso(username, password) == 2){
                    int sceltaAdmin = 1;
                    char codice[10] = "";
                    char citta[100] = "";
                    int indice;
                    int indiceDest;
                    int km;
                    char codiceDest[10];
                    int pres = 0;
                    printf("L'admin %s ha effettuato correttamente l'accesso!\n", username);
                    do {
                        if ((sceltaAdmin > 0) && (sceltaAdmin < 5)) {
                            printf("Scegli una delle seguenti funzioni, inserendo il numero della funzione:\n");
                            printf("1. Visualizza voli.\n");
                            printf("2. Aggiungere nuovo aereporto.\n");
                            printf("3. Aggiungere nuovo volo.\n");
                            printf("4. Rimuovere un volo.\n");
                            printf("5. Effettuare il Logout.\n");
                        } else {
                            printf("Riprova! Hai inserito un numero non valido.\n");
                        }
                        scanf("%d", &sceltaAdmin);
                        getchar();
                        switch (sceltaAdmin) {
                            case 1:
                                //visualizzare voli
                                printList(L);
                                stampaVoli(G, L);//da fixare non funziona
                                break;
                            case 2:
                                do {
                                    //Aggiungere nuovo nodo con funzione creaNodo
                                    printf("Inserisci il codice dell'aeroporto:\n");
                                    gets(codice);
                                    printf("Inserisci la citta` dove si trova l'aeroporto:\n");
                                    gets(citta);
                                    if (trovaArray(L, codice) == -1)
                                        pres = 1;
                                    else
                                        printf("Aeroporto gia` esistente, inserisci un altro codice!\n");
                                }while(pres == 0);
                                pres = 0;
                                indice = G->n + 1;
                                inserisciCoda(&L, indice, codice, citta, 1);
                                creaNodo(G);
                                printf("Aeoroporto aggiunto con successo!\n");
                                break;
                            case 3:
                                //Aggiungere nuovo arco con funzione Aggiungi
                                printf("Inserisci il codice dell'aeroporto di partenza:\n");
                                gets(codice);
                                if(trovaArray(L, codice) != -1) {
                                    indice = trovaArray(L, codice) - 1;
                                    printf("Inserisci il codice dell'aeroporto di destinazione:\n");
                                    gets(codiceDest);
                                    if (trovaArray(L, codiceDest) != -1) {
                                        indiceDest = trovaArray(L, codiceDest);
                                        if(strcmp(codice, codiceDest) != 0){
                                            if (trovaCitta(L, codiceDest) != NULL) {
                                                strcpy(citta, trovaCitta(L, codiceDest));
                                                printf("Inserisci i km tra i due aeroporti:\n");
                                                scanf("%d", &km);
                                                getchar();
                                                if (presente(G, codiceDest, indice) == 0) {
                                                    Aggiungi(G, citta, codiceDest, km, 1, indiceDest, indice);
                                                    printf("Tratta creata correttamente!\n");
                                                } else
                                                    printf("Errore!Tratta gia` esistente!\n");
                                            } else
                                                printf("ERRORE! Il codice inserito non corrisponde a nessun aeroporto.\n");
                                        }else
                                            printf("ERRORE! L'aeroporto di partenza non puo` essere uguale a quello di destinazione.\n");
                                    } else
                                        printf("ERRORE! Il codice inserito non corrisponde a nessun aeroporto.\n");
                                } else
                                    printf("ERRORE! Il codice inserito non corrisponde a nessun aeroporto.\n");
                                break;
                            case 4:
                                //Rimuovere un arco con funzione rimuovi
                                do {
                                    printf("Inserisci il codice dell'aeroporto di partenza:\n");//da testare
                                    gets(codice);
                                    indice = trovaArray(L, codice) - 1;
                                    printf("Inserisci il codice dell'aeroporto di destinazione rimuovendo la tratta:\n");
                                    gets(codiceDest);
                                    if (trovaArray(L, codice) != -1 && trovaArray(L, codiceDest) != -1) {
                                        rimuoviTratta(codice, codiceDest, indice, G);

                                    } else
                                        printf("I dati inseriti non corrispondo controlla di aver inserito i codici giusti!\n");
                                }while (trovaArray(L, codice) == 0 && trovaArray(L, codiceDest) == 0);
                                break;
                        }
                    }while(sceltaAdmin != 5);
                }
                break;
        }
    }while (scelta != 3);
    saveToDatabase(&L, G);
    free(G);
    free(L);
    sqlite3_close(db);
}