[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-24ddc0f5d75046c5622901739e7c5dd533143b0c8e959d652212380cedb1ea36.svg)](https://classroom.github.com/a/vnVUxNv7)

# Progetto-finale-2023-Scalogna-Quest

Progetto finale Programmazione Procedurale UniPG Informatica

## Nome: Carlos Floriano

## Cognome: Hlade

## Commenti/modifiche al progetto:

Il giocatore termina il suo turno solamente quando avanza, indietreggia, prende il tesoro.
Se il giocatore prova a indietreggiare quando si trova nella prima zona, il turno non terminerà, ma apparirà un messaggio di errore dicendo che non può indietreggiare, cosi facendo potrà rigiocare il proprio turno.
Il giocatore quando prova a prendere un tesoro, se ci riesce il turno termina, altrimenti nel caso non ci siano tesori, quest'ultimo si rigenera e il giocatore può rifare il turno siccome non ha preso nessun tesoro.

Il gioco finisce solamente se tutti i giocatori muoiono, oppure quando un giocatore arriva all'ultima zona e uccide l'abitante delle segrete dell'ultima zona.

Ho deciso di non far terminare il gioco solamente quando rimaneva in vita un giocatore, perchè nel caso ci siano solamente 2 giocatori, il gioco sarebbe troppo breve.

All'inizio del gioco per facilitare i giocatori, la prima zona non ha nessuna porta.

Per facilitare i giocatori ho deciso di fare in modo che nella prima zona è impossibile che appaia, tranne quando si usa la funzione "avanza", perchè nell'aprire la porta della seconda zona si può sempre trovare un abitante.
