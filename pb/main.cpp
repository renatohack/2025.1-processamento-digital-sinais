#include <Arduino.h>

#define MIC_PIN A0
#define BUTTON_DISPLAY_PIN 4
#define BUTTON_COLETA_PIN 2

#define PI 3.14159265358979323846
#define NUM_AMOSTRAS 256

bool lastButtonStateDisplay = HIGH;
bool lastButtonStateColeta = HIGH;
bool toggleState = false;

float tempoTotalAmostra = 0.256;  // f_s = 1000 Hz
float threshold = 15;  // Ajuste conforme a escala dos int16
float frequenciaAmostragem;
float deltaFrequencia;

int16_t amostras[NUM_AMOSTRAS];
int16_t magnitudes[NUM_AMOSTRAS];

float freqDominant = 0.0;
float ampDominant = 0.0;
float freqMax = 0.0;
float ampMax = 0.0;

void setup() {
  Serial.begin(9600);
  pinMode(BUTTON_DISPLAY_PIN, INPUT_PULLUP);
  pinMode(BUTTON_COLETA_PIN, INPUT_PULLUP);

  frequenciaAmostragem = NUM_AMOSTRAS / tempoTotalAmostra;  // 1000 Hz
  deltaFrequencia = frequenciaAmostragem / NUM_AMOSTRAS;    // ~3.9 Hz
}

void loop() {
  if (checkButtonToggle(BUTTON_COLETA_PIN, lastButtonStateColeta)) {
    Serial.println("Capturando e processando...");
    coletarEProcessar();
    atualizarDisplay();
  }

  if (checkButtonToggle(BUTTON_DISPLAY_PIN, lastButtonStateDisplay)) {
    toggleState = !toggleState;
    atualizarDisplay();
  }

  delay(50);
}

bool checkButtonToggle(int buttonPin, bool &lastButtonState) {
  bool currentButtonState = digitalRead(buttonPin);
  bool toggled = false;

  if (currentButtonState == LOW && lastButtonState == HIGH) {
    toggled = true;
  }

  lastButtonState = currentButtonState;
  return toggled;
}

void atualizarDisplay() {
  if (!toggleState) {
    Serial.print("Frequencia dominante: ");
    Serial.print(freqDominant, 1);
    Serial.print(" Hz | Amplitude: ");
    Serial.println(ampDominant, 2);
  } else {
    Serial.print("Frequencia maxima acima do threshold: ");
    Serial.print(freqMax, 1);
    Serial.print(" Hz | Amplitude: ");
    Serial.println(ampMax, 2);
  }
}

void coletarEProcessar() {
  capturarSinalReal(MIC_PIN, NUM_AMOSTRAS, tempoTotalAmostra, amostras);
  calcularDFT(amostras, magnitudes, NUM_AMOSTRAS);
  extrairResultados(magnitudes, NUM_AMOSTRAS, threshold, &freqDominant, &ampDominant, &freqMax, &ampMax);
}

void capturarSinalReal(int pin, int numAmostras, float tempoTotal, int16_t* buffer) {
  unsigned long tempoPorAmostra = (tempoTotal * 1e6) / numAmostras;
  unsigned long t0 = micros();

  for (int i = 0; i < numAmostras; i++) {
    buffer[i] = analogRead(pin) - 512;
    while (micros() - t0 < tempoPorAmostra * (i + 1));
  }
}

void calcularDFT(int16_t* amostras, int16_t* magnitudes, int numAmostras) {
  for (int k = 0; k < numAmostras; k++) {
    float real = 0;
    float imag = 0;

    for (int n = 0; n < numAmostras; n++) {
      float angle = 2 * PI * k * n / numAmostras;
      real += amostras[n] * cos(angle);
      imag -= amostras[n] * sin(angle);
    }

    float magnitude = sqrt(real * real + imag * imag) / numAmostras;
    magnitudes[k] = (int16_t)magnitude;
  }
}

void extrairResultados(int16_t* magnitudes, int numAmostras, float thresholdManual, float* freqDominant, float* ampDominant, float* freqMax, float* ampMax) {
  float maiorAmp = 0;
  int indiceMaiorAmp = 0;

  int indiceMaiorFreq = -1;
  float ampMaiorFreq = 0;

  // Faixa de análise (em bins)
  int binMin = ceil(80.0 / deltaFrequencia);   // Ex: 80 Hz
  int binMax = floor(300.0 / deltaFrequencia); // Ex: 300 Hz

  // Cálculo da média para threshold dinâmico
  float media = 0;
  int count = 0;
  for (int k = binMin; k <= binMax; k++) {
    media += magnitudes[k];
    count++;
  }
  media = (count > 0) ? media / count : 0;

  float threshold = max(media * 1.5, thresholdManual);  // threshold dinâmico com mínimo manual

  // Análise da faixa útil
  for (int k = binMin; k <= binMax; k++) {
    if (magnitudes[k] > maiorAmp) {
      maiorAmp = magnitudes[k];
      indiceMaiorAmp = k;
    }

    if (magnitudes[k] > threshold && k > indiceMaiorFreq) {
      indiceMaiorFreq = k;
      ampMaiorFreq = magnitudes[k];
    }
  }

  *freqDominant = indiceMaiorAmp * deltaFrequencia;
  *ampDominant = maiorAmp;

  if (indiceMaiorFreq >= 0) {
    *freqMax = indiceMaiorFreq * deltaFrequencia;
    *ampMax = ampMaiorFreq;
  } else {
    *freqMax = 0;
    *ampMax = 0;
  }
}

