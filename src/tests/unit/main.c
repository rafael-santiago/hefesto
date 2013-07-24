#include <string.h>

#include "test_defs.h"

#include "htest.h"
#include "../../structs_io.h"
#include "../../types.h"
#include "../../hvm_str.h"
#include "../../parser.h"
#include "../../hvm_func.h"
#include "../../hvm_syscall.h"
#include "../../dep_chain.h"
#include "../../vfs.h"
#include "../../hvm_toolset.h"
#include "../../here/here.h"

#if HEFESTO_TGT_OS == HEFESTO_WINDOWS

#include <time.h>

#endif

char *hefesto_common_stack_ctx_tests() {
    hefesto_common_stack_ctx *stack = NULL;
    char *data;
    size_t dsize;
    printf("-- running hefesto_common_stack_ctx_test\n");
    stack = hefesto_common_stack_ctx_push(stack, "foobar", 6,
                                          HEFESTO_VAR_TYPE_STRING);
    HTEST_CHECK("stack == NULL", stack != NULL);
    data = hefesto_common_stack_ctx_data_on_top(stack);
    HTEST_CHECK("stack top != foobar", strcmp(data, "foobar") == 0);
    dsize = hefesto_common_stack_ctx_dsize_on_top(stack);
    HTEST_CHECK("stack top dsize != 6", dsize == 6);
    stack = hefesto_common_stack_ctx_pop(stack);
    HTEST_CHECK("stack with 1 item after pop != NULL", stack == NULL);
    stack = hefesto_common_stack_ctx_push(stack, "beavis", 6,
                                          HEFESTO_VAR_TYPE_STRING);
    stack = hefesto_common_stack_ctx_push(stack, "stuart", 6,
                                          HEFESTO_VAR_TYPE_STRING);
    stack = hefesto_common_stack_ctx_push(stack, "butt-head", 9,
                                          HEFESTO_VAR_TYPE_STRING);
    data = hefesto_common_stack_ctx_data_on_top(stack);
    HTEST_CHECK("stack top != butt-head", strcmp(data,
                                                 "butt-head") == 0);
    stack = hefesto_common_stack_ctx_pop(stack);
    data = hefesto_common_stack_ctx_data_on_top(stack);
    HTEST_CHECK("stack top != stuart", strcmp(data, "stuart") == 0);
    stack = hefesto_common_stack_ctx_pop(stack);
    data = hefesto_common_stack_ctx_data_on_top(stack);
    HTEST_CHECK("stack top != beavis", strcmp(data, "beavis") == 0);
    stack = hefesto_common_stack_ctx_pop(stack);
    HTEST_CHECK("stack with 1 item after pop != NULL", stack == NULL);
    printf("-- passed.\n");
    return NULL;
}

char *hefesto_common_list_ctx_tests() {

    hefesto_common_list_ctx *list = NULL, *copy;
    printf("-- running hefesto_common_list_ctx_tests\n");
    list = add_data_to_hefesto_common_list_ctx(list, "hls", 3);
    list = add_data_to_hefesto_common_list_ctx(list, "middle", 6);
    list = add_data_to_hefesto_common_list_ctx(list, "hefesto", 7);
    copy = cp_hefesto_common_list_ctx(list);
    HTEST_CHECK("get_hefesto_common_list_ctx_count() != 3",
                get_hefesto_common_list_ctx_count(list) == 3);
    list = del_data_from_hefesto_common_list_ctx(list, "middle",
                                                 HEFESTO_VAR_TYPE_STRING);
    HTEST_CHECK("get_hefesto_common_list_ctx_count() != 2",
                get_hefesto_common_list_ctx_count(list) == 2);
    HTEST_CHECK("common list head != hls", strcmp(list->data, "hls") == 0);
    HTEST_CHECK("common list tail != hefesto",
                strcmp((char *)get_hefesto_common_list_ctx_tail(list)->data,
                       "hefesto") == 0);
    list = del_data_from_hefesto_common_list_ctx(list, "hls",
                                                 HEFESTO_VAR_TYPE_STRING);
    HTEST_CHECK("get_hefesto_common_list_ctx_count() != 1",
                get_hefesto_common_list_ctx_count(list) == 1);
    HTEST_CHECK("common list head != hefesto",
                strcmp(list->data, "hefesto") == 0);
    list = del_data_from_hefesto_common_list_ctx(list, "hefesto",
                                                 HEFESTO_VAR_TYPE_STRING);
    HTEST_CHECK("get_hefesto_common_list_ctx_count() != 0",
                get_hefesto_common_list_ctx_count(list) == 0);
    HTEST_CHECK("common list after all removes possible != NULL",
                list == NULL);
    HTEST_CHECK("copy list don't has 3 elements",
                get_hefesto_common_list_ctx_count(copy) == 3);
    HTEST_CHECK("1st item != hls",
                strcmp((char *)copy->data, "hls") == 0);
    HTEST_CHECK("2nd item != middle",
                strcmp((char *)copy->next->data, "middle") == 0);
    HTEST_CHECK("3rd item != hefesto",
                strcmp((char *)copy->next->next->data, "hefesto") == 0);
    del_hefesto_common_list_ctx(copy);
    printf("-- passed.\n");

    return NULL;
}

char *hefesto_var_list_ctx_tests() {

    hefesto_var_list_ctx *vlist = NULL;
    printf("-- running hefesto_var_list_ctx_tests\n");
    vlist = add_var_to_hefesto_var_list_ctx(vlist,
                                            "int_var",
                                            HEFESTO_VAR_TYPE_INT);
    vlist = add_var_to_hefesto_var_list_ctx(vlist,
                                            "str_var",
                                            HEFESTO_VAR_TYPE_STRING);
    vlist = add_var_to_hefesto_var_list_ctx(vlist,
                                            "file_var",
                                            HEFESTO_VAR_TYPE_FILE_DESCRIPTOR);
    HTEST_CHECK("int_var not found",
                get_hefesto_var_list_ctx_name("int_var", vlist) != NULL);
    HTEST_CHECK("str_var not found",
                get_hefesto_var_list_ctx_name("str_var", vlist) != NULL);
    HTEST_CHECK("file_var not found",
                get_hefesto_var_list_ctx_name("file_var", vlist) != NULL);
    HTEST_CHECK("1st var != int type",
                vlist->type == HEFESTO_VAR_TYPE_INT);
    HTEST_CHECK("2nd var != string type",
                vlist->next->type == HEFESTO_VAR_TYPE_STRING);
    HTEST_CHECK("3rd item != file type",
                vlist->next->next->type == HEFESTO_VAR_TYPE_FILE_DESCRIPTOR);
    del_hefesto_var_list_ctx(vlist);
    printf("-- passed.\n");

    return NULL;
}

char *hefesto_func_list_ctx_tests() {

    hefesto_func_list_ctx *flist = NULL;
    printf("-- running hefesto_func_list_ctx_tests\n");
    flist = add_func_to_hefesto_func_list_ctx(flist,
                                              "hls_int_type_function",
                                              HEFESTO_VAR_TYPE_INT);
    HTEST_CHECK("hls_int_type_function not found",
                get_hefesto_func_list_ctx_name("hls_int_type_function",
                                               flist) != NULL);
    HTEST_CHECK("function result type != int",
                flist->result_type == HEFESTO_VAR_TYPE_INT);
    del_hefesto_func_list_ctx(flist);
    printf("-- passed.\n");

    return NULL;
}

char *options_tests() {

    hefesto_options_ctx *options = NULL;
    printf("-- running parse_options_tests\n");
    options = add_option_to_hefesto_options_ctx(options, "single-option=X");
    options = add_option_to_hefesto_options_ctx(options,
                                                "multi-option=X, Y ,Z");
    HTEST_CHECK("1st option != single-option",
                strcmp(options->option, "single-option") == 0);
    HTEST_CHECK("single-option != X",
                strcmp(options->data->data, "X") == 0 &&
                            options->data->next == NULL);
    HTEST_CHECK("2nd option != multi-option",
                strcmp(options->next->option, "multi-option") == 0);
    HTEST_CHECK("multi-option != X, Y ,Z",
                strcmp(options->next->data->data, "X") == 0 &&
                strcmp(options->next->data->next->data, "Y ") == 0 &&
                strcmp(options->next->data->next->next->data, "Z") == 0);
    del_hefesto_options_ctx(options);
    printf("-- passed.\n");

    return NULL;
}

