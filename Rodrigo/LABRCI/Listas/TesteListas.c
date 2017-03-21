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

                    //Estrutura que guarda o Endereço da HEAD e TAIL da lista de Mensagens
                    //E guarda o numero de mensagens guardadas
typedef struct List{
    struct MSG * head;
    struct MSG * tail;
    int size;
    
} List_msg;

                    //Estrutura que guar as Mensagens e o sei relogio lógico
typedef struct MSG{
    char message[141];
    int clock;
    
    struct MSG * ptr;
    
}msg;

                                            //Função que Regista na Lista as Mensagens
msg * RegMsg(msg * tail, char * message, int clock ){
    msg * new;
    new= (msg*)malloc(sizeof(msg));
    
    
    if  ((((tail->clock)-clock)<0) || (((tail->clock)-clock)==0)){
        new->clock=tail->clock + 1;
    }else{
        new->clock=clock;
    }
    
    
    strcpy(new->message, message);
    new->ptr=NULL;
    
    tail->ptr=new;
    tail=new;

    return tail;
}

                        //Função que remove da Lista as mensagens mais antigas
void RemovMsg(msg * head){
    msg * old, *new;
    
    old = head->ptr;
    new = old->ptr;
    head->ptr=new;
    
    free(old);
    
}
                                //Função que recebe uma string onde vai escrever TODAS as Mensagens
void PrintMsg(msg* head, char * message){

    msg * next;
    next= head->ptr;
    
    strcpy(message, "MESSAGES\n");
    while (next) {
        strcat(message, next->message);
        next=next->ptr;
    }
}

                            //Função que recebe uma string onde vai escrever as ULTIMAS N Mensagens
void Print_n_Msg(msg* head, char * message, int n){
    int i;
    msg * next;
    next= head->ptr;
    
    for (i=0; i<n; i++) {
        next= next->ptr;
    }
    
    strcpy(message, "MESSAGES\n");
    while (next) {
        strcat(message, next->message);
        next=next->ptr;
    }
}


                        //FUNÇÃO QUE APAGA LISTA
void free_list(msg *head) {
    msg *prev = head;
    msg *cur = head;
    while(cur) {
        prev = cur;
        cur = prev->ptr;
        free(prev);
    }
}

int main(){
    
    char message[141],buffer[141], final[141], final2[141];
    int clock, count=0;
    
    int n;
    
    List_msg * imsg;
    msg *head, *tail;
    head = NULL;
    
                                    //Inicialização da Lista de Mensagens
    head = (msg *)malloc(sizeof(msg));
    head->clock=0;
    head->ptr=NULL;
    
    tail=head->ptr;
                                        // Inicialização da estrutura de memoria da Lista
    imsg = (List_msg *)malloc(sizeof(List_msg));
    imsg->head = head;
    imsg->tail = tail;
    imsg->size = 0;
    while (count<6) {
        
        fgets(buffer, 141, stdin);
        
        sscanf(buffer, "%s %d",message,&clock);
        
        
                                //Verificação e acerto do numero de mensagens
        if (imsg->size>200) {
            RemovMsg(imsg->head);
            imsg->size--;
        }
                                    //Guardar mensagens
        if (imsg->tail==NULL) {
            imsg->tail = RegMsg(imsg->head,message,clock);
            imsg->size++;
        }else{
            imsg->tail = RegMsg(imsg->tail,message,clock);
            imsg->size++;
        }
       // PrintMsg(imsg->head);
        count++;
    }

    fgets(buffer, 141, stdin);
    
    sscanf(buffer, "%d",&n);
    
                        //Devolve Todas as mensagens da lista
    PrintMsg(imsg->head, final);
    
                        //Devolve as ultimas N mensagens da Lista
    Print_n_Msg(imsg->head, final2, n);
    
    free_list(head);
    free(imsg);
        return 0;
}









/*void main() {
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
}*/
