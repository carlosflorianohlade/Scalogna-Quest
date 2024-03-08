#include "gamelib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

Giocatore *giocatori[4] = {NULL, NULL, NULL, NULL};
Zona_segrete *pFirst = NULL;
Zona_segrete *pLast = NULL;

int c = 0; // Variabile di controllo input
unsigned short n_giocatori = 0;
unsigned short gioco_impostato = 0;                  // se è gia stato impostato va a 1 senno 0
unsigned short mappa_creata = 0;                     // se è stata creata correttamente va a 1 senno 0
unsigned short porta_aperta = 0;                     // se la porta è stata aperta va a 1 senno 0
unsigned short scappato = 0;                         // Se il giocatore è scappato va a 1 senno 0
unsigned short tesoro_preso = 0;                     // se il giocatore ha preso un tesoro va a 1 senno 0
static Giocatore *crea_giocatore(int);               
static void scegli_classe(Giocatore *giocatore);     
static void aumenta_sacrifica(Giocatore *giocatore); 
static int genera_random(int min, int max);          
static Abitante *generaAbitanteCasuale();            

// nomi
static char *nome_classe(Giocatore *giocatore); 
static char *nome_zona(Zona_segrete *zona);     
static char *nome_tesoro(Zona_segrete *tesoro);
static char *nome_porta(Zona_segrete *porta);   

static void pulisci_schermo(); // riga 451

// funzioni per la creazione della mappa
static enum Tipo_tesoro genera_tipo_tesoro();         
static enum Tipo_zona genera_tipo_zona();             
static enum Tipo_porta genera_tipo_porta();           
static void genera_mappa();                           
static void inserisci_zona(unsigned short posizione); 
static void cancella_zona(unsigned short posizione);  
static void cancella_all_zone();
static void stampa_mappa(); 
static int count_zone();    
static void chiudi_mappa();

// funzioni gioca
static void stampa_giocatore(int indice_giocatore);
static void stampa_zona(int indice_giocatore);
static void avanza(int giocatore_attuale);
static void indietreggia(int giocatore_attuale);
static void apri_porta(int giocatore_attuale);
static void prendi_tesoro(int giocatore_attuale);
static void scappa(int giocatore_attuale);
static void gioca_potere_speciale(Giocatore *giocatore_attuale, Abitante *abitante_attuale);
static void combatti(int indice_giocatore, Abitante *abitante_attuale);
static void menu_abitante(unsigned short scelta, int indice_giocatore, Zona_segrete *zona_attuale, Abitante *abitante);
static void attacco_giocatore(Giocatore *giocatore_attuale, Abitante *abitante_attuale);
static void attacco_abitante(Giocatore *giocatore_attuale, Abitante *abitante_attuale);
void imposta_gioco()
{
    unsigned short switch_mappa = 0;
    int posizione = 0;
    time_t t;
    srand((unsigned)time(&t));

    // controllo se il gioco è stato impostato
    if (gioco_impostato == 1)
    {
        printf("Il gioco è già stato impostato in precedenza, la vecchia configurazione verra eliminata.\n");
        for (int i = 0; i < n_giocatori; i++)
        {
            free(giocatori[i]);
        }
        cancella_all_zone();
        pFirst = NULL;
        pLast = NULL;
        gioco_impostato = 0;
        mappa_creata = 0;
    }

    // creazione giocatori
    do
    {
        printf("Inserisci il numero di giocatori (minimo 1, massimo 4): ");
        scanf("%hu", &n_giocatori);
        while ((c = getchar()) != '\n' && c != EOF); // Pulizia buffer
        if (n_giocatori < 1 || n_giocatori > 4)
            printf("\033[31mAttenzione!\033[0m Opzione non valida, per favore inserisci \033[31mun numero da 1 a 4\033[0m.\n");
    } while (n_giocatori < 1 || n_giocatori > 4);

    printf("Premere invio per continuare ...\n");
    while (getchar() != '\n');
    pulisci_schermo();

    // alloco dinamicamente la memoria per ogni giocatore
    for (int i = 0; i < n_giocatori; i++)
        giocatori[i] = crea_giocatore(i);

    // creazione mappa
    printf("Giocatori creati correttamente. Ora è il momento di creare la mappa. Premere invio per continuare ...\n");
    while (getchar() != '\n');
    pulisci_schermo();
    do
    {

        printf("Premi \033[1;93m1\033[0m per generare la mappa.\nPremi \033[1;93m2\033[0m per inserire una zona.\nPremi \033[1;93m3\033[0m per cancellare una zona.\nPremi \033[1;93m4\033[0m per stampare la mappa.\nPremi \033[1;93m5\033[0m per chiudere la creazione della mappa.\n");
        scanf("%hu", &switch_mappa);
        while ((c = getchar()) != '\n' && c != EOF); // Pulizia buffer
        switch (switch_mappa)
        {
        case 1:
            genera_mappa();
            break;
        case 2:
            do
            {
                printf("\nVuoi inserire una zona, dopo quale zona vuoi inserire questa nuova zona?\n");
                scanf("%d", &posizione);
                while ((c = getchar()) != '\n' && c != EOF); // Pulizia buffer

                if (posizione < 0 || posizione > count_zone())
                {
                    printf("\nInserisci un numero maggiore o uguale a 0 e minore o uguale a %d.", count_zone());
                }
            } while (posizione < 0 || posizione > count_zone());
            inserisci_zona(posizione);
            break;

        case 3:
            do
            {
                printf("\nQuale zona vuoi cancellare?\n");
                scanf("%d", &posizione);
                while ((c = getchar()) != '\n' && c != EOF); // Pulizia buffer

                if (posizione < 1 || posizione > count_zone())
                {
                    printf("\nInserisci un numero maggiore di 0 e minore o uguale a %d.", count_zone());
                }
            } while (posizione < 1 || posizione > count_zone());
            cancella_zona(posizione);
            break;
        case 4:
            stampa_mappa(pFirst);
            break;
        case 5:
            chiudi_mappa();
            break;
        default:
            break;
        }
    } while (switch_mappa != 5 || mappa_creata != 1);
    gioco_impostato = 1;

    // inizializzo la posizione dei giocatori
    for (int i = 0; i < n_giocatori; i++)
    {
        giocatori[i]->posizione = pFirst;
    }
}

