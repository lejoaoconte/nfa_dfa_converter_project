// =============================================================================
// ARQUIVO: automata.h
//
// PROPÓSITO:
//    Cabeçalho central do projeto. Define todas as constantes globais,
//    bibliotecas padrão e as principais estruturas de dados (structs)
//    que representam os autômatos (NFA, DFA) e seus componentes.
// =============================================================================

#ifndef AUTOMATA_H // "Header Guard" para evitar inclusão dupla
#define AUTOMATA_H

// --- Inclusões de Bibliotecas Padrão ---
#include <stdio.h>   // Para entrada/saída (printf, FILE, fopen, etc.)
#include <stdlib.h>  // Para alocação de memória (malloc, exit) e conversão (atoi)
#include <string.h>  // Para manipulação de strings (strcmp, strstr, strcpy, etc.)
#include <stdbool.h> // Para usar os tipos 'bool', 'true' e 'false'
#include <ctype.h>   // Para funções de caracteres (isspace)

// --- Constantes Configuráveis ---
#define MAX_STATES 50            // Limite de estados do NFA
#define MAX_ALPHABET 26          // Limite de símbolos do alfabeto
#define MAX_STATE_NAME_LEN 32    // Comprimento máximo do nome de um estado (ex: "q0")
#define MAX_LINE_LEN 1024        // Tamanho máximo da linha lida do arquivo
#define MAX_TEMP_TRANSITIONS 500 // Limite de transições lidas do JFLAP
#define MAX_DFA_STATES 256       // Limite de estados gerados no DFA

// --- Estruturas de Dados ---

// Representa um conjunto de estados do NFA.
// Esta é a base para um estado do DFA.
typedef struct
{
    // Um array de booleanos. Se states[i] == true, o estado 'i' pertence ao conjunto.
    bool states[MAX_STATES];
    // Contador de quantos estados estão no conjunto (para otimização).
    int size;
} StateSet;

// Estrutura temporária usada apenas durante a leitura do JFLAP
// para armazenar transições antes de processá-las.
typedef struct
{
    int from_id; // ID original do estado de origem (do JFLAP)
    int to_id;   // ID original do estado de destino (do JFLAP)
    char symbol; // O símbolo (char) que dispara a transição
} TempTransition;

// Estrutura principal do Autômato Finito Não-Determinístico (NFA)
typedef struct
{
    int num_states;        // Número total de estados
    int num_symbols;       // Número total de símbolos no alfabeto
    int start_state_index; // Índice (interno) do estado inicial

    // Array de booleanos. Se final_states[i] == true, o estado 'i' é final.
    bool final_states[MAX_STATES];

    // Tabela de transição do NFA: [estado_origem][simbolo] -> Conjunto de estados de destino
    StateSet transitions[MAX_STATES][MAX_ALPHABET];

    // O alfabeto (array de caracteres)
    char alphabet[MAX_ALPHABET];

    // Nomes originais dos estados (ex: "q0", "q1") lidos do JFLAP
    char state_names[MAX_STATES][MAX_STATE_NAME_LEN];

    // Mapeia o ID do JFLAP (que pode ser não sequencial) para nosso índice interno (0, 1, 2...)
    int id_to_index_map[MAX_STATES * 2]; // Tamanho extra por segurança
} NFA;

// Estrutura principal do Autômato Finito Determinístico (DFA)
typedef struct
{
    int num_states;        // Número total de estados gerados
    int num_symbols;       // Número de símbolos (o mesmo do NFA)
    int start_state_index; // Índice (interno) do estado inicial do DFA

    // Array de booleanos para os estados finais do DFA.
    bool final_states[MAX_DFA_STATES];

    // Tabela de transição do DFA: [estado_origem][simbolo] -> UM estado de destino
    // Armazena o *índice* do estado de destino. -1 significa sem transição (estado de erro).
    int transitions[MAX_DFA_STATES][MAX_ALPHABET];

    // O alfabeto (copiado do NFA).
    char alphabet[MAX_ALPHABET];

    // Mapeia cada estado do DFA (índice 0, 1, 2...) ao seu
    // conjunto de estados do NFA correspondente.
    // Ex: dfa_states[0] pode ser o StateSet {q0}
    // Ex: dfa_states[1] pode ser o StateSet {q0, q1}
    StateSet dfa_states[MAX_DFA_STATES];
} DFA;

#endif // AUTOMATA_H