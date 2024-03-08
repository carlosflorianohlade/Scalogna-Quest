// Progetto finale - Hlade Carlos Floriano
#include "gamelib.h"
#include <stdlib.h>
#include <stdio.h>

int main(void)
{
    unsigned short opzione = 0;
    int c = 0;

    printf("   _   _   _   _   _   _   _   _     _     _   _   _   _   _   \n");
    printf("  / \\ / \\ / \\ / \\ / \\ / \\ / \\ / \\   / \\   / \\ / \\ / \\ / \\ / \\  \n");
    printf(" ( S | c | a | l | o | g | n | a ) ( - ) ( Q | u | e | s | t ) \n");
    printf("  \\_/ \\_/ \\_/ \\_/ \\_/ \\_/ \\_/ \\_/   \\_/   \\_/ \\_/ \\_/ \\_/ \\_/  \n");

    do
    {
        printf("> \033[1;93m1\033[0m: Imposta Gioco.\n");
        printf("> \033[1;96m2\033[0m: Gioca.\n");
        printf("> \033[1;91m3\033[0m: Esci.\n\n");

        printf("\033[92mScelta:\033[0m ");
        scanf("%hu", &opzione);
        while ((c = getchar()) != '\n' && c != EOF); // Pulizia buffer

        switch (opzione)
        {
        case 1:
            imposta_gioco();
            opzione = 0; // Variabile resettata per impedire errori di input
            break;

        case 2:
            gioca();
            opzione = 0;
            break;

        case 3:
            termina_gioco();
            printf("è stato un piacere :)\n");
            break;

        default:
            printf("\033[31mAttenzione!\033[0m Opzione non valida, per favore inserisci \033[31mun numero da 1 a 3\033[0m.\n");
        }
    } while (opzione != 3);
    return 0;
}