void gioca()
{
    if (gioco_impostato == 1)
    {
        int turno_finito = 0; // Se 1, il turno è finito, altrimenti no
        int gioco_finito = 0; // Se 1, il gioco è finito, altrimenti no

        while (!gioco_finito)
        {
            int giocatori_vivi = 0;

            for (int i = 0; i < n_giocatori; i++)
            {
                if (giocatori[i] != NULL)
                    giocatori_vivi++;
            }

            if (giocatori_vivi == 0)
            {
                printf("\033[1;91mTutti i giocatori sono morti :(\033[0m\n");
                gioco_finito = 1;
                termina_gioco();
            }
            else
            {
                for (int i = 0; i < n_giocatori; i++)
                {
                    if (giocatori[i] == NULL && i < (n_giocatori - 1))
                    {
                        if (giocatori[i + 1] != NULL)
                        {
                            giocatori[i] = giocatori[i + 1];
                            giocatori[i + 1] = NULL;
                        }
                    }
                }
            }
            // Ordine turni
            int ordine_turni[giocatori_vivi];
            for (int i = 0; i < giocatori_vivi; i++)
            {
                int duplicato;
                int nuovo_numero;
                do
                {
                    duplicato = 0; // Inizializza duplicato fuori dal loop
                    nuovo_numero = genera_random(1, giocatori_vivi);

                    // Verifica se il numero è già presente nell'array
                    for (int j = 0; j < i; j++)
                    {
                        if (ordine_turni[j] == nuovo_numero)
                        {
                            duplicato = 1;
                            break;
                        }
                    }
                    // Se duplicato è 1, ripeti il ciclo while per generare un nuovo numero
                } while (duplicato);
                ordine_turni[i] = nuovo_numero;
            }

            // Loop dei giocatori
            for (int i = 0; i < giocatori_vivi; i++)
            {
                do
                {
                    int giocatore_corrente = ordine_turni[i];
                    if (giocatori[giocatore_corrente - 1]->p_vita == 0)
                    {
                        free(giocatori[giocatore_corrente - 1]);
                        giocatori[giocatore_corrente - 1] = NULL;
                        turno_finito = 1;
                    }
                    else if (giocatori[giocatore_corrente - 1] != NULL)
                    {
                        printf("Premere invio per continuare ...\n");
                        while (getchar() != '\n');
                        pulisci_schermo();
                        printf("Turno del giocatore \033[1;93m%s\033[0m\n", giocatori[giocatore_corrente - 1]->nickname);

                        if (giocatori[giocatore_corrente - 1]->posizione == pFirst)
                        {
                            giocatori[giocatore_corrente - 1]->posizione->t_porta = 0;
                            // Siccome è la prima zona, per facilitare il gioco si parte che la prima zona non ha nessuna porta.
                        }

                        unsigned short scelta = 0;

                        do
                        {
                            printf("Scegli un'opzione:\n");
                            printf("1. Avanza\n");
                            printf("2. Indietreggia\n");
                            printf("3. Stampa giocatore\n");
                            printf("4. Stampa zona\n");
                            printf("5. Prendi tesoro\n");
                            scanf("%hu", &scelta);
                            while ((c = getchar()) != '\n' && c != EOF); // Pulizia buffer

                            switch (scelta)
                            {
                            case 1:
                                avanza(giocatore_corrente - 1);
                                turno_finito = 1;
                                break;
                            case 2:
                                // Se il giocatore è già nella prima zona, permetti un'altra azione
                                if (giocatori[giocatore_corrente - 1]->posizione == pFirst)
                                {
                                    printf("\x1b[31mIl giocatore è già nella prima zona. Puoi fare un'altra azione.\x1b[0m\n");
                                    turno_finito = 0;
                                }
                                else
                                    turno_finito = 1;
                                indietreggia(giocatore_corrente - 1);
                                break;
                            case 3:
                                stampa_giocatore(giocatore_corrente - 1);
                                turno_finito = 0;
                                break;
                            case 4:
                                stampa_zona(giocatore_corrente - 1);
                                turno_finito = 0;
                                break;
                            case 5:
                                prendi_tesoro(giocatore_corrente - 1);

                                if (tesoro_preso == 1)
                                    turno_finito = 1;
                                else
                                    turno_finito = 0;
                                break;
                            default:
                                printf("Scelta non valida.\n");
                            }
                        } while (scelta < 1 || scelta > 5);
                        // Se il giocatore è nell'ultima zona, dichiara la fine del gioco
                        if (giocatori[giocatore_corrente - 1]->posizione == pLast)
                        {
                            printf("\033[1;93mSei arrivato all'ultima zona, per vincere devi uccidere il tuo ultimo abitante delle segrete!!\033[0m\n");
                            Abitante *abitante = generaAbitanteCasuale();
                            unsigned short scelta = 0;

                            menu_abitante(scelta, giocatore_corrente - 1, pLast, abitante);
                            if (giocatori[giocatore_corrente - 1]->p_vita > 0)
                            {
                                printf("\033[1;96mIl giocatore %s ha vinto! Il gioco è terminato.\033[0m\n", giocatori[giocatore_corrente - 1]->nickname);
                                gioco_finito = 1;
                                termina_gioco();
                            }
                            else{
                                printf("\033[1;96mIl gioccatore %s non ha sconfitto l'abitante!, Nessun giocatore ha vinto. Il gioco è terminato.\033[0m\n", giocatori[giocatore_corrente - 1]->nickname);
                                gioco_finito = 1;
                                termina_gioco();
                            }
                        }
                    }
                    printf("\n");
                } while (!turno_finito);
            }
        }
    }
    else
    {
        printf("Il gioco non è stato impostato.\n");
    }
}
void termina_gioco()
{
    if (gioco_impostato == 1)
    {
        for (int i = 0; i < n_giocatori; i++)
        {
            free(giocatori[i]);
        }
        cancella_all_zone();
        gioco_impostato = 0;
        mappa_creata = 0;
        pFirst = NULL;
        pLast = NULL;
    }
}

