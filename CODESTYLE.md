Codestyle
---------

Se você quiser contribuir com o projeto, eu adoto o seguinte code-style:

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
        printf("%d %s muita gente %d elefantes incomodam mto mais.\n", (i > 1) ? "elefantes incomodam" :
                                                                                 "elefante incomoda" i, i+1);
    }

No for é assim:

    for (i = 1; i < 1000; i++) {
        printf("%d %s muita gente %d elefantes incomodam mto mais.\n", (i > 1) ? "elefantes incomodam" :
                                                                                 "elefante incomoda" i, i+1);
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
      