char *hvm_str_conversion_tests() {

    char *x = hvm_int_to_str(101);
    printf("-- running hvm_str_conversion_tests\n");
    HTEST_CHECK("x != \"101\"", strcmp(x, "101") == 0);
    HTEST_CHECK("hvm_str_to_int() != 101", hvm_str_to_int(x) == 101);
    free(x);
    printf("-- passed.\n");

    return NULL;
}

char *hvm_str_format_tests() {

    char *fmt_str;
    hefesto_var_list_ctx *lo_vars = NULL, *gl_vars = NULL, *vp;
    int dec;
    printf("-- running hvm_str_format\n");
    lo_vars = add_var_to_hefesto_var_list_ctx(lo_vars,
                                              "age", HEFESTO_VAR_TYPE_INT);
    gl_vars = add_var_to_hefesto_var_list_ctx(gl_vars,
                                              "I", HEFESTO_VAR_TYPE_STRING);
    lo_vars = add_var_to_hefesto_var_list_ctx(lo_vars,
                                              "was_born",
                                              HEFESTO_VAR_TYPE_STRING);
    gl_vars = add_var_to_hefesto_var_list_ctx(gl_vars, "years_ago",
                                              HEFESTO_VAR_TYPE_STRING);
    dec = 10000;
    vp = get_hefesto_var_list_ctx_name("age", lo_vars);
    vp = assign_data_to_hefesto_var(vp, &dec, sizeof(dec));
    vp = get_hefesto_var_list_ctx_name("I", gl_vars);
    vp = assign_data_to_hefesto_var(vp, "Eu", 2);
    vp = get_hefesto_var_list_ctx_name("was_born", lo_vars);
    vp = assign_data_to_hefesto_var(vp, "nasci", 5);
    vp = get_hefesto_var_list_ctx_name("years_ago", gl_vars);
    vp = assign_data_to_hefesto_var(vp, "anos atras", 10);
    fmt_str = hvm_str_format("$I + \" \" + $was_born + \" \" + $age + \" \""
                             " + $years_ago + \".\"", &lo_vars, &gl_vars, NULL);
    HTEST_CHECK("fmt_str != Eu nasci 10000 anos atras.",
                strcmp(fmt_str, "Eu nasci 10000 anos atras.") == 0);
    del_hefesto_var_list_ctx(lo_vars);
    del_hefesto_var_list_ctx(gl_vars);
    free(fmt_str);
    printf("-- passed.\n");

    return NULL;
}

size_t create_test_code(char *fname, char *cbuf) {
    FILE *fp;
    size_t fsize = 0;
    if ((fp = fopen(fname, "w")) != NULL) {
        fprintf(fp, "%s", cbuf);
        fclose(fp);
        if ((fp = fopen(fname, "r")) != NULL) {
            fseek(fp, 0L, SEEK_END);
            fsize = ftell(fp);
            fclose(fp);
        }
    }
    return fsize;
}

char *hvm_function_recurssion_tests() {

    hefesto_func_list_ctx *function = NULL;
    hefesto_var_list_ctx *lo_vars = NULL;
    hefesto_var_list_ctx *gl_vars = NULL;
    char *code = "function fatorial(value type int) : result type int {\n"
                 "\tif ($value == 0) result 1;\n"
                 "\tresult fatorial($value - 1) * $value;\n}\n";
    void *result;
    int errors = 0;
    create_test_code("ftest.hls", code);
    function = compile_and_load_hls_code("ftest.hls", &errors,
                                         &gl_vars, NULL, NULL);
    remove("ftest.hls");
    printf("-- running function_execution_tests\n");
    HTEST_CHECK("function == NULL", function != NULL);
    HTEST_CHECK("function->code == NULL", function->code != NULL);
    // basic recurssion, save and stack restore must be working
    // else you f_cked something.
    result = hvm_call_function("fatorial(3)", &lo_vars, &gl_vars, function);
    HTEST_CHECK("result == NULL", result != NULL);
    HTEST_CHECK("result != 6", *(int *)result == 6);
    free(result);
    result = hvm_call_function("fatorial(0)", &lo_vars, &gl_vars, function);
    HTEST_CHECK("result == NULL", result != NULL);
    // result must interrupts execution and go back...
    HTEST_CHECK("result != 1", *(int *)result == 1);
    free(result);
    del_hefesto_func_list_ctx(function);
    printf("-- passed.\n");

    return NULL;
}

char *hvm_if_tests() {

    hefesto_func_list_ctx *function = NULL;
    hefesto_var_list_ctx *lo_vars = NULL;
    hefesto_var_list_ctx *gl_vars = NULL;
    printf("-- running hvm_if_tests\n");
    char *code = "function if_zero_results_one(value type int) : "
                 "result type int {\n"
                 "\tif ($value == 0) result 1;\n"
                 "\telse result 0;\n}\n";
    void *result;
    int errors = 0;
    create_test_code("ftest.hls", code);
    function = compile_and_load_hls_code("ftest.hls",
                                         &errors,
                                         &gl_vars,
                                         NULL,
                                         NULL);
    remove("ftest.hls");
    result = hvm_call_function("if_zero_results_one(10)",
                               &lo_vars, &gl_vars, function);
    HTEST_CHECK("result == NULL",
                result != NULL);
    HTEST_CHECK("result != 0",
                *(int *)result == 0);
    free(result);
    result = hvm_call_function("if_zero_results_one(0)",
                               &lo_vars, &gl_vars, function);
    HTEST_CHECK("result == NULL",
                result != NULL);
    HTEST_CHECK("result != 0",
                *(int *)result == 1);
    free(result);
    del_hefesto_func_list_ctx(function);
    printf("-- passed.\n");

    return NULL;
}

char *hvm_while_tests() {

    hefesto_func_list_ctx *function = NULL;
    hefesto_var_list_ctx *lo_vars = NULL;
    hefesto_var_list_ctx *gl_vars = NULL;
    printf("-- running hvm_while_tests\n");
    char *code = "function while_test(limit type int) : result type int"
                 "{\n\tvar i type int;\n\t$i = 0;\n\twhile ($i < $limit)"
                 " {\n\t\t$i = $i + 1;\n\t}\n\tresult $i;\n}\n";
    void *result;
    int errors = 0;
    create_test_code("ftest.hls", code);
    function = compile_and_load_hls_code("ftest.hls",
                                         &errors,
                                         &gl_vars,
                                         NULL, NULL);
    remove("ftest.hls");
    result = hvm_call_function("while_test(1000)",
                               &lo_vars,
                               &gl_vars, function);
    HTEST_CHECK("result == NULL", result != NULL);
    HTEST_CHECK("result != 1000", *(int *)result == 1000);
    free(result);
    result = hvm_call_function("while_test(0)", &lo_vars,
                               &gl_vars, function);
    HTEST_CHECK("result == NULL", result != NULL);
    HTEST_CHECK("result != 0", *(int *)result == 0);
    free(result);
    del_hefesto_func_list_ctx(function);
    printf("-- passed.\n");

    return NULL;
}

