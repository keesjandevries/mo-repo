MAINDIR= pwd
RFLAGS=`root-config --cflags --libs`

    g++ test.cpp -o test_point.x -fPIC \
        -Ipackages/include/softsusy/ \
        -ISLHA/inc/ \
        -Ipackages/include/ \
        -Iinterfaces/ \
        -Lobj/\
        -Lpackages/lib -lsoft \
        -LSLHA/libs -lSLHAfile  $RFLAGS \
        -Lpackages/lib64 -lFH \
         -lgfortran \
        -Wl,-rpath=packages/lib:SLHA/libs:packages/lib64
