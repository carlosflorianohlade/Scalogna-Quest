void imposta_gioco();
void gioca();
void termina_gioco();

enum Classe_giocatore
{
    barbaro,
    nano,
    elfo,
    mago
};

enum Tipo_zona
{
    corridoio,
    scala,
    sala_banchetto,
    magazzino,
    giardino,
    posto_guardia,
    prigione,
    cucina,
    armeria,
    tempio
};

enum Tipo_tesoro
{
    nessun_tesoro,
    veleno,
    guarigione,
    doppia_guarigione
};

enum Tipo_porta
{
    nessuna_porta,
    porta_normale,
    porta_da_scassinare
};

typedef struct Giocatore
{
    char nickname[10];
    enum Classe_giocatore Classe;
    struct Zona_segrete *posizione;
    unsigned char p_vita;
    unsigned char dadi_attacco;
    unsigned char dadi_difesa;
    unsigned char mente;
    unsigned char potere_speciale;
} Giocatore;

typedef struct Zona_segrete
{
    struct Zona_segrete *zona_successiva;
    struct Zona_segrete *zona_precedente;
    enum Tipo_zona t_zona;
    enum Tipo_tesoro t_tesoro;
    enum Tipo_porta t_porta;
} Zona_segrete;

typedef struct Abitante
{
    char nome[30];
    unsigned char dadi_attacco;
    unsigned char dadi_difesa;
    unsigned char p_vita;
} Abitante;