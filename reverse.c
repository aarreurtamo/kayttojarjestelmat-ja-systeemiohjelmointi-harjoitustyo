#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Määritetään tietue linkitettyä listaa varten
typedef struct yksiRivi {
    char *pRivi;
    struct yksiRivi *pSeuraava;
    struct yksiRivi *pEdellinen;
} YKSIRIVI;

// Alustetaan aliohjelmat
void parametreja3(char *argc[]);
void parametreja2(char *argc[]);
void parametreja1(char *argc[]);


int main(int argc, char *argv[]) {

    // Tarkistetaan komentoriviparamterien määrä ja else if-rakennetta käyttäen, mitä aliohjelmaa käytetään.
    if (argc > 3) {
        fprintf(stderr, "usage: reverse <input> <output>\n");
        exit(1);
    } else if (argc == 3) {
        parametreja3(argv);
    } else if (argc == 2) {
        parametreja2(argv);
    } else if (argc == 1) {
        parametreja1(argv);
    }
    return(0);
}

void parametreja3(char *argv[]) {
    char *rivi = NULL;
    size_t len = 0;
    ssize_t pituus = 1;

    // Tarkistetaan, että input- ja output- tiedostot eivät ole samat.
    if (strcmp(argv[1],argv[2]) == 0) {
        fprintf(stderr, "Input and output file must differ\n");
        exit(1);
    }

    // Avataan tiedostot
    FILE *inputTiedosto = fopen(argv[1],"r"), *outputTiedosto = fopen(argv[2],"w");

    // Tarkistetaan, että tiedostojen avaaminen onnistui.
    if (inputTiedosto == NULL) {
        fprintf(stderr, "error: cannot open file '%s'\n",argv[1]);
        exit(1);
    } else if (outputTiedosto == NULL) {
        fprintf(stderr, "error: cannot open file '%s'\n",argv[2]);
        exit(1);       
    }

    // Alustetaan osoittimet
    YKSIRIVI *pAlku = NULL, *pLoppu = NULL, *pUusi, *ptr;

    // Luetaan ensimmäinen rivi.
    pituus = getline(&rivi,&len,inputTiedosto);

    // Luodaan apumuuttuja, jonka avulla tiedetään, jos inputTiedosto olisikin tyhjä.
    int tyhjyysTarkistus = 0;

    // Lisätään rivit linkitettyyn listaan while-loopin avulla.
    while (pituus > 0) {

        // Varataan muisti ja tarkistetaan, että se onnistuu
        if ((pUusi = (YKSIRIVI*)malloc(sizeof(YKSIRIVI))) == NULL) {
            fprintf(stderr, "malloc failed\n");
            free(rivi);
            fclose(inputTiedosto);
            fclose(outputTiedosto);            
            exit(1);
        }

        // Tallennetaan rivin osoite solmuun
        pUusi->pRivi = strdup(rivi);

        // Määritetään seuraavan solmun osoite NULL:iksi
        pUusi->pSeuraava = NULL;

        // Lisätään uusi solmu linkitettyyn listaan
        if (pAlku == NULL) {
            pUusi->pEdellinen = NULL;
            pAlku = pUusi;
            pLoppu = pUusi;
        } else {
            pLoppu->pSeuraava = pUusi;
            pUusi->pEdellinen = pLoppu;
            pLoppu = pUusi;
        }

        // Luetaan uusi rivi
        pituus = getline(&rivi,&len,inputTiedosto);

        // Päivitetään apumuuttuja
        tyhjyysTarkistus++;
    }
    
    free(rivi);
    fclose(inputTiedosto);

    // Tarkistetaan, oliko inputTiedosto tyhjä.
    if (tyhjyysTarkistus < 2) {
        fprintf(stderr, "Input file is empty\n");
        fclose(outputTiedosto);
        exit(1);
    }

    // Uuden tiedoston kirjoittaminen
    ptr = pLoppu;
    while (ptr != NULL) {
        fprintf(outputTiedosto,"%s",ptr->pRivi);
        ptr = ptr->pEdellinen;
    }
    fclose(outputTiedosto);

    // Muistin vapautus
    ptr = pAlku;
    while (ptr != NULL) {
        pAlku = ptr->pSeuraava;
        free(ptr);
        ptr = pAlku;
    }
    return;
}