static Giocatore *crea_giocatore(int i)
{
    Giocatore *nuovo_giocatore = (Giocatore *)malloc(sizeof(Giocatore));

    do
    {
        printf("Inserisci il nome del %d° giocatore: ", i + 1);
        if (fgets(nuovo_giocatore->nickname, 10, stdin) != NULL)
            if (!strchr(nuovo_giocatore->nickname, '\n'))
                while ((c = getchar()) != '\n' && c != EOF);// Pulizia buffer
        nuovo_giocatore->nickname[strcspn(nuovo_giocatore->nickname, "\n")] = 0; // rimuove il \n alla fine della stringa

        if (nuovo_giocatore->nickname[0] != '\0')
            break;
    } while (strcmp(nuovo_giocatore->nickname, "") == 0);

    scegli_classe(nuovo_giocatore);
    aumenta_sacrifica(nuovo_giocatore);

    return nuovo_giocatore;
}
static void scegli_classe(Giocatore *giocatore)
{
    unsigned short choice = 0, mente_random = 0;
    do
    {
        printf("Scegli la tua classe (\033[1;91m0\033[0m:Barbaro | \033[1;91m1\033[0m:Nano | \033[1;91m2\033[0m:Elfo | \033[1;91m3\033[0m:Mago): ");
        scanf("%hu", &choice);
        while ((c = getchar()) != '\n' && c != EOF); // Pulizia buffer
    } while (choice < 0 || choice > 3);

    giocatore->Classe = choice;

    printf("\033[92mHai scelto la classe:\033[0m \033[1;96m%s\n\033[0m", nome_classe(giocatore));

    switch (choice)
    {
    case barbaro:
        giocatore->dadi_attacco = 3;
        giocatore->dadi_difesa = 2;
        giocatore->p_vita = 8;
        mente_random = genera_random(1, 2); // 1 o 2
        giocatore->mente = mente_random;
        giocatore->potere_speciale = 0;
        break;

    case nano:
        giocatore->dadi_attacco = 2;
        giocatore->dadi_difesa = 2;
        giocatore->p_vita = 7;
        mente_random = genera_random(2, 3); // 2 o 3
        giocatore->mente = mente_random;
        giocatore->potere_speciale = 1;
        break;

    case elfo:
        giocatore->dadi_attacco = 2;
        giocatore->dadi_difesa = 2;
        giocatore->p_vita = 6;
        mente_random = genera_random(3, 4); // 3 o 4
        giocatore->mente = mente_random;
        giocatore->potere_speciale = 1;
        break;

    case mago:
        giocatore->dadi_attacco = 1;
        giocatore->dadi_difesa = 2;
        giocatore->p_vita = 4;
        mente_random = genera_random(4, 5); // 4 o 5
        giocatore->mente = mente_random;
        giocatore->potere_speciale = 3;
        break;
    }
}
#include <stdio.h>

static void aumenta_sacrifica(Giocatore *giocatore)
{
    unsigned short choice = 0;

    do
    {
        printf("\nSe vuoi aumentare 1 punto vita togliendone 1 a mente premi: \033[1;96m[1]\033[0m\nSe vuoi fare il viceversa premi: \033[1;96m[2]\033[0m\n\033[1;96mSe non vuoi fare niente premi un'altro qualsiasi numero > 0.\033[0m\n");
        scanf("%hu", &choice);

        // Pulizia del buffer
        while ((c = getchar()) != '\n' && c != EOF);
        printf("\n");

        switch (choice)
        {
        case 1:
            giocatore->p_vita++;
            giocatore->mente--;
            printf("ora hai %d punti vita e %d punti mente.\n\n", giocatore->p_vita, giocatore->mente);
            break;
        case 2:
            giocatore->p_vita--;
            giocatore->mente++;
            printf("ora hai %d punti vita e %d punti mente.\n\n", giocatore->p_vita, giocatore->mente);
            break;
        default:
            break;
        }
    } while (choice < 1);
}

