clc
clear all
close all

%frequencias dos sinais
f1 = 330;
f2 = 440;

%periodos dos sinais
t1= 1 / f1;
t2= 1 / f2;

%comprimento de onda
lambda1 = (3*10^8)/f1;
lambda2 = (3*10^8)/f2;

%sinal no tempo
t = 0:0.0001:1;
x1 = 1*sin(2*pi*100*t);
x2 = 1*sin(2*pi*1000*t);
figure(1);
plot(t,x1);
figure(2);
plot(t,x2);

%ouvir cada sinal
sound(x1);
sound(x2);

%soma dos sinais
xt = x1 + x2;

%mostrar a soma no tempo
figure(3);
plot(t,xt);

%ouvir a soma
sound(xt);

%amostragem dos sinais
Fs1 = (10*100); %Alta resolução
Fs2 = (10*1000);%Alta resolução
n = 0:50;
xn1 = 1*sin(2*pi*100*n/Fs1);
xn2 = 1*sin(2*pi*1000*n/Fs2);
figure(4);
stem (n,xn1);
figure (5);
stem (n,xn2);

%amostragem com aliasing dos sinais
Fs1a = 1.5*100;
Fs2a = 1.5*1000;
xn1a = 1*sin(2*pi*100*n/Fs1a);
xn2a = 1*sin(2*pi*1000*n/Fs2a);
figure(6);
stem(n,xn1a);
figure(7);
stem(n,xn2a);

%amostragem com aliasing dos sinais
Fs1a = 1.5*100;
Fs2a = 1.5*1000;
xn1a = 1*sin(2*pi*100*n/Fs1a);
xn2a = 1*sin(2*pi*1000*n/Fs2a);
figure(6);
stem (n,xn1a);
figure (7);
stem (n,xn2a);

%espectro frequencia de cada sinal
% FFT com alta resolução
N = 2048;  % número de pontos na FFT
% FFT do sinal x1 (100 Hz)
dftx1 = fft(x1, N);
modulox1 = abs(fftshift(dftx1));
% FFT do sinal x2 (1000 Hz)
dftx2 = fft(x2, N);
modulox2 = abs(fftshift(dftx2));
% Eixo de frequências
Fs = 10 * 1000;  % taxa de amostragem máxima (a maior usada entre os sinais)
f = linspace(-Fs/2, Fs/2, N);
% Gráficos organizados
figure (8);
subplot(1,2,1); plot(f, modulox1); title('Módulo - Sinal 100 Hz'); xlabel('Frequência (Hz)'); ylabel('|X(f)|'); grid on;
subplot(1,2,2); plot(f, modulox2); title('Módulo - Sinal 1000 Hz'); xlabel('Frequência (Hz)'); ylabel('|X(f)|'); grid on;

%ouvir o resultado de cada fft
sound(modulox1)
sound(modulox2)

% Espectro da soma dos sinais
dftxt = fft(xt, N);
moduloxt = abs(fftshift(dftxt));

figure(9);
plot(f, moduloxt); title('Espectro da Soma'); xlabel('Frequência (Hz)'); ylabel('|X(f)|');

% Ouvir espectro da soma
sound(moduloxt, 10000);
