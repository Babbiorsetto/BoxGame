1. Si specificano a linea di comando I parametri del server (porta TCP)

1. Il server viene avviato
1. Inizia ad attendere connessioni. Continuera' a permetterle per tutta la durata del funzionamento
    - Ai client connessi si da la possibilita' di registrarsi o effettuare l'accesso
1. Quando il primo giocatore effettua l'accesso, inizia la partita
1. Il server ciclicamente permette un turno a ogni giocatore
    - Se nuovi giocatori si uniscono, il loro turno viene messo in coda
    - Se un giocatore abbandona, il suo turno viene saltato finche' non torna. Il punteggio viene conservato
    - A ogni turno di ogni giocatore, il server mostra lo spazio di gioco a ogni giocatore
1. Al termine I giocatori presenti possono esprimere la volonta' di partecipare alla partita successiva ed essere in coda gia' dal primo turno
