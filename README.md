# PPM

Problema e programa para leitura de arquivos .ppm em C! 

# Running

Build: 

```console
$ make
```

Execute:

```console
$ ./main test.ppm
```

Output:
```console
compressed:
1 3 0 0 0
0 0 4 0 0
2 0 0 2 0
1 2 0 0 1
Contrast:
255 255 255 0 0 0 0 0 0 0 0 0
255 255 255 255 255 255 255 255 255 255 255 255
255 255 255 255 255 255 255 255 255 255 255 255
255 255 255 0 0 0 0 0 0 255 255 255
```

# Problema

Embora os computadores trabalhem com zeros e uns, é mais conveniente para as pessoas usarem outras formas de representar dados. No caso de informação visual, diferentes formatos de arquivo adotam diferentes abordagens para codificar as cores dos pixels que compõe uma imagem digital, permitindo sua leitura, análise e edição. O formato .ppm, por exemplo, permite a criação de imagens simplesmente escrevendo valores RGB (do inglês Red, Green and Blue) em um arquivo texto e mudando sua extensão antes de abri-lo em um software de desenho. Considere o arquivo abaixo:

```
P3 2 2 255
0 0 0 255 0 0
0 255 0 255 0 0
```
Na primeira linha temos o código do formato do arquivo (P3), dois valores indicando as dimensões da imagem em número de pixels (2x2=4 pixels) e o intervalo de intensidade das cores primárias que colorem cada pixel (0-255). Como o nome RGB sugere, estamos falando de um modelo que sobrepõe três cores primárias (vermelho, verde e azul) em diferentes intensidades para gerar um espectro com 256^3 cores. Cada cor do espectro corresponde, então, a três valores (um para cada primária) entre 0-255, sendo necessário usar três posições de uma matriz para armazenar a cor de um único pixel. Na segunda e terceira linhas do exemplo temos os três valores RGB de cada um dos quatro pixels da imagem. Observe que na prática, essa imagem 2x2 precisa ser codificada em uma matriz 2x(2*3)=2x6, portanto a codificação de cores usando valores RGB infla o tamanho em bytes da imagem.

Felizmente, algoritmos de compressão permitem que os dados de um arquivo sejam codificados de forma mais econômica. No caso de imagens, uma abordagem simples de compressão é a contagem de cores consecutivas (leia mais). Por exemplo, considere a matriz 4x12 usada para codificar uma figura 4x4:
```
P3 4 4 255
0 0 0 255 0 0 255 0 0 255 0 0
0 255 0 0 255 0 0 255 0 0 255 0
0 0 0 0 0 0 0 0 255 0 0 255
0 0 0 255 0 0 255 0 0 255 255 0
```
Na primeira linha da matriz temos um pixel de cor 0 0 0 seguido por três pixels de cor 255 0 0. Podemos então representar a primeira cor com o valor 1 (um pixel consecutivo), a segunda cor com o valor 3 (três pixels consecutivos) e as demais cores com o valor 0 (zero pixels consecutivos). Esses valores são colocados em um vetor cujos índices sempre correspondem a uma cor específica, ou seja, assumindo que os índices 0, 1, 2, 3 e 4 correspondem às cores 0 0 0, 255 0 0, 0 255 0, 0 0 255 e 255 255 0, respectivamente, a primeira linha da matriz é representada pelo vetor [1, 3, 0, 0, 0]. A segunda linha possui quatro pixels de cor 0 255 0 e zero pixels de outra cor, então seu vetor fica [0, 4, 0, 0, 0] e assim por diante. Ao final da compressão temos a seguinte matriz 4x5:
```
1 3 0 0 0
0 0 4 0 0
2 0 0 2 0
1 2 0 0 1
```
Uma redução de 23 índices em relação à matriz original!

Neste trabalho você deve escrever um programa que receba as linhas de um arquivo .ppm, leia a matriz de cores em formato RGB e imprima a matriz comprimida. Todas as matrizes corresponderão a uma imagem quadrada de até 12x12 pixels, pintada com as mesmas cinco cores anteriores e cuja representação comprimida deve seguir a ordem estabelecida anteriormente (índices 0, 1, 2, 3 e 4 correspondem às cores 0 0 0, 255 0 0, 0 255 0, 0 0 255 e 255 255 0, respectivamente). Essa mesma ordem dita a incidência das cores na linha, por exemplo: assumindo que as cinco cores estejam presentes, elas sempre aparecerão na ordem 0 0 0, 255 0 0, 0 255 0, 0 0 255 e 255 255 0; se apenas quatro cores estiverem presentes, essas quatro seguirão a mesma ordem, pulando a cor ausente; o mesmo vale para quando apenas três, duas ou uma cores estivem presentes na linha. Similarmente, a mesma cor não aparecerá múltiplas vezes na mesma linha, ou seja, se a linha já possui X pixels consecutivos de cor A, não haverá outra sequência de Y pixels consecutivos de cor A.

Após imprimir a matriz comprimida, imagine que você queira destacar uma parte específica da imagem original, criando uma silhueta em alto contraste para melhor visualizar um exame médico, por exemplo. Tal efeito pode ser obtido alterando a cor dos pixels irrelevantes para um tom claro e a cor dos pixels importantes para um tom escuro. Neste caso, determine a cor com o maior número de pixels na imagem e imprima uma matriz RGB substituindo essa cor por 0 0 0 e substituindo as outras cores por 255 255 255. No exemplo abaixo, a cor mais presente foi 255 0 0, então os pixels dessa cor mudaram para 0 0 0, enquanto os outros mudaram para 255 255 255. Em caso de empates, mantenha as cores majoritárias inalteradas e mude apenas as cores minoritárias para 255 255 255.

Exemplo de execução:
```
P3 4 4 255
0 0 0 255 0 0 255 0 0 255 0 0
0 255 0 0 255 0 0 255 0 0 255 0
0 0 0 0 0 0 0 0 255 0 0 255
0 0 0 255 0 0 255 0 0 255 255 0
```
```
1 3 0 0 0
0 0 4 0 0
2 0 0 2 0
1 2 0 0 1
```
```
255 255 255 0 0 0 0 0 0 0 0 0
255 255 255 255 255 255 255 255 255 255 255 255
255 255 255 255 255 255 255 255 255 255 255 255
255 255 255 0 0 0 0 0 0 255 255 255
```