#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

#define NTHREADS 10

pthread_mutex_t x_mutex;
pthread_cond_t x_cond;

int vetorDeInteiros[NTHREADS]; // Inicia vetor de inteiros
int bloqueadas = 0; //Variável de auxílio para a função barreira

void barreira(int nthreads) {
    pthread_mutex_lock(&x_mutex); //inicio secao critica
    if (bloqueadas == (nthreads-1)) { 
      //ultima thread a chegar na barreira
      pthread_cond_broadcast(&x_cond);
      bloqueadas=0;
    } else {
      bloqueadas++;
      pthread_cond_wait(&x_cond, &x_mutex);
    }
    pthread_mutex_unlock(&x_mutex); //fim secao critica
}

void *calculaSoma (void *t) {
  long int acumulador = 0.0;
  int idDaThread = *(int*)t;
  
  for(int i=0; i < NTHREADS; i++){
  	int acumuladorParcial = 0;
  	for(int k=0; k < NTHREADS; k++){
  		acumuladorParcial += vetorDeInteiros[k];
  	}
  	
  	acumulador += acumuladorParcial;
  }
  
  printf("Sou a Thread %d, achei a soma %ld e vou aguardar as outras threads também acharem\n", idDaThread, acumulador);
  barreira(NTHREADS);
  
  int valorAleatorio = rand() % 10; 
  printf("Sou a Thread %d e irei colocar o valor %d na posição %d do vetor e aguardar as outras threads\n", idDaThread, valorAleatorio, idDaThread);
  vetorDeInteiros[idDaThread] = valorAleatorio;
  
  barreira(NTHREADS);
  printf("Sou a Thread %d e irei retornar o valor %ld para a main\n", idDaThread, acumulador);
  
  pthread_exit((void *) acumulador);
}


/* Funcao principal */
int main(int argc, char *argv[]) {
  
  pthread_t threads[NTHREADS]; //Inicia vetor de threads
  int id[NTHREADS]; //Inicia vetor de identificadores de threads
  long int retorno = 0.0; //Valor de retorno das threadas
  bool solucaoEstaCorreta = true; //Variável axuliar para verificar corretude da aplicacao
  
  srand(time(0)); //Seed para gerador de número aleatório
  
  for(int i=0; i < NTHREADS; i++){
  	vetorDeInteiros[i] = rand() % 10; //Define numero aleatorio entre 0 e 9
  }

  //Cria as threads
  for(int i = 0; i < NTHREADS; i++){
  	id[i] = i;
  	pthread_create(&threads[i], NULL, calculaSoma, (void *) &id[i]);
  }

  /* Espera todas as threads completarem */
  for (int i = 0; i < NTHREADS; i++) {
    int retornoLocal = 0.0;
    if(pthread_join(threads[i], (void**) &retorno)){
         fprintf(stderr, "ERRO--pthread_join\n");
         return 3;
    }
    
    if(retornoLocal != 0.0 && retornoLocal != retorno){
    	solucaoEstaCorreta = false;
    }
    
    retornoLocal = retorno;
    
    printf("Sou a main e recebi o valor %ld da thread %d\n", retorno, id[i]);
  }
  
  if(solucaoEstaCorreta) printf("Solução está correta\n");
  else printf("Solução está incorreta\n");


  /* Desaloca variaveis e termina */
  pthread_mutex_destroy(&x_mutex);
  pthread_cond_destroy(&x_cond);
  return 0;
}
