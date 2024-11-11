#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdlib.h>

sem_t mutex;            //para garantir exclusão mútua na ponte
sem_t florestaAlta;     //para contar quantos viajantes da Floresta Alta já atravessaram
sem_t grandeRio;        //para sinalizar quando um viajante do Grande Rio pode atravessar

int contadorFlorestaAlta = 0;

void *viajanteFlorestaAlta(void *arg) {
    while (1) {
        sem_wait(&mutex); //solicita acesso à ponte
        
        //simula a travessia
        printf("Viajante da Floresta Alta atravessou a ponte\n");
        contadorFlorestaAlta++;
        
        //se cinco viajantes da Floresta Alta já atravessaram, sinaliza para o Grande Rio
        if (contadorFlorestaAlta == 5) {
            sem_post(&grandeRio);
            contadorFlorestaAlta = 0; //reseta o contador
        }
        
        sem_post(&mutex); //libera a ponte
        
        sleep(rand() % 2 + 1); //simula tempo de travessia
    }
}

void *viajanteGrandeRio(void *arg) {
    while (1) {
        sem_wait(&grandeRio); //espera até que cinco viajantes da Floresta Alta tenham atravessado
        sem_wait(&mutex);     //solicita acesso à ponte
        
        //simula a travessia
        printf("Viajante do Grande Rio atravessou a ponte\n");
        
        sem_post(&mutex); //libera a ponte
        
        sleep(rand() % 2 + 1); //simula tempo de travessia
    }
}

int main() {
    int N = 10; //número de viajantes da Floresta Alta
    int M = 5;  //número de viajantes do Grande Rio

    pthread_t viajantesFloresta[N], viajantesGrandeRio[M];

    //inicialização dos semáforos
    sem_init(&mutex, 0, 1);          //apenas um viajante na ponte por vez
    sem_init(&florestaAlta, 0, 0);   //inicializa com zero, pois ninguém atravessou ainda
    sem_init(&grandeRio, 0, 0);      //inicializa com zero, pois nenhum viajante do Grande Rio pode atravessar ainda

    //criação das threads para viajantes da Floresta Alta
    for (int i = 0; i < N; i++) {
        pthread_create(&viajantesFloresta[i], NULL, viajanteFlorestaAlta, NULL);
    }

    //criação das threads para viajantes do Grande Rio
    for (int i = 0; i < M; i++) {
        pthread_create(&viajantesGrandeRio[i], NULL, viajanteGrandeRio, NULL);
    }

    //espera as threads terminarem (nunca vão terminar no loop atual)
    for (int i = 0; i < N; i++) {
        pthread_join(viajantesFloresta[i], NULL);
    }
    
    for (int i = 0; i < M; i++) {
        pthread_join(viajantesGrandeRio[i], NULL);
    }

    //destruição dos semáforos (nunca alcançado neste exemplo)
    sem_destroy(&mutex);
    sem_destroy(&florestaAlta);
    sem_destroy(&grandeRio);

    return 0;
}