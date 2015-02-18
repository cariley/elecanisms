function Reduced_Vector = Reduce_Data_Points(Input_Vector, num_final_data_points)
% this function takes a vector and reduces its length by binning values and
% averaging the bins. This reduces the size of long vectors for plotting
% discreet points, and improves the accuracy of measured values in the
% proccess.

% if length(Input_Vector)/ num_final_data_points is not sufficiently large,
% unusual effects are expected (not confirmed). if it is less than one,
% there will probably be errors. at a guess, 2-3 should be large enough,
% but for the filter to be effective, 10 is probably better.

binsize = length(Input_Vector)/num_final_data_points; % how many data 
% points will be combined into one. can be a non-integer. it is rounded
% with each step of data combine

Original_Vector = Input_Vector;
Final_Vector = [];
current_index = 0;
next_bin = 0;

while current_index < length(Original_Vector)
    sum = 0;
    next_bin = next_bin + binsize;
    end_index = min(floor(next_bin), length(Original_Vector)); % should 
    % never be the length of the original vector.
    sumlength = end_index - current_index;
    
    % old way of determining the next ending index, but it had the problem
    % that if the input number of points isn't divisible by the output,
    % then the final point gets the shaft.
    %sumlength = min(binsize, length(Original_Vector) - current_index);
    for current_index = current_index+1: end_index % the plus one is because
        % matlab does not increment the counter at the conclusion of the
        % loop.
        sum = sum + Original_Vector(current_index);
    end
    average = sum/sumlength;
    Final_Vector(end+1) = average;
    
end

Reduced_Vector = Final_Vector;
end