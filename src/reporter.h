// =============================================================================
// ARQUIVO: reporter.h
//
// PROPÓSITO:
//    Define a interface pública do módulo 'reporter'.
//    Este módulo é responsável por toda a saída de dados:
//    - Imprimir autômatos no console.
//    - Salvar o resultado em arquivos (.txt e .jff).
// =============================================================================

#ifndef REPORTER_H
#define REPORTER_H

#include "automata.h" // Precisa das definições de NFA e DFA

// --- Protótipos das Funções Públicas ---

// Imprime uma tabela formatada do NFA no console.
void nfa_print(NFA *nfa);

// Imprime uma tabela formatada do DFA no console.
// Requer o NFA para traduzir os conjuntos de estados em nomes.
void dfa_print(DFA *dfa, NFA *nfa);

// Salva um relatório de texto detalhado do DFA em um arquivo.
void dfa_save_txt(DFA *dfa, NFA *nfa, const char *filename);

// Salva o DFA resultante em um novo arquivo .jff, pronto para ser
// aberto no JFLAP.
void dfa_save_jflap(DFA *dfa, NFA *nfa, const char *filename);

#endif // REPORTER_H