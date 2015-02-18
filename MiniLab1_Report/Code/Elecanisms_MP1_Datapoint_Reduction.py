# future updates- self identify how many channels are included
# self identify how many output data points there should be, by taking an input
# of what factor of reduction you want
# use librarys for creating CSV file, and for reading CSV file, rather than bitbanging it all
# time the operation, report this time to the user

import dataFilters as filters
num_datapoints_output = 200000 # ~ 14000000/100


file = open('SpinDown_Data.txt','r', 0) # open the file, grab the text
text = file.read()
print("Text read!")

parsed_text = text.splitlines() # get the individual lines
# get rid of the first two header lines
line1 = parsed_text.pop(0)
line2 = parsed_text.pop(0)

print("header stripped")
print("header line1: " + line1)
print("header line2: " + line2)

Ch1 = [] # this will be one data vector
T = [] # this is the time vector
loop_counter = 0
for line in parsed_text:
    try:
        data_points = line.split(',')
        T.append(float(data_points[0]))
        Ch1.append(float(data_points[1]))
        loop_counter += 1
        if (loop_counter % 1000000) == 0:
            print("Completed " + str(float(loop_counter)/1000000) + "M Data Points")
    except:
        print("Line didn't contain expected data points!")
        print("This was line number: " + str(loop_counter))
        print("The line was: " + line)
print("Finished Parsing Data!")
print("Number of data points: " + str(len(T)))

print("correcting Time vector for appropriate units")
header_values = line2.split(',')
time_increment = float(header_values[3])
for counter in range(len(T)):
    T[counter] = T[counter] * time_increment

# debugging
#print(T)
#print(Ch1)

# now, we have the data vectors, reduce the number of data points!
print("Beginning reduction of Time Vector")
T_filtered = filters.filterByBinning(T, num_datapoints_output)
# shift everything to start at 0 time
initial_time = T_filtered[0]
for counter in range(len(T_filtered)):
    T_filtered[counter] = T_filtered[counter] - initial_time
print("Time vector finished!")
print("Beginning reduction of Ch1 Vector")
Ch1_filtered = filters.filterByDropping(Ch1, num_datapoints_output)
print("Ch1 vector finished!")

# debugging
#print(T_filtered)


# now, re-write the shortened file as a CSV, with proper headings
print("Writing reduced data back to file!")
output_file = open('Reduced_Data.txt','w', 0) # open the file, grab the text
output_text = "Time(Seconds), Channel1(Volts)\n"
for counter in range(len(T_filtered)):
    output_text += str(T_filtered[counter])
    output_text += ", "
    output_text += str(Ch1_filtered[counter])
    if counter < (len(T_filtered)-1): # don't add newline at the end!
        output_text += "\n"

output_file.write(output_text)
print("Proccess finished!! Reduced CSV file ready!")
