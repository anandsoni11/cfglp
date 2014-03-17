if [ "$1" = "c" ]
then
    make -f Makefile.cfg clean
    rm test_files/*.ic
    rm test_files/*.spim
elif [ "$1" = "2" ]
then
    make -f Makefile.cfglp
elif [ "$1" = "3" ]
then
    make -f Makefile.cfg FILE=$2
else
    echo "wrong code"
fi
