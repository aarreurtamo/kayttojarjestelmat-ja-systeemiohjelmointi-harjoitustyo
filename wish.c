#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/wait.h>
#include<stdbool.h>
#include <fcntl.h>


/* Luodaan ja määritetään LINKEDLISTITEM-rakenne linkitettyä listaa varten */
typedef struct LinkedListItem {
    char *pValue;  /* Linkitetyn listan alkion arvo*/
    struct LinkedListItem *pNext; /* osoitin seuraavaan alkioon*/
} LINKEDLISTITEM;


/* runProgram-funktio, sisältää toiminnallisuuden joka liittyy itse ohjelman ajamiseen execv-funktiolla.
Tämän funktion sisälle on myös rakennetty toiminnallisuus, joka liittyy outputin uudelleen ohjaamiseen. */
int runProgram(char *args[],char* currentPath,char* redirectionFile);

/* myExit on sisäänrakennettu funktio, johon on implementoitu lopetusehdon toteutumisen tarkastaminen.*/
int myExit(char *line);

/* newLinkedListItem-funktio luo uuden alkion linkitettyyn listaan*/
LINKEDLISTITEM *newLinkedListItem(char *itemValue,LINKEDLISTITEM *pStart);

/* memoryRelease-funktio, vapauttaa linkitetyn listan varaaman muistin */
void memoryRelease(LINKEDLISTITEM *pStart);

/* myCd on sisään rakennettu funktio, jolla vaihdetaan kansiota, jossa työskennellään.*/
void myCd(char *uusi);

/* myPath on sisään rakennetty funktio, jolla voidaan tallentaa linkitettyyn listaan uudet polut.*/
LINKEDLISTITEM *myPath(char *arguments[],LINKEDLISTITEM* pFirstPath);

/* getRigthPath-funktio etsii polut sisältävästä linkitetystä listasta polun, josta syötetty komento löytyy*/
char *getRigthPath(LINKEDLISTITEM *pFirstPath,char *commandName);

/* makeArrey-funktio tallentaa komennon argumentit execv-funktion vaatimaan muotoon arreyksi. */
char** makeArrey(char *line,char* splitSign);

/* bashMode-funktio hoitaa bash-moodin ajamisen*/
LINKEDLISTITEM* bashMode(int argc, char *argv[],LINKEDLISTITEM* pFirstPath,char** assumptionPaths);

/* runningNavigation-funktio tarkistaa, onko kyseessä sisäänrakennettu funktio vai ei, ja kutsuu sen perusteella tarvittavia aliohjelmia.*/
LINKEDLISTITEM* runningNavigation(char** arguments,LINKEDLISTITEM *pFirstPath,char* redirectionFile);

/* parallelHelp-funktiota kutsutaan aina kun ohjelmaa ajetaan. Se tarkistaa onko kyseessä yksittäinen vai rinnakkainen ajo ja kutsuu tämän perusteella
   tarvittavia aliohjelmia. */
LINKEDLISTITEM* parallelHelp(char** assumptionPaths, LINKEDLISTITEM* pFirstPath,char *line );


/*--  PÄÄOHJELMA -- */