char *hvm_syscalls_file_io_tests() { 

    hefesto_var_list_ctx *lo_vars = NULL, *gl_vars = NULL;
    hefesto_type_t otype;
    hefesto_file_handle *fhandle = NULL;
    void *result;

    printf("-- running hvm_syscalls_file_io_tests\n");

    //  fopen w
    fhandle = (hefesto_file_handle *)
                hefesto_sys_call("hefesto.sys.fopen(\"test.dat\", \"w\")",
                                 &lo_vars, &gl_vars, NULL, &otype);
    HTEST_CHECK("fhandle == NULL", fhandle != NULL);

    lo_vars = add_var_to_hefesto_var_list_ctx(lo_vars,
                                              "fhandle",
                                              HEFESTO_VAR_TYPE_FILE_DESCRIPTOR);
    gl_vars = get_hefesto_var_list_ctx_name("fhandle", lo_vars);
    gl_vars = assign_data_to_hefesto_var_file_type(gl_vars, fhandle);
    gl_vars = NULL;
    //del_hefesto_file_handle(fhandle, 0);

    result = hefesto_sys_call("hefesto.sys.fwrite(\"with power of soul\", 18, $fhandle)",
                              &lo_vars, &gl_vars, NULL, &otype);
    HTEST_CHECK("result == NULL", result != NULL);
    HTEST_CHECK("result != 18", *(int *)result == 18);
    free(result);

    //  fclose
    result = hefesto_sys_call("hefesto.sys.fclose($fhandle)",
                              &lo_vars, &gl_vars, NULL, &otype);
    HTEST_CHECK("result == NULL", result != NULL);
    HTEST_CHECK("result != 1", *(int *)result == 1);
    free(result);

    del_hefesto_var_list_ctx(lo_vars);
    lo_vars = NULL;

    //  fopen r
    fhandle = (hefesto_file_handle *)
                hefesto_sys_call("hefesto.sys.fopen(\"test.dat\", \"r\")",
                                 &lo_vars, &gl_vars, NULL, &otype);
    HTEST_CHECK("fhandle == NULL", fhandle != NULL);

    lo_vars = add_var_to_hefesto_var_list_ctx(lo_vars,
                                              "fhandle",
                                              HEFESTO_VAR_TYPE_FILE_DESCRIPTOR);
    lo_vars = add_var_to_hefesto_var_list_ctx(lo_vars,
                                              "fbuffer",
                                              HEFESTO_VAR_TYPE_STRING);
    gl_vars = get_hefesto_var_list_ctx_name("fhandle", lo_vars);
    gl_vars = assign_data_to_hefesto_var_file_type(gl_vars, fhandle);
    gl_vars = NULL;
    //del_hefesto_file_handle(fhandle, 0);

    //   fread
    result = hefesto_sys_call("hefesto.sys.fread($fbuffer, 18, $fhandle)",
                              &lo_vars, &gl_vars, NULL, &otype);
    HTEST_CHECK("result == NULL", result != NULL);
    HTEST_CHECK("result != 18", *(int *)result == 18);
    free(result);

    gl_vars = get_hefesto_var_list_ctx_name("fbuffer", lo_vars);
    HTEST_CHECK("fbuffer == NULL", gl_vars != NULL && gl_vars->contents != NULL);
    HTEST_CHECK("fbuffer != with power of soul",
                strcmp(gl_vars->contents->data, "with power of soul") == 0);
    gl_vars = NULL;

    //  feof
    result = hefesto_sys_call("hefesto.sys.fread($fbuffer, 18, $fhandle)",
                              &lo_vars, &gl_vars, NULL, &otype);
    HTEST_CHECK("result == NULL", result != NULL);
    HTEST_CHECK("result != 0", *(int *)result == 0);
    free(result);

    result = hefesto_sys_call("hefesto.sys.feof($fhandle)",
                              &lo_vars, &gl_vars, NULL, &otype);
    HTEST_CHECK("result == NULL", result != NULL);
    HTEST_CHECK("result != 1", *(int *)result == 1);
    free(result);

    //  fseek_to_begin
    result = hefesto_sys_call("hefesto.sys.fseek_to_begin($fhandle)",
                              &lo_vars, &gl_vars, NULL, &otype);
    HTEST_CHECK("result == NULL", result != NULL);
    HTEST_CHECK("result != 0", *(int *)result == 0);
    free(result);

    //   fseek_to_end
    result = hefesto_sys_call("hefesto.sys.fseek_to_end($fhandle)",
                              &lo_vars, &gl_vars, NULL, &otype);
    HTEST_CHECK("result == NULL", result != NULL);
    HTEST_CHECK("result != 0", *(int *)result == 0);
    free(result);

    //   fseek
    result = hefesto_sys_call("hefesto.sys.fseek($fhandle, 2)",
                              &lo_vars, &gl_vars, NULL, &otype);
    HTEST_CHECK("result == NULL", result != NULL);
    HTEST_CHECK("result != 0", *(int *)result == 0);
    free(result);

    //   ftell
    result = hefesto_sys_call("hefesto.sys.ftell($fhandle)",
                              &lo_vars, &gl_vars, NULL, &otype);
    HTEST_CHECK("result == NULL", result != NULL);
    HTEST_CHECK("result != 2", *(int *)result == 2);
    free(result);

    //   fsize
    result = hefesto_sys_call("hefesto.sys.fsize($fhandle)",
                              &lo_vars, &gl_vars, NULL, &otype);
    HTEST_CHECK("result == NULL", result != NULL);
    HTEST_CHECK("result != 18", *(int *)result == 18);
    free(result);

    //  fclose
    result = hefesto_sys_call("hefesto.sys.fclose($fhandle)",
                              &lo_vars, &gl_vars, NULL, &otype);
    HTEST_CHECK("result == NULL", result != NULL);
    HTEST_CHECK("result != 1", *(int *)result == 1);
    free(result);

    del_hefesto_var_list_ctx(lo_vars);
    lo_vars = NULL;

    //  replace_in_file
    result = hefesto_sys_call("hefesto.sys.replace_in_file(\"test.dat\","
                                                          "\"with power of soul\","
                                                          "\"anything is possible\")",
                              &lo_vars, &gl_vars, NULL, &otype);
    HTEST_CHECK("result == NULL", result != NULL);
    HTEST_CHECK("result != 1", *(int *)result == 1);
    free(result);

    fhandle = (hefesto_file_handle *)
                hefesto_sys_call("hefesto.sys.fopen(\"test.dat\", \"r\")",
                                 &lo_vars, &gl_vars, NULL, &otype);
    HTEST_CHECK("fhandle == NULL", fhandle != NULL);

    lo_vars = add_var_to_hefesto_var_list_ctx(lo_vars,
                                              "fhandle",
                                              HEFESTO_VAR_TYPE_FILE_DESCRIPTOR);
    lo_vars = add_var_to_hefesto_var_list_ctx(lo_vars,
                                              "fbuffer",
                                              HEFESTO_VAR_TYPE_STRING);
    gl_vars = get_hefesto_var_list_ctx_name("fhandle", lo_vars);
    gl_vars = assign_data_to_hefesto_var_file_type(gl_vars, fhandle);
    gl_vars = NULL;
    del_hefesto_file_handle(fhandle, 0);

    result = hefesto_sys_call("hefesto.sys.fread($fbuffer, 20, $fhandle)",
                              &lo_vars, &gl_vars, NULL, &otype);
    HTEST_CHECK("result == NULL", result != NULL);
    HTEST_CHECK("result != 20", *(int *)result == 20);
    free(result);

    gl_vars = get_hefesto_var_list_ctx_name("fbuffer", lo_vars);
    HTEST_CHECK("fbuffer == NULL", gl_vars != NULL && gl_vars->contents != NULL);
    HTEST_CHECK("fbuffer != anything is possible",
                strcmp(gl_vars->contents->data, "anything is possible") == 0);
    gl_vars = NULL;

    //  fclose
    result = hefesto_sys_call("hefesto.sys.fclose($fhandle)",
                              &lo_vars, &gl_vars, NULL, &otype);
    HTEST_CHECK("result == NULL", result != NULL);
    HTEST_CHECK("result != 1", *(int *)result == 1);
    free(result);

    printf("-- passed.\n");

    return NULL;
}

