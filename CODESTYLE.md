Codestyle
---------

If you want to contribute with this project, I follow this codestyle:

    * Codes with columns limited in 80-120 bytes
    * White spaces are used to separate the code in logical blocks, save your disorder for the next ioccc
    * #pragmas are forbidden
    * include guards need to be defined with 1 -> #define _EXAMPLE_H 1
    * if/else, while and for blocks are K&R based
    * Indentation deepness: 8 spaces
    * Use 0 as string delimitator instead of '\0'
    * Use NULL to represent null pointers
    * Do not use ultraExplainedBoringNames, you're an C programmer, honor it or do a harakiri...
    * Static functions need to be prototyped at the module beginning
    * Pointers declaration: "void *vptr = NULL;" and not: "void* vptr = NULL;"

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

Best practices
--------------

    * Give credit to yourself and keep the credit from others
    * If you created a new feature be ready to include an acceptable documentation in your pull request
    * If you wish to create a new thing try to turn it possible in a straightforward way
    * Always seek to kill bugs in this way: "K.I.S.S! K.I.S.S! Bang! Bang!"
    * What K.I.S.S does mean? Thank you, but goodbye!
    * Write tests
    * Don't trust in the unit tests only, build the application and *** always *** use it
    * Don't write crazy tests that in the future will make impossible needed fixes due to perform tests
      under impossible situations
    * Learn with musicians which read transcriptions from other musicians, create the code reading habit
      seeking to learning and not only for refactory issues
    * I'm brazilian and I write comments in portuguese, if you speak portuguese too and wish to comment with this language,
      include if possible an english version of your comment
    * Your ideas and specialities in some subject is what are welcome in this project, these things need to
      stay even if you have gotten away, have been abducted or have gone to write code in BASIC, never retain information.
      In this way your mind can produce more things here and in other places

Codestyle
---------

Se você quiser contribuir com o projeto, eu adoto o seguinte codestyle:

    * Códigos limitados a 80-120 colunas
    * Códigos separados em blocos lógicos, guarde sua desordem para o próximo ioccc
    * #pragmas não são permitidos
    * include guards precisam ser definidos com 1 -> #define _EXAMPLE_H 1
    * Blocos if/else, while e for são baseados no K&R
    * Endentação: 8 espaços
    * Use 0 para determinar final de string, não '\0'
    * Use NULL para representar pointeiros nulos
    * Não use nomesSuperExplicadosLongosAlemDeMuitoEntediantesDeLer, você programa em C, honre isso ou se honre cometendo imediatamente um harakiri. ;D
    * Prototipe todas as funções estáticas no início do módulo
    * Declaração de ponteiros: "void *vptr = NULL;" e não: "void* vptr = NULL;" 
    
Assim fica um bloco If:

    if (1 == 0) {
        printf("Tudo que era sólido já derreteu!\n");
    } else {
        printf("Tudo que é sólido pode derreter!\n");
    }

Evite esse tipo de construção:

    if (1 == 0)
        printf("Tudo que era sólido já derreteu!\n");
    else
        printf("Tudo que é sólido pode derreter!\n");
        
No while você usa:

    int i = 0;
    while (i++ > 1000) {
        printf("%d %s muita gente %d elefantes incomodam mto mais.\n", i, (i > 1) ? "elefantes incomodam" :
                                                                                    "elefante incomoda", i+1);
    }

No for é assim:

    for (i = 1; i < 1000; i++) {
        printf("%d %s muita gente %d elefantes incomodam mto mais.\n", i, (i > 1) ? "elefantes incomodam" :
                                                                                    "elefante incomoda", i+1);
    }
    
Switch é assim:

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

Boas práticas
-------------

    * Se dê crédito e mantenha o crédito dos outros
    * Se criou uma nova feature esteja pronto a incluir uma documentação aceitável no seu push request
    * Se quiser criar algo novo tente possibilitar o resultado do jeito mais direto possível, o usuário
      não chama Ulisses para ter que passar um perrengue danado para alcançar o objetivo, nós
      desenvolvedores que temos que ser Ulisses o suficiente para fazer o difícil ridiculamente fácil
      para o usuário *** interessado ***
    * Sempre busque matar bugs dessa forma: "K.I.S.S! K.I.S.S! Bang! Bang!"
    * Escreva testes
    * Não confie apenas nos unit tests, construa o aplicativo e use-o *** sempre ***
    * Não escreva testes malucos que no futuro impossibilitarão fixes por conta de testarem situações
      anti-físicas (que nunca irão ocorrer na prática no universo do aplicativo), melhor não ter teste
      do que ter um teste assim
    * Aprenda com os músicos que leêm partituras de outros músicos, crie o hábito de ler o código dos outros
      não só para dar refactory mas para aprender
    * Sou brasileiro e me dou ao luxo de comentar em português, se você também fala português e quiser comentar
      com esse idioma bacana, inclua a versão do comentário também em inglês se possível
    * Suas ideias e especialidade em algum assunto é que são bem-vindas ao projeto, essas coisas precisam se
      manter mesmo que você caia fora, seja abduzido ou passe programar em BASIC, jamais retenha informações.
      Dê vazão para que sua mente consiga produzir mais coisas aqui e em outro lugar
