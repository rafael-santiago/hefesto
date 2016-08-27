# BNF

Follows the possible productions supported the implemented Language.

```
<hefesto-script> ::= (<include-stmt> | <toolset-decl> | <var-decl> |
                      <func-decl> | <prj-decl> | <comment>)+

<stmt> ::= (<var-decl> | <if-stmt> | <while-stmt> | <comment> |
            (<func-call>   |
             <attrib>      |
             <string-conv> |
             <list-conv>   |
             <sys-call>    |
             <prj-call>) <line-end> (<stmt>)* )

<comment> ::= ("#" (<:US-ASCII:>)* "\n")*

<include-stmt> ::= ("include" <incl-path> "\n")*

<incl-path> ::= "~" (<:UNIX-PATH:>)+ | (<:UNIX-PATH:>)+

<func-decl> ::= ("function" | "local" "function") <func-name>
                 <func-arg-decl> ":" "result" "type" <res-type>
                 <block-begin> <stmt> <block-end>

<func-arg-decl> ::= "(" (<func-arg-decl-item>)* ")"

<func-arg-decl-item> ::= <var-name> ":" <var-type> |
                         ("," <func-arg-decl-item>)*

<var-decl> ::= ("var" | "local" "var") <var-name>
               <var-type> <line-end>

<prj-decl> ::= "project" <proj-name> ":"
               (<toolset-name> | <toolset-name> ":" <dep-chain>) ":"
               <forge-args> <line-end>

<forge-args> ::= <const-arg> | ("," <const-arg>)*

<const-arg> ::= <var-ref> | <string> | <number>

<dep-chain> ::= "dependencies" <expr>

<toolset-name> ::= "toolset" (<var-ref> | <string>)

<if-stmt> ::= ("if" <expr> <stmt> |
               "if" <expr> <block-begin> <stmt> <block-end>)
              (<else-stmt>)*

<else-stmt> ::= "else" (<block-begin> <stmt> <block-end> |
                        <if-stmt>)

<while-stmt> ::= ("while" <expr> <stmt> |
                  "while" <expr> <block-begin>
                          <stmt> <block-end>)

<toolset-decl> ::=  "toolset" <string>
                    "forge" "function" <string> ":"
                    (<forge-helpers-decl>)?
                    <toolset-commands-decl> <toolset-decl-end>

<toolset-decl-end> ::= "$"

<forge-helpers-decl> ::= "forge" "helpers"
                         (<forge-helpers-items>)+ ":"

<forge-helpers-items> ::= <string> | ("," <forge-helpers-items>)*

<toolset-commands-decl> ::= ("command" <string> ":"
                             (<toolset-command-var>)*
                             "<" (<:US-ASCII:>)* ">")*

<toolset-command-var> ::= <var-name> |
                          ("," <toolset-command-var>)*

<attrib> ::= <var-ref> "=" <expr>

<line-end> ::= ";"

<var-name> ::= <:US-ASCII-LETTER:> | <:DIGIT:> |
               "_" | (<varname>)*

<func-name> ::= <:US-ASCII-LETTER:> | <:DIGIT:> |
                "_" | (<funcname>)*

<proj-name> ::= <:US-ASCII-LETTER:> | <:DIGIT:> |
                "_" | "-" | (<projname>)*

<string> ::= """ (<:US-ASCII:>)* """

<number> ::= <numbersig> (<:DIGIT:>)+ | (<:DIGIT:>)+

<number-sig> ::= "+" | "-"

<var-type> ::= "int" | "string" | "file" | "list"

<res-type> ::= "int" | "string" | "list" | "none"

<arg-list> ::= <arg-list-begin> <arg-list-item> <arg-list-end>

<arg-list-item> ::= <var-ref> | <string> | <number> | <expr> |
                    <func-call> | <string-conv> | <list-conv>

<var-ref> ::= "$"<var-name>

<arg-list-begin> ::= "("

<arg-list-end> ::= ")" | ("," <arg-list-item>)+

<operator> ::= "+" | "-"  | "*"  | "/"  | "<"  |
               ">" | "<=" | ">=" | "<<" | ">>" |
               "&" | "|"  | "||" | "&&" | "^"

<operand> ::= <var-ref> | <string> | <number> | <sys-call> |
              <prj-call> | <func-call> | <string-conv> |
              <list-conv> | <nested-op>

<nested-op> ::= "(" (<expr>)+ ")"

<expr> ::= <operand> <operator> <operand> | (<expr>)+

<block-begin> ::= "{"

<block-end> ::= "}"

<sys-call> ::= "hefesto" "." "sys" "." ("replace_in_file"   |
                                        "ls"                |
                                        "pwd"               |
                                        "cd"                |
                                        "rm"                |
                                        "fopen"             |
                                        "fwrite"            |
                                        "fread"             |
                                        "fclose"            |
                                        "cp"                |
                                        "run"               |
                                        "mkdir"             |
                                        "rmdir"             |
                                        "echo"              |
                                        "env"               |
                                        "feof"              |
                                        "prompt"            |
                                        "fseek"             |
                                        "fseek_to_begin"    |
                                        "fseek_to_end"      |
                                        "fsize"             |
                                        "ftell"             |
                                        "exit"              |
                                        "os_name"           |
                                        "get_option"        |
                                        "make_path"         |
                                        "last_forge_result" |
                                        "forge"             |
                                        "byref"             |
                                        "time"              |
                                        "setenv"            |
                                        "unsetenv"          |
                                        "lines_from_file"   |
                                        "call_from_module"  |
                                        "get_func_addr"     |
                                        "call_func_addr") <arg-list>

<func-call> ::= <func-name> <arg-list>

<prj-call> ::= "hefesto"  "." "project" "." ("name"      |
                                             "toolset"   |
                                             "dep_chain" |
                                             "abort"     |
                                             "cmdline"   |
                                             "file_path") <arg-list>

<string-conv> ::= <var-ref> "." ("at"    |
                                 "len"   |
                                 "match" |
                                 "replace") <arg-list>

<list-conv> ::= <var-ref> "." ("item"      |
                               "count"     |
                               "add_item"  |
                               "del_item"  |
                               "ls"        |
                               "clear"     |
                               "index_of"  |
                               "del_index" |
                               "swap") <arg-list>
```
