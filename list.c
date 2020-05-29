//
// Created by gianl on 28/05/2020.
//

#include <malloc.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "list.h"
#include "sqlite3.h"
sqlite3 *db;

Prenotazioni InizializzaL(char partenza[100], char destinazione[100], char utente[100]){
    Prenotazioni L = (Prenotazioni)malloc(sizeof(struct Nodo));
    strcpy(L->partenza, partenza);
    strcpy(L->destinazione, destinazione);
    strcpy(L->utente, utente);
    L->next = NULL;
    return L;
}

void StampaL(Prenotazioni L){
    if (L != NULL) {
        printf("Utente: %s\n", L->utente);
        printf("Citta' di Partenza: %s\n", L->partenza);
        printf("Citta' di Destinazione: %s\n", L->destinazione);
        printf("\n");
        StampaL(L->next);
    }
}

void InserisciLDB(char partenza[100], char destinazione[100], char utente[100]){
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

void InserisciL(Prenotazioni *L, char partenza[100], char destinazione[100], char utente[100]){
    Prenotazioni ultimo;
    Prenotazioni temp;
    temp = InizializzaL(partenza, destinazione, utente);
    if (*L == NULL) {
        *L = temp;
    } else {
        ultimo = *L;
        while (ultimo->next != NULL)
            ultimo = ultimo->next;
        ultimo->next = temp;
    }
    InserisciLDB(partenza, destinazione, utente);
}

//DA FIXARE QUANDO BISOGNA ELIMINARE IL PRIMO ELEMENTO
void EliminaLDB(char partenza[100], char destinazione[100], char utente[100]){
    char *sql;
    int rc;
    sqlite3_stmt *stmt;

    sql = "DELETE FROM PRENOTATO WHERE USERNAME = ?1 AND AEROPART = ?2 AND AERODEST = ?3;";
    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    sqlite3_bind_text(stmt, 1, utente, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, partenza, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, destinazione, -1, SQLITE_STATIC);
    rc = sqlite3_step(stmt);
    if(rc != SQLITE_DONE){
        printf("Exit \n");
        sqlite3_close(db);
        exit(-1);
    }
    sqlite3_finalize(stmt);
    printf("\nLa prenotazione dell' utente %s del volo che parte da %s con destinazione a %s e' stata correttamente cancellata. \n", utente, partenza, destinazione);
}


//DA FIXARE QUANDO NON VENGONO PASSATI PARAMETRI SBAGLIATI
void EliminaElem(Prenotazioni *L, char partenza[100], char destinazione[100], char utente[100]){
    struct Nodo *tmp = *L;
    struct Nodo *prev;

    if(tmp != NULL && ((strcmp(tmp->partenza, partenza) == 0) && (strcmp(tmp->destinazione, destinazione) == 0) && (strcmp(tmp->utente, utente) == 0))){
        *L = tmp->next;
        free(tmp);
        return;
    }
    while(tmp != NULL && ((strcmp(tmp->partenza, partenza) != 0) && (strcmp(tmp->destinazione, destinazione) != 0) && (strcmp(tmp->utente, utente) != 0))){
        prev = tmp;
        tmp = tmp->next;
    }
    if(tmp == NULL)
        return;
    prev->next = tmp->next;
    free(tmp);
    EliminaLDB(partenza, destinazione, utente);
}

//Codice per provare le funzioni
/*char mat[] = "pep";
    char pos[] = "wes";
    char fas[] = "sgrodolo";
    char lol[] = "res";
    char holt[] = "holt";
    char mm[] = "djs";
    char ss[] = "sw";
    char bb[] = "dsa";
    char gg[] = "qqq";
    char sa[] = "SOP";
    Prenotazioni *L= NULL;
    Graph *G;

    creaDatabase(G);
    G = CreaGrafo(1);

    InserisciL(&L, mat, pos, fas);
    InserisciL(&L, lol, holt, mm);
    InserisciL(&L, ss, bb, gg);
    EliminaElem(&L, sa, holt, mm);
    StampaL(L);*/