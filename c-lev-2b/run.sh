if [ "$1" = "c" ]
then
    make -f Makefile.cfg clean
    make -f Makefile.cfglp clean
elif [ "$1" = "2" ]
then
    make -f Makefile.cfglp
else
    echo "wrong code"
fi
