#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {

    // Alustetaan muuttujat
    char *vanha = NULL, *uusi= NULL;
    int luku = 1, ascii, b=0; // Muodostetaan apumuuttaja b loopin ensimmäistä kierrosta varten. 

    // Varataan muistia ja tarkistetaan, että muistin varaaminen onnistui.
    vanha = (char *)malloc(2*sizeof(char));
    if (vanha == NULL) {
        printf("Muistin tallentaminen epäonnistui. ");
        exit(1);
    }

    uusi =  (char *)malloc(2*sizeof(char));
    if (uusi == NULL) {
        free(vanha);
        printf("Muistin tallentaminen epäonnistui. ");
        exit(1);
    }

    for(int tiedostoIndeksi = 1; tiedostoIndeksi<argc;tiedostoIndeksi++)  {
        // Avataan tiedosto ja tarkistetaan, että avaaminen onnistui
        FILE *tiedosto;
        if ((tiedosto = fopen(argv[tiedostoIndeksi],"r")) == NULL) {
            printf("Avaaminen epäonnistui");
            exit(1);
        }

        // Käydään while loopissa tiedoston merkit läpi
        while (fgets(uusi,2,tiedosto) != NULL) {
            // Jos merkit eroavat toisistaan ollaan saavuttu uuden merkin kohdalle. 
            if (strstr(vanha,uusi) == NULL ) {
                if (b == 1) {
                    ascii = (int)*vanha;
                    fwrite(&luku,4,1,stdout);
                    fwrite(&ascii,1,1,stdout);
                    luku = 1;
                }
            // Jos merkit ovat samat lisätään lukua, joka kertoo, kuinka monta merkkiä on peräkkäin.
            } else {
                if (b == 1) {
                    luku++;
                }      
            }
            // Määritetään uusi merkki vanhaksi
            vanha = strdup(uusi);
            b = 1;
        }
    
        // Kirjoitetaan tiedoston viimeinen merkki loopin ulkopuolella.
        ascii = (int)*vanha;
        fwrite(&luku,4,1,stdout);
        fwrite(&ascii,1,1,stdout);

        // Suljetaan tiedosto. 
        fclose(tiedosto);
    }

    // Vapautetaan muisti.
    free(vanha);
    free(uusi);

    return(0);

    

}