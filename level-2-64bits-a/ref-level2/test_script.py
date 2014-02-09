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



cfgFiles=[]
for f in correct_files:
    command = "make -f Makefile.cfg FILE=" + f+ " >/dev/null";
    os.system(command);
    cfgFiles.append(path+"/"+f+"s306.cfg"); 

print("Done generating cfg files\n");

for f in cfgFiles:
    print("Testing file  " + f ); 
    command = "./cfglp64 -tokens -d " + f + " > expected " 
    os.system(command) 
    command = "./cfglp -tokens -d " + f + " > generated " 
    os.system(command)
    os.system(command) 
    os.system("diff -B expected generated")

print("\n*****Done with the checking cfg files EXCEPT infi loops!!***\n")

exit(0)

error_files=[]
for f in files:
    fileName,fileExt = os.path.splitext( path + f)
    if( fileExt == '.ecfg'):
        error_files.append(path + "/" +f)


for f in error_files:
    print("Testing file  " + f ); 
    command = "./cfglp64 -tokens  " + f + " 2> expected " 
    os.system(command) 
    command = "./cfglp -tokens " + f + " 2> generated " 
    os.system(command)
    command = "./cfglp64 -ast  " + f + " 2> expectedast " 
    os.system(command) 
    command = "./cfglp -ast " + f + " 2> generatedast " 
    os.system(command)
    command = "./cfglp64 -eval  " + f + " 2> expectedeval " 
    os.system(command) 
    command = "./cfglp -eval " + f + " 2> generatedeval " 
    os.system(command)
    #print("diff starts here \n");
    os.system("diff -B expected generated")
    os.system("diff -B expectedast generatedast")
    os.system("diff -B expectedeval generatedeval")

print("\n*****Done with the checking ecfg files!!***\n")


#print cfgFiles

