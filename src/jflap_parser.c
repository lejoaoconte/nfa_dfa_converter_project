// =============================================================================
// ARQUIVO: jflap_parser.c
//
// PROPÓSITO:
//    Implementa a lógica de leitura e análise (parsing) do arquivo JFLAP.
//    Como o JFLAP usa XML, este arquivo contém funções para "dissecar"
//    o XML de forma manual (buscando por tags e atributos) e preencher
//    a estrutura NFA.
// =============================================================================

#include "jflap_parser.h" // Inclui seu próprio cabeçalho
#include "state_set.h"    // Precisa das funções de 'state_set'

// --- Funções Auxiliares Estáticas ---
// (static = visível apenas dentro deste arquivo)

// Inicializa a estrutura NFA com valores padrão (zeros e -1).
static void nfa_init(NFA *nfa)
{
    nfa->num_states = 0;
    nfa->num_symbols = 0;
    nfa->start_state_index = -1;
    // Preenche os arrays com valores padrão.
    memset(nfa->final_states, false, sizeof(nfa->final_states));
    memset(nfa->alphabet, 0, sizeof(nfa->alphabet));
    // -1 indica um ID não mapeado.
    for (int i = 0; i < MAX_STATES * 2; i++)
        nfa->id_to_index_map[i] = -1;

    // Inicializa a tabela de transição (matriz de conjuntos de estados).
    for (int i = 0; i < MAX_STATES; i++)
    {
        for (int j = 0; j < MAX_ALPHABET; j++)
        {
            state_set_init(&nfa->transitions[i][j]);
        }
    }
}

// Extrai o valor de um atributo XML (ex: de 'id="0"' extrai "0").
static char *extract_attribute(const char *line, const char *attr, char *buffer)
{
    char search_str[50];
    sprintf(search_str, "%s=\"", attr); // Cria a string de busca (ex: "id=\"")

    char *start = strstr(line, search_str); // Encontra a string de busca na linha
    if (!start)
        return NULL; // Atributo não encontrado

    start += strlen(search_str);    // Avança o ponteiro para depois de '="'
    char *end = strchr(start, '"'); // Encontra a aspa de fechamento
    if (!end)
        return NULL; // Linha mal formatada

    // Copia o conteúdo entre 'start' e 'end' para o 'buffer'.
    strncpy(buffer, start, end - start);
    buffer[end - start] = '\0'; // Adiciona o terminador nulo.
    return buffer;
}

// Extrai o valor entre tags XML (ex: de '<from>1</from>' extrai "1").
static char *extract_tag_value(const char *line, const char *tag, char *buffer)
{
    char start_tag[50], end_tag[50];
    sprintf(start_tag, "<%s>", tag); // ex: "<from>"
    sprintf(end_tag, "</%s>", tag);  // ex: "</from>"

    char *start = strstr(line, start_tag); // Encontra a tag de abertura
    if (!start)
        return NULL;
    start += strlen(start_tag); // Avança para depois da tag

    char *end = strstr(start, end_tag); // Encontra a tag de fechamento
    if (!end)
        return NULL;

    // Copia o conteúdo para o buffer.
    strncpy(buffer, start, end - start);
    buffer[end - start] = '\0';

    // --- Limpeza de Espaços em Branco (Trim) ---
    // Remove espaços no início
    char *p = buffer;
    while (*p && isspace((unsigned char)*p))
        p++;
    memmove(buffer, p, strlen(p) + 1); // Desloca a string para a esquerda

    // Remove espaços no fim
    char *end_p = buffer + strlen(buffer) - 1;
    while (end_p > buffer && isspace((unsigned char)*end_p))
        *end_p-- = '\0';

    return buffer;
}

// --- Função Pública ---