int main(int argc, char *argv[]) {

    /* Tarkistetaan argumenttejen määrä. */
    if (argc > 2) {
        char error_message[30] = "An error has occurred\n";
        write(STDERR_FILENO, error_message, strlen(error_message));
        exit(1);
    }

    /* -------------------------  OLETUSPOLUN ASETTAMINEN ------------------------- */

    /*Aloitetaan ohjelma asettamalla oletuspolku "/bin". Luodaan kolmen alkion arrey ja ladataan keskimmäiseen alkioon lukujono "/bin".*/
    
    char** assumptionPaths = (char**)malloc(3*sizeof(char*));

    if (assumptionPaths == NULL) {
        char error_message[30] = "An error has occurred\n";
        write(STDERR_FILENO, error_message, strlen(error_message));
        exit(1);
    }

    assumptionPaths[1] = strdup("/bin");

    LINKEDLISTITEM* pFirstPath = NULL;

    /*Asetataan oletuspolku kutsumalla myPath funktiota*/
    pFirstPath = myPath(assumptionPaths,pFirstPath);
    

    /* -------------------------  BASH-MOODISSA AJAMINEN ------------------------- */

    /*Katsotaan, tarvitseeko ajaa bath-muodossa, jos tarvitsee kutsutaan bashMode funktiota*/
    if (argc == 2) {
        pFirstPath = bashMode(argc,argv,pFirstPath,assumptionPaths);
        if (pFirstPath != NULL) {
            if (strstr(pFirstPath->pValue,"exit") != NULL) {
                memoryRelease(pFirstPath);
                free(assumptionPaths);
                return(0);
            }
        }
    }
    
    /* -------------------------  INTERACTIVE-MOODISSA AJAMINEN ------------------------- */

    /* Alusteen muttujat getline funktiota varten, jotta voidaan lukua komentoriviltä.*/
    char *line = NULL;
    size_t len = 0;

    /*Muodostetaan while-looppi, jota jatketaan exit komentoon asti*/
    while(true) {

        printf("wish> "); 
        
        /*Kutsutaan getline-funktiota, jotta saadaan syötetty komento(t).*/
        getline(&line,&len,stdin);

        /*Tarkistetaan, oliko syötetty komento exit komento*/
        if (myExit(line) == 0) {
            /*poistutaan loopista break komennolla, mikäli exit käsky kutsuttiin*/
            break;
       } 

        /* Suoritetaan ohjelman ajaminen parallelHelp-funktion avulla. */
        pFirstPath = parallelHelp(assumptionPaths,pFirstPath,line);
    }

    /* -------------------------  Ohjelman lopetus ------------------------- */
    
    /*Vapautetaan polut sisältäneen linkityslistan viemä muisti. Jos polkua ei ole asetettu, muistia ei ole varattu.*/
    if (pFirstPath != NULL) {
        memoryRelease(pFirstPath);
    }
    /*Vapautetaan oletuspolun asettamiseen varattu muisti*/
    free(assumptionPaths);
    
    /*Lopetetaan ohjelma*/
    return(0);
}

int runProgram(char *commandArguments[], char* currentPath, char* redirectionFile) {
    int returnValue = 0;

    /* Varataan muisti, polulle, johon on lisätty perään komennon nimi */
    char* extendedCurrentPath = (char*)malloc(sizeof(currentPath)+sizeof(commandArguments[0])+sizeof(char)*3);

    /* Tarkistetaan, että muistin varaus onnistui. */
    if (extendedCurrentPath == NULL) {
        char error_message[30] = "An error has occurred\n";
        write(STDERR_FILENO, error_message, strlen(error_message));
        return returnValue;
    }

    /* Kirjoitetaan polku muuttujaan extendedCurrentPath */
    sprintf(extendedCurrentPath,"%s/%s",currentPath,commandArguments[0]);


    int rc = fork();
    /* Katsotaan, että uuden prosessin luominnen onnistui. */
    if (rc < 0) {
        char error_message[30] = "An error has occurred\n";
        write(STDERR_FILENO, error_message, strlen(error_message));
        return returnValue;

    /* Katsotaan, taas exev-funktion onnistuminen. */
    } else if (rc == 0) {

        /* Katsotaan, onko kyseessä uudelleenohjaus. */
        if (redirectionFile != NULL) {
            char* redirectionFile1 = strtok(redirectionFile," \n");

            /* Avataan tiedosto, joka löytyyy commandArguments arrayn viimeisestä alkiosta. */
            int myFile = open(redirectionFile1, O_WRONLY | O_CREAT | O_TRUNC,0644);

            /* Tarkistetaan, että tiedoston avaaminen onnistui. */
            if (myFile < 0) {
                char error_message[30] = "An error has occurred\n";
                write(STDERR_FILENO, error_message, strlen(error_message));
                return returnValue;
            }

            /* Ohjataan standard-output ja standard error tiedostoon myFile. */
            dup2(myFile,STDOUT_FILENO);
            dup2(myFile,STDERR_FILENO);

            /* Suljetaan tiedosto, johon ajettu ohjelma kirjoitettiin. */
            close(myFile);
        }

        /* Ajetaan komento execv-funktiolla */
        if (execv(extendedCurrentPath,commandArguments) == -1) {
            char error_message[30] = "An error has occurred\n";
            write(STDERR_FILENO, error_message, strlen(error_message));
            returnValue = -1;
        };
    /* Jos kyseessä on alkuperäinen prosessi odotellaan, lapsiprosessejan valmistumista. */
    } else {
        wait(NULL);
    }
    
    
    /* Vapautetaan extendedCurrentPath- arrayn viemä muisti. */
    free(extendedCurrentPath);

    /* Palautetaan returnValue. Jos kaikki meni hyvin paluuarvo on 0. */
    return returnValue;

    /* Jos execv komento ei onnistunut uusi lapsiprosessi on edelleen käynnissä, jolloin se on lopetettava hallitusti.
        Funktio runProgarm antaa tästä tiedon sitä kusuvalle ohjelmalle  palauttamalla arvon -1. */
}


