// =============================================================================
// ARQUIVO: reporter.c
//
// PROPÓSITO:
//    Implementa as funções de geração de relatórios (impressão no console
//    e salvamento em arquivos).
// =============================================================================

#include "reporter.h"  // Inclui seu próprio cabeçalho
#include "state_set.h" // Precisa de 'state_set_sprint_names'

// --- Funções Públicas ---

// Imprime o NFA lido.
void nfa_print(NFA *nfa)
{
    printf("\nNFA Readed:\n");
    printf("Initial State: %s\n", nfa->state_names[nfa->start_state_index]);
    printf("Final States: ");
    bool is_first = true;
    for (int i = 0; i < nfa->num_states; i++)
    {
        if (nfa->final_states[i])
        {
            if (!is_first)
                printf(", ");
            printf("%s", nfa->state_names[i]);
            is_first = false;
        }
    }
    printf("\n\t\t Transition Table\t\n");
    printf("==================================================\n");
    char buffer[256]; // Buffer para nomes de conjuntos

    // Imprime cabeçalho da tabela (símbolos)
    printf("||\t%s\t||", "State");
    for (int i = 0; i < nfa->num_symbols; i++)
        printf("\t%c\t||", nfa->alphabet[i]);
    printf("\n");

    // Imprime linhas da tabela (uma para cada estado)
    for (int i = 0; i < nfa->num_states; i++)
    {
        // Adiciona prefixos "->" (inicial) e "*" (final)
        char prefix[5] = "  ";
        if (i == nfa->start_state_index)
            strcpy(prefix, "->");
        if (nfa->final_states[i])
            strcat(prefix, "*");
        else
            strcat(prefix, " ");

        // Imprime o nome do estado
        printf("||\t%s %s\t||\t", prefix, nfa->state_names[i]);

        // Imprime as transições para cada símbolo
        for (int j = 0; j < nfa->num_symbols; j++)
        {
            // Gera a string do conjunto de destino (ex: "{q1,q2}")
            state_set_sprint_names(buffer, &nfa->transitions[i][j], nfa);
            printf("%s\t||\t", buffer); // Imprime com alinhamento
        }
        printf("\n");
    }
}

// Imprime o DFA resultante.
void dfa_print(DFA *dfa, NFA *nfa)
{
    char buffer[512]; // Buffer para nomes de conjuntos (estados do DFA)
    printf("\nDFA Resulted:\n");

    // Imprime estado inicial
    state_set_sprint_names(buffer, &dfa->dfa_states[dfa->start_state_index], nfa);
    printf("Initial State: %s\n", buffer);

    // Imprime estados finais
    printf("Final States: ");
    bool is_first = true;
    for (int i = 0; i < dfa->num_states; i++)
    {
        if (dfa->final_states[i])
        {
            if (!is_first)
                printf(", ");
            state_set_sprint_names(buffer, &dfa->dfa_states[i], nfa);
            printf("%s", buffer);
            is_first = false;
        }
    }

    printf("\n\t\t Transition Table\t\n");
    printf("==================================================\n");

    // Imprime cabeçalho da tabela (símbolos)

    printf("||   %s\t||", "State");
    for (int i = 0; i < dfa->num_symbols; i++)
        printf("\t%c\t||", dfa->alphabet[i]);
    printf("\n");

    // Imprime linhas da tabela (uma para cada estado do DFA)
    for (int i = 0; i < dfa->num_states; i++)
    {
        // Adiciona prefixos "->" (inicial) e "*" (final)
        char prefix[5] = "  ";
        if (i == nfa->start_state_index)
            strcpy(prefix, "->");
        if (nfa->final_states[i])
            strcat(prefix, "*");
        else
            strcat(prefix, " ");

        // Imprime o nome do estado (ex: "{q0,q1}")
        state_set_sprint_names(buffer, &dfa->dfa_states[i], nfa);
        printf("||  %s%s\t||", prefix, buffer);

        // Imprime as transições para cada símbolo
        for (int j = 0; j < dfa->num_symbols; j++)
        {
            int dest_index = dfa->transitions[i][j];
            if (dest_index != -1)
            {
                // Gera a string do conjunto de destino
                state_set_sprint_names(buffer, &dfa->dfa_states[dest_index], nfa);
                printf("\t%s\t||", buffer);
            }
            else
            {
                printf("\t%s\t||", "∅"); // Sem transição
            }
        }
        printf("\n");
    }
}

