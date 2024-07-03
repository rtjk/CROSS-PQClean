# for every every parameter set create the corresponding directory
# then copy the templates for the 'clean' and 'avx2' implementations inside that directory
# finally substitute the actual parameters
# parameters are listed in parameter_sets.csv

import os
import fileinput
import re
import csv
import shutil
import datetime

def replace_in_file(path, text_to_search, replacement_text):
  with fileinput.FileInput(path, inplace=True) as file:
    for line in file:
      print(re.sub(text_to_search, replacement_text, line), end='')

def replace_in_dir(dir, text_to_search, replacement_text):
    for root, dirs, files in os.walk(dir):
        for file in files:
            file_path = os.path.join(root, file)
            if os.path.isfile(file_path):
                replace_in_file(file_path, text_to_search, replacement_text)

# use the unifdef utility to remove #if, #ifdef, #elif, etc.
def remove_dead_code(dir):
    # call unifdef on the parameter definition file set.h
    unifdef_command = 'unifdefall '+dir+'/set.h > tmp_set.h ; mv tmp_set.h '+dir+'/set.h'
    os.system(unifdef_command)
    # use the new set.h to remove dead code in all other files
    for file in os.listdir(dir):
        if file.endswith('.c') or file.endswith('.h'):
            unifdef_command = 'unifdef -m -f '+dir+'/set.h '+dir+'/'+file
            os.system(unifdef_command)

# output here
TARGET_DIR = './crypto_sign'

# check that the unifdef package is installed
if os.system('which unifdef > /dev/null 2>&1') != 0:
    raise Exception('unifdef package not found, please install it')

# delete output directory if it already exists
if os.path.exists(TARGET_DIR):
    shutil.rmtree(TARGET_DIR)

# csv file
# the first column '__dir__' constains the directory name for a given set of parameters
# the other columns contain the values to replace (e.g. replace "__random-bytes__" with "16")
csv_filename = './parameter_sets.csv'

# files and directories to copy
meta_file = '/META.yml'
clean_dir = '/clean'
avx2_dir = '/avx2'

with open(csv_filename, 'r') as csvfile:

    csv_reader = csv.DictReader(csvfile)
    columns = csv_reader.fieldnames

    for row in csv_reader:
        
        # create the directory for a given set of parameters 
        dir = row['__dir__']
        dir = os.path.join(TARGET_DIR, dir)
        if(not os.path.isdir(dir)):
            os.makedirs(dir)
            # copy META.yml
            shutil.copyfile('..' + meta_file, dir + meta_file)
            # copy the reference (clean) implementation template
            shutil.copytree('..' + clean_dir, dir + clean_dir)
            # copy the optimized (avx2) implementation template
            shutil.copytree('..' + clean_dir, dir + avx2_dir)
            shutil.copytree('..' + avx2_dir, dir + avx2_dir, dirs_exist_ok=True)
            # replace 'column_name' with 'column_value' in META.yml
            for column_name in columns:
                column_value = row[column_name]
                replace_in_file(dir + meta_file, column_name, column_value)

        dir = dir + '/' + row['__implementation__']

        # replace 'column_name' with 'column_value' in the files
        for column_name in columns:
            column_value = row[column_name]
            replace_in_dir(dir, column_name, column_value)

        remove_dead_code(dir)

current_time = datetime.datetime.now().strftime("%H:%M")
print("Implementations placed in", TARGET_DIR, "@", current_time)