char *hvm_syscalls_filesystem_tests() {

    void *result;
    char temp[256], test_dir[256];
    char *complains = NULL;
    hefesto_type_t otype;
    hefesto_var_list_ctx *lo_vars = NULL, *gl_vars = NULL;
    hefesto_func_list_ctx *func = NULL;
    FILE *fp;

    printf("-- running hvm_syscalls_filesystem_tests\n");

    //  setting the cwd and saving fullpath to compare below.

    result = hefesto_sys_call("hefesto.sys.pwd()",
                              &lo_vars, &gl_vars, func, &otype);
    sprintf(test_dir, "%s", result);

    hefesto_init();

    //  removing previous occurrence of "test_directory" subdir

    remove("test.dat");
    remove("test_directory/test.dat.bak");
    remove("test_directory/src_cp_dir.dat");
    remove("test_directory/src_cp_dir.dat.bak");
    remove("test_directory/sub_dir/src_cp_dir.dat");
    remove("test_directory/sub_dir/src_cp_dir.dat.bak");
    remove("test_directory/sub_dir");
    remove("test_directory/empty_sub_dir");
    remove("src_cp_dir/src_cp_dir.dat");    
    remove("src_cp_dir/src_cp_dir.dat.bak");
    remove("src_cp_dir/sub_dir/src_cp_dir.dat");    
    remove("src_cp_dir/sub_dir/src_cp_dir.dat.bak");
    rmdir("src_cp_dir/empty_sub_dir");
    rmdir("src_cp_dir/sub_dir");
    rmdir("src_cp_dir");
    remove("test.dat.a");
    remove("test.dat.b");
    remove("test.dat.c");
    remove("test_regex_cp/test.dat.a");
    remove("test_regex_cp/test.dat.b");
    remove("test_regex_cp/test.dat.c");
    rmdir("test_regex_cp/test.dat.d");
    rmdir("test_regex_cp");
    rmdir("test.dat.d");

    result = hefesto_sys_call("hefesto.sys.rmdir(\"test_directory\")",
                              &lo_vars, &gl_vars, NULL, &otype);
    free(result);

    //  mkdir
    result = hefesto_sys_call("hefesto.sys.mkdir(\"test_directory\")",
                              &lo_vars, &gl_vars, NULL, &otype);
    HTEST_CHECK("result == NULL", result != NULL);
    HTEST_CHECK("result != 0", *(int *)result == 0);
    free(result);

    //  mkdir
    result = hefesto_sys_call("hefesto.sys.mkdir(\"src_cp_dir\")",
                              &lo_vars, &gl_vars, NULL, &otype);
    HTEST_CHECK("result == NULL", result != NULL);
    HTEST_CHECK("result != 0", *(int *)result == 0);
    free(result);

    //  cd
    result = hefesto_sys_call("hefesto.sys.cd(\"test_directory\")",
                              &lo_vars, &gl_vars, NULL, &otype);
    free(result);

    result = hefesto_sys_call("hefesto.sys.cd(\"..\")",
                              &lo_vars, &gl_vars, NULL, &otype);
    free(result);
    result = hefesto_sys_call("hefesto.sys.cd(\"src_cp_dir\")",
                              &lo_vars, &gl_vars, NULL, &otype);
    free(result);

    result = hefesto_sys_call("hefesto.sys.mkdir(\"empty_sub_dir\")",
                              &lo_vars, &gl_vars, NULL, &otype);
    free(result);
    result = hefesto_sys_call("hefesto.sys.mkdir(\"sub_dir\")",
                              &lo_vars, &gl_vars, NULL, &otype);
    free(result);

    result = hefesto_sys_call("hefesto.sys.cd(\"..\")",
                              &lo_vars, &gl_vars, NULL, &otype);
    free(result);

    result = hefesto_sys_call("hefesto.sys.cd(\"test_directory\")",
                              &lo_vars, &gl_vars, NULL, &otype);
    free(result);

    //  pwd again
    result = hefesto_sys_call("hefesto.sys.pwd()",
                              &lo_vars, &gl_vars, NULL, &otype);
    HTEST_CHECK("result == NULL", result != NULL);
    sprintf(temp, "%s", test_dir);
    temp[strlen(temp)+1] = 0;
    temp[strlen(temp)] = HEFESTO_PATH_SEP;
    strcat(temp, "test_directory");
    HTEST_CHECK("result != temp", strcmp(result, temp) == 0);
    free(result);

    //  cd with relative path/indirection hefesto_init() done this...
    result = hefesto_sys_call("hefesto.sys.cd(\"..\")",
                              &lo_vars, &gl_vars, NULL, &otype);
    free(result);

    complains = hvm_syscalls_file_io_tests();

    if (complains != NULL) return complains;

    //  cp
    result = hefesto_sys_call("hefesto.sys.cp(\"test.dat\","
                              "\"test_directory/test.dat.bak\")",
                              &lo_vars, &gl_vars, NULL, &otype);
    HTEST_CHECK("result == NULL", result != NULL);
    HTEST_CHECK("result != 1", *(int *)result == 1);
    free(result);

    //  cp between directories
    result = hefesto_sys_call("hefesto.sys.cp(\"test.dat\","
                              "\"src_cp_dir/src_cp_dir.dat\")",
                              &lo_vars, &gl_vars, NULL, &otype);
    HTEST_CHECK("result == NULL", result != NULL);
    HTEST_CHECK("result != 1", *(int *)result == 1);
    free(result);

    result = hefesto_sys_call("hefesto.sys.cp(\"test.dat\","
                              "\"src_cp_dir/src_cp_dir.dat.bak\")",
                              &lo_vars, &gl_vars, NULL, &otype);
    HTEST_CHECK("result == NULL", result != NULL);
    HTEST_CHECK("result != 1", *(int *)result == 1);
    free(result);

    result = hefesto_sys_call("hefesto.sys.cp(\"src_cp_dir/src_cp_dir.dat\","
                              "\"src_cp_dir/sub_dir/src_cp_dir.dat\")",
                              &lo_vars, &gl_vars, NULL, &otype);
    free(result);
    result = hefesto_sys_call("hefesto.sys.cp(\"src_cp_dir/src_cp_dir.dat\","
                              "\"src_cp_dir/sub_dir/src_cp_dir.dat.bak\")",
                              &lo_vars, &gl_vars, NULL, &otype);
    free(result);

    result = hefesto_sys_call("hefesto.sys.cp(\"src_cp_dir\","
                              "\"test_directory\")", &lo_vars,
                              &gl_vars, NULL, &otype);
    HTEST_CHECK("result == NULL", result != NULL);
    HTEST_CHECK("result != 1", *(int *)result == 1);
    free(result);

    result = hefesto_sys_call("hefesto.sys.cd(\"test_directory\")",
                              &lo_vars, &gl_vars, NULL, &otype);
    free(result);

    //  ls
    result = hefesto_sys_call("hefesto.sys.ls(\"test.dat.bak\")",
                              &lo_vars, &gl_vars, NULL, &otype);
    HTEST_CHECK("result == NULL", result != NULL);
    HTEST_CHECK("result != 1", *(int *)result == 1);
    free(result);

    result = hefesto_sys_call("hefesto.sys.cd(\"empty_sub_dir\")",
                              &lo_vars, &gl_vars, NULL, &otype);
    free(result);

    result = hefesto_sys_call("hefesto.sys.ls(\".*\")",
                              &lo_vars, &gl_vars, NULL, &otype);
    HTEST_CHECK("result == NULL", result != NULL);
    HTEST_CHECK("result != 0", *(int *)result == 0);

    result = hefesto_sys_call("hefesto.sys.cd(\"..\")",
                              &lo_vars, &gl_vars, NULL, &otype);
    free(result);

    result = hefesto_sys_call("hefesto.sys.cd(\"sub_dir\")",
                              &lo_vars, &gl_vars, NULL, &otype);
    free(result);

    result = hefesto_sys_call("hefesto.sys.ls(\".*\")",
                              &lo_vars, &gl_vars, NULL, &otype);
    HTEST_CHECK("result == NULL", result != NULL);
    HTEST_CHECK("result != 0", *(int *)result == 2);

    result = hefesto_sys_call("hefesto.sys.cd(\"..\")",
                              &lo_vars, &gl_vars, NULL, &otype);
    free(result);

    result = hefesto_sys_call("hefesto.sys.cd(\"..\")",
                              &lo_vars, &gl_vars, NULL, &otype);
    free(result);

    //  make_path
    result = hefesto_sys_call("hefesto.sys.make_path(\"/home\", \"rs\")",
                              &lo_vars, &gl_vars, NULL, &otype);
    HTEST_CHECK("result == NULL", result != NULL);
#if HEFESTO_TGT_OS == HEFESTO_LINUX || HEFESTO_TGT_OS == HEFESTO_FREEBSD
    HTEST_CHECK("result != /home/rs", strcmp(result, "/home/rs") == 0);
#elif HEFESTO_TGT_OS == HEFESTO_WINDOWS
    HTEST_CHECK("result != /home\\rs", strcmp(result, "/home\\rs") == 0);
#endif
    free(result);

    //  rm
    result = hefesto_sys_call("hefesto.sys.rm(\"test.dat\")",
                              &lo_vars, &gl_vars, NULL, &otype);
    HTEST_CHECK("result == NULL", result != NULL);
    HTEST_CHECK("result != 1", *(int *)result == 0);
    free(result);

    result = hefesto_sys_call("hefesto.sys.rm(\"test_directory/test.dat.bak\")",
                              &lo_vars, &gl_vars, NULL, &otype);
    HTEST_CHECK("result == NULL", result != NULL);
    HTEST_CHECK("result != 1", *(int *)result == 0);
    free(result);

    result = hefesto_sys_call("hefesto.sys.rm("
                              "\"test_directory/src_cp_dir.dat\")",
                              &lo_vars, &gl_vars, NULL, &otype);
    HTEST_CHECK("result == NULL", result != NULL);
    HTEST_CHECK("result != 1", *(int *)result == 0);
    free(result);

    result = hefesto_sys_call("hefesto.sys.rm("
                              "\"test_directory/src_cp_dir.dat.bak\")",
                              &lo_vars, &gl_vars, NULL, &otype);
    HTEST_CHECK("result == NULL", result != NULL);
    HTEST_CHECK("result != 1", *(int *)result == 0);
    free(result);

    result = hefesto_sys_call("hefesto.sys.rm("
                              "\"test_directory/sub_dir/src_cp_dir.dat\")",
                              &lo_vars, &gl_vars, NULL, &otype);
    HTEST_CHECK("result == NULL", result != NULL);
    HTEST_CHECK("result != 1", *(int *)result == 0);
    free(result);

    result = hefesto_sys_call("hefesto.sys.rm("
                              "\"test_directory/sub_dir/src_cp_dir.dat.bak\")",
                              &lo_vars, &gl_vars, NULL, &otype);
    HTEST_CHECK("result == NULL", result != NULL);
    HTEST_CHECK("result != 1", *(int *)result == 0);
    free(result);

    result = hefesto_sys_call("hefesto.sys.rm("
                              "\"src_cp_dir/src_cp_dir.dat\")",
                              &lo_vars, &gl_vars, NULL, &otype);
    HTEST_CHECK("result == NULL", result != NULL);
    HTEST_CHECK("result != 1", *(int *)result == 0);
    free(result);

    result = hefesto_sys_call("hefesto.sys.rm("
                              "\"src_cp_dir/src_cp_dir.dat.bak\")",
                              &lo_vars, &gl_vars, NULL, &otype);
    HTEST_CHECK("result == NULL", result != NULL);
    HTEST_CHECK("result != 1", *(int *)result == 0);
    free(result);

    result = hefesto_sys_call("hefesto.sys.rm("
                              "\"src_cp_dir/sub_dir/src_cp_dir.dat\")",
                              &lo_vars, &gl_vars, NULL, &otype);
    HTEST_CHECK("result == NULL", result != NULL);
    HTEST_CHECK("result != 1", *(int *)result == 0);
    free(result);

    result = hefesto_sys_call("hefesto.sys.rm("
                              "\"src_cp_dir/sub_dir/src_cp_dir.dat.bak\")",
                              &lo_vars, &gl_vars, NULL, &otype);
    HTEST_CHECK("result == NULL", result != NULL);
    HTEST_CHECK("result != 1", *(int *)result == 0);
    free(result);

    //  rmdir
    result = hefesto_sys_call("hefesto.sys.rmdir("
                              "\"src_cp_dir/sub_dir\")",
                              &lo_vars, &gl_vars, NULL, &otype);
    HTEST_CHECK("result == NULL", result != NULL);
    HTEST_CHECK("result != 0", *(int *)result == 0);
    free(result);

    result = hefesto_sys_call("hefesto.sys.rmdir(\""
                              "src_cp_dir/empty_sub_dir\")",
                              &lo_vars, &gl_vars, NULL, &otype);
    HTEST_CHECK("result == NULL", result != NULL);
    HTEST_CHECK("result != 0", *(int *)result == 0);
    free(result);

    result = hefesto_sys_call("hefesto.sys.rmdir(\"src_cp_dir\")",
                              &lo_vars, &gl_vars, NULL, &otype);
    HTEST_CHECK("result == NULL", result != NULL);
    HTEST_CHECK("result != 0", *(int *)result == 0);
    free(result);

    result = hefesto_sys_call("hefesto.sys.rmdir(\""
                              "test_directory/sub_dir\")",
                              &lo_vars, &gl_vars, NULL, &otype);
    HTEST_CHECK("result == NULL", result != NULL);
    HTEST_CHECK("result != 0", *(int *)result == 0);
    free(result);

    result = hefesto_sys_call("hefesto.sys.rmdir(\""
                              "test_directory/empty_sub_dir\")",
                              &lo_vars, &gl_vars, NULL, &otype);
    HTEST_CHECK("result == NULL", result != NULL);
    HTEST_CHECK("result != 0", *(int *)result == 0);
    free(result);

    result = hefesto_sys_call("hefesto.sys.rmdir("
                              "\"test_directory\")",
                              &lo_vars, &gl_vars, NULL, &otype);
    HTEST_CHECK("result == NULL", result != NULL);
    HTEST_CHECK("result != 0", *(int *)result == 0);
    free(result);

    result = hefesto_sys_call("hefesto.sys.mkdir(\"test_regex_cp\")",
                              &lo_vars, &gl_vars, NULL, &otype);
    HTEST_CHECK("result == NULL", result != NULL);
    HTEST_CHECK("result != 0", *(int *)result == 0);
    free(result);

    fp = fopen("test.dat.a", "w");
    fclose(fp);
    fp = fopen("test.dat.b", "w");
    fclose(fp);
    fp = fopen("test.dat.c", "w");
    fclose(fp);
    mkdir("test.dat.d");

    //  regex cp
    result = hefesto_sys_call("hefesto.sys.cp(\"test\\.dat\\.[abd]$\","
                              "\"test_regex_cp\")",
                              &lo_vars, &gl_vars, NULL, &otype);
    HTEST_CHECK("result == NULL", result != NULL);
    HTEST_CHECK("result != 1", *(int *)result == 1);
    free(result);

    result = hefesto_sys_call("hefesto.sys.cd(\"test_regex_cp\")",
                              &lo_vars, &gl_vars, NULL, &otype);
    free(result);

    result = hefesto_sys_call("hefesto.sys.ls(\"test\\.dat\\.a$\")",
                              &lo_vars, &gl_vars, NULL, &otype);
    HTEST_CHECK("result == NULL", result != NULL);
    HTEST_CHECK("result != 1", *(int *)result == 1);
    free(result);

    result = hefesto_sys_call("hefesto.sys.ls(\"test\\.dat\\.b$\")",
                              &lo_vars, &gl_vars, NULL, &otype);
    HTEST_CHECK("result == NULL", result != NULL);
    HTEST_CHECK("result != 1", *(int *)result == 1);
    free(result);

    result = hefesto_sys_call("hefesto.sys.ls(\"test\\.dat\\.d$\")",
                              &lo_vars, &gl_vars, NULL, &otype);
    HTEST_CHECK("result == NULL", result != NULL);
    HTEST_CHECK("result != 1", *(int *)result == 1);
    free(result);

    result = hefesto_sys_call("hefesto.sys.cd(\"..\")",
                              &lo_vars, &gl_vars, NULL, &otype);
    free(result);

    remove("test.dat.a");
    remove("test.dat.b");
    remove("test.dat.c");
    remove("test_regex_cp/test.dat.a");
    remove("test_regex_cp/test.dat.b");
    remove("test_regex_cp/test.dat.c");
    remove("test_regex_cp/test.dat.d");
    rmdir("test_regex_cp");
    rmdir("test.dat.d");

    printf("-- passed.\n");

    return NULL;
}

