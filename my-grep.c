#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// Luodaan tietue stdin lukemista varten
typedef struct solmu {
    char *pRivi;
    struct solmu *pSeuraava;
} SOLMU;

void parametreja2(char *argv[]);

int main(int argc, char *argv[]) {

    // Tarkistetaan, että komentoriviparametrejä annettiin oikea määrä
    if (argc == 1) {
        printf("my-grep: searchterm [file ...]\n");
        exit(1);
    }
    else if (argc == 2) {
        parametreja2(argv);
        return(0);
    }

    // Tehdään askeltaja
    int i = 2;

    while (i < argc) {
        // Alustetaan getline funktion vaatimat muuttujat
        char *rivi = NULL;
        size_t pituus = 0;
        ssize_t merkitRivilla = 1;

        // Tiedoston avaaminen
        FILE *tiedosto = fopen(argv[i],"r");
        if (tiedosto == NULL) {
            fprintf(stderr,"my-grep: cannot open file\n");
            exit(1);
        }

        // Luetaan ensimmäinen rivi.
        merkitRivilla = getline(&rivi,&pituus,tiedosto);

        // Tarkistetaan, onko tiedosto tyhjä
        if (merkitRivilla == -1) {
            return(0);
        }

        // Kirjoitetaan hyväksytyt rivit stdout:iin.
        while (merkitRivilla > 0) {
            if (strstr(rivi,argv[1]) != NULL) {
                printf("%s",rivi);
            }
            merkitRivilla = getline(&rivi,&pituus,tiedosto);
        }

        // Suljetaan tiedosto
        fclose(tiedosto);

        // Tehdään rivinvaihto tiedostojen välille
        printf("\n");
        // Päivitetään askeltaja
        i++;
    }
    return(0);
}

void parametreja2(char *argv[]) {
    char *rivi = NULL;
    size_t pituus = 0;
    ssize_t merkitRivilla = 1;

    // Luetaan ensimmäinen rivi.
    merkitRivilla = getline(&rivi,&pituus,stdin);

    // Tarkistetaan, onko stdin tyhjä
    if (merkitRivilla == -1) {
        return;
    }

    // Linkitetyn listan alustus
    SOLMU *pAlku = NULL, *pLoppu , *ptr, *pUusi;

    // Kirjoitetaan hyväksytyt rivit stdout:iin
    while (merkitRivilla > 0) {

        // Tallennetaan ensin hyväksytyt rivit linkitettyyn listaan.
        if (strstr(rivi,argv[1]) != NULL) {

            if ((pUusi = (SOLMU*)malloc(sizeof(SOLMU))) == NULL) {
                fprintf(stderr, "malloc failed\n");
                free(rivi);
            }

            pUusi->pRivi = strdup(rivi);
            pUusi->pSeuraava = NULL;
            if (pAlku == NULL) {
                pAlku = pUusi;
                pLoppu = pUusi;
            } else {
                pLoppu->pSeuraava = pUusi;
                pLoppu = pUusi;
            }
        }
        merkitRivilla = getline(&rivi,&pituus,stdin);
    }

    // Tulostetaan sitten hyväksytyt rivit linkitetystä listasta

    printf("\n"); // Jätetään väli stdout ja stdin väliin

    ptr = pAlku;
    while (ptr != NULL) {
        printf("%s",ptr->pRivi);
        ptr = ptr->pSeuraava;
    }
    return;

    // Muistin vapautus
    ptr = pAlku;
    while (ptr != NULL) {
        pAlku = ptr->pSeuraava;
        free(ptr);
        ptr = pAlku;
    }

    return;


}