static int genera_random(int min, int max)
{
    return rand() % (max - min + 1) + min;
}

static void pulisci_schermo()
{
    system("clear");
}

// Funzione per generare il tipo di zona
static enum Tipo_zona genera_tipo_zona()
{
    int probabilita = genera_random(1, 100);

    switch (probabilita)
    {
    case 1 ... 10:
        return armeria;
    case 11 ... 20:
        return cucina;
    case 21 ... 30:
        return giardino;
    case 31 ... 40:
        return magazzino;
    case 41 ... 50:
        return posto_guardia;
    case 51 ... 60:
        return prigione;
    case 61 ... 70:
        return sala_banchetto;
    case 71 ... 80:
        return scala;
    case 81 ... 90:
        return tempio;
    default:
        return corridoio;
    }
}

// Funzione per generare il tipo di tesoro
static enum Tipo_tesoro genera_tipo_tesoro()
{
    int probabilita = genera_random(1, 100);

    switch (probabilita)
    {
    case 1 ... 25:
        return veleno;
    case 26 ... 50:
        return guarigione;
    case 51 ... 75:
        return doppia_guarigione;
    default:
        return nessun_tesoro;
    }
}

// Funzione per generare il tipo di porta
static enum Tipo_porta genera_tipo_porta()
{
    int probabilita = genera_random(1, 100);

    switch (probabilita)
    {
    case 1 ... 33:
        return porta_da_scassinare;
    case 34 ... 66:
        return porta_normale;
    default:
        return nessuna_porta;
    }
}
// nomi
static char *nome_classe(Giocatore *giocatore)
{
    switch (giocatore->Classe)
    {
    case 0:
        return "barbaro";
    case 1:
        return "nano";
    case 2:
        return "elfo";
    case 3:
        return "mago";
    default:
        return NULL;
    }
}
static char *nome_zona(Zona_segrete *zona)
{
    switch (zona->t_zona)
    {
    case 0:
        return "corridoio";
    case 1:
        return "scala";
    case 2:
        return "sala banchetto";
    case 3:
        return "magazzino";
    case 4:
        return "giardino";
    case 5:
        return "posto guardia";
    case 6:
        return "prigione";
    case 7:
        return "cucina";
    case 8:
        return "armeria";
    case 9:
        return "tempio";
    default:
        return NULL;
    }
}
static char *nome_tesoro(Zona_segrete *tesoro)
{
    switch (tesoro->t_tesoro)
    {
    case 0:
        return "nessun tesoro";
    case 1:
        return "veleno";
    case 2:
        return "guarigione";
    case 3:
        return "doppia guarigione";
    default:
        return NULL;
    }
}
static char *nome_porta(Zona_segrete *porta)
{
    switch (porta->t_porta)
    {
    case 0:
        return "nessuna porta";
    case 1:
        return "porta normale";
    case 2:
        return "porta da scassinare";
    default:
        return NULL;
    }
}

static void genera_mappa()
{
    printf("Generazione Mappa...\n");
    for (int i = 0; i < 15; i++)
    {
        Zona_segrete *nuova_zona = (Zona_segrete *)malloc(sizeof(Zona_segrete));
        if (nuova_zona == NULL)
        {
            fprintf(stderr, "Errore: impossibile allocare memoria per la nuova zona.\n");
            exit(EXIT_FAILURE);
        }
        nuova_zona->t_zona = genera_tipo_zona();
        nuova_zona->t_tesoro = genera_tipo_tesoro();
        nuova_zona->t_porta = genera_tipo_porta();

        nuova_zona->zona_successiva = NULL;
        nuova_zona->zona_precedente = NULL;

        if (pFirst == NULL && pLast == NULL)
        { // non ci sono zone
            pFirst = nuova_zona;
            pLast = nuova_zona;
        }
        else
        {
            nuova_zona->zona_successiva = pFirst;
            pFirst->zona_precedente = nuova_zona;
            pFirst = nuova_zona;
        }
    }
    printf("\nMappa creata con successo. Premere invio per continuare ...\n");
    while (getchar() != '\n');
    pulisci_schermo();
}
static void inserisci_zona(unsigned short posizione)
{
    Zona_segrete *temp = pFirst;
    Zona_segrete *temp2 = NULL;
    Zona_segrete *nuova_zona = (Zona_segrete *)malloc(sizeof(Zona_segrete));
    nuova_zona->t_zona = genera_tipo_zona();
    nuova_zona->t_tesoro = genera_tipo_tesoro();
    nuova_zona->t_porta = genera_tipo_porta();

    if (posizione == 0)
    {
        // printf("Non ci sono zone nella mappa.\n");
        nuova_zona->zona_precedente = NULL;

        if (pFirst != NULL)
        {
            // Collega la zona_successiva della nuova zona alla prima zona esistente
            nuova_zona->zona_successiva = pFirst;

            // Aggiorna la zona_precedente della prima zona esistente alla nuova zona
            pFirst->zona_precedente = nuova_zona;
        }
        else
        {
            // Se non ci sono zone nella mappa, la nuova zona è anche l'ultima zona
            nuova_zona->zona_successiva = NULL;
            pLast = nuova_zona;
        }
        // La nuova zona diventa la prima zona
        pFirst = nuova_zona;
    }
    else
    {

        while (posizione != 1)
        {
            temp = temp->zona_successiva;
            posizione--;
        }

        if (temp->zona_successiva == NULL)
        {
            temp->zona_successiva = nuova_zona;
            nuova_zona->zona_precedente = temp;
            nuova_zona->zona_successiva = NULL;
        }
        else
        {
            temp2 = temp->zona_successiva;
            temp->zona_successiva = nuova_zona;
            temp2->zona_precedente = nuova_zona;
            nuova_zona->zona_successiva = temp2;
            nuova_zona->zona_precedente = temp;
        }
    }
}