int myExit(char *line) {

    /* myExit-funktio palauttaa arvon 1, jos ohjelmaa ei lopeteta ja arvon 0 jos ohjelma lopetetaan. */

    /* Kopioidaan line osoitin osoittimeen lineCopy.  */
    char *lineCopy;

    /* Varataan muisti lineCopy osoittimeen. */
    lineCopy = (char*)malloc(strlen(line)*sizeof(char));

    /* Tarkistetaan, että muistin varaus onnistui. */
    if (lineCopy == NULL) {
        char error_message[30] = "An error has occurred\n";
        write(STDERR_FILENO, error_message, strlen(error_message));
        return 0;
    }

    /* Tehdään kopiointi. */
    strcpy(lineCopy,line);

    /* Pilkotaan ensimmäinen argumentti, ja lopetetaan, jos se on "exit". */
    char* itemValue = strtok(lineCopy," \n");

    /* Katsotaan, oliko komentorivi tyhjä. */
    if (itemValue == NULL) {
        free(lineCopy);
        return 1;
    }
    /* Tarkistetaan, toteutuuko lopetusehto. */
    if (strstr(itemValue,"exit")  != NULL) {
        if (strlen(itemValue) == strlen("exit")) {
            /* Vapautetaan muisti lineCopyn viemä muisti. */
            free(lineCopy);
            return 0;
        }
    }
    free(lineCopy);
    return 1;
}

LINKEDLISTITEM* newLinkedListItem(char *itemValue,LINKEDLISTITEM *pStart) {

    /* Määritetään uuden alkion osoitin. */
    LINKEDLISTITEM *pNew;

    /* Varataan muisti uudelle alkiolle ja tarkistetaan, että muistin varaaminen onnistui. */
    if ((pNew = (LINKEDLISTITEM*)malloc(sizeof(LINKEDLISTITEM))) == NULL) {
        char error_message[30] = "An error has occurred\n";
        write(STDERR_FILENO, error_message, strlen(error_message));
        memoryRelease(pStart);
        return NULL;
    }

    /* Asetetaan uuden alkion jäsenmuuttujat. */
    pNew->pValue = strdup(itemValue);
    pNew->pNext = NULL;
    return pNew;
}

void memoryRelease(LINKEDLISTITEM *pStart) {

    /* Määritetään apumuuttujaksi osoitin pTemp. */
    LINKEDLISTITEM* pTemp = pStart;

    /* Käydään linkitetyn listan alkiot läpi ja vapautetaan ne. */
    while (pTemp != NULL) {
        pStart = pTemp->pNext;
        free(pTemp);
        pTemp = pStart;
    }
    return;

}

void myCd(char *newWorkingFolder) {
    /* Vaihdetetaan working folder käyttäen chdir-funktiota. */
    chdir(newWorkingFolder);
    return;
}

