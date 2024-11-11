#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

//semaforos
sem_t fogaoLivre, maxPanelas, exclusaoMutua;
sem_t frigideiraLivre, panelaLivre;

//variaveis compartilhadas
int numFrigideiras, numPanelas, numFrigideirasAtual = 0, numPanelasAtual = 0;
int tipoUtensilioEmUso = 0;                 //0: Nenhum, 1: Frigideira, 2: Panela

//função para a thread frigideira
void *frigideira(void *arg) {
    while (1) {
        sem_wait(&frigideiraLivre);
        sem_wait(&exclusaoMutua);
        if (tipoUtensilioEmUso == 1 || tipoUtensilioEmUso == 0) {
            if (numFrigideirasAtual < 3) {  //máximo de 3 frigideiras
                numFrigideirasAtual++;
                tipoUtensilioEmUso = 1;
                printf("Frigideira em uso. Total de frigideiras: %d, Total de panelas: %d\n", numFrigideirasAtual, numPanelasAtual);
                sem_post(&exclusaoMutua);
                sem_post(&fogaoLivre);      //libera o fogão para outras frigideiras

                //simula o uso da frigideira
                sleep(rand() % 3 + 1);

                sem_wait(&exclusaoMutua);
                numFrigideirasAtual--;
                if (numFrigideirasAtual == 0) {
                    tipoUtensilioEmUso = 0;
                }
                printf("Frigideira liberada. Total de frigideiras: %d, Total de panelas: %d\n", numFrigideirasAtual, numPanelasAtual);
                sem_post(&exclusaoMutua);
                sem_post(&frigideiraLivre); //libera a frigideira
            } else {
                sem_post(&exclusaoMutua);
                sem_post(&frigideiraLivre); //libera a frigideira se não puder usar o fogão
            }
        } else {
            sem_post(&exclusaoMutua);
            sem_post(&frigideiraLivre);     //libera a frigideira se o fogão estiver em uso por panelas
        }
    }
    return NULL;
}

//função para a thread panela
void *panela(void *arg) {
    while (1) {
        sem_wait(&panelaLivre);
        sem_wait(&exclusaoMutua);
        if (tipoUtensilioEmUso == 2 || tipoUtensilioEmUso == 0) {
            if (numPanelasAtual < 3) { //máximo de 3 panelas
                numPanelasAtual++;
                tipoUtensilioEmUso = 2;
                printf("Panela em uso. Total de frigideiras: %d, Total de panelas: %d\n", numFrigideirasAtual, numPanelasAtual);
                sem_post(&exclusaoMutua);
                sem_post(&fogaoLivre); //libera o fogão para outras panelas

                //simula o uso da panela
                sleep(rand() % 3 + 1);

                sem_wait(&exclusaoMutua);
                numPanelasAtual--;
                if (numPanelasAtual == 0) {
                    tipoUtensilioEmUso = 0;
                }
                printf("Panela liberada. Total de frigideiras: %d, Total de panelas: %d\n", numFrigideirasAtual, numPanelasAtual);
                sem_post(&exclusaoMutua);
                sem_post(&panelaLivre); //libera a panela
            } else {
                sem_post(&exclusaoMutua);
                sem_post(&panelaLivre); //libera a panela se não puder usar o fogão
            }
        } else {
            sem_post(&exclusaoMutua);
            sem_post(&panelaLivre);     //libera a panela se o fogão estiver em uso por frigideiras
        }
    }
    return NULL;
}

//função para imprimir o estado atual do fogao
void imprimir_estado() {
    sem_wait(&exclusaoMutua);
    printf("Estado atual: %d frigideiras e %d panelas em uso\n", numFrigideirasAtual, numPanelasAtual);
    sem_post(&exclusaoMutua);
}

int main() {
    //inicialização dos semaforos
    sem_init(&fogaoLivre, 0, 1);        //apenas uma boca de fogao
    sem_init(&maxPanelas, 0, 3);        //máximo de 3 panelas simultaneas (não usado diretamente aqui)
    sem_init(&exclusaoMutua, 0, 1);     //exclusão mutua
    sem_init(&frigideiraLivre, 0, 5);   //número de frigideiras
    sem_init(&panelaLivre, 0, 5);       //número de panelas

    //inicialização das variaveis
    numFrigideiras = 5;                 //número de frigideiras
    numPanelas = 5;                     //número de panelas

    //inicialização da semente para o gerador de números aleatórios
    srand(time(NULL));

    //criação das threads
    pthread_t threadsFrigideira[numFrigideiras];
    pthread_t threadsPanela[numPanelas];

    for (int i = 0; i < numFrigideiras; i++) {
        pthread_create(&threadsFrigideira[i], NULL, frigideira, NULL);
    }

    for (int i = 0; i < numPanelas; i++) {
        pthread_create(&threadsPanela[i], NULL, panela, NULL);
    }

    //loop principal para manter o programa rodando
    while (1) {
        sleep(1);                       //aguarda 1 segundo
        imprimir_estado();              //imprime o estado atual do fogao
    }

    //destruir os semaforos (não necessário aqui, pois o programa não termina)
    //sem_destroy(&fogaoLivre);
    //sem_destroy(&maxPanelas);
    //sem_destroy(&exclusaoMutua);
    //sem_destroy(&frigideiraLivre);
    //sem_destroy(&panelaLivre);

    return 0;
}