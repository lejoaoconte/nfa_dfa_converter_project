// =============================================================================
// ARQUIVO: state_set.c
//
// PROPÓSITO:
//    Implementa as funções auxiliares para manipulação de 'StateSet'.
// =============================================================================

#include "state_set.h"

// Inicializa um conjunto de estados, definindo todos os estados como 'false'
// e o tamanho como 0. 'memset' é uma função rápida para preencher memória.
void state_set_init(StateSet *set)
{
    memset(set->states, false, sizeof(set->states));
    set->size = 0;
}

// Adiciona um estado a um conjunto.
void state_set_add(StateSet *set, int state_index)
{
    // Verifica se o estado já não está no conjunto antes de adicionar.
    if (!set->states[state_index])
    {
        set->states[state_index] = true;
        set->size++; // Incrementa o contador de tamanho.
    }
}

// Compara dois conjuntos de estados.
bool state_set_equals(StateSet *set1, StateSet *set2)
{
    // 'memcmp' compara blocos de memória. É a forma mais rápida de
    // verificar se os dois arrays 'states' são idênticos.
    return memcmp(set1->states, set2->states, sizeof(set1->states)) == 0;
}

// Realiza a operação de união (destino = destino U origem).
void state_set_union(StateSet *dest, StateSet *source)
{
    // Itera por todos os estados possíveis.
    for (int i = 0; i < MAX_STATES; i++)
    {
        // Se o estado 'i' existe no conjunto de origem...
        if (source->states[i])
        {
            // ...adiciona-o ao conjunto de destino.
            // A função 'state_set_add' já lida com duplicatas.
            state_set_add(dest, i);
        }
    }
}

// Cria uma string legível para um conjunto de estados (ex: "{q0,q1}").
void state_set_sprint_names(char *buffer, StateSet *set, NFA *nfa)
{
    // Se o conjunto estiver vazio, usa o símbolo de conjunto vazio.
    if (set->size == 0)
    {
        strcpy(buffer, "∅");
        return;
    }

    // Inicia a string com '{'.
    strcpy(buffer, "{");
    bool is_first = true; // Flag para controlar a adição de vírgulas.

    // Itera por todos os estados do NFA.
    for (int i = 0; i < nfa->num_states; i++)
    {
        // Se o estado 'i' pertence a este conjunto...
        if (set->states[i])
        {
            // Adiciona uma vírgula antes de todos, exceto do primeiro.
            if (!is_first)
                strcat(buffer, ",");

            // Adiciona o nome do estado (ex: "q0") à string.
            strcat(buffer, nfa->state_names[i]);
            is_first = false; // Os próximos estados não são o primeiro.
        }
    }
    // Fecha a string com '}'.
    strcat(buffer, "}");
}