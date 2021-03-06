import os  
path = "test_files"
files=[]
for f in os.listdir(path):
    files.append(f) 

correct_files=[]
for f in files:
    fileName,fileExt = os.path.splitext( path + f)
    if( fileExt == '.c'):
        correct_files.append(f)



for f in correct_files:
    command = "make -f Makefile.cfg FILE=" + f+ " >/dev/null";
    os.system(command);

#cfg generated from c, now collect all cfg
files=[]
for f in os.listdir(path):
    files.append(f) 

cfgFiles=[]
for f in files:
    fileName,fileExt = os.path.splitext( path + f)
    if( fileExt == '.cfg'):
        cfgFiles.append(f)

print("Done generating cfg files\n");

for f in cfgFiles:
    print("Testing file  " + f ); 
    command = "./r_cfglp64 -tokens -d " + f + " > expected " 
    os.system(command) 
    command = "./cfglp64 -tokens -d " + f + " > generated " 
    os.system(command)
    os.system("diff -B expected generated")

    command = "./r_cfglp64 -ast -d " + f + " > expected_ast " 
    os.system(command) 
    command = "./cfglp64 -ast -d " + f + " > generated_ast " 
    os.system(command)
    os.system("diff -B expected_ast generated_ast")

    command = "./r_cfglp64 -eval -d " + f + " > expected_eval " 
    os.system(command) 
    command = "./cfglp64 -eval -d " + f + " > generated_eval " 
    os.system(command)
    os.system("diff -B expected_eval generated_eval")

print("\n*****Done with the checking cfg files EXCEPT infi loops!!***\n")

#exit(0)

error_files=[]
for f in files:
    fileName,fileExt = os.path.splitext( path + f)
    if( fileExt == '.ecfg'):
        error_files.append(path + "/" +f)


for f in error_files:
    print("Testing file  " + f ); 
    command = "./r_cfglp64 -tokens  " + f + " 2> expected " 
    os.system(command) 
    command = "./cfglp64 -tokens " + f + " 2> generated " 
    os.system(command)
    os.system("diff -B expected generated")

    command = "./r_cfglp64 -ast  " + f + " 2> expected_ast " 
    os.system(command) 
    command = "./cfglp64 -ast " + f + " 2> generated_ast " 
    os.system(command)
    os.system("diff -B expected_ast generated_ast")

    command = "./r_cfglp64 -eval  " + f + " 2> expected_eval " 
    os.system(command) 
    command = "./cfglp64 -eval " + f + " 2> generated_eval " 
    os.system(command)
    os.system("diff -B expected_eval generated_eval")

print("\n*****Done with the checking ecfg files!!***\n")


#print cfgFiles