static void cancella_zona(unsigned short posizione)
{
    if (posizione == 1)
    {
        Zona_segrete *temp = pFirst;
        pFirst = pFirst->zona_successiva;
        if (pFirst != NULL)
        {
            pFirst->zona_precedente = NULL; // Imposta la zona precedente a NULL per la nuova prima zona
        }
        free(temp);
    }
    else
    {
        Zona_segrete *temp = pFirst;
        Zona_segrete *temp2 = NULL;
        while (posizione > 1)
        {
            temp = temp->zona_successiva;
            posizione--;
        }
        if (temp->zona_successiva == NULL)
        {
            Zona_segrete *temp = pFirst;
            Zona_segrete *temp2;
            while (temp->zona_successiva != NULL)
            {
                temp = temp->zona_successiva;
            }
            temp2 = temp->zona_precedente;
            temp2->zona_successiva = NULL;
            free(temp);
        }
        else
        {
            temp2 = temp->zona_precedente;
            temp2->zona_successiva = temp->zona_successiva;
            temp->zona_successiva->zona_precedente = temp2;
            free(temp);
            temp = NULL;
        }
    }
}

// Cancella tutte le zone
static void cancella_all_zone()
{
    while (pFirst != NULL)
    {
        Zona_segrete *temp = pFirst;
        pFirst = pFirst->zona_successiva;
        free(temp);
    }
}

static void stampa_mappa()
{
    unsigned short count = 0;
    printf("La mappa è la seguente:\n");
    if (pFirst == NULL)
    { // non ci sono nodi nella lista
        printf("Non ci sono zone nella mappa.\n");
    }
    else
    {
        Zona_segrete *pScan = pFirst;

        while (pScan != NULL)
        {
            printf("%hu° ZONA\n", count + 1);
            printf("Tipo Zona: %s\n", nome_zona(pScan));
            printf("Tipo Tesoro: %s\n", nome_tesoro(pScan));
            printf("Tipo Porta: %s\n", nome_porta(pScan));
            printf("-------------\n");

            pScan = pScan->zona_successiva; // pScan viene aggiornato per puntare al nodo successivo nella lista
            count++;
        }
    }
}

static int count_zone()
{
    unsigned int count = 0;
    Zona_segrete *pCount = pFirst;

    while (pCount != NULL)
    {
        count++;
        pCount = pCount->zona_successiva;
    }

    return count;
}

static void chiudi_mappa()
{
    int zone_mancanti = 0;
    zone_mancanti = 15 - count_zone();
    if (zone_mancanti > 0)
    {
        printf("------------------------------------------------\n");
        printf("Le zone devono essere almeno quindici, ne mancano: %d\n", zone_mancanti);
        mappa_creata = 0;
    }
    else
    {
        mappa_creata = 1;
        printf("Il gioco è stato completamente impostato. Premere invio per continuare....\n");
        while (getchar() != '\n');
        pulisci_schermo();
    }
}

// funzioni gioca
static void avanza(int indice_giocatore)
{
    if (indice_giocatore < 0 || indice_giocatore >= 4 || giocatori[indice_giocatore] == NULL || giocatori[indice_giocatore]->posizione == NULL)
    {
        printf("Errore: Il giocatore non ha una posizione valida.\n");
        return;
    }

    Giocatore *giocatore_attuale = giocatori[indice_giocatore];
    Zona_segrete *zona_attuale = giocatore_attuale->posizione;

    if (zona_attuale->zona_successiva != NULL)
    {
        apri_porta(indice_giocatore);
        if (porta_aperta == 1)
        {
            giocatore_attuale->posizione = zona_attuale->zona_successiva;
            printf("Il giocatore %s avanza di una zona.\n", giocatore_attuale->nickname);

            int probabilita = genera_random(1, 100);
            if (probabilita <= 33)
            {
                Abitante *abitante = generaAbitanteCasuale();
                unsigned short scelta = 0;
                menu_abitante(scelta, indice_giocatore, zona_attuale, abitante);
            }
        }
        else
            printf("Siccome non sei riuscito ad aprire la porta, non avanzi di nessuna zona.\n");
    }
    porta_aperta = 0;
}

