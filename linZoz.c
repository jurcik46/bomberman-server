//
// Created by Jurco on 21. 11. 2018.
//
#include "linZoz.h"

Polozka *getLZPolozka(const LinZoz *zoz, int pos) {
    Polozka *akt = zoz->zac;
    int i = 0;
    while (akt != NULL && i < pos) {
        akt = akt->nas;
        i++;
    }
    return akt;

}

Polozka *createLZPolozka(double data) {
    Polozka *pomNova = malloc(sizeof(Polozka));
    pomNova->data = data;
    pomNova->nas = NULL;
    return pomNova;

}

void initLZ(LinZoz *zoz) {
    zoz->zac = zoz->kon = NULL;
    zoz->pocet = 0;


}

void disposeLZ(LinZoz *zoz) {
    Polozka *akt = zoz->zac;
    while (akt != NULL) {
        zoz->zac = zoz->zac->nas;
        free(akt);
        akt = zoz->zac;

    }
    zoz->kon = NULL;
    zoz->pocet = 0;

}

void addLZ(LinZoz *zoz, double data) {
    Polozka *pomNova = createLZPolozka(data);
    if (zoz->zac == NULL) {
        zoz->zac = pomNova;
    } else {
        zoz->kon->nas = pomNova;
    }
    zoz->kon = pomNova;
    zoz->pocet++;
}


void printLZ(const LinZoz *zoz) { // vytlacit  na obrazovku
    Polozka *akt = zoz->zac;
    while (akt != NULL) {
        printf("%f ", akt->data);
        akt = akt->nas;
    }
    printf("\n");

}

void prependLZ(LinZoz *zoz, double data) {
    Polozka *pomNova = createLZPolozka(data);
    pomNova->nas = zoz->zac;

    if (zoz->zac == NULL) {
        zoz->kon = pomNova;
    }
    zoz->zac = pomNova;
    zoz->pocet++;

}

_Bool tryInsertLZ(LinZoz *zoz, double data, int pos) {

    if (pos < 0 || pos > zoz->pocet) {
        return false;
    }
    if (pos == 0) {
        prependLZ(zoz, data);
        return true;
    }
    Polozka *akt = getLZPolozka(zoz, pos - 1);

    Polozka *pomNova = createLZPolozka(data);
    pomNova->nas = akt->nas;
    akt->nas = pomNova;
    zoz->pocet++;

    return true;

}

_Bool tryGetLZ(LinZoz *zoz, int pos, double *data) {
    if (pos < 0 || pos >= zoz->pocet) {
        return false;
    }
    Polozka *akt = getLZPolozka(zoz, pos);

    *data = akt->data;
    return true;

}

_Bool tryCopyLZ(const LinZoz *src, LinZoz *dest) {
    if (src == dest) {
        return true;
    }
    disposeLZ(dest);
    Polozka *akt = src->zac;
    while (akt != NULL) {
        addLZ(dest, akt->data);
        akt = akt->nas;

    }

    return true;
}
