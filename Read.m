clear all
close all

load('SplineData')

Coefs = reshape(Coefs,length(lambda)-4,length(beta)-4);

surf(lambda(3:end-2),beta(3:end-2),max(Coefs.',0))

