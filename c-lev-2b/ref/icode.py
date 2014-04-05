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
        cfgFiles.append(path+"/"+f); 
print("Done generating cfg files\n");

print(cfgFiles)
print("#cfg Files ", len(cfgFiles))

for f in cfgFiles:
    print("Testing file  " + f ); 
    icodefile = f+".ic";

    command = "./r_cfglp64 -icode " + f
    os.system(command)

    os.system("cp " + icodefile + "  icode2.txt");

    command = "./cfglp64 -icode " + f
    os.system(command)

    os.system("cp " + icodefile + "  icode1.txt");

    os.system("diff -bB icode1.txt icode2.txt");


#ecfg files
print("-------------------\nCHECKING ecfg FILES\n");

error_files=[]
for f in files:
    fileName,fileExt = os.path.splitext( path + f)
    if( fileExt == '.ecfg'):
        error_files.append(path + "/" +f)
print(error_files)
print("#ecfg Files ", len(error_files))


for f in error_files:
    print("Testing file  " + f ); 
    command = "./r_cfglp64 -icode  " + f + " 2> icode2.txt " 
    os.system(command) 
    command = "./cfglp64 -icode " + f + " 2> icode1.txt " 
    os.system(command)
    os.system("diff -bB icode1.txt icode2.txt");