static void indietreggia(int indice_giocatore)
{
    if (indice_giocatore < 0 || indice_giocatore >= 4 || giocatori[indice_giocatore] == NULL || giocatori[indice_giocatore]->posizione == NULL)
    {
        printf("Errore: Il giocatore non ha una posizione valida.\n");
        return;
    }

    Giocatore *giocatore_attuale = giocatori[indice_giocatore];
    Zona_segrete *zona_attuale = giocatore_attuale->posizione;

    if (zona_attuale->zona_precedente != NULL)
    {
        giocatore_attuale->posizione = zona_attuale->zona_precedente;
        printf("Il giocatore %s indietreggia di una zona.\n", giocatore_attuale->nickname);
        int probabilita = genera_random(1, 100);

        if (scappato == 1)
        {
            giocatore_attuale->posizione = zona_attuale->zona_precedente;
            probabilita = 100;
            scappato = 0;
        }

        if(giocatore_attuale->posizione == pFirst)
            probabilita = 100;

        if (probabilita <= 33)
        {
            Abitante *abitante = generaAbitanteCasuale();
            unsigned short scelta = 0;
            menu_abitante(scelta, indice_giocatore, zona_attuale, abitante);
        }
    }
}

static void stampa_giocatore(int indice_giocatore)
{
    if (indice_giocatore < 0 || indice_giocatore >= 4 || giocatori[indice_giocatore] == NULL || giocatori[indice_giocatore]->posizione == NULL)
    {
        printf("Errore: Il giocatore non ha una posizione valida.\n");
        return;
    }

    if (giocatori[indice_giocatore] != NULL)
    {
        printf("Giocatore %d\n - Nome: %s\n, Classe: %s\n, Dadi Attacco: %d\n, Dadi Difesa: %d\n, Punti Vita: %d\n, Punti Mente: %d\n, Potere Speciale: %d\n",
               indice_giocatore + 1, giocatori[indice_giocatore]->nickname, nome_classe(giocatori[indice_giocatore]),
               giocatori[indice_giocatore]->dadi_attacco, giocatori[indice_giocatore]->dadi_difesa,
               giocatori[indice_giocatore]->p_vita, giocatori[indice_giocatore]->mente, giocatori[indice_giocatore]->potere_speciale);
    }
}

static void stampa_zona(int indice_giocatore)
{
    if (indice_giocatore < 0 || indice_giocatore >= 4 || giocatori[indice_giocatore] == NULL || giocatori[indice_giocatore]->posizione == NULL)
    {
        printf("Errore: Il giocatore non ha una posizione valida.\n");
        return;
    }

    Giocatore *giocatore_attuale = giocatori[indice_giocatore];
    Zona_segrete *zona_corrente = giocatore_attuale->posizione;

    if (zona_corrente != NULL)
    {

        printf("\nTipo Zona: %s\n", nome_zona(zona_corrente));
        if (zona_corrente->t_tesoro == nessun_tesoro)
        {
            printf("Non è presente un tesoro in questa zona.\n");
        }
        else
        {
            printf("è presente un tesoro in questa zona.\n");
        }
        if (zona_corrente->t_porta == nessuna_porta)
        {
            printf("Non è presente una porta in questa zona.\n");
        }
        else
        {
            printf("è presente una porta in questa zona.\n");
        }
        printf("------------------\n");
    }
    else
    {
        printf("Il giocatore non si trova in una zona valida.\n");
    }
}
static void apri_porta(int indice_giocatore)
{
    if (indice_giocatore < 0 || indice_giocatore >= 4 || giocatori[indice_giocatore] == NULL || giocatori[indice_giocatore]->posizione == NULL)
    {
        printf("Errore: Il giocatore non ha una posizione valida.\n");
        return;
    }

    Giocatore *giocatore_attuale = giocatori[indice_giocatore];
    Zona_segrete *zona_attuale = giocatore_attuale->posizione;

    if (zona_attuale != NULL)
    {
        if (zona_attuale->zona_successiva->t_porta == porta_da_scassinare)
        {
            printf("Questa porta per essere aperta va scassinata.\n");
            int numero_dado = genera_random(1, 6);
            if (numero_dado <= giocatore_attuale->mente)
            {
                printf("\033[1;96mLa porta è stata scassinata con successo.\033[0m\n");
                porta_aperta = 1;
            }
            else
            {
                int probabilita = genera_random(1, 100);
                if (probabilita <= 10)
                {
                    printf("\033[1;91mHai sbagliato a scassinare la porta. Riprendi il percorso dalla PRIMA stanza delle segrete.\033[0m\n");
                    giocatore_attuale->posizione = pFirst;
                    porta_aperta = 0;
                }
                else if (probabilita <= 10 + 40)
                {
                    printf("\033[1;91mHai sbagliato a scassinare la porta. Perdi un punto vita.\033[0m\n");
                    giocatore_attuale->p_vita--;
                    if (giocatore_attuale->p_vita == 0)
                    {
                        printf("Il giocatore %s è morto!\n", giocatore_attuale->nickname);
                    }
                    else
                        porta_aperta = 0;
                }
                else if (probabilita <= 10 + 50 + 40)
                {
                    Abitante *abitante = generaAbitanteCasuale();
                    unsigned short scelta = 0;
                    menu_abitante(scelta, indice_giocatore, zona_attuale, abitante);
                    
                    if (abitante->p_vita == 0)
                    {
                        printf("Hai sconfitto l'abitante, quindi hai aperto la porta.\n");
                        porta_aperta = 1;
                    }
                    else
                    {
                        porta_aperta = 0;
                    }
                }
            }
        }
        else if (zona_attuale->zona_successiva->t_porta == nessuna_porta)
        {
            printf("Non c'è nessuna porta.\n");
            porta_aperta = 1;
        }
        else if (zona_attuale->zona_successiva->t_porta == porta_normale)
        {
            printf("La porta è normale, quindi l'hai aperta senza problemi.\n");
            porta_aperta = 1;
        }
    }
    else
    {
        printf("Il giocatore non si trova in una zona valida.\n");
    }
}
static void prendi_tesoro(int indice_giocatore)
{
    if (indice_giocatore < 0 || indice_giocatore >= 4 || giocatori[indice_giocatore] == NULL || giocatori[indice_giocatore]->posizione == NULL)
    {
        printf("Errore: Il giocatore non ha una posizione valida.\n");
        return;
    }

    Giocatore *giocatore_attuale = giocatori[indice_giocatore];
    Zona_segrete *zona_corrente = giocatore_attuale->posizione;

    if (zona_corrente != NULL)
    {
        switch (zona_corrente->t_tesoro)
        {
        case 0:
            printf("Siccome non ci sono tesori disponibili in questa zona, si genera un nuovo tesoro.\n");
            zona_corrente->t_tesoro = genera_tipo_tesoro();
            tesoro_preso = 0;
            break;

        case 1:
            printf("Hai trovato il veleno...|Perdi due punti di vita.\n");
            if (giocatore_attuale->p_vita == 1)
            {
                giocatore_attuale->p_vita = 0;
            }
            else
                giocatore_attuale->p_vita -= 2;

            if (giocatore_attuale->p_vita == 0)
                printf("Il giocatore %s è morto!\n", giocatore_attuale->nickname);
            zona_corrente->t_tesoro = 0;
            tesoro_preso = 1;
            break;
        case 2:
            printf("Hai trovato una guarigione...|Guadagni un punto di vita.\n");
            giocatore_attuale->p_vita++;
            zona_corrente->t_tesoro = 0;
            tesoro_preso = 1;
            break;
        case 3:
            printf("Hai trovato una doppia guarigione...|Guadagni due punti di vita.\n");
            giocatore_attuale->p_vita += 2;
            zona_corrente->t_tesoro = 0;
            tesoro_preso = 1;
            break;
        }
    }
    else
    {
        printf("Il giocatore non si trova in una zona valida.\n");
    }
}
static void scappa(int indice_giocatore)
{
    if (indice_giocatore < 0 || indice_giocatore >= 4 || giocatori[indice_giocatore] == NULL || giocatori[indice_giocatore]->posizione == NULL)
    {
        printf("Errore: Il giocatore non ha una posizione valida.\n");
        return;
    }

    Giocatore *giocatore_attuale = giocatori[indice_giocatore];

    int numero_dado = genera_random(1, 6);
    if (numero_dado <= giocatore_attuale->mente)
    {
        scappato = 1;
        printf("Sei scappato!!!\n");
        indietreggia(indice_giocatore);
    }
}

