

load matlab_spindown_data;

linewidth = 6;
label_fontsize = 30;
title_fontsize = 36;

% trim data for only the slow down part- when the data was grabbed
% it was still speeding up slightly at the beginning
index = 1;
for counter = 1:length(TimeSeconds)
    if TimeSeconds(counter) > 1.8
        time(index) = TimeSeconds(counter);
        volts(index) = Channel1Volts(counter);
        index = index + 1;
    end
end

% first, show raw data from the scope- both the illegible
figure();
plot(time,volts, 'linewidth', 1)
xlabel('Time (seconds)', 'fontsize', label_fontsize)
ylabel('Magnetic Sensor Reading (Volts)', 'fontsize', label_fontsize)
title('Spindown Test Raw Data', 'fontsize', title_fontsize)

figure();
index = 1;
for counter = 1:length(time) % grab a snapshot
    if counter > 15000 && counter < 15500
        time_superShort(index) = time(counter);
        volts_superShort(index) = volts(counter);
        index = index + 1;
    end
end
plot(time_superShort, volts_superShort, 'linewidth', linewidth)
xlabel('Time (seconds)', 'fontsize', label_fontsize)
ylabel('Magnetic Sensor Reading (Volts)', 'fontsize', label_fontsize)
title('Spindown Test Raw Data Snapshot', 'fontsize', title_fontsize)

% now, correct the data to account for 180 degree flips, and
% convert to units (degrees)
max_volts = max(volts);
min_volts = min(volts);
volts_range = max_volts - min_volts;
num_flips = 0;
index = 1;
last_flip = -10;
in_transistion = 0; % data points that occur during a conversion are a little
% tricky to deal with.
for counter = 1:length(time)
    % convert volts to degrees, 0 to 180
    current_degrees = ((volts(counter)-min_volts)/volts_range) * 180;
    % check for a flip
    if counter > 15 && counter < (length(time) - 15)
        % conditions- data point is local maximum, and the nearby change
        % is large in magnitude (not just bouncing around)
        local_max_check = volts((counter-15):(counter+15));
        if min(local_max_check) == volts(counter) && volts(counter) < ...
                (max(local_max_check) - volts_range*.1)
            in_transistion = 1;
            num_flips = num_flips + 1;
        end
        if max(local_max_check) == volts(counter) && volts(counter) > ...
                (min(local_max_check) + volts_range*.1)
            in_transistion = 0;
            flip_time(index) = time(counter);
            flip_volts(index) = volts(counter);
            index = index + 1;
            last_flip = counter;
        end
    end 
    degrees(counter) = (num_flips * 180) + (180 - current_degrees);
    % now, deal with the data points in the transition
    if in_transistion == 1
        degrees(counter) = degrees(counter-1);
    end
end


% visualizing flips, debugging only
figure
plot(time,volts,'o')
hold on
plot(flip_time, flip_volts, 'rx')
xlabel('Time (seconds)', 'fontsize', label_fontsize)
ylabel('Magnetic Sensor Reading (Volts)', 'fontsize', label_fontsize)
title('Visualizing flip detection (DEBUGGING)', 'fontsize', title_fontsize)

% plotting position over time, only debugging (since we reduce datapoints
% later, and it looks cleaner)
figure
plot(time,degrees, 'o', 'linewidth', linewidth)
xlabel('Time (seconds)', 'fontsize', label_fontsize)
ylabel('Cumulative Position (degrees)', 'fontsize', label_fontsize)
title('Corrected Position Data (DEBUGGING)', 'fontsize', title_fontsize)

% calculating velocity
figure
velocity_time_temp = Reduce_Data_Points(time,150);
velocity_degrees_temp = Reduce_Data_Points(degrees,150);
% position data
plot(velocity_time_temp, velocity_degrees_temp, 'linewidth', linewidth)
xlabel('Time (seconds)', 'fontsize', label_fontsize)
ylabel('Cumulative Position (degrees)', 'fontsize', label_fontsize)
title('Corrected Position Data', 'fontsize', title_fontsize)
figure
velocity_time = diff(velocity_time_temp);
velocity_time = velocity_time_temp(1:length(velocity_time_temp)-1) + velocity_time;
velocity_degrees = diff(velocity_degrees_temp);

% finally! Velocity plot!
plot(velocity_time,velocity_degrees, 'o', 'linewidth', linewidth)
xlabel('Time (seconds)', 'fontsize', label_fontsize)
ylabel('Velocity (degrees/second)', 'fontsize', label_fontsize)
title('Spindown Test: Velocity', 'fontsize', title_fontsize)

% cleaning up
clear counter; clear current_degrees; clear degrees; clear flip_time;
clear flip_volts; clear in_transistion; clear index; clear last_flip;
clear linewidth; clear local_max_check; clear max_volts; clear min_volts;
clear num_flips; clear time; clear time_superShort; clear velocity_degrees;
clear velocity_degrees_temp; clear velocity_time; clear velocity_time_temp;
clear volts; clear volts_range; clear volts_superShort; clear label_fontsize;
clear title_fontsize;
