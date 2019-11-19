# Segundo trabalho de Estrutura de Dados
Programa para comprimir um arquivo

## Compilação e execução
Na pasta raiz, rodar o comando _make_ para gerar a pasta `bin` onde o executável `compressor` será criado.

Para executar o programa, usar o comando `bin/compressor $X nome_do_arquivo.extensao` substituindo a váriavel _X_ por _-c_, caso seja desejada uma compressão, ou _-d_ caso seja desejada uma descompressão.

Apenas arquivos comprimidos neste programa poderão ser descomprimidos pelo mesmo.

## Detalhes de implementação
O compressor faz uso do código de Huffman simples, armazenando a tabela de compressão em um novo arquivo junto com a codificação do arquivo original (em bits).

O melhor desempenho é esperado com arquivos de grandes repetições, como arquivos de texto.

## Contribuidores

* Antônio Dias - [antoniofdias](https://github.com/antoniofdias)
* Rafael Nagy - [rafaelcnagy](https://github.com/rafaelcnagy)