void parametreja2(char *argv[]) {
    char *rivi = NULL;
    size_t len = 0;
    ssize_t pituus = 1;
    // Avataan tiedostot
    FILE *inputTiedosto = fopen(argv[1],"r");

    // Tarkistetaan, että tiedoston avaaminen onnistui
    if (inputTiedosto == NULL) {
        fprintf(stderr, "error: cannot open file '%s'\n",argv[1]);
        exit(1);
    }

    // Alustetaan osoittimet
    YKSIRIVI *pAlku = NULL, *pLoppu = NULL, *pUusi, *ptr;

    // Luetaan ensimmäinen rivi.
    pituus = getline(&rivi,&len,inputTiedosto);

    // Luodaan apumuuttuja, sitä varten, että tiedetään, jos inputTiedosto olisikin tyhjä.
    int tyhjyysTarkistus = 0;

    // Lisätään rivit linkitettyyn listaan while-loopin avulla.
    while (pituus > 0) {

        // Varataan muisti ja tarkistetaan, että se onnistuu
        if ((pUusi = (YKSIRIVI*)malloc(sizeof(YKSIRIVI))) == NULL) {
            fprintf(stderr, "malloc failed\n");
            free(rivi);
            fclose(inputTiedosto);
            exit(1);
        }

        // Tallennetaan rivin osoite solmuun
        pUusi->pRivi = strdup(rivi);
        pUusi->pSeuraava = NULL;

        // Lisätään solmu linkitettyyn listaan
        if (pAlku == NULL) {
            pUusi->pEdellinen = NULL;
            pAlku = pUusi;
            pLoppu = pUusi;
        } else {
            pLoppu->pSeuraava = pUusi;
            pUusi->pEdellinen = pLoppu;
            pLoppu = pUusi;
        }

        // Luetaan seuraava rivi
        pituus = getline(&rivi,&len,inputTiedosto);

        // Päivitetään apumuuttuja
        tyhjyysTarkistus++;
    }
    
    free(rivi);
    fclose(inputTiedosto);

    // Tarkistetaan, oliko inputTiedosto tyhjä
    if (tyhjyysTarkistus < 2) {
        fprintf(stderr, "Input file is empty\n");
        exit(1);
    }

    // Kirjoitaan rivit standard output:iin.
    ptr = pLoppu;
    while (ptr != NULL) {
        printf("%s",ptr->pRivi);
        ptr = ptr->pEdellinen;
    }

    // Muistin vapautus
    ptr = pAlku;
    while (ptr != NULL) {
        pAlku = ptr->pSeuraava;
        free(ptr);
        ptr = pAlku;
    }
    return;
}

void parametreja1(char *argv[]) {
    char *rivi = NULL;
    size_t len = 0;
    ssize_t pituus = 1;

    // Alustetaan osoittimet
    YKSIRIVI *pAlku = NULL, *pLoppu = NULL, *pUusi, *ptr;

    // Luetaan ensimmäinen rivi.
    pituus = getline(&rivi,&len,stdin);

    // Luodaan apumuuttuja, sitä varten, että tiedetään, jos stdin olisikin tyhjä.
    int tyhjyysTarkistus = 0;

    // Lisätään rivit linkitettyyn listaan while-loopin avulla.
    while (pituus > 0) {

        // Varataan muisti ja tarkistetaan, että se onnistuu
        if ((pUusi = (YKSIRIVI*)malloc(sizeof(YKSIRIVI))) == NULL) {
            fprintf(stderr, "malloc failed\n");
            free(rivi);
            exit(1);
        }

         // Tallennetaan rivin osoite solmuun
        pUusi->pRivi = strdup(rivi);

        pUusi->pSeuraava = NULL;

        // Lisätään uusi solmu linkitettyyn listaan
        if (pAlku == NULL) {
            pUusi->pEdellinen = NULL;
            pAlku = pUusi;
            pLoppu = pUusi;
        } else {
            pLoppu->pSeuraava = pUusi;
            pUusi->pEdellinen = pLoppu;
            pLoppu = pUusi;
        }

        // Luetaan uusi rivi
        pituus = getline(&rivi,&len,stdin);

        // Päivitetään apumuuttuja
        tyhjyysTarkistus++;
    }
    

    // Tarkistetaan, oliko stdin tyhjä
    if (tyhjyysTarkistus < 2) {
        fprintf(stderr, "stdin is empty\n");
        exit(1);
    }

    free(rivi);

    // Kirjoitaan rivit standard output:iin.
    ptr = pLoppu;
    printf("\n");
    while (ptr != NULL) {
        printf("%s",ptr->pRivi);
        ptr = ptr->pEdellinen;
    }

    // Muistin vapautus
    ptr = pAlku;
    while (ptr != NULL) {
        pAlku = ptr->pSeuraava;
        free(ptr);
        ptr = pAlku;
    }
    return;
}

