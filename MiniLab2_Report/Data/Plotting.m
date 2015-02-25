

load MP2Data
all_time = linspace(0,100,10000);
linewidth = 6;
label_fontsize = 26;
title_fontsize = 30;

% plot 1: spring!
time = all_time(1:2688); % just the length of the data
% now, trim to just the interesting section
%5.4 to 8.1 seconds
% indecies 541 to 811
time_temp = time(541:811);
position_temp = spring_position(541:811);
torque_temp = spring_torque(541:811);
% plotting the full data, just for initial visualization
%plot(time, spring_position, 'b', 'linewidth', linewidth)
%hold on;
%plot(time, spring_torque, 'r', 'linewidth', linewidth)

% now, plot the interesting section
plot(time_temp, position_temp, 'b', 'linewidth', linewidth)
hold on;
plot(time_temp, torque_temp, 'r', 'linewidth', linewidth)
xlabel('Time (seconds)', 'fontsize', label_fontsize)
ylabel('Position (degrees) and Torque (arbitrary units)', 'fontsize', label_fontsize)
title('Spring Test', 'fontsize', title_fontsize)
myLegend = legend('Position (degrees)', 'Torque (current measurement)');
set(myLegend, 'fontsize', 24)

clear time; clear time_temp; clear position_temp; clear torque_temp;