// Salva o relatório em .txt.
void dfa_save_txt(DFA *dfa, NFA *nfa, const char *filename)
{
    FILE *f = fopen(filename, "w"); // Abre o arquivo para escrita ("w" = write)
    if (!f)
    {
        perror("ERROR when saving .txt file");
        return;
    }

    char buffer[512]; // Buffer para nomes de estados

    fprintf(f, "DFA Report\n");
    fprintf(f, "=============================\n\n");

    // Salva estado inicial
    state_set_sprint_names(buffer, &dfa->dfa_states[dfa->start_state_index], nfa);
    fprintf(f, "Initial State: %s\n\n", buffer);

    // Salva lista de estados finais
    fprintf(f, "Final States:\n");
    for (int i = 0; i < dfa->num_states; i++)
    {
        if (dfa->final_states[i])
        {
            state_set_sprint_names(buffer, &dfa->dfa_states[i], nfa);
            fprintf(f, "  - %s\n", buffer);
        }
    }

    // Imprime a tabela de transições detalhada
    fprintf(f, "\nDetailed Transition Table:\n");
    fprintf(f, "==================================================\n");
    // Imprime cabeçalho da tabela (símbolos)
    fprintf(f, "||     %s      ||", "State");
    for (int i = 0; i < dfa->num_symbols; i++)
        fprintf(f, "      %c      ||", dfa->alphabet[i]);
    fprintf(f, "\n");

    // Imprime linhas da tabela (uma para cada estado do DFA)
    for (int i = 0; i < dfa->num_states; i++)
    {
        // Adiciona prefixos "->" (inicial) e "*" (final)
        char prefix[5] = "  ";
        if (i == nfa->start_state_index)
            strcpy(prefix, "->");
        if (nfa->final_states[i])
            strcat(prefix, "*");
        else
            strcat(prefix, " ");

        // Imprime o nome do estado (ex: "{q0,q1}")
        state_set_sprint_names(buffer, &dfa->dfa_states[i], nfa);
        fprintf(f, "||  %s%s\t\t", prefix, buffer);
        if(i == 0){
            fprintf(f, "\t\t||");
        }else{
            fprintf(f, "||");
        }

        // Imprime as transições para cada símbolo
        for (int j = 0; j < dfa->num_symbols; j++)
        {
            int dest_index = dfa->transitions[i][j];
            if (dest_index != -1)
            {
                // Gera a string do conjunto de destino
                state_set_sprint_names(buffer, &dfa->dfa_states[dest_index], nfa);
                fprintf(f, "\t\t%s\t ", buffer);
                if(buffer[3] == ',' && i == 1){
                    fprintf(f, "||");
                }else{
                    fprintf(f, "\t||");
                }
            }
            else
            {
                fprintf(f, "\t%s\t||", "∅"); // Sem transição
            }
        }
        fprintf(f, "\n");
    }

    fclose(f); // Fecha o arquivo
}

// Salva o relatório em .jff.
void dfa_save_jflap(DFA *dfa, NFA *nfa, const char *filename)
{
    FILE *f = fopen(filename, "w");
    if (!f)
    {
        perror("ERROR when saving .jff file");
        return;
    }

    char buffer[512]; // Buffer para nomes de estados

    // --- Escreve o Cabeçalho XML do JFLAP ---
    fprintf(f, "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n");
    fprintf(f, "\n");
    fprintf(f, "<structure>\n\t<type>fa</type>\n\t<automaton>\n");

    // --- Escreve os Estados ---
    fprintf(f, "\t\t\n");
    int x = 50, y = 100; // Posições X/Y para layout visual no JFLAP
    for (int i = 0; i < dfa->num_states; i++)
    {
        // O nome do estado no JFLAP será o nosso conjunto (ex: "{q0,q1}")
        state_set_sprint_names(buffer, &dfa->dfa_states[i], nfa);
        fprintf(f, "\t\t<state id=\"%d\" name=\"%s\">\n", i, buffer);
        fprintf(f, "\t\t\t<x>%.1f</x>\n\t\t\t<y>%.1f</y>\n", (float)x, (float)y);

        // Adiciona tags <initial/> e <final/> se necessário
        if (i == dfa->start_state_index)
            fprintf(f, "\t\t\t<initial/>\n");
        if (dfa->final_states[i])
            fprintf(f, "\t\t\t<final/>\n");

        fprintf(f, "\t\t</state>\n");

        // Atualiza as coordenadas para o próximo estado
        x += 180;    // Move para a direita
        if (x > 800) // Se passar da borda, quebra a linha
        {
            x = 50;
            y += 100;
        }
    }

    // --- Escreve as Transições ---
    fprintf(f, "\t\t\n");
    for (int i = 0; i < dfa->num_states; i++) // Para cada estado de origem
    {
        for (int j = 0; j < dfa->num_symbols; j++) // Para cada símbolo
        {
            int dest_index = dfa->transitions[i][j];
            // Se houver uma transição (não for -1)...
            if (dest_index != -1)
            {
                fprintf(f, "\t\t<transition>\n");
                fprintf(f, "\t\t\t<from>%d</from>\n", i);                // ID de origem
                fprintf(f, "\t\t\t<to>%d</to>\n", dest_index);           // ID de destino
                fprintf(f, "\t\t\t<read>%c</read>\n", dfa->alphabet[j]); // Símbolo
                fprintf(f, "\t\t</transition>\n");
            }
        }
    }

    // --- Fecha o XML ---
    fprintf(f, "\t</automaton>\n</structure>\n");
    fclose(f); // Fecha o arquivo
}