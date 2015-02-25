

load MP2Data
all_time = linspace(0,100,10000);
linewidth = 6;
label_fontsize = 26;
title_fontsize = 30;

% quick note on units: all units for the following plots are arbitrary.
% torque was determined by measuring current through the motor by measuring
% voltage drop across a resistor (amplified) with a 10 bit A/D, and
% position was measured using a 10 bit A/D to track angle of a wheel (with
% a significantly different diameter than the joystick itself), and
% velocity was determined by taking the differential of position. So the
% units make absolutely no sense, but the scales are all linear, so the
% relative relationships are still sensical and interesting.

% plot 1: spring!
time = all_time(1:2688); % just the length of the data
% now, trim to just the interesting section
%5.4 to 8.1 seconds
% indecies 541 to 811
time_temp = time(541:811);
position_temp = spring_position(541:811);
torque_temp = spring_torque(541:811);
% plotting the full data, just for initial visualization
figure()
plot(time, spring_position, 'b', 'linewidth', linewidth)
hold on;
plot(time, spring_torque, 'r', 'linewidth', linewidth)
title('DEBUGGING: spring position and torque over time (full range)')

% now, plot the interesting section
figure()
plot(time_temp, position_temp, 'b', 'linewidth', linewidth)
hold on;
plot(time_temp, torque_temp, 'r', 'linewidth', linewidth)
xlabel('Time', 'fontsize', label_fontsize)
ylabel('Position and Torque', 'fontsize', label_fontsize)
title('Spring Test', 'fontsize', title_fontsize)
myLegend = legend('Position', 'Torque');
set(myLegend, 'fontsize', 24)

clear time; clear time_temp; clear position_temp; clear torque_temp;


% plot 2: damper!
time = all_time(1:1651); % just the length of the data
damper_current_smaller = damper_current./50; % its not in units anyways, so might
% as well make it on a more reasonable scale relative to the other things
% being plotted.
% now, trim to just the interesting section
%3.1 to 7.2 seconds
% indecies 311 to 721
time_temp = time(311:721);
velocity_temp = damper_velocity(311:721);
current_temp = damper_current_smaller(311:721);
% plotting the full data, just for initial visualization
figure()
plot(time, damper_velocity, 'b', 'linewidth', linewidth)
hold on;
plot(time, damper_current_smaller, 'r', 'linewidth', linewidth)
title('DEBUGGING: damper velocity and scaled torque (full range)')
figure()
plot(damper_velocity, damper_current, 'b', 'linewidth', linewidth)
title('DEBUGGING: damper velocity vs. torque (full range)')

% now, plot the interesting section
figure()
plot(time_temp, velocity_temp, 'b', 'linewidth', linewidth)
hold on;
plot(time_temp, current_temp, 'r', 'linewidth', linewidth)
xlabel('Time', 'fontsize', label_fontsize)
ylabel('Velocity and Torque', 'fontsize', label_fontsize)
title('Damper Test', 'fontsize', title_fontsize)
myLegend = legend('Velocity', 'Torque');
set(myLegend, 'fontsize', 24)
ylim([-35,50]);

clear time; clear time_temp; clear velocity_temp; clear current_temp;



% plot 3: Texture!
time = all_time(1:985); % just the length of the data
texture_torque_larger = texture_current.*50; % its not in units anyways, so might
% as well make it on a more reasonable scale relative to the other things
% now, trim to just the interesting section
% UPDATE: I am no longer trimming the vecotrs- just using xlim and ylim
% to only plot the part I'm interested in.
% plotting the full data, just for initial visualization
figure()
plot(time, texture_position, 'b', 'linewidth', linewidth)
hold on;
plot(time, texture_torque_larger, 'r', 'linewidth', linewidth)
title('DEBUGGING: texture position and scaled torque over time (full range)')
figure()
plot(texture_position, texture_current, 'b', 'linewidth', linewidth)
title('DEBUGGING: texture position vs torque (full range)')

% now, plot the interesting section
figure()
plot(time, texture_position, 'b', 'linewidth', linewidth)
hold on;
plot(time, texture_torque_larger, 'r', 'linewidth', linewidth)
xlabel('Time', 'fontsize', label_fontsize)
ylabel('Position and Torque', 'fontsize', label_fontsize)
title('Texture Test', 'fontsize', title_fontsize)
myLegend = legend('Position', 'Torque');
set(myLegend, 'fontsize', 24)
xlim([.7,6.2]);
% another interesting plot!
figure()
plot(texture_position, texture_current, 'b', 'linewidth', linewidth)
xlabel('Position', 'fontsize', label_fontsize)
ylabel('Torque', 'fontsize', label_fontsize)
title('Texture Position vs. Torque', 'fontsize', title_fontsize)
xlim([500,6500]);

clear time; clear time_temp; clear position_temp; clear torque_temp;
clear texture_torque_larger;


% plot 4: Wall!
time = all_time(1:956); % just the length of the data
% plotting the full data, just for initial visualization
figure()
plot(time, wall_position, 'b', 'linewidth', linewidth)
hold on;
plot(time, wall_current, 'r', 'linewidth', linewidth)
title('DEBUGGING: wall position and torque (full range)')
figure()
plot(wall_position, wall_current, 'b', 'linewidth', linewidth)
title('DEBUGGING: wall position vs torque (full range)')

% now, plot the interesting section
figure()
plot(wall_position, wall_current, 'b', 'linewidth', linewidth)
xlabel('Position', 'fontsize', label_fontsize)
ylabel('Torque', 'fontsize', label_fontsize)
title('Wall Test (Position vs. Torque)', 'fontsize', title_fontsize)
ylim([80,900]);
xlim([-2250, 3350]);

clear time; clear time_temp; clear velocity_temp; clear current_temp;



% done with plotting, time to clear variables.
clear all_time; clear label_fontsize; clear linewidth; clear myLegend; 
clear title_fontsize; clear damper_current_smaller;

