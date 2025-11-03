// =============================================================================
// ARQUIVO: state_set.h
//
// PROPÓSITO:
//    Define a interface pública para o módulo 'state_set'.
//    Este módulo contém funções auxiliares para criar, comparar e
//    manipular a estrutura 'StateSet'.
// =============================================================================

#ifndef STATE_SET_H
#define STATE_SET_H

#include "automata.h" // Precisa da definição de StateSet e NFA

// --- Protótipos das Funções ---

// Inicializa um conjunto de estados como vazio.
void state_set_init(StateSet *set);

// Adiciona um estado (por seu índice) a um conjunto.
void state_set_add(StateSet *set, int state_index);

// Compara dois conjuntos de estados. Retorna 'true' se forem idênticos.
bool state_set_equals(StateSet *set1, StateSet *set2);

// Adiciona todos os estados do conjunto 'source' (origem) ao 'dest' (destino).
void state_set_union(StateSet *dest, StateSet *source);

// Cria uma string formatada (ex: "{q0,q1}") para um conjunto de estados.
void state_set_sprint_names(char *buffer, StateSet *set, NFA *nfa);

#endif // STATE_SET_H