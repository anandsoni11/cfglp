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

for f in cfgFiles:
    print("Testing file  " + f ); 
    command = "./r_cfglp64 -tokens -eval " + f
    os.system(command)
    icfile = f+".toks";
    os.system("cp " + icfile + "  t2.txt");

    command = "./cfglp64 -tokens -eval " + f
    os.system(command)
    os.system("cp " + icfile + "  t1.txt");

    os.system("diff -bB t1.txt t2.txt");
    continue
