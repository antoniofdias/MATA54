# Primeiro trabalho de Estrutura de Dados
Resolução de Hashing com encadeamento

## Compilação e execução
Na pasta raiz, rodar o comando _make_ para gerar a pasta 'bin' onde o executável 'a.out' será criado. Ao executá-lo, é criado o arquivo 'file.bin' (caso não exista), no qual serão feitas as operações sobre registros.

## Detalhes de implementação
O arquivo é estruturado sobre uma tabela _hash_ e iniciado com uma lista circular duplamente encadeada de posições vazias. Visando executar a maioria das operações sobre o arquivo em tempo linear, é mantido um rodapé com informações sobre o início da lista de posições vazias, o número de registros preenchidos e o somatório dos tamanhos de cadeia em progressão aritimética.