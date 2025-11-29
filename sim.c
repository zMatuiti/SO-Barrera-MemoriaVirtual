#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    long nvp;
    int usado;
} Frame;

/* Busca si la página virtual nvp está cargada en algún marco.
 * Devuelve el índice del marco si lo encuentra (HIT), o -1 si no (FALLO).
 */
int buscar_en_frames(Frame *frames, int n_frames, long nvp) {
    for (int i = 0; i < n_frames; i++) {
        if (frames[i].nvp == nvp) {
            return i;
        }
    }
    return -1;
}

/* Busca un marco libre (nvp == -1).
 * Devuelve el índice del marco libre, o -1 si no hay libres.
 */
int buscar_marco_libre(Frame *frames, int n_frames) {
    for (int i = 0; i < n_frames; i++) {
        if (frames[i].nvp == -1) {
            return i;
        }
    }
    return -1;
}

// Algoritmo de reemplazo Reloj.
int reloj_reemplazo(Frame *frames, int n_frames, int *puntero, long nvp_nueva) {
    int libre = buscar_marco_libre(frames, n_frames);
    if (libre != -1) {
        frames[libre].nvp = nvp_nueva;
        frames[libre].usado = 1;
        return libre;
    }
    while (1) {
        int i = *puntero;

        if (frames[i].usado == 0) {
            frames[i].nvp = nvp_nueva;
            frames[i].usado = 1;
            *puntero = (i + 1) % n_frames;
            return i;
        } else {
            frames[i].usado = 0;
            *puntero = (i + 1) % n_frames;
        }
    }
}
int calcular_b(unsigned long page_size) {
    int b = 0;
    unsigned long x = 1;
    while (x < page_size) {
        x <<= 1; // x = x * 2
        b++;
    }
    return b;
}

int main(int argc, char *argv[]) {
    if (argc < 4) {
        fprintf(stderr, "Uso: %s Nmarcos tamanioMarco [--verbose] traza.txt\n", argv[0]);
        return 1;
    }

    int arg_index = 1;
    int Nmarcos = atoi(argv[arg_index++]);
    unsigned long page_size = strtoul(argv[arg_index++], NULL, 10);

    int verbose = 0;

    // Si el siguiente argumento es --verbose, activarlo
    if (strcmp(argv[arg_index], "--verbose") == 0) {
        verbose = 1;
        arg_index++;
    }

    // El último argumento siempre es el archivo de traza
    if (arg_index >= argc) {
        fprintf(stderr, "Error: falta archivo de traza.\n");
        return 1;
    }

    const char *trace_filename = argv[arg_index];

    int b = calcular_b(page_size);
    unsigned long mask = page_size - 1;

    Frame *frames = (Frame *)malloc(sizeof(Frame) * Nmarcos);
    if (!frames) {
        perror("malloc");
        return 1;
    }

    for (int i = 0; i < Nmarcos; i++) {
        frames[i].nvp = -1;
        frames[i].usado = 0;
    }

    int puntero_reloj = 0;
    long total_refs = 0;
    long page_faults = 0;

    FILE *f = fopen(trace_filename, "r");
    if (!f) {
        perror("fopen");
        free(frames);
        return 1;
    }

    char line[256];
    while (fgets(line, sizeof(line), f) != NULL) {
        char *p = line;
        while (*p == ' ' || *p == '\t') p++;
        if (*p == '\n' || *p == '\0') continue;

        total_refs++;

        char *endptr;
        unsigned long DV = strtoul(p, &endptr, 0);
        unsigned long offset = DV & mask;
        unsigned long nvp = DV >> b;

        int marco = buscar_en_frames(frames, Nmarcos, (long)nvp);
        int hit = 0;

        if (marco != -1) {
            hit = 1;
            frames[marco].usado = 1;
        } else {
            page_faults++;
            marco = reloj_reemplazo(frames, Nmarcos, &puntero_reloj, (long)nvp);
        }

        unsigned long DF = ((unsigned long)marco << b) | offset;

        if (verbose) {
            printf("DV=0x%lx nvp=0x%lx offset=0x%lx %s marco=%d DF=0x%lx\n",
                   DV, nvp, offset,
                   hit ? "HIT" : "FALLO",
                   marco, DF);
        }
    }

    fclose(f);

    printf("Totales:\n");
    printf("  Referencias: %ld\n", total_refs);
    printf("  Fallos de pagina: %ld\n", page_faults);

    if (total_refs > 0) {
        double tasa = (double)page_faults / (double)total_refs;
        printf("  Tasa de fallos: %.6f\n", tasa);
    }

    free(frames);
    return 0;
}