LINKEDLISTITEM *myPath(char *arguments[],LINKEDLISTITEM* pFirstPath) {

    /* Alustetaan askeltaja index arvoon 1, koska arguments arreyn ensimmäinen alkio 
    sisältää itse komennon path*/
    int index = 1;

    /* Vapautetaan vanhojen polkujen viemä muisti, jos niitä on olemassa. */
    if (pFirstPath != NULL) {
        memoryRelease(pFirstPath);
    }

    /* Alustetaan ensimmäinen alkio NULL:iksi ja luodaan pointerit linkitetyn listan 
    ensimmäiselle ja viimeiselle alkiolle. */
    pFirstPath = NULL;
    LINKEDLISTITEM *pNewPath, *pEndPath;

    /* Käydään argumentit läpi while-loopissa ja lisätään ne linkitettyyn listaan.*/
    while (arguments[index] != NULL) {

        /* Kutsutaan uuden alkion luomiseksi newLinkedListItem-funktiota. */
        pNewPath = newLinkedListItem(arguments[index],pFirstPath);

        /* Tarkistetaan, että alkion luomien onnistui.*/
        if (pNewPath == NULL) {
            char error_message[30] = "An error has occurred\n";
            write(STDERR_FILENO, error_message, strlen(error_message));
            return NULL;
        }
        if (pFirstPath == NULL) {
            pFirstPath = pNewPath;
            pEndPath = pNewPath;
        } else {
            pEndPath->pNext =pNewPath;
            pEndPath = pNewPath;
        }
        /* Korotetaan askeltajaa. */
        index++;
    }
    /* Palauetaan linkitetyn listan ensimmäisen alkion osositin. */
    return pFirstPath;
}

char *getRigthPath(LINKEDLISTITEM *pFirstPath,char *commandName) {

    /* Alustetaan apumuuttuja, johon tallennetaan polku ja etsittävä ohjelma. */
    char* currentPath = NULL;

    /* Alustetaan osoitin polut sisältävän linkitetyn listan ensimmäiseen alkioon. */
    LINKEDLISTITEM* pTemp = pFirstPath;

    /* Käydään while-loopissa polut läpi ja tarkistetaan access-funktiolla, löytyykö etsitty komento jostain polusta */
    while (pTemp != NULL) {

        /* Vartaan muisti apumuuttujaan. */
        currentPath = (char*)malloc(sizeof(pTemp->pValue)+sizeof(commandName)+sizeof(char)*2);

        /* Tarkistetaan, että muistin varaaminen onnistui. */
        if (currentPath == NULL) {
            char error_message[30] = "An error has occurred\n";
            write(STDERR_FILENO, error_message, strlen(error_message));
            return NULL;
        }

        /* Kirjoitetaan polku, johon on lisätty komennon nimi perään, muuttujaan currentPath. */
        sprintf(currentPath,"%s/%s",pTemp->pValue,commandName);
        if (access(currentPath,X_OK) == 0) {
            /* Jos polku löyty palautetaan se ja vapautetaan apumuuttujan viemä muisti. */
            free(currentPath);
            return pTemp->pValue;
        }
        /* Etsitään uusi alkio linkitetystä listasta ja tyhjennetään currentPath apumuuttujan varaama muisti.  */
        pTemp=pTemp->pNext;
        free(currentPath);
    }

    /* Jos polkua ei löytynyt palautetaan NULL*/
    return NULL;
}


