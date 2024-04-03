import os
import fileinput
import re
import csv
import shutil

def replace_in_file(path, text_to_search, replacement_text):
  with fileinput.FileInput(path, inplace=True) as file:
    for line in file:
      print(re.sub(text_to_search, replacement_text, line), end='')

def replace_in_dir(dir, text_to_search, replacement_text):
    if(not os.path.isdir(dir)):
       raise Exception('Directory not found')
    else:
        for root, dirs, files in os.walk(dir):
            for file in files:
                file_path = os.path.join(root, file)
                if os.path.isfile(file_path):
                    replace_in_file(file_path, text_to_search, replacement_text)

# Output here
TARGET_DIR = './crypto_sign'

# csv file
# the first column 'dir' constains the directory name for a given set of parameters
# the other columns contain the values to replace (e.g. replace "__random-bytes__" with "16")
csv_filename = './parameter_sets.csv'

# files and dirs to copy
meta_file = '/META.yml'
clean_dir = '/clean'

with open(csv_filename, 'r') as csvfile:

    csv_reader = csv.DictReader(csvfile)

    for row in csv_reader:
        
        # create the directory for a given set of parameters 
        dir = row['__dir__']
        dir = os.path.join(TARGET_DIR, dir)
        os.makedirs(dir)

        # copy META.yml
        shutil.copyfile('..' + meta_file, dir + meta_file)
        # copy the clean implementation template
        shutil.copytree('..' + clean_dir, dir + clean_dir)

        # TODO: add 'optimezed-implementation' and follow_symlinks
        # shutil.copyfile(source, destination, *, follow_symlinks = True)

        columns = csv_reader.fieldnames

        # replace 'column_name' with 'column_value' in the files
        for column_name in columns:
            column_value = row[column_name]
            replace_in_dir(dir, column_name, column_value)