char *hvm_byref_syscall_test() {

    hefesto_var_list_ctx *lo_vars = NULL, *gl_vars = NULL, *vp;
    hefesto_func_list_ctx *function = NULL;
    hefesto_type_t otype;
    int works = 0;
    char *code = "function byref_test(value type int) : result type none {\n"
                 "\t$value = 1;\n\thefesto.sys.byref($value);\n}\n";
    int errors = 0;
    void *result;
    time_t t;
    struct tm *tmp;
    char out[255];

    create_test_code("byref_test.hls", code);
    function = compile_and_load_hls_code("byref_test.hls",
                                         &errors,
                                         &gl_vars, NULL, NULL);
    if (function == NULL) exit(1);
    remove("byref_test.hls");
    printf("-- hvm_byref_syscall_test\n");
    gl_vars = add_var_to_hefesto_var_list_ctx(gl_vars, "byref_works",
                                              HEFESTO_VAR_TYPE_INT);
    vp = get_hefesto_var_list_ctx_name("byref_works", gl_vars);
    vp = assign_data_to_hefesto_var(vp, &works, sizeof(works));

    result = hvm_call_function("byref_test($byref_works)",
                               &lo_vars, &gl_vars, function);

    HTEST_CHECK("result != NULL", result == NULL);
    vp = get_hefesto_var_list_ctx_name("byref_works", gl_vars);
    HTEST_CHECK("byref_works != 1", *(int *)vp->contents->data == 1);

    t = time(NULL);
    tmp = localtime(&t);
    memset(out, 0, sizeof(out));
    strftime(out, sizeof(out)-1, "%H:%M", tmp);
    result = hefesto_sys_call("hefesto.sys.time(\"%H:%M\")",
                              &lo_vars, &gl_vars, NULL, &otype);
    HTEST_CHECK("result != out", strcmp(out, result) == 0);
    free(result);

    del_hefesto_var_list_ctx(gl_vars);
    del_hefesto_var_list_ctx(lo_vars);
    del_hefesto_func_list_ctx(function);
    printf("-- passed.\n");

    return NULL;
}

