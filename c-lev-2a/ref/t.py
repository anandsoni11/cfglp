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
    tokfile = f+".toks";
    astfile = f+".ast";
    #evalfile = f + ".eval";

    command = "./r_cfglp64 -tokens -ast " + f
    os.system(command)

    os.system("cp " + tokfile + "  tok2.txt");
    os.system("cp " + astfile + "  ast2.txt");
    #os.system("cp " + evalfile + "  eval2.txt");

    command = "./cfglp64 -tokens -ast " + f
    os.system(command)

    os.system("cp " + tokfile + "  tok1.txt");
    os.system("cp " + astfile + "  ast1.txt");
    #os.system("cp " + evalfile + "  eval1.txt");

    os.system("diff -bB tok1.txt tok2.txt");
    os.system("diff -bB ast1.txt ast2.txt");
    #os.system("diff -bB eval1.txt eval2.txt");
