import fileinput
import re


import utility

replace_list = [

]

for i, elem in enumerate(replace_list):
    if i % 2 == 0:
        utility.replace_in_file('./parameter_sets.csv', replace_list[i+1], replace_list[i])