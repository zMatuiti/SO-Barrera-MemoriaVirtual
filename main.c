#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

// barrera

typedef struct
{
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    int count;
    int N;
    int etapa;
} Barrier;

// inicializa la barrera
void barrier_init(Barrier *b, int n_threads)
{
    pthread_mutex_init(&b->mutex, NULL);
    pthread_cond_init(&b->cond, NULL);
    b->N = n_threads;
    b->count = 0;
    b->etapa = 0;
}

// limpia los recursos de la barrera
void barrier_destroy(Barrier *b)
{
    pthread_mutex_destroy(&b->mutex);
    pthread_cond_destroy(&b->cond);
}

// wait() para la sincronizacion de hebras
void barrier_wait(Barrier *b)
{
    // 1. Lock (patron lock -> modificar -> decidir -> unlock)
    pthread_mutex_lock(&b->mutex);

    // 2. capturar la etapa actual en variable local
    // Esto es vital para que la barrera sea reutilizable, una hebra que se despierta, debe saber si la etapa global ya cambio o sigue en la misma. [cite: 38]
    int my_etapa = b->etapa;

    b->count++;

    if (b->count < b->N)
    {
        // si NO es la ultima hebra, espera mientras la etapa no cambie
        while (b->etapa == my_etapa)
        {
            pthread_cond_wait(&b->cond, &b->mutex);
        }
    }
    else
    {
        // si ES la ultima hebra en llegar
        b->count = 0;                     // resetea
        b->etapa++;                       // Incrementa etapa para liberar a las otras
        pthread_cond_broadcast(&b->cond); // despierta a todas
    }

    // 3. Unlock
    pthread_mutex_unlock(&b->mutex);
}

// estructura para los argumentos de las hebras
typedef struct
{
    int id;
    int num_etapas;
    Barrier *barrera;
} ThreadArgs;

// la hebra
void *thread_func(void *arg)
{
    ThreadArgs *args = (ThreadArgs *)arg;
    int tid = args->id;
    int etapas = args->num_etapas;
    Barrier *barrera = args->barrera;

    for (int e = 0; e < etapas; e++)
    {
        // usleep aleatorio
        int sleep_time = rand() % 500000; // 0.5 segundos
        usleep(sleep_time);

        // imprime antes de esperar
        printf("[tid %d] esperando en etapa %d\n", tid, e);

        // llama a wait()
        barrier_wait(barrera);

        // imprime despues de pasar
        printf("[tid %d] paso barrera en etapa %d\n", tid, e);
    }

    return NULL;
}

int main(int argc, char *argv[])
{
    // valores por defecto segun enunciado
    int N = 5; // num de hebras
    int E = 4; // num de etapas

    // lectura de argumentos opcionales
    if (argc > 1)
        N = atoi(argv[1]);
    if (argc > 2)
        E = atoi(argv[2]);

    printf("--- Iniciando Simulacion: %d hebras, %d etapas ---\n", N, E);

    // semilla para aleatoriedad
    srand(time(NULL));

    // inicializar la barrera
    Barrier barrera;
    barrier_init(&barrera, N);

    pthread_t threads[N];
    ThreadArgs args[N];

    // creacion de las hebras
    for (int i = 0; i < N; i++)
    {
        args[i].id = i;
        args[i].num_etapas = E;
        args[i].barrera = &barrera;
        if (pthread_create(&threads[i], NULL, thread_func, &args[i]) != 0)
        {
            perror("Error al crear hebra");
            return 1;
        }
    }

    // espera a que terminen
    for (int i = 0; i < N; i++)
    {
        pthread_join(threads[i], NULL);
    }

    // limpia
    barrier_destroy(&barrera);
    printf("---------- Simulacion finalizada ----------\n");

    return 0;
}