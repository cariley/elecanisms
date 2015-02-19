
import math
import numpy as np


"""This function filters data by reducing the number of points (to numFinalDataPoints)
by binning values and averaging the bins. This reduces the size of large vectors to make
plotting discreet points easier, and acts like a low pass filter on the data, often improving
the accuracy of measured values in the process. inputData is expected to be a python list, or 
anything that allows the same operations. Function translated from MatLab, so variable naming 
conventions may be different/ mixed. Also, there are more efficient pythonic ways of computing this
filter."""
def filterByBinning(inputData, numFinalDataPoints):
    """One known issue: if len(inputData)/numFinalDataPoints is not sufficiently large, unusual
    effects are expected (since the data size isn't actually being reduced much). values of less
    than 1 may even throw errors. at a guess, 2-3 should be large enough, but for the filter to be
    more effective, 10 is probably better."""
    binsize = float(len(inputData))/numFinalDataPoints # how many data points will be combined into 
    # one. can be a non-integer. it is rounded with each step of data combine

    originalData = inputData
    filteredData = []
    current_index = 0
    next_bin = 0

    # each time through the loop calculates one bin worth of data.
    while current_index < len(originalData):
        sum = 0
        next_bin = next_bin + binsize
        end_index = int (min(math.floor(next_bin), len(originalData))) # should never be the length of the original vector.
        sumlength = end_index - current_index
    
        for current_index in range(current_index, end_index): 
            sum = sum + originalData[current_index]
        average = sum/sumlength
        filteredData.append(average)
        current_index += 1
    
    return filteredData

"""This function filters data by reducing the number of points (to numFinalDataPoints)
by dropping any extra points."""
def filterByDropping(inputData, numFinalDataPoints):
    binsize = float(len(inputData))/numFinalDataPoints # how many data points will be combined into 
    # one. can be a non-integer. it is rounded with each step of data combine

    originalData = inputData
    filteredData = []
    current_index = 0
    next_bin = 0

    # each time through the loop calculates one bin worth of data.
    while current_index < len(originalData):
        sum = 0
        next_bin = next_bin + binsize
        end_index = int (min(math.floor(next_bin), len(originalData)))
        while end_index >= len(originalData):  # ??? I don't know why this occurs . . . would have to look at the algorithem closer :(
            end_index = end_index - 1
        middle_index = int(math.floor((end_index-current_index)/2 + current_index))
        #print("middle index: " + str(middle_index) + ", start index: " + str(current_index) + ", ending index: " + str(end_index))     
        current_index = end_index
        filteredData.append(originalData[middle_index])
        current_index += 1
    
    return filteredData


"""Basic testing to verify that the algorithems are working as expected."""
def main():
    testData = np.linspace(1, 10, 10)
    print ("Original Test Data: " + str(testData))
    filteredTestData = filterByDropping(testData, 4)
    print ("Filtered Test Data: " + str(filteredTestData))
    
    # verifying that it works with python lists as well
    # NOT DONE YET

if __name__ == "__main__":
    main()