char *hvm_syscalls_tests() {

    // XXX: these are not tested syscalls:
    //  hefesto_sys_echo, hefesto_sys_prompt, hefesto_sys_exit,
    //  hefesto_sys_get_option, hefesto_sys_last_forge_result,
    //  hefesto_sys_forge.
    hefesto_type_t otype;
    hefesto_var_list_ctx *lo_vars = NULL, *gl_vars = NULL;
    char *result = NULL;
    void *res;

    printf("-- running hvm_syscalls_tests\n");

    if ((result = hvm_syscalls_filesystem_tests()) == NULL &&
        (result = hvm_byref_syscall_test()) == NULL) {
        //  os_name
        res = hefesto_sys_call("hefesto.sys.os_name()",
                               &lo_vars, &gl_vars, NULL, &otype);
        HTEST_CHECK("result == NULL", res != NULL);
        HTEST_CHECK("result == HEFESTO_SYS_OS_NAME_RETURN",
                    strcmp(res, HEFESTO_SYS_OS_NAME_RETURN) == 0);
        free(res);

        //  env
#if HEFESTO_TGT_OS == HEFESTO_LINUX || HEFESTO_TGT_OS == HEFESTO_FREEBSD
        res = hefesto_sys_call("hefesto.sys.env(\"PWD\")",
                               &lo_vars, &gl_vars, NULL, &otype);
#elif HEFESTO_TGT_OS == HEFESTO_WINDOWS
        res = hefesto_sys_call("hefesto.sys.env(\"PATH\")",
                               &lo_vars, &gl_vars, NULL, &otype);
#endif
        HTEST_CHECK("result == NULL", res != NULL);
        HTEST_CHECK("result == (null)", strcmp(res, "(null)") != 0);
        free(res);

        //  run
        res = hefesto_sys_call("hefesto.sys.run("
                               "\"echo \\\"hefesto.sys.run test passed"
                               " :)\\\"\")", &lo_vars, &gl_vars, NULL, &otype);
        HTEST_CHECK("result == NULL", res != NULL);
        HTEST_CHECK("result != 0", *(int *)res == 0);
        free(res);

        //  async run
        hvm_rqueue_set_queue_size(2);

        lo_vars = add_var_to_hefesto_var_list_ctx(lo_vars,
                                                  "jobs",
                                                  HEFESTO_VAR_TYPE_LIST);
        gl_vars = get_hefesto_var_list_ctx_name("jobs", lo_vars);
        gl_vars->contents = add_data_to_hefesto_common_list_ctx(gl_vars->contents,
                                               "echo \"async run test: hi larry.\"", 34);
        gl_vars->contents = add_data_to_hefesto_common_list_ctx(gl_vars->contents,
                                               "echo \"async run test: hi curly.\"", 34);
        gl_vars->contents = add_data_to_hefesto_common_list_ctx(gl_vars->contents,
                                               "echo \"async run test: hi moe.\"", 32);
        gl_vars = NULL;

        res = hefesto_sys_call("hefesto.sys.run($jobs)",
                               &lo_vars, &gl_vars, NULL, &otype);
        HTEST_CHECK("result == NULL", res != NULL);
        HTEST_CHECK("result != 0", *(int *)res == 0);
        free(res);

        del_hefesto_var_list_ctx(lo_vars);
        lo_vars = NULL;

    }
    if (result == NULL) printf("-- passed.\n");

    return result;

}

char *regex_tests() {

    char user_regex[HEFESTO_MAX_BUFFER_SIZE];
    char text[HEFESTO_MAX_BUFFER_SIZE];
    here_search_program_ctx *search_program;
    here_search_result_ctx *search_result;
    printf("-- regex_tests\n");
    strncpy(user_regex, "^Wild thing", sizeof(user_regex)-1);
    search_program = here_compile(user_regex, NULL);
    HTEST_CHECK("search_program == NULL", search_program != NULL);
    strncpy(text, "Wild thing you make me everything", sizeof(text)-1);
    search_result = here_match_string(text, search_program);
    HTEST_CHECK("\"Wild thing\" not found", here_matches(search_result) == 1);
    del_here_search_result_ctx(search_result);

    text[0] = 'w';
    search_result = here_match_string(text, search_program);
    HTEST_CHECK("\"Wild thing\" found", here_matches(search_result) == 0);
    del_here_search_result_ctx(search_result);
    strncpy(text, " Wild thing you make me everything", sizeof(text)-1);
    search_result = here_match_string(text, search_program);
    HTEST_CHECK("\"Wild thing\" found", here_matches(search_result) == 0);
    del_here_search_result_ctx(search_result);

    del_here_search_program_ctx(search_program);

    strncpy(user_regex, "^Wild thing$", sizeof(user_regex)-1);
    search_program = here_compile(user_regex, NULL);
    HTEST_CHECK("search_program == NULL", search_program != NULL);
    search_result = here_match_string(text, search_program);
    HTEST_CHECK("\"Wild thing\" found", here_matches(search_result) == 0);
    del_here_search_result_ctx(search_result);
    del_here_search_program_ctx(search_program);
    strncpy(user_regex, ".*you make me.*", sizeof(user_regex)-1);
    search_program = here_compile(user_regex, NULL);
    HTEST_CHECK("search_program == NULL", search_program != NULL);
    search_result = here_match_string(text, search_program);
    HTEST_CHECK("\"you make me\" not found", here_matches(search_result) == 1);
    del_here_search_program_ctx(search_program);
    del_here_search_result_ctx(search_result);
    strncpy(text, "Theres a red house over younder", sizeof(text)-1);
    strncpy(user_regex, "Theres a (green|blue) house", sizeof(user_regex)-1);
    search_program = here_compile(user_regex, NULL);
    HTEST_CHECK("search_program == NULL", search_program != NULL);
    search_result = here_match_string(text, search_program);
    HTEST_CHECK("Some pattern found", here_matches(search_result) == 0);
    del_here_search_program_ctx(search_program);
    del_here_search_result_ctx(search_result);
    strncpy(user_regex, "Theres a (red|blue|green) house", sizeof(user_regex)-1);
    search_program = here_compile(user_regex, NULL);
    HTEST_CHECK("search_program == NULL", search_program != NULL);
    search_result = here_match_string(text, search_program);
    HTEST_CHECK("\"Theres a red house\" not found", here_matches(search_result) == 1);
    del_here_search_result_ctx(search_result);
    del_here_search_program_ctx(search_program);
    strncpy(user_regex, "Theres a (blue|green|red) house", sizeof(user_regex)-1);
    search_program = here_compile(user_regex, NULL);
    HTEST_CHECK("search_program == NULL", search_program != NULL);
    search_result = here_match_string(text, search_program);
    HTEST_CHECK("\"Theres a red house\" not found", here_matches(search_result) == 1);
    del_here_search_result_ctx(search_result);
    del_here_search_program_ctx(search_program);
    strncpy(text, "Theres a ReD house over younder", sizeof(text)-1);
    strncpy(user_regex,
            "Theres a ([rR]e[Dd]|blue|green) house", sizeof(user_regex)-1);
    search_program = here_compile(user_regex, NULL);
    HTEST_CHECK("search_program == NULL", search_program != NULL);
    search_result = here_match_string(text, search_program);
    HTEST_CHECK("\"Theres a ReD house\" not found", here_matches(search_result) == 1);
    del_here_search_program_ctx(search_program);
    del_here_search_result_ctx(search_result);
    strncpy(text, "Are you experienced?$", sizeof(text)-1);
    strncpy(user_regex, "[aA]re.*y(O|[AaEeIiuUOo])u experi[Ee]nced\\?.*$",
            sizeof(user_regex)-1);
    search_program = here_compile(user_regex, NULL);
    HTEST_CHECK("search_program == NULL", search_program != NULL);
    search_result = here_match_string(text, search_program);
    HTEST_CHECK("\"Are you experienced?\" not found", here_matches(search_result) == 1);
    del_here_search_result_ctx(search_result);
    del_here_search_program_ctx(search_program);
    printf("-- passed.\n");

    return NULL;
}

