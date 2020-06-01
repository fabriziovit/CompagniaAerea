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

    sqlite3_prepare_v2(db, "select * from LIBRI ORDER BY ID ASC;", -1, &stmt, NULL);
    while((rc = sqlite3_step(stmt)) == SQLITE_ROW)
        inserisciCoda(L, sqlite3_column_int(stmt, 0), (char*)sqlite3_column_text(stmt, 1), (char *)sqlite3_column_text(stmt, 2), 0);
    sqlite3_finalize(stmt);

    sqlite3_prepare_v2(db, "select * from AEROPORTO;", -1, &stmt, NULL);
    while((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        index++;
    }
    sqlite3_finalize(stmt);

    G = CreaGrafo(index);
    //query per caricare tutti i voli nel grafo quindi fare una query where indice array trovi con un join tra aeroporto e tratte su aeroporto e
    // caricare con aggiungi la citta di destinazione in base al naeropo e il numero di km da db
    //index = naeropo;
    //km da prendere
    //Aggiungi(G,cittadestinazione, km, 0, index);
    //sqlite3_finalize(stmt);
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
void saveToDatabase(aeroporto *L, Graph *P) {
    char *sql;
    int rc;
    sqlite3_stmt *stmt;
    aeroporto temp = *L;

/*
    if (!temp) {
        printf("Salvataggio non effettuato, la lista e' vuota\n");
    } else {
        while (temp!=NULL){
            if(temp->is_present == 0){
                //insert
                sql="INSERT INTO LIBRI(ID, TITOLO, AUTORE, QUANTITA) VALUES(?1,?2,?3,?4);";
                sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
                sqlite3_bind_int(stmt, 1, temp->codiceLibro);
                sqlite3_bind_text(stmt, 2, temp->titolo, -1, SQLITE_STATIC);
                sqlite3_bind_text(stmt, 3, temp->autore, -1, SQLITE_STATIC);
                sqlite3_bind_int(stmt, 4, temp->quantita);

                rc = sqlite3_step(stmt);
                if (rc != SQLITE_DONE) {
                    printf("ERROR inserting data: %s\n", sqlite3_errmsg(db));
                    sqlite3_close(db);
                    exit(-1);
                }
                sqlite3_finalize(stmt);
            }
            else if(temp->is_present==2){
                //update
                sql="UPDATE LIBRI SET QUANTITA = ?1 WHERE ID= ?2";
                sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
                sqlite3_bind_int(stmt, 1, temp->quantita);
                sqlite3_bind_int(stmt, 2, temp->codiceLibro);
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

    if (!P) {
        printf("Salvataggio non effettuato, non ci sono richieste in pending\n");
    } else {
        Queue *e;
        int i;
        int id;
        for (i = 0; i < P->n; i++) {
            e = P->q[i];
            id = TrovaIDByIndex(*L, i);
            while (e!=NULL) {
                if(e->inseritoRT == 1) {
                    sql = "INSERT INTO RICHIESTE(ID, MATRICOLA) VALUES(?1,?2);";
                    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
                    sqlite3_bind_int(stmt, 1, id);
                    sqlite3_bind_text(stmt, 2, e->matricola, -1, SQLITE_STATIC);
                    rc = sqlite3_step(stmt);
                    if (rc != SQLITE_DONE) {
                        printf("ERROR inserting data: %s\n", sqlite3_errmsg(db));
                        sqlite3_close(db);
                        exit(-1);
                    }
                    sqlite3_finalize(stmt);
                }
                e = e->next;
            }
        }
        free(e);
    }*/
//Da vedere per salvare nel db alla fine del programma
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


int getRowAero(char *codice){
    char *sql;
    int rc;
    sqlite3_stmt *stmt;
    sql = "SELECT * FROM AEROPORTO WHERE CODAERO = ?1;";
    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    sqlite3_bind_text(stmt, 1, codice, -1, SQLITE_STATIC);
    rc = sqlite3_step(stmt);
    if(rc == SQLITE_ROW) {
        return sqlite3_column_int(stmt, 0);
    } else
        return 0;
}

char *getCitta(char *codice){
    char *sql;
    int rc;
    sqlite3_stmt *stmt;
    sql = "SELECT * FROM AEROPORTO WHERE CODAERO = ?1;";
    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    sqlite3_bind_text(stmt, 1, codice, -1, SQLITE_STATIC);
    rc = sqlite3_step(stmt);
    if(rc == SQLITE_ROW)
        return (char*)sqlite3_column_text(stmt, 2);
    else
        return NULL;
}

int rimuoviTratta(char *codicePartenza, char *codiceDestinazione){
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
        printf("La tratta che va dall'aeroporto %s a quello %s e` stata correttamente cancellata.\n\n", codicePartenza, codiceDestinazione);
    } else{
        printf("Gli aeroport inseriti non hanno una tratta, controlla di aver inserito i dati corretti!\n\n");
    }
}

int main() {
    char username[MAX];
    char nome[MAX], cognome[MAX];
    char password[33];
    int scelta = 1;
    Graph *G;
    aeroporto L=NULL;

    creaDatabase(G , &L);
    G = CreaGrafo(1);
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
                        switch (sceltaUtente) {
                            case 1:
                                //Visualizzare i voli
                                stampaVoli(G, L);
                                break;
                            case 2:
                                //Possibilita di prenotare un volo un altro switch all'interno di questo case con possibilita di tornare indietro
                                //AggiungiPrenotazioniDB(partenza, destinazione, utente);
                                //Oppure scegliere gli algoritmi, piu economico, piu veloce
                                break;
                            case 3:
                                //visualizzare tutte le prenotazioni effettuate
                                break;
                        }
                    }while(sceltaUtente != 4);
                }else if(EffettuaAccesso(username, password) == 2){
                    int sceltaAdmin = 1;
                    char codice[10];
                    char citta[100];
                    int indice;
                    int indiceDest;
                    int km;
                    char codiceDest[10];
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
                        switch (sceltaAdmin) {
                            case 1:
                                //visualizzare voli
                                printList(L);
                                stampaVoli(G, L);
                                break;
                            case 2:
                                //Aggiungere nuovo nodo con funzione creaNodo
                                printf("Inserisci il codice dell'aeroporto:\n");
                                gets(codice);
                                printf("Inserisci la citta` dove si trova l'aeroporto:\n");
                                gets(citta);
                                //salvataggio nel db al termine del programma
                                indice = creaNodo(G)+1;//da testare
                                inserisciCoda(&L, indice, codice, citta, 1);
                                break;
                            case 3:
                                //Aggiungere nuovo arco con funzione Aggiungi
                                do{
                                    printf("Inserisci il codice dell'aeroporto di partenza:\n");
                                    gets(codice);
                                    if(getRowAero(codice) != 0) {
                                        indice = getRowAero(codice) - 1;
                                        printf("Inserisci il codice dell'aeroporto di destinazione:\n");
                                        gets(codiceDest);
                                        if (getRowAero(codiceDest) != 0) {
                                            indiceDest = getRowAero(codice);
                                            if (getCitta(codiceDest) != NULL) {
                                                strcpy(citta, getCitta(codiceDest));
                                                printf("Inserisci i km tra i due aeroporti:\n");
                                                scanf("%d", &km);
                                            }else
                                                printf("Il codice inserito non corrisponde a nessun aereo!Riprova.\n");
                                        } else{
                                            printf("Il codice inserito non corrisponde a nessun aereo!Riprova.\n");
                                        }
                                    } else{
                                        printf("Il codice inserito non corrisponde a nessun aereo!Riprova.\n");
                                    }
                                    //aggiungi nel grafo
                                    Aggiungi(G, citta, codiceDest, km, 1 ,indiceDest, indice);//da testare
                                    //Aggiunta del volo nel db tabella TRATTE
                                }while (getRowAero(codice) == 0 || getCitta(codice) == NULL);
                                break;
                            case 4:
                                //Rimuovere un arco con funzione rimuovi
                                do {
                                    printf("Inserisci il codice dell'aeroporto di partenza:\n");//da testare
                                    gets(codice);
                                    indice = getRowAero(codice) - 1;
                                    printf("Inserisci il codice dell'aeroporto di destinazione rimuovendo la tratta:\n");
                                    gets(codiceDest);
                                    if (getRowAero(codice) != 0 && getRowAero(codiceDest) != 0) {
                                        rimuoviTratta(codice, codiceDest);
                                        Rimuovi(G, indice, codiceDest, getRowAero(codiceDest));
                                    } else
                                        printf("I dati inseriti non corrispondo controlla di aver inserito i codici giusti!\n");
                                }while (getRowAero(codice) == 0 && getRowAero(codiceDest) == 0);
                                break;
                        }
                    }while(sceltaAdmin != 5);
                }
                break;
        }
    }while (scelta != 3);
    //saveToDatabase(&L, P, &Lista);
    sqlite3_close(db);
}