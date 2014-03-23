if [ "$1" = "c" ]
then
    make -f Makefile.cfg clean
    rm -f test_files/*.toks
    rm -f test_files/*.ast
    rm -f test_files/*.eval
    rm -f test_files/*.sym
    rm -f test_files/*.prog
    rm -f test_files/*.ic
    rm -f test_files/*.spim
    
    #clean temp txt files
    rm -f icode*.txt
    rm -f compile*.txt
    rm -f symtab*.txt
    rm -f program*.txt
    rm -f t1.txt
    rm -f t2.txt
    #cleaning cfg files corresponding to .c only. Remaining cfg untouched
    suffix='s306.cfg'
    for f in test_files/*.c
    do
        f=$f$suffix
        echo $f
        rm -f $f
    done

elif [ "$1" = "2" ]
then
    make -f Makefile.cfglp
elif [ "$1" = "3" ]
then
    make -f Makefile.cfg FILE=$2
else
    echo "wrong code"
fi
