if [ "$1" = "c" ]
then
    make -f Makefile.cfg clean
    rm test_files/*.ic
    rm test_files/*.spim
elif [ "$1" = "1" ]
then
    make -f Makefile.cfg FILE=test.c
elif [ "$1" = "2" ]
then
    make -f Makefile.cfglp
    ./cfglp test_files/test.cs306.cfg -tokens
elif [ "$1" = "r" ]
then
    ./cfglp test_files/test.cs306.cfg -tokens
elif [ "$1" = "3" ]
then
    make -f Makefile.cfg FILE=$2
elif [ "$1" = "4" ]
then
    ./cfglp test_files/$2s306.cfg -tokens -d > t1.txt
elif [ "$1" = "5" ]
then
    ./cfglp64 test_files/$2s306.cfg -tokens -d > t2.txt
elif [ "$1" = "6" ]
then
    ./cfglp test_files/$2 -tokens -d > t1.txt
elif [ "$1" = "7" ]
then
    ./cfglp64 test_files/$2 -tokens -d > t2.txt
else
    echo "wrong code"
fi