static void gioca_potere_speciale(Giocatore *giocatore_attuale, Abitante *abitante_attuale)
{

    if (giocatore_attuale->potere_speciale > 0)
    {
        printf("Usi un potere speciale per uccidere immediatamente l'abitante.\n");
        giocatore_attuale->potere_speciale--;

        if (abitante_attuale != NULL)
        {
            abitante_attuale->p_vita = 0;
            printf("Hai ucciso l'abitante usando un potere speciale.\n");

            free(abitante_attuale);
            abitante_attuale = NULL;
        }
    }
}

static Abitante *generaAbitanteCasuale()
{
    Abitante *nuovo_abitante = (Abitante *)malloc(sizeof(Abitante));
    if (nuovo_abitante == NULL)
    {
        exit(EXIT_FAILURE);
    }

    char *nomi[] = {"ombra malefica", "stregone delle tenebre", "notturno infestato", "demone crudele", "spettro della perdizione", "orco sanguinario"};

    int indiceNome = rand() % (sizeof(nomi) / sizeof(nomi[0]));
    strcpy(nuovo_abitante->nome, nomi[indiceNome]);

    nuovo_abitante->dadi_attacco = genera_random(1, 6);
    nuovo_abitante->dadi_difesa = genera_random(1, 6);
    nuovo_abitante->p_vita = genera_random(1, 8);

    printf("Appare un abitante delle segrete!\n");

    printf("Nome: %s\n", nuovo_abitante->nome);
    return nuovo_abitante;
}
static void combatti(int indice_giocatore, Abitante *abitante_attuale)
{

    if (indice_giocatore < 0 || indice_giocatore >= 4 || giocatori[indice_giocatore] == NULL || giocatori[indice_giocatore]->posizione == NULL)
    {
        printf("Errore: Il giocatore non ha una posizione valida.\n");
        return;
    }

    Giocatore *giocatore_attuale = giocatori[indice_giocatore];

    int dado_giocatore = genera_random(1, 6);
    int dado_abitante = genera_random(1, 6);

    if (dado_giocatore >= dado_abitante)
    {
        if (abitante_attuale->p_vita > 0 && giocatore_attuale->p_vita > 0)
            attacco_giocatore(giocatore_attuale, abitante_attuale);
        if (abitante_attuale->p_vita > 0 && giocatore_attuale->p_vita > 0)
            attacco_abitante(giocatore_attuale, abitante_attuale);
    }

    if (dado_giocatore < dado_abitante)
    {
        if (abitante_attuale->p_vita > 0 && giocatore_attuale->p_vita > 0)
            attacco_abitante(giocatore_attuale, abitante_attuale);
        if (abitante_attuale->p_vita > 0 && giocatore_attuale->p_vita > 0)
            attacco_giocatore(giocatore_attuale, abitante_attuale);
    }

    if (abitante_attuale->p_vita == 0)
    {
        printf("L'abitante è morto!\n");
        free(abitante_attuale);
        abitante_attuale = NULL;
    }
    else if (giocatore_attuale->p_vita == 0)
        printf("Il giocatore %s è morto!\n", giocatore_attuale->nickname);
}