LINKEDLISTITEM* bashMode(int argc, char *argv[],LINKEDLISTITEM* pFirstPath,char** assumptionPaths) {

    /* Avataan tiedosto. */
    FILE* myFile = NULL;
    myFile = fopen(argv[1],"r");
    if (myFile == NULL) {
        free(assumptionPaths);
        memoryRelease(pFirstPath);
        exit(1);
    }

    /* Määritellään getline-funktion tarvitsemat parametrit. */
    char *line = NULL;
    size_t len = 0;  
    ssize_t read;

    /* Alustetaan linkitetyn listan hallintaan tarvittavat muuttujat. */
    LINKEDLISTITEM *pFirstLine, *pEndLine, *pNewLine, *pTemp;
    pFirstLine = pEndLine = NULL;


    /* Luetaan tiedosto linkitettyyn listaan while-loopissa*/
    while(feof(myFile) == 0) {
        read = getline(&line,&len,myFile);
        if  (read == -1) {
            /* Tarkistetaan, oliko vielä jäljellä rivi, joka voidaan lukea. */
            break;       
        } else if (read > 1) {
            pNewLine = newLinkedListItem(line,pFirstLine);
            /* Tarkistetaan, että uuden alkion luominen onnistui. */
            if (pNewLine == NULL) {
                char error_message[30] = "An error has occurred\n";
                write(STDERR_FILENO, error_message, strlen(error_message));
                return NULL;
            }
            /* Katsotaan, oliko kyseessö linkitetyn listan ensimmäinen alkio. */
            if (pFirstLine == NULL) {
                pFirstLine = pNewLine;
                pEndLine = pNewLine;
            } else {
                pEndLine->pNext = pNewLine;
                pEndLine = pNewLine;
            }
        }      
    }  

    fclose(myFile);

    /* Ajetaan rivit linkitetystä listasta. */
    pTemp = pFirstLine;
    while(pTemp != NULL) {
        /*Tarkistetaan, oliko syötetty komento exit komento*/
        int exitValue = myExit(pTemp->pValue);
        if ( exitValue == 0) {
            if (pFirstPath != NULL) {
                memoryRelease(pFirstPath);
            } 
            /* Määritetään pFirstPath alkioon lopetusehto. */
            pFirstPath = newLinkedListItem("exit",pFirstPath);
           /*poistutaan loopista break komennolla, mikäli exit käsky kutsuttiin*/
            break;
       } 
        pFirstPath = parallelHelp(assumptionPaths,pFirstPath,pTemp->pValue);
        pTemp = pTemp->pNext;
    }

    /* Vapautetaan linkitetyn listan viemä muisti. */
    memoryRelease(pFirstLine);

    return pFirstPath;
}

char** makeArrey(char *line,char* splitSign) {

    /* Määritetään tarvittavat osoittimet linkitetyn listan ylläpitoon  */
    LINKEDLISTITEM *pStart, *pEnd , *pTemp, *pNew;

    /* Alustetaan uusi osoitin ja alun osoitin NULL:iksi. */
    pStart = pNew = NULL;

    /* Alustetaan askeltaja, joka laskee linkitetyn listan alkoiden määrän.  */
    int index1 = 0;


    /* Pilkotaan ensimmäisen alkion arvo. */
    char* itemValue = strtok(line,splitSign);

    /* Tarkistetaan, oliko pilkottava muuttuja tyhjä. */
    if (itemValue == NULL) {
        return NULL;
    }

    /* Tallennetaan pilkotut alkiot linkitettyyn listaan while-loopissa. */
    while (itemValue != NULL) {
        /* Muodostetaan uusi alkio. */
        pNew = newLinkedListItem(itemValue,pStart);

        /* Tarkistaan, että alkion muodostaminen onnistui. */
        if (pNew == NULL) {
            char error_message[30] = "An error has occurred\n";
            write(STDERR_FILENO, error_message, strlen(error_message));
            return NULL;
        }
        if (pStart == NULL) {
            pStart = pNew;
            pEnd = pNew;
        } else {
            pEnd->pNext = pNew;
            pEnd = pNew;
        }  
        itemValue = strtok(NULL,splitSign);
        index1++;
    }

    /* Määritetään uusi askeltaja arrayn muodostamista varten. */
    int index2 = 0;

    /*Varataan muisti arraylle ja tallennetaan linkitetyn listan alkiot siihen.  */
    pTemp = pStart;

    char** arguments = (char**)malloc((index1+1)*sizeof(char*));

    while(index2 < index1) {
        arguments[index2] = strdup(pTemp->pValue);
        index2++;
        pTemp = pTemp->pNext;
    }

    /* Määritetään arrayn viimeinen alkio nulliksi, (execv funktion parametrin vaatima muoto)*/
    arguments[index2] = NULL;

    /* Vapautetetaan linkitetyn listan viemä muisti. */
    memoryRelease(pStart);

    /* Palautetaan muodostettu array */
    return arguments;
}


