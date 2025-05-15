clc
close all
clear all

re = [2 5 4];
imag = [0 6 8];

% Cálculo da magnitude
for i = 1:3
    % cálculo do raio (distância da origem) - feito em aula
    magnitude(i) = sqrt(re(i)^2 + imag(i)^2);

    % também calculei a fase (inclinação da reta)
    fase(i) = atan2(imag(i), re(i));

    fprintf('Z(%d) = %.2f + %.2fi -> |%.2f| com fase %.2f rad\n', i, re(i), imag(i), magnitude(i), fase(i));
end

