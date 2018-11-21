//
// Created by Jurco on 21. 11. 2018.
//

#ifndef BOMBERMAN_SERVER_LINZOZ_H
#define BOMBERMAN_SERVER_LINZOZ_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct polozka {
    double data;
    struct polozka *nas;
} Polozka;

typedef struct linZoz {
    Polozka *zac;
    Polozka *kon;
    int pocet;

} LinZoz;

Polozka *getLZPolozka(const LinZoz *zoz, int pos);

Polozka *createLZPolozka(double data);

void initLZ(LinZoz *zoz);

void disposeLZ(LinZoz *zoz);

void addLZ(LinZoz *zoz, double data);

void printLZ(const LinZoz *zoz);

void prependLZ(LinZoz *zoz, double data);

_Bool tryInsertLZ(LinZoz *zoz, double data, int pos);

_Bool tryGetLZ(LinZoz *zoz, int pos, double *data);

_Bool tryCopyLZ(const LinZoz *src, LinZoz *dest);

#endif //BOMBERMAN_SERVER_LINZOZ_H
