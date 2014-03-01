if [ "$1" = "c" ]
then
    make -f Makefile.cfg clean
    make -f Makefile.cfglp clean
elif [ "$1" = "1" ]
then
    make -f Makefile.cfg FILE=test.c
elif [ "$1" = "2" ]
then
    make -f Makefile.cfglp
elif [ "$1" = "r" ]
then
    ./cfglp test_files/test.cs306.cfg -tokens
else
    echo "wrong code"
fi
