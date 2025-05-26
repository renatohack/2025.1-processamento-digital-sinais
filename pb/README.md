# Analisador Espectral com DFT em Arduino

Este projeto consiste em um sistema embarcado desenvolvido com Arduino Uno R3 para análise espectral de sinais sonoros reais. A análise é feita por meio da Transformada Discreta de Fourier (DFT) implementada manualmente, sem uso de bibliotecas externas.

## Objetivo

Implementar um analisador espectral capaz de identificar:
- A frequência dominante (de maior amplitude).
- A frequência máxima acima de um threshold.

As informações são exibidas via Serial Monitor da Arduino IDE.

## Componentes utilizados

- Arduino Uno R3  
- Sensor de som KY-037 (usada apenas a saída analógica A0)  
- 2 botões microchave (coleta e alternância de exibição)  
- Protoboard e jumpers

## Funcionamento

1. Ao pressionar o botão de coleta (ligado ao pino D2), o sistema:
   - Captura 256 amostras do microfone em 0,256 s (1000 Hz de amostragem).
   - Aplica a DFT no sinal coletado.
   - Calcula a frequência dominante e a maior frequência acima do threshold.

2. O botão de exibição (ligado ao D4) alterna entre os dois modos de saída no Serial Monitor.

3. A DFT é calculada apenas para a faixa de 100 a 300 Hz, com threshold dinâmico baseado na média local.

## Testes

Foram realizados testes com sons vocais de diferentes intensidades e tons, além de capturas em silêncio. A frequência de amostragem foi ajustada para 1000 Hz para garantir precisão na faixa da voz humana.

## Unidades e Medidas

- As frequências são apresentadas em Hz.
- As amplitudes são calculadas a partir das leituras analógicas da porta A0, subtraídas de 512 (centro do range 0–1023).
- As amplitudes resultantes são valores relativos, sem unidade física absoluta (não é dB nem V), servindo apenas como métrica comparativa no espectro.

## Como usar

1. Conecte os componentes conforme o esquema elétrico do projeto.
2. Faça o upload do código via Arduino IDE.
3. Abra o Serial Monitor (baud rate 9600).
4. Pressione o botão de coleta (D2) para capturar e processar.
5. Pressione o botão de exibição (D4) para alternar entre os modos de saída.

## Links úteis

- Repositório do código-fonte: https://github.com/renatohack/2025.1-processamento-digital-sinais/tree/main/pb
- Detector de pitch online utilizado como referência: https://www.onlinemictest.com/tuners/pitch-detector/