char *hls_with_redeclared_var_test() {

    hefesto_func_list_ctx *function = NULL;
    hefesto_var_list_ctx *lo_vars = NULL;
    hefesto_var_list_ctx *gl_vars = NULL;
    char *code = "function foolction(value type string) : result type none {"
                 "\n\tvar foovar type int;\n\tvar foovar type list;\n}\n";
    void *result;
    int errors = 0;
    printf("-- running hls_with_redeclared_var_test\n");
    create_test_code("broken_hls.hls", code);
    function = compile_and_load_hls_code("broken_hls.hls",
                                         &errors, &gl_vars, NULL, NULL);
    remove("broken_hls.hls");
    del_hefesto_func_list_ctx(function);
    HTEST_CHECK("errors != 1", errors == 1);
    printf("-- passed.\n");

    return NULL;
}

char *hls_accessing_var_without_dollar_prefix() {

    hefesto_func_list_ctx *function = NULL;
    hefesto_var_list_ctx *lo_vars = NULL;
    hefesto_var_list_ctx *gl_vars = NULL;
    char *code = "function foolction(value type string) : result type none {"
                 "\n\tvar foovar type int;\n\tfoovar = 1;\n}\n";
    void *result;
    int errors = 0;
    printf("-- running hls_accessing_var_without_dollar_prefix\n");
    create_test_code("broken_hls.hls", code);
    function = compile_and_load_hls_code("broken_hls.hls", &errors, &gl_vars, NULL, NULL);
    remove("broken_hls.hls");
    del_hefesto_func_list_ctx(function);
    HTEST_CHECK("errors != 1", errors == 1);
    printf("-- passed.\n");

    return NULL;
}

char *hls_wrong_function_argument_list_decl() {

    hefesto_func_list_ctx *function = NULL;
    hefesto_var_list_ctx *lo_vars = NULL;
    hefesto_var_list_ctx *gl_vars = NULL;
    char *code = "function foolction(value type String, value type int)"
                 " : result type int {\n\tvar foovar type int;\n"
                 "\t$foovar = 1;\n}\n";
    void *result;
    int errors = 0;
    printf("-- running hls_wrong_function_argument_list_decl\n");
    create_test_code("broken_hls.hls", code);
    function = compile_and_load_hls_code("broken_hls.hls",
                                         &errors, &gl_vars, NULL, NULL);
    remove("broken_hls.hls");
    del_hefesto_func_list_ctx(function);
    del_hefesto_var_list_ctx(gl_vars);
    HTEST_CHECK("errors != 1", errors == 1);

    errors = 0;
    gl_vars = NULL;
    code = "function foolction(again type string, again type int) :"
           "result type int {\n\tvar foovar type int;\n\t$foovar = 1;\n}\n";
    function = NULL;
    create_test_code("broken_hls.hls", code);
    function = compile_and_load_hls_code("broken_hls.hls",
                                         &errors, &gl_vars, NULL, NULL);
    remove("broken_hls.hls");
    del_hefesto_func_list_ctx(function);
    HTEST_CHECK("errors != 1", errors == 1);
    printf("-- passed.\n");

    return NULL;
}

char *hls_unknown_function_result_type() {

    hefesto_func_list_ctx *function = NULL;
    hefesto_var_list_ctx *lo_vars = NULL;
    hefesto_var_list_ctx *gl_vars = NULL;
    char *code = "function foolction(string_arg type string,"
                 "int_arg type int) : result type superunknown {\n"
                 "\tvar foovar type int;\n\t$foovar = 1;\n}\n";
    void *result;
    int errors = 0;
    printf("-- running hls_unknown_function_result_type\n");
    create_test_code("broken_hls.hls", code);
    function = compile_and_load_hls_code("broken_hls.hls",
                                         &errors, &lo_vars, NULL, NULL);
    remove("broken_hls.hls");
    del_hefesto_func_list_ctx(function);
    HTEST_CHECK("errors != 1", errors == 1);
    printf("-- passed.\n");

    return NULL;
}

char *hls_uncommented_comment() {

    hefesto_func_list_ctx *function = NULL;
    hefesto_var_list_ctx *lo_vars = NULL;
    hefesto_var_list_ctx *gl_vars = NULL;
    char *code = "function foolction() : result type none {\n"
                 "\tvar foovar type int;\n\t# this function must breaks the"
                 "compilation just because next line:\n\ta uncommented comment\n"
                 "\t$foovar = 1;\n}\n";
    void *result;
    int errors = 0;
    printf("-- running hls_uncommented_comment\n");
    create_test_code("broken_hls.hls", code);
    function = compile_and_load_hls_code("broken_hls.hls",
                                         &errors, &gl_vars, NULL, NULL);
    remove("broken_hls.hls");
    del_hefesto_func_list_ctx(function);
    HTEST_CHECK("errors != 1", errors == 1);
    printf("-- passed.\n");

    return NULL;
}

char *hls_function_with_undeterminated_code_section() {

    hefesto_func_list_ctx *function = NULL;
    hefesto_var_list_ctx *lo_vars = NULL;
    hefesto_var_list_ctx *gl_vars = NULL;
    char *code = "function foolction(v type int) : result type int {\n"
                 "\t$v = 1;\n\n";
    void *result;
    int errors = 0;
    printf("-- running hls_function_with_undeterminated_code_section\n");
    create_test_code("broken_hls.hls", code);
    function = compile_and_load_hls_code("broken_hls.hls",
                                         &errors, &gl_vars, NULL, NULL);
    remove("broken_hls.hls");
    del_hefesto_func_list_ctx(function);
    HTEST_CHECK("errors != 1", errors == 1);

    function = NULL;
    errors = 0;
    code = "function foolction(v type int) : result type int \n\t$v = 1;\n}\n";
    create_test_code("broken_hls.hls", code);
    function = compile_and_load_hls_code("broken_hls.hls",
                                         &errors, &gl_vars, NULL, NULL);
    remove("broken_hls.hls");
    del_hefesto_func_list_ctx(function);
    HTEST_CHECK("errors != 1", errors == 1);
    printf("-- passed.\n");

    return NULL;

}

char *hls_else_without_if() {

    hefesto_func_list_ctx *function = NULL;
    hefesto_var_list_ctx *lo_vars = NULL;
    hefesto_var_list_ctx *gl_vars = NULL;
    char *code = "function foolction(v type int) : result type int {\n"
                 "\telse\n\t{\n\t$v = 1;\n\t}\n}\n\n";
    void *result;
    int errors = 0;
    printf("-- running hls_else_without_if\n");
    create_test_code("broken_hls.hls", code);
    function = compile_and_load_hls_code("broken_hls.hls",
                                         &errors, &gl_vars, NULL, NULL);
    remove("broken_hls.hls");
    del_hefesto_func_list_ctx(function);
    HTEST_CHECK("errors != 1", errors == 1);
    printf("-- passed.\n");

    return NULL;
}

char *hls_if_else_else() {

    hefesto_func_list_ctx *function = NULL;
    hefesto_var_list_ctx *lo_vars = NULL;
    hefesto_var_list_ctx *gl_vars = NULL;
    char *code = "function foolction(v type int) : result type int {\n"
                 "\tif ($v == 0)\n\t{\n\t$v = 2;\n\t}\n\telse\n\t{\n\t$v = 1;\n"
                 "\t}\n\telse\n\t{\n\thefesto.sys.echo(\"not expected else\\n\");"
                 "\n\t}\n}\n\n";
    void *result;
    int errors = 0;
    printf("-- running hls_if_else_else\n");
    create_test_code("broken_hls.hls", code);
    function = compile_and_load_hls_code("broken_hls.hls",
                                         &errors, &gl_vars, NULL, NULL);
    remove("broken_hls.hls");
    del_hefesto_func_list_ctx(function);
    HTEST_CHECK("errors != 1", errors == 1);
    printf("-- passed.\n");

    return NULL;
}

