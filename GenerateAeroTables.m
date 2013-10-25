clear all
clc

run MLSModel



% %Find the optimal value of Cp
% Cpmax = 0;
% for i = 1:length(Cptable(1,2:end))
%     for j = 1:length(Cptable(2:end,2))
%         Cpij = Cptable(j+1,i+1);
% 
%         if (Cpij > Cpmax )
%             Cpmax = Cpij;
%             beta_star = Cptable(1,i+1);
%             lambda_star = Cptable(j+1,1);
%         end
%     end
% end
% 
% P.beta_max = 30;
% P.beta_min = -2;
% P.lambda_max = 17;%P.lambda_star+2;
% P.lambda_min = 2;%P.lambda_star-2;


% %For display...
% index_beta_min = 0;index_beta_max = 0;index_lambda_min = 0;index_lambda_max = 0;
% for i = 2:length(Cptable(1,2:end))-1
%     if (Cptable(1,i) <= P.beta_min) && (Cptable(1,i+1) > P.beta_min)
%         index_beta_min = i;
%     end
%     if (Cptable(1,i) <= P.beta_max) && (Cptable(1,i+1) > P.beta_max)
%         index_beta_max = i;
%     end
% end
% for i = 2:length(Cptable(2:end,1))-1
%     if (Cptable(i,1) <= P.lambda_min) && (Cptable(i+1,1) > P.lambda_min)
%         index_lambda_min = i;
%     end
%     if (Cptable(i,1) <= P.lambda_max) && (Cptable(i+1,1) > P.lambda_max)
%         index_lambda_max = i;
%     end
% end
% %index_beta_min = 1;index_beta_max = length(Cptable(1,:));index_lambda_min = 1;index_lambda_max = length(Cptable(:,1));
% if (index_lambda_max == 0)
%     index_lambda_max = length(Cptable(:,1));
% end
% if (index_beta_max == 0)
%     index_beta_max = length(Cptable(1,:));
% end

%%%%%%%%%%%%

lambda0 = Cptable(2:end,1);
beta0 = Cptable(1,2:end);
%Cp0 = max(0,Cptable(index_lambda_min:index_lambda_max,index_beta_min:index_beta_max));
Cp0 = Cptable(2:end,2:end);
Ct0 = Cttable(2:end,2:end);

% Data grid
NData = 80;

lambda_Data = linspace(lambda0(1),lambda0(end),NData);
beta_Data = linspace(beta0(1),beta0(end),NData+1);

h = waitbar(0,'Reduce Cp table');
count = 0;
for i = 1:NData
    for j = 1:NData+1
         waitbar(count/(NData*(NData+1)),h);
        Cp_Data(i,j) = interp2(beta0,lambda0,Cp0,beta_Data(j),lambda_Data(i));
        count = count +1;
    end
end
close(h)

h = waitbar(0,'Reduce Ct table');
count = 0;
for i = 1:NData
    for j = 1:NData+1
         waitbar(count/(NData*(NData+1)),h);
        Ct_Data(i,j) = interp2(beta0,lambda0,Ct0,beta_Data(j),lambda_Data(i));
        count = count +1;
    end
end
close(h)

save CpData lambda_Data beta_Data Cp_Data
save CtData lambda_Data beta_Data Ct_Data


surf(lambda0,beta0,Cp0')
%axis([-5 5 -5 5 -.5 1])