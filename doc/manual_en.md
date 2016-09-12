# A *build system* called Hefesto

*by Rafael Santiago*

---

**Abstract**: This document brings some information related with this project. My motivations. General details about the ``HSL``. Project decisions. Crazy digressions and ideias that can contradict yours, if you are a sensible soul... :-)

---

## Hefesto who?

``Hefesto`` is a non-orthodox *build system*. The main concept in this application is to make clearer anything that is being performed during *build tasks* on your software project.

## The motivation

*Hefesto* is a *build system* that I wrote seeking answers for some questions that I always made about *build systems*:

- Why *build systems* abuse on the declarative Languages usage?
- Why in general the syntax of these Languages are awful?
- Why a developer can not sit down and programming the *build* instead of writing messy statements?
- Why to do it is necessary to put a bunch of trinket together?
- Is it ugly to look for clarity?
- Why to adopt the snail-philosophy to call any place home?

Maybe during your reading you will figure out that *Hefesto* is not a *build system* but more about what a *build system* in the essence is or at least should be (based on my ideas, of course!).

## Automate or die!

I develop *software* and I like automating things. I think any repetitive task is a torture just because I could be doing other things instead of repeating the same old ones.

Refreshing my last statement: I develop *software* and I like to think about new things all the time and as a result I *need* to automate in order to enjoy this precious time.

I think that the most basic autotation on a software project is its *big-bang*. Its *build*.

We have got countless ways to automate *build*. From *shell scripts*, *arcana batch files* to *Makefiles*, *Jamfiles* and so on.

During the years that I have been programming I used all kinds of those. Personally, I have never been completely assisted by none of them.

Here are some aspects that took me gave up about them and only use these tools when the situation (project) enforces me:

- Rather "symbolic languages"
- Whitespace-like language
- The application not keep all your power (capabilities) when we change the platform
- Sometimes we need to install a bunch of additional things in order to keep the build-tool's capabilities and have the work done
- The documentation is a mess
- A mix of Languages without necessity
- The usage is rather confuse which sometimes can involve two "sub-tools" even more (again: in order to have the work done)
- Visual tools based on XML which is awful when talking about versioning, taking some diffs. The things get worse when the XML is automatically generated
- Need to "adapt" (decrease) your understanding of clearness and quality (expectations) to accept the build-tool's limitations

# How Hefesto works

``Hefesto`` considers that your are a programmer which have a consistent knowledge about the compilers that you are using. If you judge yourself a sniper
but until now only have been using paintball guns... it is better to stop reading this.

This tool is based on automating the build process essence and it means compilation and linking on several times. For this kind of automation
is used the implemented dsl called ``HSL`` what means ``H``efesto ``S``cript ``L``anguage (genious!...).

The most of build systems have a magical file that existing in a specific place of the project tree.. "sheebang!" ... Here in ``Hefesto`` not...
The ``Hefesto`` allows you to call this magical file using the name that you want to. I like to call mine "Forgefiles".

Here is a sample of a Forgefile for a C library:

```
    # Comment-sample: Forgefile.hsl...

    include ~/toolsets/gcc/gcc-lib.hsl

    var sources type list;
    var includes type list;
    var cflags type list;
    var libraries type list;
    var ldflags type list;

    project here : toolset "gcc-c-lib" : $sources, $includes, $cflags, $libraries, $ldflags, "libhere.a";

    here.prologue() {
        $sources.ls(".*\\.c$");
    }
```

In the sample above you can see some HSL statements and it continues inside the "toolset". The toolset can be understood as a set or
collection of HSL code which automates the essence of a build process. The knowledge base of "how to build a library in language X",
"how to build a application in language Y", etc.

Still talking about the sample, after the commentary is done a suggestive inclusion. There we are infusing in that shown Forgefile
a previous knowledge base of how to build a C library using the GCC.

Following this, a thing that several people hates with passion. You need to declare everything (I said everything) that you going
to use. Something like: "Hey guys! I need to use this so excuse me, right?". "Here is the name that I will use to refer to this
memory area slice and I am intending to store this kind of trinket inside, okay?".

I think it is a good thing to do (talking about programming languages design) because this feature jumps from the device
to the programmers' mind so at the moment of the declaration the user will think better about the variable's implications.

I personally dislike "Donnie Darkos" jumping from nowhere along the code's body :)

Backing to the code, these declared variables are used by the forge function (the entry-point to the more internal and generic parts of the build).

After these declarations we have got the project declaration. Saying:

>"This project is called "here" it is an C-library that will be compiled with GCC. Follows the parameters in the
order that the build engine for this kind of thing expects."

The related toolset expects a source code list, a include directory list, a list containing the compiler flags, a library directory list,
a linker flag list and finally a name which will be used for creating the target library.

Some build systems do not provide ways for scanning the codes which will be processed. Some others allow the user provide the source code list from anywhere.

The Hefesto provides one simple but generical way. Sometimes there are several sub-tasks for being executed besides compiling the project. When
a project is created under Hefesto, three entry points are created together with this project: preloading, prologue e epilogue.

Take a look at the **Table 1** for knowing more about these entry points, specially when they are hitted during the process. The basic idea here is:
if exists code defined inside these entry points, on a specific moment the defined code will be executed.

Table 1: The project's stages

| **Project entry-point** |            **Executes**          |
|:-----------------------:|:--------------------------------:|
|     ``preloading``      | Before loading the toolset       |
|     ``prologue``        | Before starting the forge        |
|     ``epilogue``        | After finishing the forge        |

By now let's forget about the ``preloading`` and ``epilogue`` stages. For collecting the source code *file paths* we can use the entry point ``prologue``.

```
    here.prologue() {
        $sources.ls(".*\\.c$");
    }
```

Do not worry about the ``.ls()``... On further sections when describing the ``HSL`` in details the list conveniences will be shown and everything
should make more sense.

Once the ``Forgefile`` written, the forge should be invoked in the command line as follows:

> ``hefesto --forgefiles=Forgefile.hsl --Forgefile-projects=here``

The option --forgefiles specifies a file list which has the defined projects. The another option is dynamic and based on the passed file names by --forgefiles.
The general idea for this dynamic option is:

> --<file-name>-projects=<projects defined inside this file>

Yes, I know:

> ZzZZzzzZzzzZzZZZZZZzzzzzzZzzzZzzzzzZZZZZZZzzzzZZZ the command is pretty longer... Can I use a shorter one?

Yes you can, in this case you need to create an invocation file. This invocation file needs to reside inside where you are intending to invoke the forge. Into
this file you need to type the invocation command line.

Once the .ivk created, being inside its directory, all you should do is call hefesto without any option. If you pass some option, the data that you passed will
be merged with the option into the .ivk file.

Well, now you know the basic steps for composing a Forgefile. However, you do not know the general details about the HSL and how to create more toolsets
for your own necessities.

The toolsets are pieces of HSL code which can have their own conventions, conveniences, etc. Due to it, before using them a good thing to do is read the documentation about.
If you are using some toolset from the official base, probably you can find some text detailing the related toolset.

## How to reprocess only what have changed since the last forge?

For doing it is necessary to inform your worries about reprocessing issues when creating your project. Let's use the previous lib sample.

Previously the project declaration was as follows:

```
    project here : toolset "gcc-c-lib" : $sources, $includes, $cflags, $libraries, $ldflags, "libhere.a";
```

Now we will include the specfification of the dependency chain (a.k.a ``dep-chain``). A ``dep-chain`` basically is a ``string`` expressed in a special format:

>``<file-path>`` **:**  ``<file-path_0>`` [ **&** ``<file-path_n>`` ] **;**

If ``a.x`` depends on ``b.x`` which depends on ``c.x`` which depends on ``d.x`` and ``e.x``. The ``dep-chain`` for this situation would be:

>a.x: b.x;
>
>b.x: c.x;
>
>c.x: d.x & e.x;

Real programs can have a extense ``dep-chain``, the ``Hefesto's`` standard library includes some ``HSL`` functions for doing this job. Follows a practical
usage of the ``dep-chains``:

```
    include ~/toolsets/gcc/gcc-lib.hsl
    include ~/toolsets/common/utils/lang/c/dependency_scanner.hsl

    var deps type string;
    var sources type list;
    var includes type list;
    var cflags type list;
    var libraries type list;
    var ldflags type list;

    project here : toolset "gcc-c-lib" : dependencies $deps : $sources,
                                                              $includes,
                                                              $cflags,
                                                              $libraries,
                                                              $ldflags,
                                                              "libhere.a" ;
    here.prologue() {
        $deps = get_c_cpp_deps();
        $sources.ls(".*\\.c$");
    }
```

As you can see now the code is including the directive ``dependencies <string>`` onto project declaration. Besides the usage of the ``get_c_cpp_deps()``
function included from ``dependency_scanner.hsl``. However, you could write down your own dependency chain without any function. A malformed dependency
chain is silently dropped by the application.

Done! With the dependency chain well-declared the source codes only will be processed when explictly changed or still when other files which those
depend on are changed.

#### How to enforce the re-processing of everything

Use the option ``--forge-anyway`` when invoking the forge.

#### More details about the *dep-chains*

When you adopt the usage of ``dep-chains`` the arguments which you use during the invocation start being watched. Then if you change some argument, everything
will be reprocessed.

A file is considered changed when its content has actually changed.

### Yoda`s proposal: What about at run-time change the toolset, hmm?

Yes young *padawan*,  have no fear... it is possible. For doing it is necessary to take one Jedi's care: the *toolsets* must have the same forge function interface, in other words, these functions must receive the same arguments, following the order and type of them.

Do you remember when the project's ``entry-points`` were shown? Including the ``entry-point`` called ``preloading`` which occurs even before loading the ``toolset``...
Let's use this ``big-bang`` for creating a more suitable *Forgefile*.

Picking up the previous sample from that shown *lib*... Now, suppose that we have the following requirement:

>When on ``Windows`` is necessary to use the *Visual Studio 2012*, being it installed. Otherwise, the *GCC* will be used. Still, under other platforms the *GCC* must be always used.

Now, I will show the changed *Forgefile*. Ahead the interesting parts will be commented:

