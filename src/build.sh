#
# Hefesto's bootstrap :)
#

# It supposed should run without any manual adjustment over Linux, FreeBSD, MINIX, Solaris, NetBSD and OpenBSD.

# WARNING: If PThread is not available on your UNIX-like remove "-DHVM_ASYNC_RQUEUE" from "COMPILER_OPTS".

# WARNING: If your are on OpenBSD, maybe you need to install the libexecinfo. I have used libexecinfo-0.3v0, try
#          to install it using pkg_add whether you do not have this library previously installed.

COMPILER="gcc" # Adjust for your system compiler
COMPILER_OPTS="-c -Wall -DHVM_ASYNC_RQUEUE -Ihere/src" # Options too
LINKER="gcc" # Linker too
LIB="ar"

LINKERFLAGS=""

$COMPILER --version > /dev/null 2>&1

if [ $? -ne 0 ] ; then
    COMPILER="clang"
    LINKER="clang"
fi

$COMPILER --version > /dev/null 2>&1

if [ $? -ne 0 ] ; then
    echo "PANIC: For compiling Hefesto you should install GCC or Clang. None of them were found on your environment."
    exit 1
fi

PLATFORM=$(uname)

# WARNING: If your MINIX has support to Pthreads comment the following if-block and let's give it a try! :)
if test "$PLATFORM" = "Minix"
then
    # WARNING: This implicitly disables the Pthread support on cutest.
    COMPILER_OPTS=$(echo $COMPILER_OPTS | sed s/-DHVM_ASYNC_RQUEUE//)
fi

if test "$PLATFORM" = "FreeBSD" || test "$PLATFORM" = "Minix" || test "$PLATFORM" = "NetBSD"
then
    LINKERFLAGS="-lexecinfo"
else
    LINKERFLAGS="-ldl"
fi

CUTEST_CFLAGS=""

if test $(echo $COMPILER_OPTS | grep "HVM_ASYNC_RQUEUE" | wc -l) = 1
then
    LINKERFLAGS=${LINKERFLAGS}" -lpthread"
else
    CUTEST_CFLAGS="-DHAS_NO_PTHREAD"
fi

if test "$PLATFORM" = "SunOS"
then
    # WARNING: Does your Solaris has support for gnu bts? Comment the following var setting and let's try it.
    CUTEST_CFLAGS=${CUTEST_CFLAGS}" -DNO_CUTEST_BACKTRACING"
    COMPILER_OPTS=${COMPILER_OPTS}" -DNO_CUTEST_BACKTRACING"
fi

if [ -d /usr/local/include ] ;
then
    CUTEST_CFLAGS=${CUTEST_CFLAGS}" -I/usr/local/include"
    COMPILER_OPTS=${COMPILER_OPTS}" -I/usr/local/include"
fi


LINKER_OPTS="-o../bin/hefesto conv.o dbg.o dep_chain.o expr_handler.o exprchk.o file_io.o hlsc_msg.o\
                htask.o hvm.o hvm_alu.o hvm_func.o hvm_list.o hvm_rqueue.o hvm_str.o hvm_syscall.o\
                     hvm_thread.o hvm_toolset.o init.o lang_defs.o main.o mem.o os_detect.o parser.o\
                         src_chsum.o structs_io.o synchk.o types.o vfs.o hvm_project.o hvm_winreg.o ivk.o hvm_mod.o here/src/libhere.a -Ihere/src/ $LINKERFLAGS"

UNIT_TEST="-omain -L../../here/src ../../dbg.o ../../conv.o ../../dep_chain.o ../../expr_handler.o ../../exprchk.o ../../file_io.o ../../hlsc_msg.o\
            ../../htask.o ../../hvm.o ../../hvm_alu.o ../../hvm_func.o ../../hvm_list.o ../../hvm_rqueue.o ../../hvm_str.o\
                ../../hvm_syscall.o ../../hvm_thread.o ../../hvm_toolset.o ../../init.o ../../lang_defs.o ../../mem.o ../../os_detect.o\
                  ../../parser.o ../../src_chsum.o ../../structs_io.o ../../synchk.o ../../types.o ../../vfs.o main.o cutest.o cutest_memory.o cutest_mmap.o ../../hvm_project.o ../../hvm_winreg.o ../../ivk.o ../../hvm_mod.o $LINKERFLAGS\
                    -lhere"

ALL_OK=1

if test "$PLATFORM" = "FreeBSD" || test "$PLATFORM" = "Minix" || test "$PLATFORM" = "NetBSD" || test "$PLATFORM" = "OpenBSD"
then
    HERE_FLAGS="-lexecinfo"
else
    HERE_FLAGS="-ldl"
fi

if [ -d /usr/local/lib ] ;
then
    LINKER_OPTS=${LINKER_OPTS}" -L/usr/local/lib"
    HERE_FLAGS=${HERE_FLAGS}" -L/usr/locallib"
fi

HERE_UNIT_TEST="-ohere_unittest main.o ../libhere.a cutest/src/cutest.o cutest/src/cutest_memory.o cutest/src/cutest_mmap.o $HERE_FLAGS"

if test $(echo $COMPILER_OPTS | grep "HVM_ASYNC_RQUEUE" | wc -l) = 1
then
    HERE_UNIT_TEST=${HERE_UNIT_TEST}" -lpthread"
fi

LIBHERE_OBJS="here.o here_ctx.o here_mmachine.o here_mem.o"

# I know... this is ugly... boo!

echo "### Compiling..."

$COMPILER $COMPILER_OPTS conv.c
if test $? -gt 0
then
    ALL_OK=0
fi
$COMPILER $COMPILER_OPTS dbg.c
if test $? -gt 0
then
ALL_OK=0
fi
$COMPILER $COMPILER_OPTS dep_chain.c
if test $? -gt 0
then
    ALL_OK=0
fi
$COMPILER $COMPILER_OPTS expr_handler.c
if test $? -gt 0
then
    ALL_OK=0
fi
$COMPILER $COMPILER_OPTS exprchk.c
if test $? -gt 0
then
    ALL_OK=0
fi
$COMPILER $COMPILER_OPTS file_io.c
if test $? -gt 0
then
    ALL_OK=0
fi
$COMPILER $COMPILER_OPTS hlsc_msg.c
if test $? -gt 0
then
    ALL_OK=0
fi
$COMPILER $COMPILER_OPTS htask.c
if test $? -gt 0
then
    ALL_OK=0
fi
$COMPILER $COMPILER_OPTS hvm.c
if test $? -gt 0
then
    ALL_OK=0
fi
$COMPILER $COMPILER_OPTS hvm_alu.c
if test $? -gt 0
then
    ALL_OK=0
fi
$COMPILER $COMPILER_OPTS hvm_func.c
if test $? -gt 0
then
    ALL_OK=0
fi
$COMPILER $COMPILER_OPTS hvm_list.c
if test $? -gt 0
then
    ALL_OK=0
fi
$COMPILER $COMPILER_OPTS hvm_rqueue.c
if test $? -gt 0
then
    ALL_OK=0
fi
$COMPILER $COMPILER_OPTS hvm_str.c
if test $? -gt 0
then
    ALL_OK=0
fi
$COMPILER $COMPILER_OPTS hvm_syscall.c
if test $? -gt 0
then
    ALL_OK=0
fi
$COMPILER $COMPILER_OPTS hvm_thread.c
if test $? -gt 0
then
    ALL_OK=0
fi
$COMPILER $COMPILER_OPTS hvm_toolset.c
if test $? -gt 0
then
    ALL_OK=0
fi
$COMPILER $COMPILER_OPTS init.c
if test $? -gt 0
then
    ALL_OK=0
fi
$COMPILER $COMPILER_OPTS lang_defs.c
if test $? -gt 0
then
    ALL_OK=0
fi
$COMPILER $COMPILER_OPTS main.c
if test $? -gt 0
then
    ALL_OK=0
fi
$COMPILER $COMPILER_OPTS mem.c
if test $? -gt 0
then
    ALL_OK=0
fi
$COMPILER $COMPILER_OPTS os_detect.c
if test $? -gt 0
then
    ALL_OK=0
fi
$COMPILER $COMPILER_OPTS parser.c
if test $? -gt 0
then
    ALL_OK=0
fi
$COMPILER $COMPILER_OPTS src_chsum.c
if test $? -gt 0
then
    ALL_OK=0
fi
$COMPILER $COMPILER_OPTS structs_io.c
if test $? -gt 0
then
    ALL_OK=0
fi
$COMPILER $COMPILER_OPTS synchk.c
if test $? -gt 0
then
    ALL_OK=0
fi
$COMPILER $COMPILER_OPTS types.c
if test $? -gt 0
then
    ALL_OK=0
fi
$COMPILER $COMPILER_OPTS vfs.c
if test $? -gt 0
then
    ALL_OK=0
fi
$COMPILER $COMPILER_OPTS hvm_project.c
if test $? -gt 0
then
    ALL_OK=0
fi
$COMPILER $COMPILER_OPTS hvm_winreg.c
if test $? -gt 0
then
    ALL_OK=0
fi
$COMPILER $COMPILER_OPTS ivk.c
if test $? -gt 0
then
    ALL_OK=0
fi
$COMPILER $COMPILER_OPTS hvm_mod.c
if test $? -gt 0
then
    ALL_OK=0
fi

ALL_OK=1
cd here/src/
$COMPILER $COMPILER_OPTS here.c
if test $? -gt 0
then
    ALL_OK=0
fi
$COMPILER $COMPILER_OPTS here_ctx.c
if test $? -gt 0
then
    ALL_OK=0
fi
$COMPILER $COMPILER_OPTS here_mem.c
if test $? -gt 0
then
    ALL_OK=0
fi
$COMPILER $COMPILER_OPTS here_mmachine.c
if test $? -gt 0
then
    ALL_OK=0
else
    $LIB -r "libhere.a" $LIBHERE_OBJS
fi
cd test
cd cutest/src/
$COMPILER -c cutest.c $CUTEST_CFLAGS
$COMPILER -c cutest_memory.c $CUTEST_CFLAGS
$COMPILER -c cutest_mmap.c $CUTEST_CFLAGS
cd ../..
$COMPILER $COMPILER_OPTS main.c -Icutest/src
if test $? -gt 0
then
    ALL_OK=0
else
    $LINKER $HERE_UNIT_TEST
    ./here_unittest
fi

if test $? -gt 0
then
    ALL_OK=0
fi

cd ../../../
if test $ALL_OK -eq 1
then
echo "### Compiled."

echo "### Now running unit tests"

# Running unit tests

cd tests/unit
$COMPILER -c -I../../here/src -Icutest/src main.c $CUTEST_CFLAGS
$COMPILER -c cutest/src/cutest.c $CUTEST_CFLAGS
$COMPILER -c cutest/src/cutest_memory.c $CUTEST_CFLAGS
$COMPILER -c cutest/src/cutest_mmap.c $CUTEST_CFLAGS
$LINKER $UNIT_TEST
./main

if test $? -gt 0
then
    ALL_OK=0
fi

cd ../../

if test $ALL_OK -eq 1
then
    echo "### Unit tests are ok."
    # Now linking
    echo "### Linking..."
    $LINKER $LINKER_OPTS

    if test $? -eq 0
    then
        echo "### Linked."

        # If tests are ok, use the hefesto to install hefesto ;)

        ../bin/hefesto --forgefiles=../setup/hfst-inst.hsl --hfst-inst-projects=hefesto-install

    fi
    # Done!
else
    echo "### Unit tests has errors!"
fi

else
    echo "### Compilation errors!!"
fi