bool jflap_read_file(const char *filename, NFA *nfa)
{
    // Tenta abrir o arquivo para leitura ("r" = read).
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        perror("ERROR when open file"); // 'perror' imprime o erro do sistema
        return false;
    }

    printf("Reading JFLAP file: %s\n", filename);
    nfa_init(nfa); // Prepara a estrutura NFA.

    // Array temporário para guardar as transições lidas.
    TempTransition temp_transitions[MAX_TEMP_TRANSITIONS];
    int num_temp_transitions = 0;

    char line[MAX_LINE_LEN]; // Buffer para ler cada linha
    int current_id = -1;     // ID do estado sendo processado no momento

    // --- 1ª PASSAGEM: Ler Estados ---
    // Lê o arquivo linha por linha para encontrar estados, seus nomes, ID,
    // e se são iniciais ou finais.
    while (fgets(line, sizeof(line), file))
    {
        char id_buffer[20], name_buffer[MAX_STATE_NAME_LEN];

        // Se a linha contém "<state id" ...
        if (strstr(line, "<state id"))
        {
            // Validação de segurança: verifica limite de estados.
            if (nfa->num_states >= MAX_STATES)
            {
                fprintf(stderr, "ERROR: Quantity of state extends than %d.\n", MAX_STATES);
                fclose(file);
                return false;
            }
            // Extrai o ID e o NOME do estado.
            extract_attribute(line, "id", id_buffer);
            extract_attribute(line, "name", name_buffer);
            current_id = atoi(id_buffer); // Converte ID de string para inteiro.

            // Validação de segurança: verifica se o ID é muito grande.
            if (current_id >= MAX_STATES * 2)
            {
                fprintf(stderr, "ERROR: State id '%d' is very lengthful.\n", current_id);
                fclose(file);
                return false;
            }

            // Mapeia o ID (ex: 5) para o índice interno (ex: 2).
            nfa->id_to_index_map[current_id] = nfa->num_states;
            // Armazena o nome do estado.
            strcpy(nfa->state_names[nfa->num_states], name_buffer);
            nfa->num_states++; // Incrementa o contador de estados.
        }
        // Se a linha contém "<initial/>" (e estamos dentro de um estado)
        if (strstr(line, "<initial/>") && current_id != -1)
        {
            nfa->start_state_index = nfa->id_to_index_map[current_id];
        }
        // Se a linha contém "<final/>" (e estamos dentro de um estado)
        if (strstr(line, "<final/>") && current_id != -1)
        {
            nfa->final_states[nfa->id_to_index_map[current_id]] = true;
        }
    }

    // Imprime NFA lidas (para depuração e demonstração).
    // for (int i = 0; i < nfa->num_states; i++)
    // {
    //     printf("State ID: %d, Name: %s, Index: %d, Is Final: %s\n",
    //            i,
    //            nfa->state_names[i],
    //            i,
    //            nfa->final_states[i] ? "Yes" : "No");
    // }
    // printf("Found states: %d\n", nfa->num_states);

    // --- 2ª PASSAGEM: Ler Transições ---
    rewind(file);               // Volta ao início do arquivo.
    bool in_transition = false; // Flag: estamos dentro de uma tag <transition>?
    int from = -1, to = -1;
    char read = '\0';

    while (fgets(line, sizeof(line), file))
    {
        // Detecta o início de um bloco de transição.
        if (strstr(line, "<transition>"))
        {
            in_transition = true;
            from = -1;
            to = -1;
            read = '\0'; // Reseta as variáveis
            continue;
        }
        // Detecta o fim de um bloco de transição.
        if (strstr(line, "</transition>"))
        {
            in_transition = false;
            // Se lemos todas as partes (from, to, read), armazena temporariamente.
            if (from != -1 && to != -1 && read != '\0')
            {
                // Validação de segurança: limite de transições.
                if (num_temp_transitions >= MAX_TEMP_TRANSITIONS)
                {
                    fprintf(stderr, "ERRO: Number of transitions exceeds the limit of %d.\n", MAX_TEMP_TRANSITIONS);
                    fclose(file);
                    return false;
                }
                temp_transitions[num_temp_transitions++] = (TempTransition){from, to, read};
            }
            continue;
        }
        // Se estivermos dentro de um bloco <transition>...
        if (in_transition)
        {
            char value_buffer[MAX_LINE_LEN];
            // Tenta extrair <from>, <to> e <read>.
            if (extract_tag_value(line, "from", value_buffer))
                from = atoi(value_buffer);
            if (extract_tag_value(line, "to", value_buffer))
                to = atoi(value_buffer);
            if (extract_tag_value(line, "read", value_buffer))
                read = value_buffer[0]; // Pega apenas o primeiro caractere.
        }
    }
    fclose(file); // Fecha o arquivo.

    // Imprime as transições lidas (para depuração e demonstração).
    // for (int i = 0; i < num_temp_transitions; i++)
    // {
    //     printf("Transition: from ID %d to ID %d on '%c'\n",
    //            temp_transitions[i].from_id,
    //            temp_transitions[i].to_id,
    //            temp_transitions[i].symbol);
    // }

    // --- 3ª ETAPA: Processar Transições Temporárias ---
    // Constrói o alfabeto e a tabela de transições final do NFA.
    for (int i = 0; i < num_temp_transitions; i++)
    {
        char symbol = temp_transitions[i].symbol;
        int symbol_index = -1;

        // Procura o símbolo no alfabeto.
        for (int j = 0; j < nfa->num_symbols; j++)
        {
            if (nfa->alphabet[j] == symbol)
            {
                symbol_index = j;
                break;
            }
        }
        // Se o símbolo é novo, adiciona ao alfabeto.
        if (symbol_index == -1)
        {
            // Validação de segurança: limite do alfabeto.
            if (nfa->num_symbols >= MAX_ALPHABET)
            {
                fprintf(stderr, "ERROR: Quantity of symbols exceeds the limit of %d.\n", MAX_ALPHABET);
                return false;
            }
            symbol_index = nfa->num_symbols++;
            nfa->alphabet[symbol_index] = symbol;
        }

        // Converte os IDs de 'from' e 'to' para nossos índices internos.
        int from_index = nfa->id_to_index_map[temp_transitions[i].from_id];
        int to_index = nfa->id_to_index_map[temp_transitions[i].to_id];

        // Se os índices são válidos, adiciona a transição na tabela.
        if (from_index != -1 && to_index != -1)
        {
            // Adiciona o estado 'to_index' ao conjunto de destino.
            state_set_add(&nfa->transitions[from_index][symbol_index], to_index);
        }
    }

    // Imprime o alfabeto extraído (para depuração e demonstração).
    // printf("Extract alphabet: ");
    // for (int i = 0; i < nfa->num_symbols; i++)
    //     printf("%c ", nfa->alphabet[i]);

    printf("JFLAP file read successfully!\n\n");
    return true;
}