char *hls_wrong_while_statement() {

    hefesto_func_list_ctx *function = NULL;
    hefesto_var_list_ctx *lo_vars = NULL;
    hefesto_var_list_ctx *gl_vars = NULL;
    char *code = "function foolction(v type int) : result type int {\n"
                 "\t$v = 1;\n\twhile ($v == 1 || v == 2)\n\t{\n"
                 "\t\thefesto.sys.echo(\"Hey Beavis, we got a hang..."
                 "Huh-huh!\");\n\t}\n}\n\n";
    void *result;
    int errors = 0;
    printf("-- running hls_wrong_while_statement\n");
    create_test_code("broken_hls.hls", code);
    function = compile_and_load_hls_code("broken_hls.hls",
                                         &errors, &gl_vars, NULL, NULL);
    remove("broken_hls.hls");
    del_hefesto_func_list_ctx(function);
    HTEST_CHECK("errors != 1", errors == 1);
    printf("-- passed.\n");

    return NULL;
}

char *hls_unterminated_code_lines() {

    hefesto_func_list_ctx *function = NULL;
    hefesto_var_list_ctx *lo_vars = NULL;
    hefesto_var_list_ctx *gl_vars = NULL;
    char *code = "function foolction(v type int) : result type int {\n"
                 "\t$v = 1\n\t}\n\n";
    void *result;
    int errors = 0;
    printf("-- running hls_unterminated_code_lines\n");
    create_test_code("broken_hls.hls", code);
    function = compile_and_load_hls_code("broken_hls.hls",
                                         &errors, &gl_vars, NULL, NULL);
    remove("broken_hls.hls");
    del_hefesto_func_list_ctx(function);
    HTEST_CHECK("errors != 1", errors == 1);

    code = "function foolction(v type int) : result type int {\n"
           "\tif ($v == 0) {\n\t\t$v = 1\n\t}\n}\n\n";

    errors = 0;
    function = NULL;
    create_test_code("broken_hls.hls", code);
    function = compile_and_load_hls_code("broken_hls.hls",
                                         &errors, &gl_vars, NULL, NULL);
    remove("broken_hls.hls");
    del_hefesto_func_list_ctx(function);
    HTEST_CHECK("errors != 1", errors == 1);

    printf("-- passed.\n");

    return NULL;

}

char *hls_compilation_tests() {

    char *result;
    printf("-- running compilation_tests\n");
    result = hls_with_redeclared_var_test();
    if (result != NULL) return result;
    result = hls_accessing_var_without_dollar_prefix();
    if (result != NULL) return result;
    result = hls_wrong_function_argument_list_decl();
    if (result != NULL) return result;
    result = hls_unknown_function_result_type();
    if (result != NULL) return result;
    result = hls_uncommented_comment();
    if (result != NULL) return result;
    result = hls_function_with_undeterminated_code_section();
    if (result != NULL) return result;
    result = hls_else_without_if();
    if (result != NULL) return result;
    result = hls_if_else_else();
    if (result != NULL) return result;
    result = hls_wrong_while_statement();
    if (result != NULL) return result;
    result = hls_unterminated_code_lines();
    if (result != NULL) return result;
    printf("-- passed.\n");

    return NULL;
}

char *dep_chain_tests() {

    FILE *fp;
    char *dep_chain = "foo.bar: bar.foo;bar.foo: foo.baz;";
    char *fullpath, *cwd;
    hefesto_dep_chain_ctx *dep_chain_ctx =
            get_dep_chain_by_user_dep_string(dep_chain), *d;
    hefesto_var_list_ctx *lo_vars = NULL, *gl_vars = NULL;
    void *result;
    printf("-- running dep_chain_tests\n");
    remove(".hefesto-src-chsum-base");
    fp = fopen("foo.bar", "w");
    fclose(fp);
    fp = fopen("bar.foo", "w");
    fclose(fp);
    fp = fopen("foo.baz", "w");
    fclose(fp);

    HEFESTO_CURRENT_DEP_CHAIN = dep_chain_ctx;

    cwd = hefesto_pwd();

    lo_vars = add_var_to_hefesto_var_list_ctx(lo_vars,
                                              "sources", HEFESTO_VAR_TYPE_LIST);
    fullpath = hefesto_make_path(cwd, "foo.bar", HEFESTO_MAX_BUFFER_SIZE);
    lo_vars->contents = add_data_to_hefesto_common_list_ctx(lo_vars->contents,
                                                   fullpath, strlen(fullpath));
    free(fullpath);
    fullpath = hefesto_make_path(cwd, "bar.foo", HEFESTO_MAX_BUFFER_SIZE);
    lo_vars->contents = add_data_to_hefesto_common_list_ctx(lo_vars->contents,
                                                   fullpath, strlen(fullpath));
    free(fullpath);
    hvm_toolset_call_command("hefesto.toolset.base_refresh($sources)",
                             NULL, &lo_vars, &gl_vars, NULL);

    result = hvm_toolset_call_command("hefesto.toolset.file_has_change("
                                      "$sources.item(0))", NULL, &lo_vars,
                                      &gl_vars, NULL);

    HTEST_CHECK("result == NULL", result != NULL);
    HTEST_CHECK("foo.bar has change", *(int *)result == 0);
    free(result);

    result = hvm_toolset_call_command("hefesto.toolset.file_has_change("
                                      "$sources.item(1))",
                                      NULL, &lo_vars, &gl_vars, NULL);

    HTEST_CHECK("result == NULL", result != NULL);
    HTEST_CHECK("bar.foo has change", *(int *)result == 0);
    free(result);

    fp = fopen("foo.baz", "w");
    fprintf(fp, "*\n");
    fclose(fp);

    result = hvm_toolset_call_command("hefesto.toolset.file_has_change("
                                      "$sources.item(1))",
                                      NULL, &lo_vars, &gl_vars, NULL);

    HTEST_CHECK("result == NULL", result != NULL);
    HTEST_CHECK("bar.foo hasn't change", *(int *)result == 1);
    free(result);

    result = hvm_toolset_call_command("hefesto.toolset.file_has_change("
                                      "$sources.item(0))",
                                      NULL, &lo_vars, &gl_vars, NULL);

    HTEST_CHECK("result == NULL", result != NULL);
    HTEST_CHECK("foo.bar has change", *(int *)result == 1);
    free(result);

    fp = fopen("bar.foo", "w");
    fprintf(fp, "**\n");
    fclose(fp);

    result = hvm_toolset_call_command("hefesto.toolset.file_has_change("
                                      "$sources.item(0))",
                                      NULL, &lo_vars, &gl_vars, NULL);

    HTEST_CHECK("result == NULL", result != NULL);
    HTEST_CHECK("foo.bar has change", *(int *)result == 1);
    free(result);

    del_hefesto_var_list_ctx(lo_vars);
    del_hefesto_dep_chain_ctx(dep_chain_ctx);
    free(cwd);
    remove("foo.bar");
    remove("bar.foo");
    remove("foo.baz");
    remove(".hefesto-src-chsum-base");

    printf("-- passed.\n");

    return NULL;
}

char *run_tests() {
    printf("* running tests...\n");
    HTEST_RUN(hefesto_common_stack_ctx_tests);
    HTEST_RUN(hefesto_common_list_ctx_tests);
    HTEST_RUN(hefesto_var_list_ctx_tests);
    HTEST_RUN(hefesto_func_list_ctx_tests);
    HTEST_RUN(options_tests);
    HTEST_RUN(hls_compilation_tests);
    HTEST_RUN(hvm_str_conversion_tests);
    HTEST_RUN(hvm_str_format_tests);
    HTEST_RUN(hvm_function_recurssion_tests);
    HTEST_RUN(hvm_if_tests);
    HTEST_RUN(hvm_while_tests);
    HTEST_RUN(regex_tests);
    HTEST_RUN(hvm_syscalls_tests);
    HTEST_RUN(dep_chain_tests);
    return NULL;
}

int main(int argc, char **argv) {
    char *result = run_tests();
    if (result != NULL) {
        printf("%s [%d test(s) ran]\n", result, htest_runned_tests);
        return 1;
    } else {
        printf("* all tests passed :-) [%d test(s) ran]\n", htest_runned_tests);
    }
    return 0;
}