```
    include ~/toolsets/gcc/gcc-lib.hsl
    include ~/toolsets/vc/vc110-lib.hsl
    include ~/toolsets/common/utils/lang/c/dependency_scanner.hsl

    var deps type string;
    var sources type list;
    var includes type list;
    var cflags type list;
    var libraries type list;
    var ldflags type list;
    var current_toolset type string;

    project here : toolset $current_toolset : dependencies $deps : $sources,
                                                                   $includes,
                                                                   $cflags,
                                                                   $libraries,
                                                                   $ldflags,
                                                                   "libhere.a" ;

    function has_vs_110() : result type int {
        var winreg_rkey type list;
        $winreg_rkey.add_item("HKLM");
        $winreg_rkey.add_item("HKCU");

        var subkeys type list;
        $subkeys.add_item("\\SOFTWARE\\Microsoft\\VisualStudio\\SxS\\VS7\\11.0");
        $subkeys.add_item("\\SOFTWARE\\Wow6432Node\\Microsoft\\VisualStudio\\SxS\\VS7\\11.0");

        var s type int;
        var subkey type string;
        var w type int;
        var root_key type string;
        var install_dir type string;

        $s = 0;
        while ($s < $subkeys.count()) {
            $subkey = $subkeys.item($s);
            $w = 0;
            while ($w < $winreg_rkey.count()) {
                $root_key = $winreg_rkey.item($w);
                $install_dir = hefesto.sys.env("WINREG:" +
                                               $root_key +
                                               $subkey);
                if ($install_dir.len() > 0) result 1;
                $w = $w + 1;
            }
            $s = $s + 1;
        }
        result 0;
    }

    here.preloading() {
        $current_toolset = "gcc-c-lib";
        if (hefesto.sys.os_name() == "windows") {
            if (has_vs_110()) {
                $current_toolset = "vc110-lib";
            }
        }
    }

    here.prologue() {
        $deps = get_c_cpp_deps();
        $sources.ls(".*\\.c$");
    }
```

The function ``has_vs_110`` verifies if the registry entry related with *VS* exists. The section ``About the HSL`` shows more details about the used features.
When it exists the value ``1`` is returned, otherwise ``0``.

Into the ``preloading`` we will call ``has_vs_110`` only being under ``Windows``. Having the ``VS`` well-installed, the *Visual C* toolset is selected, otherwise
we will use the default tooselt which is the *GCC*.

The project declaration brings an indirection for the real toolset name by using the variable ``$current_toolset``. As a result, being the variable content
changed even before trying to load the toolset (we have done it on ``preloading`` phase), we will have a toolset selection at the run-time.

```
    project here : toolset $current_toolset : dependencies $deps : $sources,
                                                                   $includes,
                                                                   $cflags,
                                                                   $libraries,
                                                                   $ldflags,
                                                                   "libhere.a" ;
```

Again, for doing it is necessary to have toolsets which share the same forge function interface.

However, there is a little "problem" with the shown sample. This sample starts with:

```
    include ~/toolsets/gcc/gcc-lib.hsl
    include ~/toolsets/vc/vc110-lib.hsl
```

The forge is multi platform but we do not have to include the *Visual C* toolset every time. Being necessary to include it only when running it under ``Windows`` boxes.
An improvement related with it would be:

		include ~/toolsets/gcc/gcc-lib.hsl
		include on windows ~/toolsets/vc/vc110-lib.hsl

Done!

The directive ``include`` accepts a platform list for effectively include a file. For instance, existing the necessity of including a file only under ``FreeBSD`` and ` `Linux``. You should do:

```
    include on freebsd,linux posix/utils.hsl
```

### How can I run my tests?

The usage of ``batch-files`` or even ``shell-scripts`` in order to run the tests is kind of annoying. Usually people tends to use these type of external scripting
tools for doing other pre/post processing operations. In my opinion, tidiness is related with uniformity. Less mixing into a build project is better. To keep everything expressed with the same language would be a Paradise.

What about to use the ``entry-point epilogue`` for running your tests? You can not only run and break the build if something wrong occurs. Actually, you can also build it besides running.

Follows an example based on the previous *lib* sample:

```
    here.epilogue() {
        var notest type list;
        if (hefesto.sys.last_forge_result() == 0) {
            $notest = hefesto.sys.get_option("no-test");
            if ($notest.count() == 0) {
                run_tests();
            }
        }
    }

    function run_tests() : result type none {
        var retval type int;
        hefesto.sys.cd("test");
        $retval = hefesto.sys.forge("here-test", "Forgefile.hsl", "--bin-output-dir=bin --obj-output-dir=obj");
        hefesto.sys.cd("..");
    }
```

I am sorry if I am rushing a little bit the things here. Basically, on ``epilogue`` is verified if the lib was really ``built``. If it was, the existence of the option ``--no-test``
is checked. If this option was not passed the function ``run_tests`` is called.

The function ``run_tests`` changes the current working directory for the sub-directory ``test``. Into this sub-directory exists another forge. This another
forge is invoked. After its execution, the current working directory backs one level.

Now it is necessary to see the tests' forge:

```
    include ~/toolsets/gcc/gcc-app.hsl

    var sources type list;
    var includes type list;
    var libraries type list;
    var ldflags type list;
    var cflags type list;

    project here-test : toolset "gcc-c-app" : $sources, $includes, $cflags,
                                              $libraries, $ldflags, "here_unittest" ;

    here-test.prologue() {
        $sources.ls(".*\\.c$");
        $ldflags = hefesto.sys.get_option("ldflags");
        $ldflags.add_item("../lib/libhere.a");
        $includes = hefesto.sys.get_option("includes");
        $includes = hefesto.sys.get_option("libraries");
        $includes = hefesto.sys.get_option("cflags");
    }

    function run_unittests() : result type int {
        var retval type int;
        if (hefesto.sys.os_name() != "windows") {
            $retval = hefesto.sys.run("bin/here_unittest");
        } else {
            $retval = hefesto.sys.run("bin\\here_unittest.exe");
        }
        result $retval;
    }

    here-test.epilogue() {
        if (hefesto.sys.last_forge_result() == 0) {
            if (run_unittests() != 0) hefesto.sys.exit(1);
        }
    }
```

The most important function here is the function ``run_unittests``. This function is called from the project's ``epilogue``. Returning non-zero values
the forge will exit with 1. When a forge exit with non-zero value it makes the processing break with a error.

I would say that I found my Paradise in ``Hades``... :) I hope you too. Keep on reading the following sections. They intend to provide deeper details
about Hefesto. These details will be necessary for mastering this build tool.

## About the HSL

