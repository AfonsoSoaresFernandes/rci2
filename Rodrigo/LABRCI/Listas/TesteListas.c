//
//  main.c
//  Listas
//
//  Created by Rodrigo Lopes Do Ó Barbosa on 12/03/17.
//  Copyright (c) 2017 Rodrigo Lopes Do Ó Barbosa. All rights reserved.
//

# include <stdio.h>
# include <stdlib.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <errno.h>
# include <netdb.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <string.h>
# include <unistd.h>
# include <signal.h>

# include <time.h>


typedef struct List{
    char message[141];
    time_t time;

    struct List * next;
} List_msg;


void main() {
    //printf("%d", (((seconds/3600)/24)/365));
    
    List_msg * head = NULL;
    List_msg * aux = NULL;
    List_msg * new = NULL;
    time_t seconds, start;
    
    char buffer[141];
    int count;

    start = time(NULL);
                                                //INICIAR HEAD
    head = (List_msg*)malloc(sizeof(List_msg));
    if (head == NULL) {
        printf("Bad construction\n");
        exit(7);
    }
    
    count = 0;
                                                    //CRIAR PRIMEIRA ENTRADA
    head->next = (List_msg*)malloc(sizeof(List_msg));
    aux = head->next;
    while (1) {
        fgets(buffer, 141, stdin);
        
        seconds=time(&start);               //GUARDAR MENSAGENS E TEMPO
        strcpy(aux->message, buffer);
        aux->time = seconds;
        aux->next = NULL;
        if (count==3) {                     //NUMERO MAXIMO DE ENTRADA = (COUNT+1)
            aux->next = NULL;
            aux = aux->next;
            break;
            
        }                                               //CRIAR NOVA ENTRADA
        aux->next = (List_msg*)malloc(sizeof(List_msg));
        aux = aux->next;
        count++;
    }
    new = head->next;                               //LER DADOS DA LISTA (PRIMEIRO É O MAIS ANTIGO)
    while ((new!=NULL)) {
        printf("Mensagem:%s\nTempo:%ld\n", new->message,new->time);
        new=new->next;
        
    }
    
    
    exit(0);
}
