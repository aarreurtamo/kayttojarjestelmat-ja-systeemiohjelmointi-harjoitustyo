#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
 
    for(int tiedostoIndeksi = 1; tiedostoIndeksi < argc; tiedostoIndeksi++) {
        // Avataan tiedosto.
        FILE *tiedosto;
        if ((tiedosto = fopen(argv[tiedostoIndeksi],"r")) == NULL) {
            printf("Avaaminen epäonnistui");
            exit(1);
        }

        // Alustetaan tarvittavat muuttujat
        int kirjain, luku, i;

        // Käydään tiedoston merkit läpi while-loopissa
        while (1) {

            // Luetaan merkin määrä
            fread(&luku,4,1,tiedosto);

            // Luetaan, mikä merkki on kyseessä
            fread(&kirjain,1,1,tiedosto);

            // Katsotaan, että merkki kuuluu ASCIIn
            if (kirjain > 128) {
                fprintf(stderr,"Invalid file.");
            }

            // Jos ollaan tiedoston lopussa, breakataan loopista.
            if (feof(tiedosto) != 0) {
                break;
            }
            // Kirjoitetaan merkkejä niin kauan, kun askeltaja on pienempi, kuin merkkien määrä
            i = 0;
            while (i < luku) {
                printf("%c",kirjain);
                i++;
            }
        }
        // Suljetaan tiedosto ja lopetetaan ohjelma
        fclose(tiedosto);
    }
    return(0);
}