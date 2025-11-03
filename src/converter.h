// =============================================================================
// ARQUIVO: converter.h
//
// PROPÓSITO:
//    Define a interface pública do módulo 'converter'.
//    Este módulo contém a lógica central do projeto: o algoritmo
//    de Construção de Subconjuntos para converter um NFA em um DFA.
// =============================================================================

#ifndef CONVERTER_H
#define CONVERTER_H

#include "automata.h" // Precisa das definições de NFA e DFA

// --- Protótipo da Função Pública ---

// Executa o algoritmo de Construção de Subconjuntos.
// Recebe o NFA (leitura) e preenche a estrutura DFA (escrita).
bool nfa_to_dfa_convert(NFA *nfa, DFA *dfa);

#endif // CONVERTER_H