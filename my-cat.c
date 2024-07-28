#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {

    // Tarkistetaan, annettiinko yhtäkään tulostettavaa tiedostoa.
    if (argc == 1) {
        return(0);
    }

    // Tehdään askeltaja
    int i = 1;

    while (i < argc) {
        // Alustetaan getline funktion vaatimat muuttujat
        char *rivi = NULL;
        size_t pituus = 0;
        ssize_t merkitRivilla = 1;

        // Tiedoston avaaminen
        FILE *tiedosto = fopen(argv[i],"r");
        if (tiedosto == NULL) {
            fprintf(stderr,"my-cat: cannot open file\n");
            exit(1);
        }

        // Luetaan ensimmäinen rivi.
        merkitRivilla = getline(&rivi,&pituus,tiedosto);

        // Tarkistetaan, onko tiedosto olemassa tai onko se tyhjä
        if (merkitRivilla == -1) {
            return(0);
        }

        // Kirjoitetaan tiedosto stdin:iin.
        while (merkitRivilla > 0) {
            printf("%s",rivi);
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