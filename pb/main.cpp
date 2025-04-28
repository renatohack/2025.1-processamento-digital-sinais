// INIT

// include
#include <Adafruit_LiquidCrystal.h>

// define
#define POT_PIN_0 A0
#define POT_PIN_1 A1
#define POT_PIN_2 A2

#define BUTTON_DISPLAY_PIN 4
#define BUTTON_COLETA_PIN 2

#define PI 3.14159265358979323846
#define NUM_AMOSTRAS 128

// variáveis
bool lastButtonStateDisplay = HIGH;
bool lastButtonStateColeta = HIGH;
bool toggleState = false;

float freqDominant = 0.0;
float ampDominant = 0.0;
float freqMax = 0.0;
float ampMax = 0.0;

float tempoTotalAmostra = 1;
float threshold = 0.2;
float amostras[NUM_AMOSTRAS];
float frequenciaAmostragem;
float deltaFrequencia;

Adafruit_LiquidCrystal lcd_1(0);

// SETUP
void setup() {
  lcd_1.begin(24, 2);
  lcd_1.setBacklight(1);

  Serial.begin(9600);

  pinMode(BUTTON_DISPLAY_PIN, INPUT_PULLUP);
  pinMode(BUTTON_COLETA_PIN, INPUT_PULLUP);
  
  frequenciaAmostragem = NUM_AMOSTRAS / tempoTotalAmostra;
  deltaFrequencia = frequenciaAmostragem / NUM_AMOSTRAS;
}

// LOOP
void loop() {
  if (checkButtonToggle(BUTTON_COLETA_PIN, lastButtonStateColeta)) {
    lcd_1.clear();
    lcd_1.setCursor(0, 0);
    lcd_1.print("Calculando...");
    coletarEProcessar();
    atualizarDisplay(toggleState, freqDominant, ampDominant, freqMax, ampMax);
  }

  if (checkButtonToggle(BUTTON_DISPLAY_PIN, lastButtonStateDisplay)) {
    toggleState = !toggleState;
    atualizarDisplay(toggleState, freqDominant, ampDominant, freqMax, ampMax);
  }

  delay(50);
}


// AUX FUNCTIONS
bool checkButtonToggle(int buttonPin, bool &lastButtonState) {
  bool currentButtonState = digitalRead(buttonPin);
  bool toggled = false;

  if (currentButtonState == LOW && lastButtonState == HIGH) {
    toggled = true;
  }
  
  lastButtonState = currentButtonState;
  return toggled;
}


void atualizarDisplay(bool toggleState, float freqDominant, float ampDominant, float freqMax, float ampMax) {
  lcd_1.clear();
  showDisplay(toggleState, freqDominant, ampDominant, freqMax, ampMax);
}


void showDisplay(bool toggleState, float freqDominant, float ampDominant, float freqMax, float ampMax) {
  lcd_1.setCursor(0, 0);

  if (!toggleState) {
    lcd_1.print("F_dom");
    lcd_1.setCursor(0, 1);
    printFreqAmp(freqDominant, ampDominant);
  } else {
    lcd_1.print("F_max");
    lcd_1.setCursor(0, 1);
    printFreqAmp(freqMax, ampMax);
  }
}



void printFreqAmp(float freq, float amp) {
  lcd_1.print("F ");
  String freqStr = String(freq, 1);
  lcd_1.print(freqStr);
  lcd_1.print(" A ");
  String ampStr = String(amp, 2);
  lcd_1.print(ampStr);
}

void coletarEProcessar() {
  float valorPot0 = analogRead(POT_PIN_0) * (30.0 / 1023.0);
  float valorPot1 = analogRead(POT_PIN_1) * (30.0 / 1023.0);
  float valorPot2 = analogRead(POT_PIN_2) * (30.0 / 1023.0);

  generateSample(valorPot0, valorPot1, valorPot2, NUM_AMOSTRAS, amostras);

  static float magnitudes[NUM_AMOSTRAS];
  
  calcularDFT(amostras, magnitudes, NUM_AMOSTRAS);

  
  extrairResultados(magnitudes, NUM_AMOSTRAS, threshold, &freqDominant, &ampDominant, &freqMax, &ampMax);
}


void generateSample(float valorPot0, float valorPot1, float valorPot2, int numAmostras, float* vetorAmostras) {
  float freq1 = valorPot0;
  float freq2 = valorPot1;
  float freq3 = valorPot2;
  
  float amp1 = random(20, 101) / 100.0;
  float amp2 = random(20, 101) / 100.0;
  float amp3 = random(20, 101) / 100.0;
  
  float tempoPorAmostra = tempoTotalAmostra / numAmostras;

  for (int n = 0; n < numAmostras; n++) {
    float t = n * tempoPorAmostra;

    vetorAmostras[n] = 
      amp1 * sin(2 * PI * freq1 * t) + 
      amp2 * sin(2 * PI * freq2 * t) + 
      amp3 * sin(2 * PI * freq3 * t);
  }
}

void calcularDFT(float* amostras, float* magnitudes, int numAmostras) {
  for (int k = 0; k < numAmostras; k++) {
    float real = 0;
    float imag = 0;

    for (int n = 0; n < numAmostras; n++) {
      float angle = 2 * PI * k * n / numAmostras;
      real += amostras[n] * cos(angle);
      imag -= amostras[n] * sin(angle);
    }

    magnitudes[k] = sqrt(real * real + imag * imag) / numAmostras;
  }
}


void extrairResultados(float* magnitudes, int numAmostras, float threshold, float* freqDominant, float* ampDominant, float* freqMax, float* ampMax) {
  float maiorAmp = 0;
  int indiceMaiorAmp = 0;

  int indiceMaiorFreq = -1;
  float ampMaiorFreq = 0;

  for (int k = 0; k < numAmostras / 2; k++) { // analisar apenas metade (por simetria)
    if (magnitudes[k] > maiorAmp) {
      maiorAmp = magnitudes[k];
      indiceMaiorAmp = k;
    }

    if (magnitudes[k] > threshold) {
      if (k > indiceMaiorFreq) {
        indiceMaiorFreq = k;
        ampMaiorFreq = magnitudes[k];
      }
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