// =============================================================================
// ARQUIVO: jflap_parser.h
//
// PROPÓSITO:
//    Define a interface pública do módulo 'jflap_parser'.
//    Sua única responsabilidade é ler um arquivo .jff e preencher
//    uma estrutura NFA com os dados.
// =============================================================================

#ifndef JFLAP_PARSER_H
#define JFLAP_PARSER_H

#include "automata.h" // Precisa da definição de NFA

// --- Protótipo da Função Pública ---

// Lê o arquivo JFLAP (.jff) e preenche a estrutura NFA.
// Retorna 'true' em caso de sucesso e 'false' em caso de erro.
bool jflap_read_file(const char *filename, NFA *nfa);

#endif // JFLAP_PARSER_H