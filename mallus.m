clear
load ’malus.mat’ % raw data
load ’adc.mat’ % calibration constants

w = pi/180;

v_long = m_adc0 * malus_adc0 + b_adc0;

% Cut initial dynamics from data
% ... and start roughly at a maxima
skip=55;
v = v_long(skip:end);
% fold up the data
for row = 1:floor(length(v)/360)
	data(row,:) = v(1+(row-1)*360:row*360);
end
i = [0:359];
s = sqrt(var(data)+0.01ˆ2);
errorbar(w*i,max(data),s,’.’)
v = max(data);

phi = 0; % initial guess
for j = 1:10 % arbitrary number of iterations
	coswiphi = cos(w*i+phi); % somehow it feels more efficient
	sinwiphi = sin(w*i+phi); % to precalculate these
	V0 = sum (v.*coswiphi.ˆ2./s.ˆ2) / sum (coswiphi.ˆ4./s.ˆ2);
	chi2 = sum((v - V0.*coswiphi.ˆ2).ˆ2./s.ˆ2);
	dchi2dphi = 4*V0*sum((v-V0.*coswiphi.ˆ2) .*coswiphi.*sinwiphi./s.ˆ2);
	d2chi2dphi2 = 12*V0ˆ2*sum(coswiphi.ˆ2./s.ˆ2) - 16*V0ˆ2*sum(coswiphi.ˆ4./s.ˆ2)- 4*V0*sum(v./s.ˆ2)+ 8*V0*sum(v.*coswiphi.ˆ2./s.ˆ2);
	
phis(j) = phi; % save values for later
V0s(j) = V0; % postmortem
chi2s(j) = chi2;
dchi2s(j) = dchi2dphi;
d2chi2s(j) = d2chi2dphi2;
[ chi2, phi V0 ] % show intermediate values
% update guess
phi = phi - (dchi2dphi) / (2*d2chi2dphi2/2);
end
figure(1)
errorbar(180*(w*i+phi)/pi, v - V0*cos(w*i+phi).ˆ2,s,’.’)
xlabel(’Angle between polaroids’);
ylabel(’Residual’);
% This bit just generates the contour plot
% in the region of best fit
dphi = max(abs(phis-phis(end)))*1.5;
dV0 = max(abs(V0s-V0s(end)))*10;
phi_v = linspace(phis(end)-dphi,phis(end)+dphi,50);
v0_v = linspace(V0s(end)-dV0,V0s(end)+dV0,50);
for j=1:length(phi_v)
coswiphi = cos(w*i+phi_v(j));
for k=1:length(v0_v)

chi2_m(k,j) = sum((v - v0_v(k).*coswiphi.ˆ2).ˆ2./s.ˆ2);
end
end
figure(2)
contour(phi_v, v0_v, chi2_m,20)
hold on
plot(phis,V0s)
xlabel(’\phi’)
ylabel(’V_0’)
