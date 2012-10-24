MODIR=micromegas_2.4.5/MSSM/
 g++ -o mo_main mo_main.cpp \
        $MODIR../sources/micromegas.a               \
        $MODIR../CalcHEP_src/lib/dynamic_me.a       \
        $MODIR../sources/micromegas.a               \
        "$MODIR"work/work_aux.a                     \
        "$MODIR"lib/aLib.a                          \
        "$MODIR"../CalcHEP_src/lib/sqme_aux.so      \
        "$MODIR"../CalcHEP_src/lib/libSLHAplus.a    \
        "$MODIR"../CalcHEP_src/lib/num_c.a          \
        "$MODIR"../CalcHEP_src/lib/serv.a           \
        -lX11   -rdynamic -ldl   -lm                \