LINKEDLISTITEM* runningNavigation(char** arguments,LINKEDLISTITEM *pFirstPath, char* redirectionFile) {
    /* Alustetaan muuttuja nykyiselle polulle. */
    char* currentPath; 

    /* Muodostetaan valinta rakenne, jolla tarkistetaan onko kyseessä sisäänrakennettu funktio. */


    if (strstr(arguments[0],"cd") != NULL) {
        /*Jos kyseessä oli cd funktio ajataan se antamalla parametriksi komennon toinen argumentti arguments[1] */
            myCd(arguments[1]);
    } else if (strstr(arguments[0],"path") != NULL) {
        /* Jos kyseessä on path funktio, ajetaan myPath funktio antamalla sille argumenteiksi komennon argumentit 
          sekä osoitin polut sisältävän linkitetyn listan ensimmäiseen alkioon*/
        pFirstPath = myPath(arguments,pFirstPath);
    } else {
        /* Jos kyseessä ei ollut sisäänrakennettu funktio, etsitään ensin löydetäänkö komento, jostain nykyisestä polusta
           kutsumalla getRigthPath-funktiota ja antamlla sille parametreiksi komennon ensimmäinen argumentti sekä osoitin 
           polut sisältävän linkitetyn listan ensimmäiseen alkioon*/
        currentPath = getRigthPath(pFirstPath,arguments[0]);

        /* Jos komento löydetään, jostain polusta ajetaan se*/
        if (currentPath != NULL) {
            /* Käytetään ajamiseen runProgram-funktiota. Annetaan parametreiksi komennon argumentit, polku, josta komento löytyy 
            sekä tiedosto, johon mahdollisesti uudenlleenohjataan ohjelman output.*/
            if (runProgram(arguments,currentPath,redirectionFile) == -1) {
                /* Jos ohjelmanajaminen epäonnistuu lopetetaan ohjelma hallitusti. */
                memoryRelease(pFirstPath);
                pFirstPath = NULL;
                pFirstPath = newLinkedListItem("exit",pFirstPath);
            };
        /* Jos komentoa ei löytynyt kirjoitetaan virheilmoitus. */
        } else {
            char error_message[30] = "An error has occurred\n";
            write(STDERR_FILENO, error_message, strlen(error_message));
        }
    }
    /* Palautetaan osoitin  polut sisältävän linkitetyn listan ensimmäiseen alkioon. */
    return pFirstPath;
}


