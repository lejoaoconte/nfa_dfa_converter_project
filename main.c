// =============================================================================
// ARQUIVO: main.c
//
// PROPÓSITO:
//    Ponto de entrada principal do programa. Ele orquestra o fluxo de
//    execução:
//    1. Valida os argumentos de entrada.
//    2. Chama o 'parser' (analisador) para ler o arquivo JFLAP e carregar o AFN.
//    3. Chama o 'reporter' (relator) para imprimir o AFN lido.
//    4. Chama o 'converter' (conversor) para executar o algoritmo AFN -> AFD.
//    5. Chama o 'reporter' novamente para imprimir e salvar o AFD resultante.
// =============================================================================

// Inclui os cabeçalhos dos módulos que ele precisa orquestrar.
// "automata.h" é necessário para as definições de struct (NFA, DFA).
#include "automata.h"
#include "jflap_parser.h"
#include "converter.h"
#include "reporter.h"

// --- Função Principal ---
int main(int argc, char *argv[])
{
    // Imprime um banner de boas-vindas no console.
    printf("\n------------- NFA → DFA Converter With JFLAP Files .jff -------------\n\n");

    // --- Validação de Entrada ---
    // 'argc' (argument count) deve ser 2: o nome do programa e o nome do arquivo.
    if (argc != 2)
    {
        fprintf(stderr, "Input: %s <file.jff>\n", argv[0]);
        return 0; // Retorna 0 para indicar um erro.
    }

    // Pega o nome do arquivo do segundo argumento ('argv[1]').
    const char *filename = argv[1];

    // Procura o último '.' no nome do arquivo para verificar a extensão.
    const char *extension = strrchr(filename, '.');
    if (!extension || strcmp(extension, ".jff") != 0)
    {
        fprintf(stderr, "Error: File must be JFLAP (.jff) format.\n");
        return 0;
    }

    // --- Execução Principal ---

    // Declara a estrutura do AFN que será preenchida.
    NFA nfa;
    // Tenta ler o arquivo. Se 'jflap_read_file' retornar 'false', houve um erro.
    if (!jflap_read_file(filename, &nfa))
    {
        fprintf(stderr, "Error: Can not read JFLAP file.\n");
        return 0;
    }

    // Se a leitura foi bem-sucedida, imprime o AFN lido no console.
    nfa_print(&nfa);

    // Declara a estrutura do AFD que será preenchida pela conversão.
    DFA dfa;
    // Executa o algoritmo de conversão.
    int convert_result = nfa_to_dfa_convert(&nfa, &dfa);
    if (convert_result == 0)
    {
        fprintf(stderr, "Error: Conversion from NFA to DFA failed.\n");
        return 0;
    }

    // Imprime o AFD resultante no console.
    dfa_print(&dfa, &nfa); // Passa o NFA junto para obter os nomes dos estados.

    // Salva o AFD em dois formatos de arquivo diferentes.
    dfa_save_txt(&dfa, &nfa, "./output/result.txt");
    dfa_save_jflap(&dfa, &nfa, "./output/result.jff");

    // Imprime a mensagem final de sucesso.
    printf("\n\nThe convertion was be finished!\n");
    printf("Files:\n");
    printf("./output/result.txt - Description\n");
    printf("./output/result.jff - JFLAP File\n");

    return 0; // Retorna 0 para indicar sucesso.
}
