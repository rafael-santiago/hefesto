# Codestyle

If you want to contribute with this project, I follow this codestyle:

- Codes with columns limited in 80-120 bytes
- White spaces are used to separate the code in logical blocks, save your disorder for the next ioccc
- #pragmas are forbidden
- include guards need to be defined with 1 -> #define _EXAMPLE_H 1
- if/else, while and for blocks are K&R based
- Indentation deepness: 8 spaces
- Use 0 as string delimitator instead of '\0'
- Use NULL to represent null pointers
- Do not use ultraExplainedBoringNames, you're an C programmer, honor it or do a harakiri... ;)
- Static functions need to be prototyped at the module beginning
- Pointers declaration: "void *vptr = NULL;" and not: "void* vptr = NULL;"

This is the way that if blocks are written:

    if (6 == 9) {
        printf("I don't mind!\n");
    } else {
        printf("I don't care!\n");
    }

Avoid this type of construction:

    if (6 == 9)
        printf("I don't mind!\n");
    else
        printf("I don't care!\n");

For while you use:

    int i = 99;
    while (i-- > 0) {
        printf("%d %s of beer on the wall.\n", i, (i > 1) ? "bottle" :
                                                           "bottles");
    }

This is the way to write for blocks:

    for (i = 99; i > 0; i--) {
        printf("%d %s of beer on the wall.\n", i, (i > 1) ? "bottle" :
                                                           "bottles");
    }

The way for switches is:

    switch (c) {

        case 0:
            //  do something
            break;

        case 1:
            //  do something
            break;

        default:
            //  do something
            break;

    }

## Best practices

- Give credit to yourself and keep the credit from others
- If you created a new feature be ready to include an acceptable documentation in your pull request
- If you wish to create a new thing try to turn it possible on a straightforward way
- Always seek to kill bugs in this way: "K.I.S.S! K.I.S.S! Bang! Bang!"
- Write tests
- Don't trust in the unit tests only, build the application and *** always *** use it
- Don't write crazy tests that in the future will make impossible needed fixes due to perform tests under impossible situations
- Your ideas and specialities in some subject is what are welcome in this project, these things need to stay even if you have gotten away, have been abducted or have gone to write code in BASIC, never retain information. On this way your mind can produce more things here and in other places
