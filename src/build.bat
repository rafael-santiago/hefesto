@SET COMPILER=gcc

@SET COMPILER_OPTS=-c

@SET LINKER=gcc

@SET LINKER_OPTS=-o../bin/hefesto.exe dbg.o dep_chain.o expr_handler.o exprchk.o file_io.o hlsc_msg.o htask.o hvm.o hvm_alu.o hvm_func.o hvm_list.o hvm_rqueue.o hvm_str.o hvm_syscall.o hvm_thread.o hvm_toolset.o init.o lang_defs.o main.o mem.o os_detect.o parser.o regex.o src_chsum.o structs_io.o synchk.o types.o vfs.o hvm_project.o

@SET UNIT_TEST=-omain.exe ../../dbg.o ../../dep_chain.o ../../expr_handler.o ../../exprchk.o ../../file_io.o ../../hlsc_msg.o ../../htask.o ../../hvm.o ../../hvm_alu.o ../../hvm_func.o ../../hvm_list.o ../../hvm_rqueue.o ../../hvm_str.o ../../hvm_syscall.o ../../hvm_thread.o ../../hvm_toolset.o ../../init.o ../../lang_defs.o ../../mem.o ../../os_detect.o ../../parser.o ../../regex.o ../../src_chsum.o ../../structs_io.o ../../synchk.o ../../types.o ../../vfs.o main.o htest.o ../../hvm_project.o

@echo ### Compiling

@%COMPILER% %COMPILER_OPTS% dbg.c
@IF %ERRORLEVEL% NEQ 0 ( GOTO COMPILATION_FAIL )

@%COMPILER% %COMPILER_OPTS% dep_chain.c
@IF %ERRORLEVEL% NEQ 0 ( GOTO COMPILATION_FAIL )

@%COMPILER% %COMPILER_OPTS% exprchk.c
@IF %ERRORLEVEL% NEQ 0 ( GOTO COMPILATION_FAIL )

@%COMPILER% %COMPILER_OPTS% expr_handler.c
@IF %ERRORLEVEL% NEQ 0 ( GOTO COMPILATION_FAIL )

@%COMPILER% %COMPILER_OPTS% file_io.c
@IF %ERRORLEVEL% NEQ 0 ( GOTO COMPILATION_FAIL )

@%COMPILER% %COMPILER_OPTS% hlsc_msg.c
@IF %ERRORLEVEL% NEQ 0 ( GOTO COMPILATION_FAIL )

@%COMPILER% %COMPILER_OPTS% htask.c
@IF %ERRORLEVEL% NEQ 0 ( GOTO COMPILATION_FAIL )

@%COMPILER% %COMPILER_OPTS% hvm.c
@IF %ERRORLEVEL% NEQ 0 ( GOTO COMPILATION_FAIL )

@%COMPILER% %COMPILER_OPTS% hvm_alu.c
@IF %ERRORLEVEL% NEQ 0 ( GOTO COMPILATION_FAIL )

@%COMPILER% %COMPILER_OPTS% hvm_func.c
@IF %ERRORLEVEL% NEQ 0 ( GOTO COMPILATION_FAIL )

@%COMPILER% %COMPILER_OPTS% hvm_list.c
@IF %ERRORLEVEL% NEQ 0 ( GOTO COMPILATION_FAIL )

@%COMPILER% %COMPILER_OPTS% hvm_project.c
@IF %ERRORLEVEL% NEQ 0 ( GOTO COMPILATION_FAIL )

@%COMPILER% %COMPILER_OPTS% hvm_rqueue.c
@IF %ERRORLEVEL% NEQ 0 ( GOTO COMPILATION_FAIL )

@%COMPILER% %COMPILER_OPTS% hvm_str.c
@IF %ERRORLEVEL% NEQ 0 ( GOTO COMPILATION_FAIL )

@%COMPILER% %COMPILER_OPTS% hvm_syscall.c
@IF %ERRORLEVEL% NEQ 0 ( GOTO COMPILATION_FAIL )

@%COMPILER% %COMPILER_OPTS% hvm_thread.c
@IF %ERRORLEVEL% NEQ 0 ( GOTO COMPILATION_FAIL )

@%COMPILER% %COMPILER_OPTS% hvm_toolset.c
@IF %ERRORLEVEL% NEQ 0 ( GOTO COMPILATION_FAIL )

@%COMPILER% %COMPILER_OPTS% init.c
@IF %ERRORLEVEL% NEQ 0 ( GOTO COMPILATION_FAIL )

@%COMPILER% %COMPILER_OPTS% lang_defs.c
@IF %ERRORLEVEL% NEQ 0 ( GOTO COMPILATION_FAIL )

@%COMPILER% %COMPILER_OPTS% main.c
@IF %ERRORLEVEL% NEQ 0 ( GOTO COMPILATION_FAIL )

@%COMPILER% %COMPILER_OPTS% mem.c
@IF %ERRORLEVEL% NEQ 0 ( GOTO COMPILATION_FAIL )

@%COMPILER% %COMPILER_OPTS% os_detect.c
@IF %ERRORLEVEL% NEQ 0 ( GOTO COMPILATION_FAIL )

@%COMPILER% %COMPILER_OPTS% parser.c
@IF %ERRORLEVEL% NEQ 0 ( GOTO COMPILATION_FAIL )

@%COMPILER% %COMPILER_OPTS% regex.c
@IF %ERRORLEVEL% NEQ 0 ( GOTO COMPILATION_FAIL )

@%COMPILER% %COMPILER_OPTS% src_chsum.c
@IF %ERRORLEVEL% NEQ 0 ( GOTO COMPILATION_FAIL )

@%COMPILER% %COMPILER_OPTS% structs_io.c
@IF %ERRORLEVEL% NEQ 0 ( GOTO COMPILATION_FAIL )

@%COMPILER% %COMPILER_OPTS% synchk.c
@IF %ERRORLEVEL% NEQ 0 ( GOTO COMPILATION_FAIL )

@%COMPILER% %COMPILER_OPTS% types.c
@IF %ERRORLEVEL% NEQ 0 ( GOTO COMPILATION_FAIL )

@%COMPILER% %COMPILER_OPTS% vfs.c
@IF %ERRORLEVEL% NEQ 0 ( GOTO COMPILATION_FAIL )

@echo ### Compiled.

@echo ### Now running unit tests

@cd tests\unit
@%COMPILER% -c main.c
@%COMPILER% -c htest.c
@%LINKER% %UNIT_TEST%
@main.exe
@cd ..\..\

@IF %ERRORLEVEL% NEQ 0 ( GOTO UNIT_TEST_COMPILATION_FAIL )

@echo ### Unit tests are ok.

@echo ### Linking...
@%LINKER% %LINKER_OPTS%

@IF %ERRORLEVEL% NEQ 0 ( GOTO HEFESTO_LINK_ERROR )

@echo ### Linked.

@..\bin\hefesto.exe --forgefiles=..\\setup\\hfst-inst.hls --hfst-inst-projects=hefesto-install
@GOTO ALL_DONE

:COMPILATION_FAIL
@echo ### Compilation errors!!
@GOTO ALL_DONE

:UNIT_TEST_COMPILATION_FAIL
@echo ### Unit tests has errors!
@GOTO ALL_DONE

:HEFESTO_LINK_ERROR
@echo ### Hefesto linking error! 
@GOTO ALL_DONE

:ALL_DONE