LINKEDLISTITEM* parallelHelp(char** assumptionPaths, LINKEDLISTITEM* pFirstPath,char *line ) {


     /* Pilkotaan komennot toisistaan &-merkistä*/
    char** commands = makeArrey(line,"&");


    /* Lasketaan komentojen määrä */
    int commandCounter = 0;
    while (commands[commandCounter] != NULL) {
        commandCounter++;
    }

    /* ------------ AJAMINEN KUN KOMENTOJA OLI VAIN YKSI ------------ */
    if (commandCounter < 2) {


        /* Pilkotaan komento ">"-merkistä mahdollista uudelleen ohjausta varten. */
        char* redirectionCommand = strtok(commands[0],">");

        /* Otetaan talteen tiedosto, johon output uudelleen ohjattaisiin*/
        char* reHelp = strtok(NULL,">");

        /* Jos argumentteja oli enemmän kuin yksi ">"-merkin jälkeen, niin annetaan virheilmoitus ja poistutaan aliohjelmasta */
        char* redirectionFile = strtok(reHelp," >\n");
        if (strtok(NULL," >\n") != NULL && redirectionFile != NULL) {
            char error_message[30] = "An error has occurred\n";
            write(STDERR_FILENO, error_message, strlen(error_message));
            free(commands);
            return pFirstPath;
        }


        /* Muodostetaan array komennon argumenteista makeArray funktiolla, 
            erottimena välilyönti ja rivin vaihtomerkki. */
        char** arguments = makeArrey(redirectionCommand," \n");

        /* Tarkistetaan, että ei annettu tyhjää komentoa. */
        if (arguments == NULL) {
            free(commands);
            return pFirstPath;
        }
        /* Kutsutaan runnigNavigation-funktiota uuden ohjelman ajamiseksi.*/
        pFirstPath = runningNavigation(arguments,pFirstPath,redirectionFile);

        /* Vapautetaan muodostetun arreyn viemä muisti */
        free(arguments);

        /* Tarkistetaan, onnistuiko ohjelman ajaminen execv-funktiolla, jos ei onnistunut lopetetaan hallitusti uusi prosessi, joka on muodostettu.*/
        if (strstr(pFirstPath->pValue,"exit") != NULL) {

            /* Vapautetaan polu sisältävän linkitetyn listan viemä muisti. */
            if (pFirstPath != NULL) {
                memoryRelease(pFirstPath);
            }

            /* Vapautetaan oletuspolku. */
            free(assumptionPaths);

            /* Lopetataan uusi alotettu prosessi. */
            return(0);
    }
    /* ------------ RINNAKKAINEN AJAMINEN ------------ */
    } else {
        /* Luodaan arrey, johon lisätään uusien prosessejen tunnisteet*/
        int pids[commandCounter];

        /* indeksin alustus*/
        int index = 0; 

        /* Ajetaaan komennot rinnakkain while-loopilla*/
        while (index < commandCounter) {
            /* Aloitetaan uusi prosessi */
            int pid = fork();

            /* Lisätään prosessin indeksi arrayhyn. */
            pids[index] = pid;

            /* Jos uusi prosessi juuri luotiin, juuri saatu tunniste on 0*/
            if (pid == 0) {

                /* Pilkotaan komento ">"-merkistä mahdollista uudelleen ohjausta varten. */
                char* redirectionCommand = strtok(commands[index],">");

                /* Otetaan talteen tiedosto, johon output uudelleen ohjattaisiin. */
                char* reHelp = strtok(NULL,">");

                /* Jos argumentteja oli enemmän kuin yksi ">"-merkin jälkeen, niin annetaan virheilmoitus ja poistutaan aliohjelmasta */
                char* redirectionFile = strtok(reHelp," >\n");
                if (strtok(NULL," >\n") != NULL && redirectionFile != NULL) {
                    char error_message[30] = "An error has occurred\n";
                    write(STDERR_FILENO, error_message, strlen(error_message));
                /* Muuten ajetaan ohjelma. */
                } else {
                    /* Muodostetaan array komennon argumenteista makeArray funktiolla, 
                    erottimena välilyönti ja rivin vaihtomerkki. */
                    char** arguments = makeArrey(redirectionCommand," \n");

                    /* Tarkistetaan löytyikö komento, jostain polusta.*/
                    char* currentPath = getRigthPath(pFirstPath,arguments[0]);

                    /* Jos komento löytyi, ajetaan se. */
                    if (currentPath != NULL) {
                        if (runProgram(arguments,currentPath,redirectionFile) == -1) {
                            memoryRelease(pFirstPath);
                            pFirstPath = NULL;
                            pFirstPath = newLinkedListItem("exit",pFirstPath);
                        }
                    /* Jos polkua ei löytynyt kirjoitetaan virheilmoitus. */
                    } else {
                        char error_message[30] = "An error has occurred\n";
                        write(STDERR_FILENO, error_message, strlen(error_message));
                    }

                    /* Vapautetaan komennon argumenteista muodostetun arreyn viemä muisti*/
                    free(arguments);
                }

                /* Vapautetaan polu sisältävän linkitetyn listan viemä muisti. */
                if (pFirstPath != NULL) {
                    memoryRelease(pFirstPath);
                }

                /* Vapautetaan oletuspolku. */
                free(assumptionPaths);

                /* Vapautetaan komentojen viemä tila*/
                free(commands);

                /* Lopetataan uusi alotettu prosessi. */
                exit(0);
            } else {
                /* Jos kyseessä oli vanha prosessi korotetaan indeksiä*/
                index++;
            }
        }
        
        /* Laitetaan alkuperäinen prosessi odottamaan muita prosesseja tunnisteet sisältävän arreyn pids avulla*/
        index = 0;
        while(index < commandCounter) {
            waitpid(pids[index],NULL,0);
            index++;
        }
    

    }
    /* Vapautetaan komennoille varattu muisti. */
    free(commands);

    /* Palautetaan osoitin polut sisältävän linkitetyn listan ensimmäiseen alkioon. */
    return pFirstPath;
}






