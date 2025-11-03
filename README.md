# Conversor de AFN em AFD Utilizando Arquivos JFLAP
## Alunos: Rafaela Oliveira e João Pedro Conte

---

### Introdução
Este trabalho tem como objetivo apresentar um conversor em C de arquivos JFLAP de Autômatos Finitos Não Deterministicos (AFN) em Autômatos Finitos Deterministicos (AFD) ou, nas siglas em inglês, que irá encontrar ao longo do código, DFA e NFA, na ordem.
Basicamente a ideia principal do código foi entender como funciona um arquivo JFLAP, oude foi verificado que o mesmo tipo de arquivo é um arquivo do tipo .xml e após isso entender o mesmo, obter os dados do mesmo e fazer a covnersão dos dados, imprimindo novamente outro .xml. Abaixo está um exemplo do tipo de arquivo do JFLAP
```xml
<?xml version="1.0" encoding="UTF-8" standalone="no"?><!--Created with JFLAP 7.1.--><structure>
	<type>fa</type>
	<automaton>
		<!--The list of states.-->
		<state id="0" name="q0">
			<x>35.0</x>
			<y>85.0</y>
			<initial/>
		</state>
		<state id="1" name="q1">
			<x>190.0</x>
			<y>91.0</y>
		</state>
		<state id="2" name="q2">
			<x>386.0</x>
			<y>88.0</y>
			<final/>
		</state>
		<!--The list of transitions.-->
		<transition>
			<from>1</from>
			<to>0</to>
			<read>b</read>
		</transition>
		<transition>
			<from>0</from>
			<to>1</to>
			<read>a</read>
		</transition>
		<transition>
			<from>1</from>
			<to>2</to>
			<read>b</read>
		</transition>
		<transition>
			<from>2</from>
			<to>1</to>
			<read>a</read>
		</transition>
	</automaton>
</structure>
```

### Como Rodar o Projeto

Para rodar o projeto é bem simples, dentro da pasta ./test há 5 arquivos de exemplo de teste, entre eles há dois com erro para testar se as partes de erro estão corretos também, antes devemos compilar o código com o comando abaixo:

```bash
gcc -Wall -Wextra -std=c11 -I./src -o main main.c src/state_set.c src/jflap_parser.c src/converter.c src/reporter.c
```

Após isso um arquvio ./main irá ser gerado e o seguinte comando deve ser dado:

```bash
./main <arquivo_de_test>.jff
```

Se quiser utilizar algum arquivo de teste disponiblizado pode ser utilizado o comando abaixo

```bash
./main ./test/test.jff
```

### Estrutura do Projeto

```
project
│   README.md
│   main.c   
│   .gitignore   
└───output
└───src
│   │   automata.h
│   │   converter.c
│   │   converter.h
│   │   jflap_parser.c
│   │   jflap_parser.h
│   │   reporter.c
│   │   reporter.h
│   │   state_set.c
│   │   state_set.h
└───test
    │   test.jff
    │   test2.jff
    │   test3.jff
    │   test4_error.jff
    │   test5_error.jff
```