static void menu_abitante(unsigned short scelta, int indice_giocatore, Zona_segrete *zona_attuale, Abitante *abitante)
{
    Giocatore *giocatore_attuale = giocatori[indice_giocatore];
    do
    {
        printf("Puoi scappare: 1, combattere: 2, oppure giocare il potere speciale: 3\n");
        scanf("%hu", &scelta);
        while ((c = getchar()) != '\n' && c != EOF); // Pulizia buffer

        switch (scelta)
        {
        case 1:
            // Controllo se il giocatore è nella prima zona
            if (giocatore_attuale->posizione == pFirst)
            {
                printf("Non puoi scappare nella prima zona.\n");
                scelta = 0;
            }
            else
            {
                printf("Hai deciso di scappare.\n");
                scappa(indice_giocatore);
            }
            break;
        case 2:
            while (abitante->p_vita != 0 && giocatore_attuale->p_vita != 0)
            {
                combatti(indice_giocatore, abitante);
            }
            break;
        case 3:
            // Controllo se il giocatore ha ancora il potere speciale disponibile
            if (giocatore_attuale->potere_speciale > 0)
            {
                gioca_potere_speciale(giocatore_attuale, abitante);
            }
            else
            {
                printf("Non hai più poteri speciali disponibili.\n");
                scelta = 0;
            }
            break;
        default:
            printf("Scelta non valida. Riprova.\n");
            break;
        }
    } while (scelta < 1 || scelta > 3);
}
static void attacco_giocatore(Giocatore *giocatore_attuale, Abitante *abitante_attuale)
{
    char pittogrammi[6] = {'X', 'X', 'X', 'W', 'W', 'B'}; // X: teschio, W:white(scudo bianco), B:black(scudo nero)
    int n_teschi_giocatore = 0;
    int n_bianchi_giocatore = 0;
    int n_teschi_abitante = 0;
    int n_neri_abitante = 0;

    for (int i = 0; i < giocatore_attuale->dadi_attacco; i++)
    {
        char temp = pittogrammi[genera_random(0, 5)];
        if (temp == 'X')
        {
            n_teschi_giocatore++;
        }
        else if (temp == 'W')
        {
            n_bianchi_giocatore++;
        }
    }

    if (abitante_attuale->p_vita > 0)
    {
        for (int i = 0; i < abitante_attuale->dadi_difesa; i++)
        {
            char temp = pittogrammi[genera_random(0, 5)];
            if (temp == 'X')
            {
                n_teschi_abitante++;
            }
            else if (temp == 'B')
            {
                n_neri_abitante++;
            }
        }
        int differenza = n_teschi_giocatore - n_neri_abitante;

        if (differenza > 0 && abitante_attuale->p_vita > 0)
        {
            if (abitante_attuale->p_vita == 1)
                abitante_attuale->p_vita = 0;
            else
                abitante_attuale->p_vita--;
        }
    }
}
static void attacco_abitante(Giocatore *giocatore_attuale, Abitante *abitante_attuale)
{
    char pittogrammi[6] = {'X', 'X', 'X', 'W', 'W', 'B'}; // X: teschio, W:white(scudo bianco), B:black(scudo nero)
    int n_teschi_giocatore = 0;
    int n_bianchi_giocatore = 0;
    int n_teschi_abitante = 0;
    int n_neri_abitante = 0;

    for (int i = 0; i < abitante_attuale->dadi_attacco; i++)
    {
        char temp = pittogrammi[genera_random(0, 5)];
        if (temp == 'X')
        {
            n_teschi_abitante++;
        }
        else if (temp == 'B')
        {
            n_neri_abitante++;
        }
    }

    if (giocatore_attuale->p_vita > 0)
    {
        for (int i = 0; i < giocatore_attuale->dadi_difesa; i++)
        {
            char temp = pittogrammi[genera_random(0, 5)];
            if (temp == 'X')
            {
                n_teschi_giocatore++;
            }
            else if (temp == 'W')
            {
                n_bianchi_giocatore++;
            }
        }
        int differenza = n_teschi_abitante - n_bianchi_giocatore;

        if (differenza > 0 && giocatore_attuale->p_vita > 0)
        {
            if (giocatore_attuale->p_vita == 1)
                giocatore_attuale->p_vita = 0;
            else
                giocatore_attuale->p_vita--;
        }
    }
}