It was not my initial intention to create a [DSL](http://en.wikipedia.org/wiki/Domain-specific_language) for the *Hefesto*, the fact is that I have
seen the necessity for doing it when developing the main concepts and ideas discussed here. One of my main intentions was to solve my problems in a
general way, efficient and clean. With no workarounds, several huge and bloated external dependencies, etc. Because all of about it had already been
done and these tools never served me in a elegant and powerful way.

Summarizing, I had more work to do, more code to write, however today I can automate much more besides compiling things.
The best of it: all can be expressed in a single one Language. As a result the build complexities can really decrease. A clean build solution is better
than a Rube Goldberg machine for doing the same job. At least is what my sense takes me to think.

### Why a new *dsl* instead of using a pre-existent one?

When using a General Purpose Language the complexities of this Language, unfortunately, will echo inside the related system. So a internal dsl would not help the users with its excessive conventions.
Still, it would make the discussed application quite dependent from other applications. Finally, it would bloat the solution without any real necessity.

### Defining the basis

First of all, the ``HSL`` should only be used for automating builds. You should not try to implement general purpose software with it.

This Language has 4 primitive types. There is not convention for using *user-defined types*, the **Table 2** lists these types.

**Tabela 2**: Primitive types available on ``HSL``.

| **Reserved word**     |                 **Type**                          |
|:---------------------:|:-------------------------------------------------:|
|     ``int``           | 32/64-bit integer                                 |
|   ``string``          | Char sequence                                     |
|    ``file``           | File descriptor                                   |
|    ``list``           | Container for the types ``int`` or ``string``     |

Fact: variables need to be declared.

From anywhere in a ``HSL`` code you can declare a variable. Following the scheme:

> **var** ``<var-name>`` **type** ``<type-name>`` **;**

The ``<var-name>`` can be composed by ``[A-Za-z0-9_]``. Only one declaration per line are allowed.

Practical examples:

```
    var my_name type string;
    var my_age type int ;
    var my_sloopy_list_decl
    type
    list;
```

To access a declared variable (sorry for the pleonasm), this variable need to be prefixed with the unary ``$``:

```
    $my_name = "John doe.";
    $my_age = -1;
```

Conditional ``branches`` can be expressed in the following way:

> **if** **(** ``<expr>`` **)** **{** ``<hsl-stmt>`` **}** [ **else** **{** ``<hsl-stmt>`` **}** ]

or

> **if** **(** ``<expr>`` **)** ``<hsl-single-stmt>`` [ **else** ``<hsl-single-stmt>`` ]


There is only one way for expressing ``loops``. Through ``while``, see:

> **while** **(** ``<expr>`` **)** **{** ``<hsl-stmt>`` **}**

or

> **while** **(** ``<expr>`` **)** ``<hsl-single-stmt>``

The **Table 3** gathers the available operators to compose expressions in ``HSL``.

**Table 3**: Available operators until now.

|   **Operator**   |     **Operation**  | **Type** |
|:----------------:|:------------------:|:--------:|
|     ``==``       |      Equals to     |  Logical |
|     ``!=``       |   Not equals to    |  Logical |
|      ``<``       |     Less than      |  Logical |
|      ``>``       |     Greater than   |  Logical |
|     ``>=``       |Greater or equals to|  Logical |
|     ``<=``       |Less of equals to   |  Logical |
|     ``&&``       |      .AND.         |  Logical |
|   &#124;&#124;   |      .OR.          |  Logical |
|     ``&``        |     .AND.          |  Bitwise |
|     &#124;       |     .OR.           |  Bitwise |
|     ``<<``       |    ``Left Shift``  |  Bitwise |
|     ``>>``       |    ``Right Shift`` |  Bitwise |

Functions can return any primitive type (excepting the file type) or none. The general form of declaring functions is:

>**function** ``<function-name>`` **(** ``<var-decl-list>`` **)** **: result type ** **{** ``<hsl-stmt>`` **}**

The accepted charset for naming functions are the same accepted for naming variables.

A practical example... A function which recursively evaluates a factorial from a given number:

```
    function factorial(n type int) : result type int {
        if ($n == 0) return 1;
        return factorial($n - 1) * $n;
    }
```

A function which returns none, whose ``return type`` should be ``none``:

```
    function say_hello() : result type none
    {
        hefesto.sys.echo("Hello, creative mind....\n");
    }
```

### The *Hefesto Syscalls*

Well, the last example of a useless function which only says "hello", was not so useless... It was handy for creating one question:

>"WTF is ``hefesto.sys.whatever``??"

Nothing is really random, believe it! A [terrible](http://en.wikipedia.org/wiki/Eastern_mole#/media/File:ScalopusAquaticus.jpg) writer [mole](https://what-if.xkcd.com/4/) has buried
that strange thing there to make you stumble and fall here. When I started to think about the HSL one of my main questions were: which tasks are common during a build task?
Which from these tasks are directly or indirectly requested (relies on...) from the Operating System?

These questions led me to create an abstraction layer which guarantees to write common steps of a *build task* independently from which Operating System should execute them.

The abstraction layer can be accessed by the Hefesto's ``sys sub-system``. In the following way:

>``hefesto.sys.<sys-call>(<arg-list>)``

This ``sub-system`` is called ``syscalls``. Do not be fooled by its name. In fact, you will find much more than classical concepts of *"syscalls"* on it.
You will find things that do much more on a single call and on a higher level than the good and old *UNIX syscalls*.

I picked this name because during my work I have figured out that the majority porting issues of a build task are directly or indirectly related with some real *syscall*.
Then here is explained the source of any future misconception.

In *Table 4* are listed the related ``syscall``.

**Table 4**: ``Hefesto syscalls`` currently implenented.

|          **Syscall**            |         **Utility**                                                                                           |
|:-------------------------------:|--------------------------------------------------------------------------------------------------------------:|
| ``replace_in_file()``           |replaces a found text by a regular expression                                                                  |
| ``lines_from_file()``	          |filters lines which match with the passed regular expression                                                   |
| ``ls()``	                  |returns the count of found files by the passed regular expression                                              |
| ``pwd()``	                  |returns the current working directory (full path)                                                              |
| ``cd()``	                  |jumps to the passed directory path                                                                             |
| ``rm()``	                  |removes a passed file path                                                                                     |
| ``cp()``	                  |copies a file/directory to a passed destination                                                                |
| ``mkdir()``	                  |creates a directory                                                                                            |
| ``rmdir()``	                  |removes a directory                                                                                            |
| ``fopen()``	                  |opens a file returning the related file descriptor (file type)                                                 |
| ``fwrite()``	                  |writes data into the passed file descriptor                                                                    |
| ``fread()``	                  |reads data from the passed file descriptor                                                                     |
| ``fclose()``	                  |cleans all allocated resources related with the passed file descriptor                                         |
| ``feof()``	                  |verifies if it is the end of file                                                                              |
| ``fseek()``	                  |jumps to a specified file's offset                                                                             |
| ``fseek_to_begin()``	          |jumps to the beginning of file                                                                                 |
| ``fseek_to_end()``	          |jumps to the end of file                                                                                       |
| ``fsize()``	                  |returns the file size (in bytes)                                                                               |
| ``ftell()``	                  |returns the current file offset                                                                                |
| ``run()``	                  |runs an external process returning its exit code                                                               |
| ``echo()``	                  |write data into the stdout                                                                                     |
| ``env()``	                  |returns the content of a environment variable                                                                  |
| ``prompt()``	                  |reads data from the keyboard waiting for the confirmation by the *ENTER* pressing                              |
| ``exit()``	                  |aborts the Hefesto virtual machine changing the exit code to the passed value                                  |
| ``os_name()``	                  |returns the current platform name                                                                              |
| ``get_option()``	          |returns the data of a passed command line                                                                      |
| ``make_path()``	          |creates a string taking two halves which corresponds a valid path for the current platform                     |
| ``last_forge_result()``	  |gets the exit code of the last executed forge process                                                          |
| ``forge()``	                  |invokes another forge project                                                                                  |
| ``byref()``	                  |updates the value of a function's argument into the external variable passed as this local argument            |
| ``time()``	                  |returns a string representing the current system Time according to the passed format                           |
| ``setenv()``	                  |creates an environment variable (volatile)                                                                     |
| ``unsetenv()``	          |removes an environment variable (volatile)                                                                     |
| ``call_from_module()``	  |calls a function implemented into a shared library                                                             |
| ``get_func_addr()``	          |returns the address of a passed function name                                                                  |
| ``call_func_addr()``	          |tries to make a function call from the passed address                                                          |

#### A practical guide for each *Hefesto syscall*

In this section you can explore details about how to use each ``syscall``. Seeing more details about the received arguments. Also what they
return. All with short and straightforward samples (not always useful [i.e.: samples in state of art...]).

##### ++replace_in_file()++

Replaces data in a file. The first argument should be the ``file path``, the second is the search pattern (regex), the third is the substitution pattern.
This ``syscall`` returns the number of replacements done.

```
    function replace_in_file_sample() : result type none {
        var replacements_nr type int;
        $replacements_nr = hefesto.sys.replace_in_file("test.txt", "^foo.*", "all initial foo to bar");
        hefesto.sys.echo("Number of replacements: " + $replacements_nr);
    }
```

##### ++lines_from_file()++

Filters lines from a file. The first argument should be the ``file path``, the second is the search pattern (regex). This ``syscall`` returns a list containing
the filtered lines.

```
    function lines_from_file_sample() : result type none {
        var retval type list;
        $retval = hefesto.sys.lines_from_file("test.txt", "^foo.*");
    }
```

##### ++ls()++

Lists files into the current working directory. Receives a argument which is the listing pattern (regex). This ``syscall`` returns the number of found files.

		function ls_sample() : result type none {
 			if (hefesto.sys.ls(".*(c|cpp|s|asm)$") > 0) {
  				hefesto.sys.echo("Hey Beavis, Huh cool files to delete here!! Yeah yeah! Butt-head remove them! remove them!\n");
 			} else {
  				hefesto.sys.echo("ohh, nothing to screw up here.\n");
 			}
		}

##### ++pwd()++

Returns the current work directory as a string. This is hard to use. Look:

```
    function pwd_only_to_PHDs() : result type none {
        hefesto.sys.echo("The current work directory is: " + hefesto.sys.pwd() + "\n");
    }
```

##### ++cd()++

Changes the ``cwd`` to the passed directory. Returning ``1`` for success and ``0`` for failure.

```
    function cd_sample() : result type none {
        hefesto.sys.cd("/home/rs");
    }
```

##### ++rm()++

Removes a file.

```
    function rm_sample() : result type none {
        hefesto.sys.rm("some.lint~");
    }
```

##### ++cp()++

Copies a file/directory. The first argument is the regex pattern which correponds the source. The second argument is the destination. Returns ``1`` for success
and ``0`` for failure.

```
    function cp_sample() : result type none {
        hefesto.sys.cp(".*\\.(exe|dll|msi)$", "wpkg");
    }
```

##### ++mkdir()++

Creates a directory under the ``cwd``.

```
    function mkdir_sample() : result type none {
        hefesto.sys.mkdir("wpkg");
    }
```

##### ++rmdir()++

Removes a empty directory.

```
    function rmdir_sample() : result type none {
        hefesto.sys.rmdir("stage");
    }
```

##### ++fopen()++

Opens a file descriptor which can be manipulated by other ``syscalls``. The first argument is the ``file path``. The second is the openning mode. These openning
mode argument follows the standard ``fopen`` from the ``libc`` ("r" = read-only, "w" = write-only, "a" = ``append`` mode). If the ``syscall`` fails is returned
the value ``0``, otherwise non-zero values will be returned.

```
    function fopen_sample() : result type none {
        var fp type file;
        $fp = hefesto.sys.fopen("test.dat", "a");
    }
```

##### ++fwrite()++

Writes data to a file descriptor previous opened with file writing or appending mode. The first argument is the data ``buffer``. The second is the buffer size.
The third is the file descriptor. This syscall returns the number of written bytes or ``-1`` when some error happens.

```
    function fwrite_sample() : result type none {
        var fp type file;
        $fp = hefesto.sys.fopen("test.dat", "a");
        var buf type string;
        $buf = "that's all folks for fwrite!\n";
        hefesto.sys.fwrite($buf, $buf.len(), $fp);
    }
```

##### ++fread()++

Reads data from a file descriptor (obvious: opened with reading mode). After reading it advances the file descriptor according to the amount of reading bytes.
The first argument is the data ``buffer``. The second argument indicates the amount of data that should be read. The third argument is the file descriptor.
This ``syscall`` returns the number of read bytes or ``-1`` when happening some error.

```
    function fread_sample() : result type none {
        var fp type file;
        $fp = hefesto.sys.fopen("test.dat", "r");
        var byte type string
        hefesto.sys.fread($byte, 1, $fp);
    }
```

##### ++fclose()++

Closes/cleans all internal resources related with the passed file descriptor.

```
    function fclose_sample() : result type none {
        var fp type file;
        $fp = hefesto.sys.fopen("test.dat", "w");
        # TODO: do some manipulation here...
        hefesto.sys.fclose($fp);
    }
```

##### ++feof()++

Returns ``1`` if the end of file was reached, otherwise ``0``. This ``syscall`` receives the file descriptor which should be verified.

```
    function feof_sample() : result type none {
        var fp type file;
        $fp = hefesto.sys.fopen("test.dat", "r");
        if (hefesto.sys.feof($fp) == 1) {
            hefesto.sys.echo("file end.\n");
        } else hefesto.sys.echo("NOT file end.\n");
        hefesto.sys.fclose($fp);
    }
```

##### ++fseek()++

Jumps to a specified file offset (always absolute). The first argument is the file descriptor. The second is the wanted new ``offset``.

```
    function fseek_sample() : result type none {
        var fp type file;
        $fp = hefesto.sys.fopen("test.dat", "r");
        hefesto.sys.fseek($fp, 10);
        hefesto.sys.fclose($fp);
    }
```

##### ++fseek_to_begin()++

Jumps to the beginning of file. Receives a file descriptor.

```
    function fseek_to_begin_sample() : result type none {
        var fp type file;
        $fp = hefesto.sys.fopen("test.dat", "r");
        hefesto.sys.fseek_to_begin($fp); # yes this is a very useless dummy sample... :D
        hefesto.sys.fclose($fp);
    }
```

##### ++fseek_to_end()++

Jumps to the end of file. Receives a file descriptor.

```
    function fseek_to_end_sample() : result type none {
        var fp type file;
        $fp = hefesto.sys.fopen("test.dat", "r");
        hefesto.sys.fseek_to_end($fp);
        hefesto.sys.fclose($fp);
    }
```

##### ++fsize()++

Returns the amount in ``bytes`` of the passed file descriptor.

```
    function fsize_sample() : result type none {
        var fp type file;
        $fp = hefesto.sys.fopen("test.dat", "r");
        hefesto.sys.echo("The file has " + hefesto.sys.fsize($fp) + " byte(s).\n");
        hefesto.sys.fclose($fp);
    }
```

##### ++ftell()++

Returns the current offset of the passed file descriptor.

```
    function ftell_sample() : result type none {
        var fp type file;
        $fp = hefesto.sys.fopen("test.dat", "r");
        hefesto.sys.echo("The current offset should be 0 and is " + hefesto.sys.ftell($fp) + ".\n");
        hefesto.sys.fclose($fp);
    }
```

##### ++run()++

Runs an external process returning its exit code. The ``run`` call is always synchronous.

```
    function sync_run_sample() : result type none {
        hefesto.sys.echo("ls /dev exits with " + hefesto.sys.run("ls /dev") + " exit code.\n");
    }
```

You can also run *n* processes if you pass to this ``syscall`` a list containing command lines for being executed. The number of concurrent process is ruled
by the option ``--qsize=n``. Into this mode, the ``syscall`` returns the sum of the exit codes.

```
    function async_run_sample() : result type none {
        var wqueue type list;
        $wqueue.add_item("ls /dev");
        $wqueue.add_item("ls /home/rs");
        $wqueue.add_item("echo \"duh!\"");
        hefesto.sys.echo("The work queue exit code is " + hefesto.sys.run($wqueue) + ".\n");
    }
```

##### ++echo()++

Prints a ``string`` on the stdout. In fact, this ``Hefesto syscall`` is pretty complicated.

```
    function echo_sample() : result type none {
        hefesto.sys.echo("Hello world.\n");
    }
```

##### ++env()++

Returns the data from a passed environment variable.

```
    function env_sample() : result type none {
        hefesto.sys.echo("The current content of Windows PATH variable is = \"" + hefesto.sys.env("PATH") + "\"\n");
    }
```

##### ++prompt()++

Returns the read data from keyboard.

```
    function prompt_sample() : result type none {
        var usr_data type string;
        $usr_data = hefesto.sys.prompt("Type about an idea and then press enter to confirm this: \n");
        hefesto.sys.echo("Hey I had an idea: " + $usr_data + "\nWhat do you think?");
    }
```

##### ++exit()++

Aborts the ``HVM`` execution exiting with the passed code.

```
    function exit_sample() : result type none {
        hefesto.sys.exit(1);
    }
```

##### ++os_name()++

Returns the name of the current platform where your ``HSL`` script is running. The name is always represented in ``lower-case`` without any information about
version. Under unix-likes it will be the name (in lower-case) returned by the ``uname -s`` (call/command).

```
    function env_sample() : result type none {
        if (hefesto.sys.os_name() == "windows") {
            hefesto.sys.echo("The current content of Windows PATH variable is = \"" + hefesto.sys.env("PATH") + "\"\n");
        }
    }
```

##### ++get_option()++

Returns the argument list of a given command line option. If there is no option related is returned an empty list.

```
    function get_option_sample() : result type none {
        var cups_of_tea type list;
        $cups_of_tea = hefesto.sys.get_option("cups-of-tea"); # --cups-of-tea=brown,black,green,red
        if ($cups_of_tea.size() == 0)
            hefesto.sys.echo("--cups-of-tea option not supplied.\n");
        else
            hefesto.sys.echo("--cups-of-tea option supplied.\n");
    }
```

##### ++make_path()++

Makes an ``string path`` following the current platform conventions for it. The first argument is the ``root path``. The second argument is the *addendum* of
this ``path``. Returns a string representing the right combination of these two halves.

```
    function make_path_sample() : result type none {
        hefesto.sys.echo("STRING PATH must be /home/rs \"" + hefesto.sys.make_path("/home", "/rs") + "\"\n.");
    }
```

##### ++last_forge_result()++

Returns the last exit code related with the last executed forge (this ``result`` can be changed by the ``exit syscall``).

```
    function last_forge_result_sample() : result type none {
        hefesto.sys.echo("The last forge result was " + hefesto.sys.last_forge_result() + ".\n");
    }
```

##### ++forge()++

Invokes another forge project. The first argument is the project contained inside the ``HSL`` informed by the second argument. The third argument are the desired
user option for this forge. This ``syscall`` returns the forge exit code and also changes the ``last_forge_result's`` returned value.

```
    function forge_sample() : result type none {
        hefesto.sys.forge("foolib", "local_deps.hsl", "--user-includes-home=../alt_incs --pre-submit-me");
        if (hefesto.sys.last_forge_result() == 0) {
            hefesto.sys.echo("foolib built with success.\n");
        } else {
            hefesto.sys.echo("unable to build foolib.\n");
            hefesto.sys.exit(1);
        }
    }
```

##### ++byref()++

Updates the data from a local function argument to an external variable which acted as this function argument during a call for the function itself. This
``syscall`` receives the local function argument which should has its data uploaded to the external argument.

```
    function caller() : result type none {
        var return type string;
        $return = "foo";
        hefesto.sys.echo($return);
        byref_sample($return);
        hefesto.sys.echo($return + "\n");
    }

    function byref_sample(io_var type string) : result type none {
        $io_var = "bar";
        hefesto.sys.byref($io_var);
    }
```

##### ++time()++

Returns a ``string`` representing the current system Time according to the passed format. The accepted formats are the same accepted by the function ``strftime()``
from the ``libc`` present on the related Operating System.

```
    function time_sample() : result type none {
        hefesto.sys.echo("Current time: " + hefesto.sys.time("%H:%M") + "\n");
    }
```

##### ++setenv()++

Creates an environment variable. However, it is a volatile operation. At the end of the process this variable is lost.

```
    function setenv_sample() : result type none {
        hefesto.sys.setenv("mytempvar", "mytempval");
    }
```

On ``Windows`` you can use this ``syscall`` for accessing the registry too. It is necessary to inform the full path where the wanted value should
be saved, prefixing this path with ``WINREG:``. In this case, this operation is non-volatile.

```
    function setenv_sample() : result type none {
        hefesto.sys.setenv("WINREG:HKCU\\Software\\Abc\\mytempvar:REG_SZ", "mytempval");
    }
```

##### ++unsetenv()++

Removes a specified environment variable. It is a volatile operation. Only affecting the ``HVM`` process where this ``syscall`` was called.

```
    function unsetenv_sample() : result type none {
        hefesto.sys.unsetenv("VSCOMPILERPATH");
    }
```

On ``Windows`` you can use this ``syscall`` for removing (permanently) registry values too. On the following way:

```
    function unsetenv_sample() : result type none {
        hefesto.sys.unsetenv("WINREG:HKLM\\Software\\Abc\\mytempvar");
    }
```

##### ++call_from_module()++

Calls a function implemented inside a shared library.

The shared function needs to be implemented using a special data structure. More info can be found in the specific documentation about module writing.

```
    function call_from_module_sample() : result type none {
        hefesto.sys.call_from_module("/usr/share/mymods/my_killer_mod.so", "killer_func", "arg1", "arg2", 3);
    }
```

You can also use the path indirection for generalizing the modules locating process.

```
    function call_from_module_sample() : result type none {
        hefesto.sys.call_from_module("~/my_killer_mod.so", "killer_func", "arg1", "arg2", 3);
    }
```

In this case, the library ``"my_killer_mod.so"`` will be searched on the configurated paths (into the ``HEFESTO_MODULES_HOME``) besides the
current working directory.

##### ++get_func_addr()++

Returns the address of a passed function name.

```
    function get_func_addr_sample() : result type int {
        result hefesto.sys.get_func_addr("dummy_clbk");
    }
```

##### ++call_func_addr()++

Tries to call a function from a passed execution address. If something wrong occurs during this process a run-time error is thrown.

```
    function call_func_addr_sample() : result type int {
        var clbk_p type int;
        $clbk_p = hefesto.sys.get_func_addr("abc_one_two_three");
        result hefesto.sys.call_from_addr($clbk_p, "abc", 1, "two", 3);
    }
```

### The *list* and *string* conveniences

Maybe at first glance you would call these conveniences as methods. However, the ``HSL`` is not a class-based Programming Language. Due to it, these "methods"
are only conveniences related with common operations which can be performed over these related types (list, string).

The **Table 5** gathers a listing about the conveniences related with the ``string`` type. The **Table 6** gathers the conveniences related with the ``list``
type.

**Table 5**: The ``string-type`` conveniences.

| **Convenience** | **Handy for**                                                              |**Usage sample**               |
|:---------------:|---------------------------------------------------------------------------:|:-----------------------------:|
|   ``at``        | Returning the current *byte* under the passed index                        | $str.at(0)                    |
|   ``len``       | Returning the *string* length                                              | $str.len()                    |
|   ``match``     | Searching a pattern onto the string data using a passed regular expression | $str.match(".*")              |
|   ``replace``   | Replacing a pattern onto the string data using a passed regular expression | $str.replace(".*", "nothing") |

**Table 6**: The ``list-type`` conveniences.

| **Convenience**  | **Handy for**                                                             | **Usage sample**                        |
|:----------------:|--------------------------------------------------------------------------:|:---------------------------------------:|
| ``item``         | Returning the item under the passed index                                 | $lst.item(0)                            |
| ``count``        | Returning the current items total count                                   | $lst.count()                            |
| ``add_item``     | Adding a new passed item into the related list                            | $lst.add_item("1")                      |
| ``del_item``     | Removing the first occurrence of the passed item                          | $lst.del_item("1")                      |
| ``del_index``    | Removing the item under the passed index                                  | $lst.del_index(1)                       |
| ``ls``           | Loading the file paths which match with the passed regular expression     | $lst.ls(".*\\\\.c$")                    |
| ``clear``        | Cleaning up the list removing all its items                               | $lst.clear()                            |
| ``index_of``     | Returning the index which are equals to the passed item otherwise ``-1``  | $lst.index_of("1")                      |
| ``swap``         | Swapping the position of two passed indexes                               | $lst.swap(0,9)                          |

#### Using samples of these conveniences

Here follows:

```
    function get_filepaths_from_directories(dir type list, regex type string) : result type list {
        var d type int;
        var dirpath type string;
        var oldcwd type string;
        var retval type list;
        $oldcwd = hefesto.sys.pwd();
        $d = 0;
        while ($d < $dir.count()) {
            $dirpath = $dir.item($d);
            if (hefesto.sys.cd($dirpath) == 1) {
                $retval.ls($regex);
            }
            $d = $d + 1;
        }
        hefesto.sys.cd($oldcwd);
        result $retval;
    }
```

How to use:

>``var dir type list;``
>
>``var os_name type string;``
>
>``$os_name = hefesto.sys.os_name();``
>
>``$dir.add_item(".");``
>
>``$dir.add_item("native/" + $os_name);``
>
>``$dir.add_item("native/" + $os_name + "/fs");``
>
>``$dir.add_item("native/" + $os_name + "/net");``
>
>``$dir.add_item("native/" + $os_name + "/mm");``
>
>``$dir.add_item("native/" + $os_name + "/ps");``
>
>``$dir.add_item("common/");``
>
>``$dir.add_item("common/fs");``
>
>``$dir.add_item("common/net");``
>
>``$dir.add_item("common/mm");``
>
>``$dir.add_item("common/ps");``
>
>``var files type list;``
>
>``$files = get_filepaths_from_directories($dir, "*.\\.c$");``

```
    function piglatinize(said type string) : result type string {
        var s type int;
        var etvalray type string;
        var punctoken type list;
        var curr_symbol type string;
        var curr_word type string;

        if ($said.match("[0123456789]") == 1) {
            hefesto.sys.echo("ANICPAY: I can't handle numbers... aieeeeeeee!!\n");
            result "(ullnay)"; # no way: "nil" is not piglatin!!!! :D
        }

        $punctoken.add_item(" ");
        $punctoken.add_item(",");
        $punctoken.add_item(";");
        $punctoken.add_item(".");
        $punctoken.add_item("?");
        $punctoken.add_item("!");
        $s = 0;
        while ($s < $said.len()) {
            $curr_symbol = $said.at($s);
            if (($s + 1) == $said.len()) {
                if ($punctoken.index_of($curr_symbol) == -1) {
                    $curr_word = $curr_word + $curr_symbol;
                    $curr_symbol = ".";
                }
            }
            if ($punctoken.index_of($curr_symbol) > -1) {
                if ($curr_word.len() > 1) {
                    var first_letter type string;
                    var second_letter type string;
                    $first_letter = $curr_word.at(0);
                    $second_letter = $curr_word.at(1);
                    var ss type int;
                    $ss = 2;
                    $etvalray = $etvalray + $second_letter;
                    while ($ss < $curr_word.len()) {
                        $etvalray = $etvalray + $curr_word.at($ss);
                        $ss = $ss + 1;
                    }
                    $etvalray = $etvalray + tolower($first_letter);
                } else {
                    $etvalray = $etvalray + $curr_word;
                }
                if ($curr_word.len() > 0) {
                    $etvalray = $etvalray + "ay";
                }
                $etvalray = $etvalray + $curr_symbol;
                $curr_word = "";
            } else {
                $curr_word = $curr_word + $said.at($s);
            }
            $s = $s + 1;
        }
        result $etvalray;
    }

    function tolower(l type string) : result type string {
        var letters type list;
        # It is not much smart but for sample issues is ok ;)
        $letters.add_item("A");
        $letters.add_item("B");
        $letters.add_item("C");
        $letters.add_item("D");
        $letters.add_item("E");
        $letters.add_item("F");
        $letters.add_item("G");
        $letters.add_item("H");
        $letters.add_item("I");
        $letters.add_item("K");
        $letters.add_item("K");
        $letters.add_item("L");
        $letters.add_item("M");
        $letters.add_item("N");
        $letters.add_item("O");
        $letters.add_item("P");
        $letters.add_item("Q");
        $letters.add_item("R");
        $letters.add_item("S");
        $letters.add_item("T");
        $letters.add_item("U");
        $letters.add_item("V");
        $letters.add_item("W");
        $letters.add_item("X");
        $letters.add_item("Y");
        $letters.add_item("Z");
        $letters.add_item("a");
        $letters.add_item("b");
        $letters.add_item("c");
        $letters.add_item("d");
        $letters.add_item("e");
        $letters.add_item("f");
        $letters.add_item("g");
        $letters.add_item("h");
        $letters.add_item("i");
        $letters.add_item("j");
        $letters.add_item("k");
        $letters.add_item("l");
        $letters.add_item("m");
        $letters.add_item("n");
        $letters.add_item("o");
        $letters.add_item("p");
        $letters.add_item("q");
        $letters.add_item("r");
        $letters.add_item("s");
        $letters.add_item("t");
        $letters.add_item("u");
        $letters.add_item("v");
        $letters.add_item("w");
        $letters.add_item("x");
        $letters.add_item("y");
        $letters.add_item("z");
        var l_index type int;
        $l_index =  $letters.index_of($l);
        if ($l_index > -1 && $l_index < 26) {
            var lower_letter type string;
            $lower_letter = $letters.item($l_index + 26);
            result $lower_letter;
        }
        result $l;
    }
```

It should be used on following way:

>``hefesto.sys.echo(piglatinize("I like to speak pig latin.") + "\n");``

### The *project* sub-system

This sub-system gathers relevant data about the current executed forge project. It can be accessed any time from everywhere into the forge.

The functions shown by the **Table 7** can be accessed in the following general form:

>``hefesto.project.<function-name>(<args>)``

**Table 7**: Implemented functions from the ``project`` sub-system until now.

|    **Function**  |                 **Handy for**                                                  |    **Calling sample**           |
|:----------------:|-------------------------------------------------------------------------------:|--------------------------------:|
|  ``name()``      | Returning the name of the current executed project                             | ``hefesto.project.name()``      |
|  ``toolset()``   | Returning the name of the picked toolset                                       | ``hefesto.project.toolset()``   |
|  ``dep_chain()`` | Returning the content of the used ``dep-chain``                                | ``hefesto.project.dep_chain()`` |
|   ``abort()``    | Aborting the executed project                                                  | ``hefesto.project.abort(1)``    |
|  ``cmdline()``   | Returning the command line which the forge is based on                         | ``hefesto.project.cmdline()``   |
|  ``file_path()`` | Returning the ``file path`` of the file where the executed project was defined | ``hefesto.project.file_path()`` |

### Is there an easy way of running these samples for playing with the Language?

Yes, sure! When you install ``Hefesto`` together are installed a [stub](https://github.com/rafael-santiago/helios/blob/master/src/include/doc/toolsets/null/README.md) ``toolset``.
When basing a project on this stub toolset you can call anything from inside the ``project entry points``.

### Where can I continue exploring the *HSL*?

The ``HSL`` has its on repository gathering useful stuff. It is called [Helios](https://github.com/rafael-santiago/helios.git). When you clone the ``Hesfesto's`` repo
the ``Helios`` is also cloned together (as a git-submodule). By the way, a very basic piece from ``Helios`` is installed on. By default, ``Helios`` includes documentation about its modules.
Take a look at this documentation, it is a good thing to do before using any additional ``Hefesto`` stuff.

You can also understand ``Helios`` as a ``Hefesto`` package manager or still as the ``Hefesto include`` directory.

There is a code collection inside ``Helios`` called [hc-functions](https://github.com/rafael-santiago/helios/blob/master/src/include/doc/hc/README.md).
Using the ``hc-functions`` is possible to perform several checkings over a current build environment. It is good for using when you want to check if the
environment really can build your software. It can verify some software exportation, library version, etc. All is done using straightforward ``HSL`` functions
which can be re-used, improved, extended, etc.

### Creating *Hefesto toolsets* and completely mastering the discussed *build system*

If you have read the prior sections and tried to run the samples, there is a last step for becoming an "advanced Hefesto user". Until now you know how to
automate pre/post steps on your *build-task* besides using the "from-factory-toolsets". Then you still are a final user being dependant from another
[``toolset devel``](http://en.wikipedia.org/wiki/Tasmanian_Devil_(Looney_Tunes)).

For this reason, I invite you to learn how to implement a ``Hefesto toolset`` from scrath.

### Before starting: a short overview about everything already seen

Things that you already know:

- The ``Hefesto`` is a *build system* that seeks tidiness during the build-task writing.
- It implements a *dsl* that is used on the extensions (single user requirements) besides on the general requirements for building something (toolsets).
- The ``HSL`` portion related with the user projects declaration have shown.
- Also were shown the ways how to run a *build* (forge).
- You have seen how to watch for changes onto files which compose your code base.
- An overview (user context) about ``dep-chains`` was presented.
- Also was shown how to include additinal steps into a build task, an example about running unit tests, after evaluating its result was detailed.
- The possibility of creating a more adaptable *Forgefile* such the toolset selection occurs at run-time was shown.
- General aspects about the ``HSL`` and its sub-system ``sys`` was detailed.

What you still do not know but after reading the following sections will can master:

- How to declare a ``toolset``.
- The ``toolset`` sub-system.
- How does Hefesto (internally) detects that a file was changed.
- The ``project`` sub-system.
- Good practices for creating and maintaining new ``toolsets``.

#### Our goal: a *GCC* toolset for compiling and generating applications written in *C*

I picked the *C* Language because it is a "compiled based language". It involves dependencies that must be resolved externally from the compiler. Nowadays,
the new Languages brings a bunch of "candy-plushy-wheels" for compiling our codes. The *C* Language makes you thing more about what really is to compile a
code. So, we will use it for your "fire baptism". After this, any survivor will rush to automate a bunch of tasks, now pretty easy when compared with this
"complicated" C toolset. Are you ready, dear (who knows) suvivor? Bitter medicine is better to drink fast... So, let's go!

#### Figuring out who depends on who

Yes, a good notion about *C* would be nice here. Even newbie. For this section I am taking in consideration C Newbie readers, okay? Do not be offended, your damn *C* guru.

Supposing that we have the following *C* ``main`` function:

```
    #include "init.h"
    #include "config.h"
    #include "options.h"
    #include <stdio.h>

    int main(int argc, char **argv) {
        if (argc > 1) {
            get_options(argv, argc);
        }
        if (do_config()) {
            printf("*** Started!\n");
            return init();
        }
        printf("*** Error.\n");
        return 1;
    }
```

In *C* a include is a little clue telling us about "the doomed mystery of the forgotten dependencies". Usually, inclusions done using double quotes are about local
files (a.k.a. dependencies), opposing to ``< ... >`` which are about external dependencies.

From now on, we will compose a function that will figure out and spit for us a ``dep-chain`` from any *C* project written using these standard ``including`` "modus-operandi".

What is the best way for serializing into lines a file content under the ``HSL``? I vote for the syscall ``lines_from_file()``. I will show the entire code
in order to comment the interesting parts later:

```
    include ~/fsutil.hsl

    function get_c_cpp_deps() : result type string {
        var files type list;
        var includes type list;
        var f type int;
        var i type int;
        var dep_chain type string;
        var str type string;
        var cwd type string;
        var exts type list;
        var e type int;
        var dep_nr type int;
        var related_files type list;

        $e = 0;

        $exts.add_item("h");
        $exts.add_item("hpp");
        $exts.add_item("c");
        $exts.add_item("C");
        $exts.add_item("CC");
        $exts.add_item("cc");
        $exts.add_item("cpp");
        $exts.add_item("CPP");

        while ($e < $exts.count()) {
            $str = ".*\\." + $exts.item($e) + "$";
            $files.ls($str);
            $f = 0;
            $cwd = hefesto.sys.pwd();
            while ($f < $files.count()) {
                $str = $files.item($f);
                $dep_chain = $dep_chain + $str + ": ";
                $includes = hefesto.sys.lines_from_file($str, "#include.*$");
                $i = 0;
                $dep_nr = 0;
                while ($i < $includes.count()) {
                    $str = $includes.item($i);
                    $str.replace(".*#include.*(\"|<)", "");
                    $str.replace("(\"|>)$", "");
                    if (isfile($str) == 1) {
                        $str = hefesto.sys.make_path($cwd, $str);
                        if ($dep_nr > 0) {
                            $dep_chain = $dep_chain + "& ";
                        }
                        $dep_chain = $dep_chain + $str + " ";
                        $dep_nr = $dep_nr + 1;
                    }
                    $i = $i + 1;
                }
                if ($exts.item($e) == "h" || $exts.item($e) == "hpp") {
                    $str = $files.item($f);
                    $str = filenamefrompath($str);
                    $str.replace("\\.(h|hpp)$", "");
                    $related_files.clear();
                    $related_files.ls(".*" + filenamefrompath($str) + "\\.(c|C|CC|cc|cpp|CPP)$");
                    if ($related_files.count() > 0) {
                        $i = 0;
                        while ($i < $related_files.count()) {
                            if ($dep_nr > 0) {
                                $dep_chain = $dep_chain + "& ";
                            }
                            $dep_chain = $dep_chain + $related_files.item($i) + " ";
                            $dep_nr = $dep_nr + 1;
                            $i = $i + 1;
                        }
                    }
                }
                $dep_chain = $dep_chain + ";\n";
                $f = $f + 1;
            }
            $files.clear();
            $e = $e + 1;
        }
        result $dep_chain;
    }
```

Yes, *parser* is a hard thing to code. Even silly at first glance, it can become a nightmare. All because it is about supporting lovely Human features: confusion,
untidiness, lack of pattern.

The lines:

```
    $exts.add_item("h");
    $exts.add_item("hpp");
    $exts.add_item("c");
    $exts.add_item("C");
    $exts.add_item("CC");
    $exts.add_item("cc");
    $exts.add_item("cpp");
    $exts.add_item("CPP");
```

Are the lines responsible for defining the relevant extensions for being scanned. Also is possible to eliminate the case issue, if you prefer doing it, feel free about.

After the lines shown above, each registered extension is used for composing a regular expression:

```
    $str = ".*\\." + $exts.item($e) + "$";
```

Beyond the iterations it could be:

> ".*\\\\.h$"...
> > ".*\\\\.hpp$"...
> > > ".*\\\\.c$"...
> > > > ".*\\\\.C$"...

Thus, we will find the relevant files. Each found file will have registered into the ``dep-chain`` (following the previous syntax presented, do you remember?).
On this way, Hefesto will be able to watch for this file:

```
    $str = $files.item($f);
    $dep_chain = $dep_chain + $str + ": ";
```

After we look inside this found file, search for more dependencies. For doing it we use a pretty simple but handy regular expression:

```
    $includes = hefesto.sys.lines_from_file($str, "#include.*$");
```

Okay, "Why did not you use a anchor in this regex?". Because, the funny & lovely Human features, like:

>      #includes "here.h"
>               #include "there.h"
>        /*let's include over yonder too*/ #include "over_yonder.h"

From each found line is extracted the ``file name`` and made a ``full path`` based on the [``cwd``](http://en.wikipedia.org/wiki/Working_directory). If the
current file is really accessible under the ``cwd`` it is included as a dependency for the previous found file.

```
    while ($i < $includes.count()) {
        $str = $includes.item($i);
        $str.replace(".*#include.*(\"|<)", "");
        $str.replace("(\"|>)$", "");
        if (isfile($str) == 1) {
            $str = hefesto.sys.make_path($cwd, $str);
            if ($dep_nr > 0) {
                $dep_chain = $dep_chain + "& ";
            }
            $dep_chain = $dep_chain + $str + " ";
            $dep_nr = $dep_nr + 1;
        }
        $i = $i + 1;
    }
```

For each found ``header`` is applied a secondary search. This search is based on their implementation files: same name excepting the extension. When accessible
under the ``cwd`` an implementation file is also taken as a dependency and added to the ``dep-chain``:

```
    if ($exts.item($e) == "h" || $exts.item($e) == "hpp") {
        $str = $files.item($f);
        $str = filenamefrompath($str);
        $str.replace("\\.(h|hpp)$", "");
        $related_files.clear();
        $related_files.ls(".*" + filenamefrompath($str) + "\\.(c|C|CC|cc|cpp|CPP)$");
        if ($related_files.count() > 0) {
            $i = 0;
            while ($i < $related_files.count()) {
                if ($dep_nr > 0) {
                    $dep_chain = $dep_chain + "& ";
                }
                $dep_chain = $dep_chain + $related_files.item($i) + " ";
                $dep_nr = $dep_nr + 1;
                $i = $i + 1;
            }
        }
    }
```

After it, the current file will have all direct dependency scanned into the ``dep-chain``. Being necessary only terminate the stream of dependencies for the
current found file:

```
    $dep_chain = $dep_chain + ";\n";
```

All that was described is repeated for each relevant file laying on the ``cwd``.  You can change the ``cwd`` before calling the ``c dependency-scanner``.
For doing it you should use the ``cd syscall``.

After processing all files the ``dep-chain`` is returned to the caller:

```
    result $dep_chain;
```

This content will be used onto the section ``dependencies`` of a project declaration.

##### Creating a toolset

A ``toolset`` needs to be declared. On declaring it, we not only can define the name of it, but also we can generalize anything that need to be done using
external tools.

What does a *C* toolset need to do using external tools? (basically)

- To compile codes.
- To link the produced object files.

Follows the related toolset:

```
    include ~/toolsets/gcc/forges/gcc_c_app_forge.hsl

    toolset "gcc-c-app" forge function "gcc_c_binary_forge" :
        forge helpers "gcc_compile_source_list",
                      "gcc_link_ofiles", "gcc_mk_app_compilation_command":
        command "compile_r": SOURCE, OUTFILE, INCLUDES, CFLAGS <
            gcc -c $SOURCE $INCLUDES $CFLAGS -o $OUTFILE
        >
        command "compile_d": SOURCE, OUTFILE, INCLUDES, CFLAGS <
            gcc -g -c $SOURCE $INCLUDES $CFLAGS -o $OUTFILE
        >
        command "link_shared": APPNAME, OBJECTS, LIBRARIES, LDFLAGS <
            gcc -o$APPNAME $OBJECTS $LIBRARIES $LDFLAGS
        >
        command "link_static": APPNAME, OBJECTS, LIBRARIES, LDFLAGS <
            gcc -static -o$APPNAME $OBJECTS $LIBRARIES $LDFLAGS
        >
    $
```

The basic syntax is:

>``toolset <string-name> forge function <hsl-function-name> : [forge helpers <helpers-list>] : <command-templates> $``

In the shown example, the ``toolset`` is called: ``gcc-c-app``. Its forge function is: ``gcc_c_binary_forge``.
This explains the ``include`` directive, the file where this function follows defined. The forge helpers are: ``gcc_compile_list``,
``gcc_link_ofiles`` and ``gcc_mk_compilation_command``. The toolset definition always ends with the "tape ending" symbol (``$``).

The implemented command templates are: ``compile_r``, ``compile_d``, ``link_shared`` and ``link_static``. A template command is nothing more than
an abstraction of an external command which your ``toolset`` will assemble. The syntax is in form:

>``command <string-name> : <string-argument-list> < <command-line template> >``

A command toolset **always** receives strings as arguments and also returns a single string based on the previous defined template.

There is a strict rule about the accesibility of these ``toolset`` commands:

>Only can be accessed by the forge function and its forge helpers.

For accessing this commands you should use the ``toolset sub-system``:

>(...)
>``$cmd = hefesto.toolset.compile_r($source_path, $outfile_path, $includes, $cflags);``
>(...)

A good approach before starting creating any toolset is to ask yourself about what are the external commands relevant to be executed in
order to get all work done. This is the genesis of any toolset in my opinion.

##### The forge function

Maybe this is the hardest part to figure out. The forge function is the place where the main *build* logics must be coded. Taking in consideration that
the forge function steps must be generalized on a way that must be useful for any ``toolset`` user.

Follows the forge function. After its code listing I will comment the relevant parts of it:

```
    function gcc_c_binary_forge(SOURCES type list,
                                INCLUDES type list,
                                CFLAGS type list,
                                LIBRARIES type list,
                                LDFLAGS type list,
                                APPNAME type string) : result type int {
        var exit_code type int;
        var includes type string;
        var cflags type string;
        var libraries type string;
        var ldflags type string;
        var objects type string;

        if ($SOURCES.count() == 0) {
            if (hefesto.project.toolset() == "gcc-c-app") {
                $SOURCES.ls(".*\\.c$");
            } else {
                $SOURCES.ls(".*\\.(cc|cpp|CC|Cpp|CPP)$");
            }
        }

        if ($SOURCES.count() == 0) {
            hefesto.sys.echo("\t(empty source list)\n");
            result 1;
        }

        var obj_output_dir type list;
        var obj_dir type string;

        $obj_output_dir = hefesto.sys.get_option("obj-output-dir");
        if ($obj_output_dir.count() > 0) {
            $obj_dir = $obj_output_dir.item(0);
            mktree($obj_dir);
        } else {
            $obj_dir = hefesto.sys.pwd();
        }

        var bin_output_dir type list;
        var bin_output type string;

        $bin_output_dir = hefesto.sys.get_option("bin-output-dir");
        if ($bin_output_dir.count() > 0) {
            $bin_output = $bin_output_dir.item(0);
            mktree($bin_output);
            $bin_output = hefesto.sys.make_path($bin_output, $APPNAME);
        } else {
            $bin_output = $APPNAME;
        }

        $exit_code = 0;

        #  GCC's include list option string
        $includes = gcc_mk_gcc_incl_str_opt($INCLUDES); 

        $cflags = gcc_mk_raw_str_opt($CFLAGS); #  GCC's compile options

        var cpu_arch type list;
        var chosen_arch type string;
        $cpu_arch = hefesto.sys.get_option("cpu-arch");
        $chosen_arch = "";
        if ($cpu_arch.count() > 0) {
            $chosen_arch = $cpu_arch.item(0);
            if ($chosen_arch != "32" && $chosen_arch != "64") {
                hefesto.sys.echo(hefesto.project.toolset() +
                                 " fatal error: invalid value for cpu-arch " +
                                 "option must be '32' or '64'.\n");
                result 1;
            }
            $cflags = $cflags + " -m" + $chosen_arch;
        }

        #  compiling...
        hefesto.sys.echo("*** compiling...\n");

        $exit_code = gcc_compile_source_list($SOURCES, $includes, $cflags, $obj_dir); 
        if ($gcc_c_project_forge_dirty_files_nr == 0 && isfile($bin_output) == 0) {
            $gcc_c_project_forge_dirty_files_nr = 1;
        }

        if ($gcc_c_project_forge_dirty_files_nr > 0 && $exit_code == 0) {
            $objects = gcc_mk_ofiles_str_opt($SOURCES, $obj_dir);
            $ldflags = gcc_mk_raw_str_opt($LDFLAGS);
            if ($chosen_arch.len() > 0) {
                $ldflags = $ldflags + "-m" + $chosen_arch;
            }
            $libraries = gcc_mk_gcc_lib_str_opt($LIBRARIES);
            #  linking...
            hefesto.sys.echo("*** now linking...\n\t*** waiting...\n");
            $exit_code = gcc_link_ofiles($objects,
                                         $libraries
                                         $ldflags,
                                         $bin_output);
        }

        if ($exit_code == 0) {
            hefesto.sys.echo("*** success.\n");
        } else {
            hefesto.sys.echo("*** failure.\n");
        }

        result $exit_code;
    }
```

The forge function has the following interface:

	function gcc_c_binary_forge(SOURCES type list,
    	  			    INCLUDES type list, CFLAGS type list,
                                    LIBRARIES type list,
                                    LDFLAGS type list,
                                    APPNAME type string) : result type int


This interface explains the sequence and types of the passed arguments during a project declaration which uses the discussed ``toolset``.
It was declared as:

```
    var sources type list;
    var includes type list;
    var cflags type list;
    var libraries type list;
    var ldflags type list;

    project sample : toolset "gcc-c-app" : $sources, $includes, $cflags,
                                           $libraries, $ldflags, "sample" ;
```

At the beginning of the presented forge function code, we can see a convenience:

```
    if ($SOURCES.count() == 0) {
        if (hefesto.project.toolset() == "gcc-c-app") {
            $SOURCES.ls(".*\\.c$");
        } else {
            $SOURCES.ls(".*\\.(cc|cpp|CC|Cpp|CPP)$");
        }
    }
```

If an empty source code list is passed, the forge function will try to scan for code files into the current working directory. This toolset is used for ``C/C++`` code
compiling, it explains the if clause based on the current chosen toolset name.

After this attempt, being ``SOURCES`` still empty:

```
    if ($SOURCES.count() == 0) {
        hefesto.sys.echo("\t(empty source list)\n");
        result 1;
    }
```

We finish here, informing the user about the impossibility of compiling something under this directory. The value ``1`` is returned to make the ``Hefesto``
"break" the forge process with an error. In this case, it will catch the user's attention about the related fact.

We have more conveniences... Sometimes we do not want to polute the project's directory with files produced by the build process (object files, etc):

```
    var obj_output_dir type list;
    var obj_dir type string;

    $obj_output_dir = hefesto.sys.get_option("obj-output-dir");
    if ($obj_output_dir.count() > 0) {
        $obj_dir = $obj_output_dir.item(0);
        mktree($obj_dir);
    } else {
        $obj_dir = hefesto.sys.pwd();
    }
```

If the user has passed the option ``--obj-output-dir=<dirpath>`` we will create and use the requested directory for putting object files there,
otherwise we will use the ``cwd`` as our object directory. The ``syscall get_option`` always returns a list. When something is passed into a requested
option, the list returned should contain at least one argument. The function ``mktree()`` is a function defined into ``fsutil.hsl``, this function
creates a directory tree if it not exists.

A similar thing is done with the binary directory, take a look:

```
    $bin_output_dir = hefesto.sys.get_option("bin-output-dir");
    if ($bin_output_dir.count() > 0) {
        $bin_output = $bin_output_dir.item(0);
        mktree($bin_output);
        $bin_output = hefesto.sys.make_path($bin_output, $APPNAME);
    } else {
        $bin_output = $APPNAME;
    }
```

Now it is the time to convert the compiler's options expressed as lists into *strings*:

```
    #  GCC's include list option string
    $includes = gcc_mk_gcc_incl_str_opt($INCLUDES);
    $cflags = gcc_mk_raw_str_opt($CFLAGS); #  GCC's compile options
```

The referenced functions above, will be further detailed. By now, you can know them superficially without any problem.

It is a good convenience the possibility of choosing the *target's* architecture. In this case, the option read by the code presented right below, allows
the choice of a *32-bit* or *64-bit* binary:

```
    var cpu_arch type list;
    var chosen_arch type string;
    $cpu_arch = hefesto.sys.get_option("cpu-arch");
    $chosen_arch = "";
    if ($cpu_arch.count() > 0) {
        $chosen_arch = $cpu_arch.item(0);
        if ($chosen_arch != "32" && $chosen_arch != "64") {
            hefesto.sys.echo(hefesto.project.toolset() +
                             " fatal error: invalid value for cpu-arch " +
                             "option must be '32' or '64'.\n");
            result 1;
        }
    }
```

When the user passes ``--cpu-arch=32`` or ``--cpu-arch=64`` will be added ``-m32`` or ``-m64`` to the compilation command. Later, this command will be directly
passed to the compiler:

```
    $cflags = $cflags + " -m" + $chosen_arch;
```

The ``-m`` option specifies for *GCC* the target's architecture. Still about the last code listing shown, if an architecture different of ``32`` and ``64`` is passed
it results in a process abortion, returning 1 besides a error message on the ``stdout``.

Now the function that actually compiles each scanned code takes the control:

```
    #  compiling...
    hefesto.sys.echo("*** compiling...\n");

    $exit_code = gcc_compile_source_list($SOURCES, $includes, $cflags, $obj_dir); 
```

The function ``gcc_compile_source_list()`` will be further detailed.

Into this toolset code exists a global variable called ``gcc_c_project_forge_dirty_files_nr``. This variable computes the total of codes effectively processed.
If after the compilation process none of the codes were processed, the existence of the target file is verified. Not existing, the counter variable is set to
``1``:

```
    if ($gcc_c_project_forge_dirty_files_nr == 0 && isfile($bin_output) == 0) {
        $gcc_c_project_forge_dirty_files_nr = 1;
    }
```

This set will guarantee the *linking* execution. Even with any code reprocessed but without the wanted target binary, the *linking* must happen:

```
    if ($gcc_c_project_forge_dirty_files_nr > 0 && $exit_code == 0) {
        $objects = gcc_mk_ofiles_str_opt($SOURCES, $obj_dir);
        $ldflags = gcc_mk_raw_str_opt($LDFLAGS);
        if ($chosen_arch.len() > 0) {
            $ldflags = $ldflags + "-m" + $chosen_arch;
        }
        $libraries = gcc_mk_gcc_lib_str_opt($LIBRARIES);
        #  linking...
        hefesto.sys.echo("*** now linking...\n\t*** waiting...\n");
        $exit_code = gcc_link_ofiles($objects,
                                     $libraries
                                     $ldflags,
                                     $bin_output);
    }
```

Of course that besides the total of processed files the *linking* must happen only when has no errors during the compiling. The ``$exit_code == 0``
in the if-clause express this necessity.

When the *linking* is needed:

- The *file paths* of all produced object files will be gathered into a *string*;
- The same is done with the linker options passed by the user (from outside, on the project declaration);
- If the architecture was specified, the option that flags it is added to the linker options;
- Also unserializes the additional *file paths* where some libraries can be found (these paths also are from outside, passed by the toolset user during the project declaration);
- The function which effectively runs the *linker* is called: ``gcc_link_ofiles()``;

After a status report based on the *exit code* is shown, in order to guide the user about what happens:

```
    if ($exit_code == 0) {
        hefesto.sys.echo("*** success.\n");
    } else {
        hefesto.sys.echo("*** failure.\n");
    }
```

Finally, the function results its *exit code* which is rather important for informing the ``Hefesto`` about what really happened during the forge
process.

```
    result $exit_code;
```

Now, go back to the fully forge function's source code and try to re-read each detail into the code. It is important really to understand what is being done there before continuing.

##### Detailing each forge helper

Previously the forge function was detailed, however, this function uses a set of support functions which will be detailed here.

###### gcc_compile_source_list()

Maybe this function is the most complex forge helper. For this reason, it will be discussed first.

In this function, you will see how to watch for file changes. This watching must be implemented by the ``toolset`` developer. To maintain the sanity of this
``feature`` is up to him.

There are two execution *branches*, one branch is related with the synchronous compiling process, the another one is related with the asynchronous compiling
process. They could be divided into two functions but here are expressed as *if-branches*.

A synchronous compiling can be understood as one file compiled per cycle. Opposing, an asynchronous compiling can be understood as *n* files compiled per cycle.

This is the function interface:

```
    function gcc_compile_source_list(sources type list,
                                     includes type string,
                                     cflags type string,
                                     obj_dir type string) : result type int
```

The function receives the list of sources to be compiled, a string carrying all additional includes directories, a string gathering the compiler options and
a string which represents the directory path specifying the place where the object files should be created. When all is gracefully processed the function
returns ``0``, otherwise it returns a non-zero value.

The beginning of the function is what follows:

```
    var size type int;
    var i type int;
    var j type int;
    var exit_code type int;
    var ofile type string;
    var qsize_list type list;
    var run_list type list;
    var qsize type int;
    var not_compiled type list;
    var not_compiled_tmp type list;
    var compile_model type list;
    var is_release type int;
    var curr_src_file type string;

    $size = $sources.count();

    if ($size == 0) result 0;

    $qsize_list = hefesto.sys.get_option("qsize");

    if ($qsize_list.count() > 0) {
        $qsize = $qsize_list.item(0);
    }
```

If passed an empty source list it will automatically return ``0``. If a non-empty source list is passed, the *Hefesto* option ``--qsize=n`` is read. This
option is an internal option recognized by ``Hefesto``. This option indicates the total of process that can be concurrently ran by the ``syscall run``. For
calling the ``run`` on an asynchronous way is needed to pass a list containing the external commands.

If the option ``--qsize`` is passed, the discussed function will reserves the ``qsize`` value for further usage. After it:

```
    $compile_model = hefesto.sys.get_option("compile-model");

    $is_release = 1;

    if ($compile_model.count() > 0) {
        if ($compile_model.item(0) != "release" &&
            $compile_model.item(0) != "debug") {
            hefesto.sys.echo(hefesto.project.toolset() +
                             " internal error: unknown compile model: \"" +
                             $compile_model.item(0) + "\"\n");
            result 1;
        }
        $is_release = ($compile_model.item(0) == "release");
    }
```

The option ``compile-model`` is read. The content passed into this option is verified. The content must be ``release`` or ``debug``. The default model
is ``release``.

Now, finally, we will actually compile some stuff:

```
    $exit_code = 0;
    $i = 0;

    $gcc_c_project_forge_dirty_files_nr = 0;

    $not_compiled = lsdup($sources);

    if ($qsize <= 1) {
        while ($i < $size && $exit_code == 0) {
            $ofile = $sources.item($i);
            $ofile = gcc_mk_ofile_name(filenamefrompath($ofile), $obj_dir);
            if (hefesto.toolset.file_has_change($sources.item($i)) == 1 ||
                isfile($ofile) == 0) {
                $gcc_c_project_forge_dirty_files_nr =
                $gcc_c_project_forge_dirty_files_nr + 1;
                $curr_src_file = $sources.item($i);
                $exit_code = hefesto.sys.run(gcc_mk_app_compilation_command($curr_src_file,
                                                                            $ofile,
                                                                            $includes,
                                                                            $cflags, $is_release));
                if ($exit_code == 0) {
                    $not_compiled.del_item($sources.item($i));
                    hefesto.sys.echo("\t*** " +	$sources.item($i) + ": no errors found.\n");
                } else {
                    hefesto.sys.echo("\t*** " + $sources.item($i) + ": errors found!\n");
                }
            } else {
                hefesto.sys.echo("\t*** " + $sources.item($i) + ": no changes.\n");
            }
            $i = $i + 1;
        }
        if ($gcc_c_project_forge_dirty_files_nr > 0) {
            hefesto.toolset.base_refresh($not_compiled);
        }
    } else {
        (...)
    }
```

The *else* is related with the asynchronous stuff, by now we will abstract it. Well, we have the following initializing code:

```
    $exit_code = 0;
    $i = 0;
    $gcc_c_project_forge_dirty_files_nr = 0;
    $not_compiled = lsdup($sources);
```

The variable which stores the *exit code* is zeroed. The list counter is zeroed too. The flag which indicates that at least one file was processed is also zeroed.
After is used a function defined into the ``lsutil.hsl``. This function returns a copy of a passed list.

The list ``not_compiled`` has a special utility. Using this list the ``Hefesto`` will be able to know what file actually has changed since the last forge.

There are two *built-in* functions into the ``toolset sub-system``:

- ``hefesto.toolset.file_has_change(<file-path>)``
- ``hefesto.toolset.base_refresh(<untouched-files>)``

The ``file_has_change()`` is pretty straightforward:

>Receives a *file path* and returns **1** if it has changed otherwise **0**.

Now the ``base_refresh()``:

>Receives a list of **not processed** *file paths*.

The *workflow* for using these functions is basically to call ``file_has_change()`` in order to know if the related file has changed. If it has changed the
specific processing imposed by the toolset must be applied over this file. If this file was sucessfully processed its file path should be removed from the
list that will be passed to the ``base_refresh()``. After processing all files, a single call to ``base_refresh()`` should be done in order to refresh the
files status for the next forge. Only with these two functions the *Hefesto* is able to know exactly what has changed since the last forge.

Returning to the code:

```
    if ($qsize <= 1) {
        while ($i < $size && $exit_code == 0) {
            $ofile = $sources.item($i);
            $ofile = gcc_mk_ofile_name(filenamefrompath($ofile), $obj_dir);
            if (hefesto.toolset.file_has_change($sources.item($i)) == 1 ||
                isfile($ofile) == 0) {
                $gcc_c_project_forge_dirty_files_nr = $gcc_c_project_forge_dirty_files_nr + 1;
                $curr_src_file = $sources.item($i);
                $exit_code = hefesto.sys.run(gcc_mk_app_compilation_command($curr_src_file,
                                                                            $ofile,
                                                                            $includes,
                                                                            $cflags, $is_release));
                if ($exit_code == 0) {
                    $not_compiled.del_item($sources.item($i));
                    hefesto.sys.echo("\t*** " + $sources.item($i) + ": no errors found.\n");
                } else {
                    hefesto.sys.echo("\t*** " + $sources.item($i) + ": errors found!\n");
                }
            } else {
                hefesto.sys.echo("\t*** " + $sources.item($i) + ": no changes.\n");
            }
            $i = $i + 1;
        }
        if ($gcc_c_project_forge_dirty_files_nr > 0) {
            hefesto.toolset.base_refresh($not_compiled);
        }
    }
```

If the requested forge is synchronous, ``--qsize=1`` (by the way, the default), for each file path (*C* code) into the list, not occuring compiling errors:

> Based on the file path, we use the function ``gcc_mk_ofile_name()`` to create an object file name, in fact by just replacing the ``.c`` extension by ``.o`` joining these new name with the objects directory string.
>
> We verify if the file ``.c`` has some change or if its related object file not exists.
>> With this condition satisfied, we increment the file processing counter and also we run the compiling command. If the file was successfully processed its path is removed from the list ``not_compiled``.
>>
>> If the file ``.c`` has not changed and its object file exists, we inform that has no changes for it. Keeping this file inside the ``not_compiled`` list.

After processing all source code files, if at least one file was processed, the ``base_refresh()`` is called and finally the *exit code* is returned to the
caller which must take some action based on the returned value.

Now let's see the asynchronous part of this forge helper:

```
    else {
        $not_compiled_tmp.clear();
        while ($i < $size) {
            $ofile = $sources.item($i);
            $ofile = gcc_mk_ofile_name(filenamefrompath($ofile), $obj_dir);
            if (hefesto.toolset.file_has_change($sources.item($i)) == 1 ||
                isfile($ofile) == 0) {
                $gcc_c_project_forge_dirty_files_nr = $gcc_c_project_forge_dirty_files_nr + 1;
                $curr_src_file = $sources.item($i);
                $run_list.add_item(gcc_mk_app_compilation_command($curr_src_file,
                                                                  $ofile,
                                                                  $includes,
                                                                  $cflags,
                                                                  $is_release));
                $not_compiled_tmp.add_item($sources.item($i));
            }
            if ($run_list.count() == $qsize) {
                $exit_code = hefesto.sys.run($run_list);
                if ($exit_code == 0) {
                    $j = 0;
                    while ($j < $not_compiled_tmp.count()) {
                        $not_compiled.del_item($not_compiled_tmp.item($j));
                        $j = $j + 1;
                    }
                    $not_compiled_tmp.clear();
                    hefesto.toolset.base_refresh($not_compiled);
                }
                $run_list.clear();
            }
            $i = $i + 1;
        }
        if($run_list.count() > 0) {
            $exit_code = hefesto.sys.run($run_list);
            if ($exit_code == 0) {
                $j = 0;
                while ($j < $not_compiled_tmp.count()) {
                    $not_compiled.del_item($not_compiled_tmp.item($j));
                    $j = $j + 1;
                }
                hefesto.toolset.base_refresh($not_compiled);
            }
        }
    }
```

The behavior is similar of the synchronous stuff. The difference is that the compiling commands are not directly executed, being they added to a list for
executing at the end of the cycle (understand it as a command pool). Another difference here is the usage of the temporary list ``not_compiled_tmp`` which
are filled per cycles. Also the ``base-refresh`` is done per cycle. A cycle is considered done when the size of ``run_list`` is equals to the value of the
``--qsize``. If the compiling of any file return an error, the process is totally interrupted. Otherwise it will continue until get all source code files
processed.

###### gcc_mk_app_compilation_command()

This function makes the commands that will be executed using the previous toolset commands:

```
    local function gcc_mk_app_compilation_command(src_name type string,
                                                  outfile type string,
                                                  includes type string,
                                                  cflags type string,
                                                  is_release type int) : result type string {
        var cmd type string;
        if ($is_release == 1) {
            $cmd = hefesto.toolset.compile_r($src_name, $outfile, $includes, $cflags);
        } else {
            $cmd = hefesto.toolset.compile_d($src_name, $outfile, $includes, $cflags);
        }
        result $cmd;
    }
```

If you have understood the concepts related with the ``toolset commands``, I think this function is quite straighforward for
you.

###### gcc_link_ofiles()

This function is responsible for *linking* the produced object files building the application using the name specified into ``appname``.

The objects list is expressed as a string, in form that *GCC* expects, e.g.: *"foo.o bar.o baz.o"*.

The variables ``libraries`` and ``ldflags`` the same thing. They are in form that *GCC* expects.

The first thing that this function does is to check if the option ``--link-model`` was passed by the user. This option allows two models ``static`` or ``shared`` (the default).
If the user has passed an alien model the process will break with an error.

The ``link-model`` decides which toolset command related with the linker should be picked. This command is made by the toolset and executed with the ``syscall run``.
The *exit code* from the *linker* is captured and returned to the caller which will take some action based on it.

Take a look at the code listing reviewing about what you have read above:

```
    function gcc_link_ofiles(objects type string,
                             libraries type string,
                             ldflags type string,
                             appname type string) : result type int {
        var exit_code type int;
        var link_model type list;
        var is_static type int;
        $link_model = hefesto.sys.get_option("link-model");
        $is_static = 0;
        if ($link_model.count() > 0) {
            if ($link_model.item(0) == "static") {
                $is_static = 1;
            } else {
                if ($link_model.item(0) != "shared") {
                    hefesto.sys.echo(hefesto.project.toolset() +
                                     " internal error: unknown link model: \"" +
                                     $link_model.item(0) + "\"\n");
                    result 1;
                }
            }
        }
        if ($is_static == 1) {
            $exit_code = hefesto.sys.run(hefesto.toolset.link_static($appname,
                                                                     $objects,
                                                                     $libraries, $ldflags));
        } else {
            $exit_code = hefesto.sys.run(
            hefesto.toolset.link_shared($appname,
                                        $objects,
                                        $libraries, $ldflags));
        }
        result $exit_code;
    }
```

Well, all done. A good exercise is try to create a ``toolset`` for automating something based on your own necessities. It is not necessary to be related with
compilers. For example, you can try to create a ``toolset`` which does the *deploy* of your package, etc. Be creative!
