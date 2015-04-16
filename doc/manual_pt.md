# Um *build system* chamado Hefesto

*por Rafael Santiago*

----

**Resumo**: Nesse documento você encontra algumas informações relacionadas ao projeto. Minhas motivações. Detalhes gerais sobre a ``HSL``. Decisões de projeto. Loucas divagações e ideias que podem ferir as suas, se você for uma alma sensível... :-)

----

## Hefesto quem?!

``Hefesto`` é um *build system* nada ortodoxo propício para gente conservadora odiar... O principal conceito dele é evidenciar tudo o que está sendo feito em termos de *build* no seu projeto de *software* lhe oferecendo as ferramentas básicas para isso.

## A motivação

*Hefesto* é um *build system* que resolvi criar buscando responder algumas perguntas que sempre fiz em termos de *build systems*:

- Por que build systems adotam linguagens excessivamente declarativas?
- Por que a sintaxe dessas linguagens em geral são horrendas?
- Por que o desenvolvedor não pode se sentir programando o build?
- Por que para que ele se sinta assim precisa empilhar tanta coisa junto?
- É feio, nada polido, prezar por.... concisão, completude e clareza?!
- Por que adotar a filosofia do caracol para se sentir em casa?

Talvez à medida que começar descrever o que criei você note que o *Hefesto* não é necessariamente um *build system* mas sobre o que um *build system* na essência é.

### Automate or die!

Eu desenvolvo *software* e gosto de automatizar coisas, qualquer tipo de tarefa repetitiva é uma tortura para mim, pois eu poderia estar fazendo outra coisa no lugar da repetição.

Atualizando minha última afirmação: eu desenvolvo *software* e gosto de pensar em coisas novas o tempo todo e por isso *preciso* automatizar.

Ao meu ver a automatização mais básica no ciclo que envolve o desenvolvimento de um *software* remonta aos primórdios de seu surgimento, em outras palavras, remonta ao seu *build*.

Temos incontáveis formas de automatizar *builds* desde *shell scripts* e *batch files* arcanas, até os *Makefiles*, *Jamfiles* e etc.

Ao longo das décadas que venho programando já usei todos os que citei e mais alguns. Pessoalmente, nunca fui atendido por completo por nenhum deles e às vezes me irritaram muito.

Vou itemizar uns aspectos gerais que me fizeram desistir deles e ++apenas (à penas mesmo) usá-los++ quando o "senso" comum me obriga:

- Linguagens simbólicas;
- Linguagens às vezes inpiradas na Whitespace;
- O poder de fogo não é constante quando mudamos de plataforma;
- Às vezes é preciso instalar uma infinidade de coisa para manter o poder de fogo, sem necessidade se o build system tivesse sido bem pensado;
- Documentação confusa e nada direta;
- Mistureba de Linguagens sem nenhuma necessidade;
- Utilização confusa, envolvendo duas ou mais ferramentas para cumprir todo o ciclo do processo de build;
- Ferramentas visuais baseadas em XML que é por sua vez horrível de versionar e tirar diffs, a coisa se torna ainda pior quando o XML é gerado automaticamente;
- Adaptar seu senso de qualidade e clareza nivelando por baixo, algo como, preciso pensar meu círculo assim pois meu *build system* me oferece uma lacuna no formato de um triângulo... "Sim, veja! É incrível como meu círculo se encaixa nesse triângulo menor que ele!!! Por essa eu não esperava...."

## Como funciona o *Hefesto*

O *Hefesto* leva em consideração que você é um programador que ++conhece++ os compiladores que está usando. Se você se julga um *sniper* mas até hoje se limitou em usar armas de *paintball* pare de ler isso.

Ele é baseado no conceito de automatizar a essência do processo de *build* de um *software* isso recai na compilação e *linkedição*, na maioria das vezes. Sou programador *C* e o *Hefesto* é em grande parte feito nela, quando não, é feito na linguagem que ele implementa, que por sinal vou apresentar depois de mostrar a cara de um *script* de *build* *Hefesto*.

Quase todo *build system* possui um arquivo mágico que se existe no diretório *sheebang!* No *Hefesto* não... ele deixa você dar o nome que melhor se adequa ao
seu estado de coisas. Eu gosto de chamar os meus de ``Forgefiles``.

Segue um exemplo de um ``Forgefile`` para uma *lib* escrita em *C*:

		# Comment-sample: Forgefile.hsl...

        include ~/toolsets/gcc/gcc-lib.hsl

        var sources type list;
        var includes type list;
        var cflags type list;

        project here : toolset "gcc-c-lib" : $sources, $includes, $cflags, "libhere.a";

        here.prologue() {
        	$sources.ls(".*\\.c$");
        }

A ``HSL`` já começa aqui e continua dentro do ``toolset``. ``HSL`` pode ser entendida como ``H``efesto ``S``cripting ``L``anguage. É a Linguagem que se usa tanto para criar *builds* ou para automatizar a essência deles. As automatizações do ``essencial de um build`` pode ser entendido como sendo um ``toolset``.

Note que no ``Forgefile`` apresentado, após o comentário, é feita uma inclusão bem sugestiva. Estamos incutindo nesse ``Forgefile`` uma base de conhecimento prévia que recai sobre como executar um *build* de uma biblioteca escrita em *C* usando o compilador *GCC*.

Depois disso, algo que muitos odeiam com paixão. Você precisa declarar tudo, absolutamente tudo o que vai usar. Algo como: "ei pessoal, preciso usar isso aqui então, com licença viu?".

Do ponto de vista do *design de Linguagens de programação*, eu pessoalmente acho que a necessidade de declaração de uma variável é uma *feature* que salta do dispositivo direto para a consciência do usuário, pois no momento da declaração ele será levado a pensar sobre o porquê e se realmente precisa daquela variável naquele dado momento.
Salvo isso, continuemos...

Essas variáveis declaradas são utilizadas pela ``função de forja`` (entenda por enquanto como o ponto de entrada para a parte mais interna do build).

Após a declaração das variáveis, ``declaramos o projeto``. Dizendo algo na linha: 

>"Esse projeto se chama "here" é uma *lib* escrita em *C* que esperamos compilar com o *GCC*. Seguem os parâmetros na ordem que o motor de build para esse tipo de coisa espera."

Esse ``toolset`` espera receber uma lista de ``códigos`` para compilar, uma lista de diretórios de ``includes`` usados durante a compilação, uma lista de ``opções de compilação`` que o compilador em questão entende e um ``nome para o arquivo`` que será produzido pelo processo, em outras palavras a *lib* propriamente dita.

Um problema com a maioria dos *build systems* é quanto a definição dos códigos a serem processados. Alguns deixam a coisa solta e podemos ler essa lista de qualquer lugar, outros não disponibilizam mecanismos para isso.

O *Hefesto* fica no meio termo, antes de um *build* começar, às vezes, para não dizer quase sempre... existem uma infinidade de coisas muito específicas àquele projeto que precisam ser feitas. Quando um projeto é criado no *Hefesto*, juntamente com ele são criados três pontos de entrada: ``preloading``, ``prologue`` e ``epilogue``.

Confira na **Tabela 1** em que momento do processo esses pontos de entrada são atingidos. A ideia básica aqui é: se existe código definido dentro deles, no momento esperado, esses códigos serão executados.

**Tabela 1**: Os estágios de um projeto *Hefesto*.

| **Project entry-point** |            **Executado**         |
|:-----------------------:|:--------------------------------:|
|      preloading         | Antes mesmo de carregar o toolset|
|       prologue          | Antes da forja ser iniciada       |
|       epilogue          | Após a forja ser finalizada      |

Esqueçamos por hora o ``preloading`` e o ``epilogue``. Uma coisa muito básica,
sempre deverá ser feita antes de começar uma forja, que é coletar os *file paths*
dos códigos que precisam ser processados. Então, no ``prologue`` do exemplo:

		here.prologue() {
        	$sources.ls(".*\\.c$");
        }

Não se preocupe com o ``.ls()``... Nas seções futuras quando descrever melhor a ``HSL`` os facilitadores de lista serão apresentados e tudo talvez faça mais sentido.

Com o ``Forgefile`` escrito, a forja poderia ser invocada na linha de comando da seguinte forma:

> ``hefesto --forgefiles=Forgefile.hsl --Forgefile-projects=here``

Note que na opção ``--forgefiles`` são especificados uma lista de arquivos contendo projetos. A outra opção é dinâmica e baseada nos arquivos passados em ``--forgefiles``. O esquema geral dessa opção dinâmica é:

> ``--<file-name>-projects=<projects defined inside this file>``

Já sei:

> ZzZZzzzZzzzZzZZZZZZzzzzzzZzzzZzzzzzZZZZZZZzzzzZZZ que comando longo... não posso digitar algo mais breve? 

Pode sim, você precisa criar um arquivo de invocação e imolar 5 cabras em nome de *Hefesto*... O arquivo de invocação precisa estar no diretório onde você deseja invocar a forja, o nome desse arquivo é ``.ivk`` e dentro dele você deve colocar a linha de comando exatamente como passaria para o *Hefesto*.

Com o ``.ivk`` configurado, estando no diretório dele, apenas digitando ``hefesto`` você terá a forja nos moldes padrão que você necessita, caso passe opções nessa chamada, as opções que você fornecer terão precedência maior quando *mergeadas* com as opções do arquivo de invocação.

E quanto as cabras?! Não precisa, era só brincadeira...

Pronto, agora você já sabe os passos básicos para compor um ``Forgefile`` no *Hefesto*, mas o que você não sabe ainda são os detalhes gerais da ``HSL`` e de como criar mais ``toolsets`` que atendam suas necessidades.

Os ``toolsets`` por sua vez são porções de código ``HSL`` que podem ter suas próprias convenções, conveniências e tal. Por isso, antes de usá-los uma boa providência é consultar a documentação específica desse ``toolset``. Se você o baixou da base oficial provavelmente encontrará algum texto que o detalhe.

### Reprocessando somente o que mudou desde a última forja, tem como?

Sim, isso envolve requisitar essa necessidade na declaração do projeto. Vamos usar o mesmo projeto de *lib* do exemplo anterior.

Anteriormente a declaração era essa:

		project here : toolset "gcc-c-lib" : $sources, $includes, $cflags, "libhere.a";

Agora vamos incluir a especificação da cadeia de dependências ou ``dep-chain``. Uma ``dep-chain`` basicamente é uma ``string`` expressa num formato especial:

>``<file-path>`` **:**  ``<file-path_0>`` [ **&** ``<file-path_n>`` ] **;**

Imagine que ``a.x`` depende de ``b.x`` que depende de ``c.x`` que por sua vez depende de ``d.x`` e ``e.x``. A ``dep-chain`` para essa situação seria:

>a.x: b.x;
>b.x: c.x;
>c.x: d.x & e.x;

No caso de programas reais isso pode ser bem massante de se escrever, a biblioteca padrão do ``Hefesto`` inclui funções em ``HSL`` que tentam inferir por meios heurísticos a interdependência entre os arquivos que compõem um projeto.

Segue um uso prático das ``dep-chains``:

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
                        		                                  "libhere.a" ;

		here.prologue() {
    		$deps = get_c_cpp_deps();
    		$sources.ls(".*\\.c$");
		}

Note que agora foi inclusa a diretiva ``dependencies <string>`` na declaração do projeto, além do uso da função ``get_c_cpp_deps()`` importada de ``dependency_scanner.hsl``. Contudo, você mesmo poderia grafar a sua ``string`` de dependência, caso quisesse. Importante ressaltar que uma ``dep-chain`` mal formada é silenciosamente descartada pelo aplicativo. Esteja sempre atento durante a composição delas.

E Pronto! Apenas com isso seus arquivos começarão serem processados somente quando houverem alterações explícitas neles ou em algum outro arquivo que eles dependam.

#### Como forçar o reprocessamento de tudo?

Use a opção ``--forge-anyway`` ao invocar uma forja.

#### Algo mais que deva saber sobre as *dep-chains*?

Sim, quando você adota o uso de ``dep-chains`` em um projeto os seus argumentos durante a invocação da forja começam ser monitorados. A decisão de projeto que adotei foi:

>Se existe um ``bit`` que seja de diferença entre os argumentos de forja atuais e os últimos de uma forja bem sucedida, **tudo será reprocessado**. Ao meu ver o preço que se paga é menor. Se não há como saber que ``flag`` é passível de reprocessamento de um mar de ``flags`` é mais confiável sempre reprocessar e acabou. A praticidade cotidiana não é uma *top-model* mas na sua simplicidade traz seus encantos...

>Sinceramente eu não acho que programadores realmente ocupados e com objetivos reais e bem conscientes ficam macaqueando ``flags`` aleatórias o tempo inteiro para um ``build`` ou disputando ``build-turfe`` por aí. Você conhece algum?!

Uma alteração de arquivo é julgada **não pela data de alteração**, mas **com base no conteúdo**. Com um ``bit`` de diferença um arquivo já será considerado "sujo".

### Yoda`s proposal: que tal em *run-time* o *toolset* mudar?

Sim meu jovem *Luke*, não tenha medo muito... isso é possível. Para isso só é preciso ter uma precaução *Jedi*: os *toolsets* a serem escolhidos devem possuir a mesma assinatura de função para a função de forja, em outras palavras, devem receber o mesmo número de argumentos, na mesma ordem e tipo.

Lembra quando foram apresentados os ``entry-points`` de um projeto de forja? Que o ``entry-point`` chamado ``preloading`` ocorre mesmo antes do ``toolset`` ao qual o projeto é dependente ser carregado? Vamos usar esse ``big-bang`` para criar um *Forgefile* mais adaptável ao ambiente.

Vamos pegar o projeto daquela *lib* apresentado e criar o seguinte requisito:

>Se estivermos no ``Windows`` daremos preferência por usar o *Visual Studio 2012*, caso ele exista na máquina. Não existindo usaremos o *GCC*, nas demais plataformas usaremos sempre o *GCC*.

Vou apresentar o *Forgefile* alterado e comentar as partes interessantes depois.

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

A função ``has_vs_110`` verifica no registro do sistema a entrada de chave existente caso haja o *Visual Studio* na máquina, leia a seção ``Sobre a HSL`` que você entenderá a lógica básica e o uso de ``hefesto syscalls`` nessa função. Se existir na máquina retornará 1, de outra forma 0.

Note que no ``preloading`` do projeto apenas chamaremos essa função se estivermos executando a forja sob o ``Windows`` (novamente, consulte a seção ``Sobre a HSL``) e se existir o *Visual Studio* na máquina, setamos o nome do toolset para "vc110-lib". Caso contrário, o ``toolset`` será o padrão "gcc-c-lib". Pelo fato da declaração possuir uma indireção para o nome do ``toolset`` via a variável que setamos no ``preloading``:

		project here : toolset $current_toolset : dependencies $deps : $sources,
        			                                                   $includes,
                    			                                       $cflags,
                                			                           "libhere.a" ;

Passamos ter a seleção do ``toolset`` em ``run-time``, pois quando o ``toolset`` estiver sendo carregado o ``Hefesto`` já vai ter passado pelo ``preloading`` do nosso projeto. Por esse motivo é importante ter a mesma assinatura de forja entre os ``toolsets``, pois só podemos declarar um projeto uma vez. Mas isso vai da inteligência e mais do que isso, do senso de reutilização do desenvolvedor dos ``toolsets`` que você for usar.

Mas ainda tem um "problema" nisso aí... observe no início do exemplo:

		include ~/toolsets/gcc/gcc-lib.hsl
		include ~/toolsets/vc/vc110-lib.hsl

Nossa forja é multiplataforma, mas não precisamos incluir o módulo ``vc110-lib.hsl`` fora do ``Windows``, não usaremos *Visual Studio* no ``Linux``, para que ocupar o ``Hefesto`` em processar esse módulo sem necessidade toda vez? Que tal melhorar para:

		include ~/toolsets/gcc/gcc-lib.hsl
		include on windows ~/toolsets/vc/vc110-lib.hsl

Pronto!

### Como rodo meus testes?

Nada me irrita mais do que a necessidade de criar ``batch-files`` ou mesmo ``shell-scripts`` para rodar testes e outras tarefas pré e pós compilação. Organização ao meu ver envolve uniformidade e quanto menos mistureba melhor e o paraíso seria perfeito se pudéssemos deixar tudo dentro da mesma esfera.

Que tal usar o ``entry-point epilogue`` para disparar a compilação e execução dos testes, avaliando se quebraram ou passaram? No exemplo, da nossa *lib*, adicionaríamos:

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

Desculpe se estou adiantando um pouco as coisa aqui, mas basicamente em ``epilogue`` verifico se o ``build`` atual de nossa *lib* foi realmente bem sucedido, se tiver sido, vejo se a opção de usuário ``--no-test`` foi passada, caso não tenha sido, chamo a função de execução dos testes.

A função ``run-tests`` por sua vez muda para o subdiretório ``test`` onde teremos um outro projeto de forja o qual disparamos. Após isso, finalizamos voltando ao diretório anterior.

Agora vamos ver como é esse *Forgefile* dos testes:

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

A função mais importante é a ``run_unittests`` chamada à partir do ``epilogue`` caso retorne algo diferente de zero sai com código 1, o que fará o *build* quebrar.

Eu posso dizer que encontrei meu paraíso no ``Hades``... :) Espero que você continue motivado(a) e siga com a seção que te ensina de fato programar na ``HSL`` e possa tirar proveito do suporte e liberdade que esse aplicativo procura promover.

## Sobre a HSL

Não era a minha intenção inicial criar uma [DSL]() para o *Hefesto*, o fato é que cheguei a conclusão disso durante o processo de maturação da ideia. Visto que desejava resolver meus problemas de uma forma mais geral.

Em resumo, tive mais trabalho, contudo, hoje consigo expressar uma boa quantidade de automatizações para diversos processos que vão além de simplesmente fazer *builds* de projetos *C/C++*.

### Por que uma *dsl* e não *scriptar* uma que já existe?

Ao meu ver, o problema de usar uma linguagem de uso geral é fazer o usuário do *software* se preocupar com convenções gerais que em suma não são importantes para expressar suas necessidades. Fora que isso cria uma dependência externa o que te faz ser refém das decisões futuras de um outro projeto. Linguagens de uso geral abrem margem para muitas possibilidades, muitas possibilidades abrem margem para desordem.

### Definindo as bases

A ``HSL`` em momento nenhum esquece que é uma linguagem feita em suma para automatizar coisas, não comece querer programar *software* de uso geral com ela. ++Por favor não...++

Ela possui 4 tipos primitivos e não existe uma conveniência para se utilizar *user-defined types*, a *Tabela 2* sumariza esses tipos.


**Tabela 2**: Tipos primitivos presentes na ``HSL``.

| **Palavra reservada** |                 **Tipo**                          |
|:---------------------:|:-------------------------------------------------:|
|     ``int``           | Inteiro de 32/64 bits                             |
|   ``string``          | Sequência de caracteres                           |
|    ``file``           | Um descritor de arquivo                           |
|    ``list``           | Container para dados do tipo ``int`` ou ``string``|

Fato: variáveis precisam ser declaradas.

Em qualquer ponto de um código ``HSL`` você pode declarar uma variável sob o seguinte esquema:

> **var** ``<var-name>`` **type** ``<type-name>`` **;**

Saiba que ``<var-name>`` pode ser composto por ``[A-Za-z0-9_]``. Pode-se ter uma e somente uma declaração por linha.

Exemplos práticos:

		var my_name type string;
		var my_age type int ;
		var my_sloopy_list_decl
		type
		list;

Para acessar uma variável declarada (me desculpe o pleonasmo) preceda-a com o símbolo ``$``, assim:

		$my_name = "John doe.";
		$my_age = -1;

``Branches`` condicionais podem ser expressos da seguinte forma:

> **if** **(** ``<expr>`` **)** **{** ``<hsl-stmt>`` **}** [ **else** **{** ``<hsl-stmt>`` **}** ]

ou

> **if** **(** ``<expr>`` **)** ``<hsl-single-stmt>`` [ **else** ``<hsl-single-stmt>`` ]

Só existe uma forma de expressar ``loops`` na Linguagem que é via ``while``, segue o formato dele:

> **while** **(** ``<expr>`` **)** **{** ``<hsl-stmt>`` **}**

ou

> **while** **(** ``<expr>`` **)** ``<hsl-single-stmt>``

A **Tabela 3** reúne os operadores disponíveis para compor expressões na ``HSL``.

**Tabela 3**: Operadores disponíveis até o momento.

|   **Operador**   |     **Operação**   | **Tipo** |
|:----------------:|:------------------:|:--------:|
|     ``==``       |      Igual a       |  Lógico  |
|     ``!=``       |   Diferente de     |  Lógico  |
|      ``<``       |     Menor que      |  Lógico  |
|      ``>``       |     Maior que      |  Lógico  |
|     ``>=``       | Maior ou igual que |  Lógico  |
|     ``<=``       | Menor ou igual a   |  Lógico  |
|     ``&&``       |      .E.           |  Lógico  |
|   &#124;&#124;   |      .OU.          |  Lógico  |
|     ``&``        |      .E.           |  Bitwise |
|     &#124;       |      .OU.          |  Bitwise |
|     ``<<``       |    ``Left Shift``  |  Bitwise |
|     ``>>``       |    ``Right Shift`` |  Bitwise |


Funções podem retornar qualquer tipo primitivo ou nada. O esquema geral de declaração é:

>**function** ``<function-name>`` **(** ``<var-decl-list>`` **)** **: result type ** **{** ``<hsl-stmt>`` **}**

Os caracteres aceitos para ``nomenclatura de uma função`` **são os mesmos** aceitos para ``nomenclatura de variáveis``.

Um exemplo mais prático... Uma função que calcula de forma recurssiva o fatorial de um número fornecido:

		function fatorial(n type int) : result type int {
			if ($n == 0) return 1;
        	return fatorial($n - 1) * $n;
    	}

Uma função que não retorna nada, cujo ``return type`` deve ser ``none``:

		function say_hello() : result type none
        {
        	hefesto.sys.echo("Hello, creative mind....\n")
        }

### As *Hefesto Syscalls*

Certo, o último exemplo com uma função inútil que só dizia alô, não foi tão inútil... Serviu para te criar uma dúvida:

>"O que afinal de contas é esse negócio de ``hefesto.sys.whatever``??"

Nada é por acaso nessa vida, acredite! Uma *abominável doninha documentadora* pôs aquilo lá para fazer você tropeçar e cair aqui.... Quando comecei pensar a ``HSL`` uma das minhas principais indagações eram sobre o que é comum a quase todo processo de *build* e dessas coisas comuns, o que é direta ou indiretamente requisitado ao ``Sistema Operacional`` em questão.

Essas indagações me levaram criar uma camada de abstração que garante escrever passos comuns em uma *build-task* mas heterogênos em termos práticos de um ``OS`` para outro, apenas uma vez e ter a garantia que rodarão iguais em qualquer plataforma suportada pelo ``Hefesto``.

Essa camada de abstração pode ser acessada à partir do ``subsistema sys`` do ``Hefesto``, da seguinte forma:

>``hefesto.sys.<sys-call>(<arg-list>)``

Esse ``subsistema`` é chamado de ``syscalls``, mas aqui não se deixe levar pelo conceito clássico de *syscall*. De fato, você encontrará muito mais do que *"syscalls"*. Encontrará coisas que fazem muito mais numa única chamada e num nível bem mais alto do que as boas e velhas *syscalls* do nosso bom e velho *Unix*, por exemplo.

Escolhi esse nome, porque durante meu trabalho notei que a maioria dos pontos de não portabilidade de uma *build-task* recaiam na maior parte sobre algumas *syscalls* verdadeiras. Então, fica aqui explicada a origem de qualquer futuro equívoco.

Uma outra forma de aceitar a não ortodoxia das ``hefesto syscalls`` pode ser entendendo que elas também englobam operações comuns de serem requisitadas e por isso postas de forma ``builtin`` no *build-system*. Criando o que eu aqui gosto de chamar de ``subsistema``.

Na **Tabela 4** segue uma listagem com uma rápida descrição do que uma referida ``syscall`` faz.

**Tabela 4**: ``Hefesto  syscalls`` disponíveis até o presente momento.

|          **Syscall**            |         **Utilitade**                                                                                         |
|:-------------------------------:|--------------------------------------------------------------------------------------------------------------:|
| ``replace_in_file()``           |substitui um texto encontrado via uma expressão regular                                                        |
| ``lines_from_file()``	          |filtra linhas que casam com a expressão regular passada                                                        |
| ``ls()``	                      |retorna a contagem de arquivos encontrados no diretório corrente que casam seus nomes com a regex fornecida    |
| ``pwd()``	                      |retorna o fullpath do diretório corrente                                                                       |
| ``cd()``	                      |muda o diretório corrente para o caminho passado                                                               |
| ``rm()``	                      |remove o filepath fornecido                                                                                    |
| ``cp()``	                      |copia um arquivo/diretório de um path existente para outro                                                     |
| ``mkdir()``	                  |cria um diretório                                                                                              |
| ``rmdir()``	                  |remove um diretório vazio                                                                                      |
| ``fopen()``	                  |abre um arquivo retornando seu descritor que deve ser atribuído a uma variável do tipo file                    |
| ``fwrite()``	                  |escreve dados para o arquivo referenciado pelo descritor de arquivo fornecido                                  |
| ``fread()``	                  |lê dados do arquivo referenciado pelo descritor de arquivo fornecido                                           |
| ``fclose()``	                  |limpa todos os recursos alocados por um descritor de arquivo                                                   |
| ``feof()``	                  |verifica se é o final do arquivo                                                                               |
| ``fseek()``	                  |pula para um determinado offset do arquivo                                                                     |
| ``fseek_to_begin()``	          |pula para o início do arquivo                                                                                  |
| ``fseek_to_end()``	          |pula para o final do arquivo                                                                                   |
| ``fsize()``	                  |retorna o tamanho do arquivo em bytes                                                                          |
| ``ftell()``	                  |retorna o offset corrente do arquivo                                                                           |
| ``run()``	                      |roda um processo externo, retornando seu exit code                                                             |
| ``echo()``	                  |escreve texto na tela                                                                                          |
| ``env()``	                      |retorna o conteúdo de uma variável de ambiente                                                                 |
| ``prompt()``	                  |lê dados do teclado, esperando a confirmação via "ENTER"                                                       |
| ``exit()``	                  |aborta a máquina virtual do hefesto, alterando o error level para o valor fornecido                            |
| ``os_name()``	                  |retorna o nome da plataforma corrente                                                                          |
| ``get_option()``	              |retorna dados de uma opção fornecida via linha de comando                                                      |
| ``make_path()``	              |cria uma string que corresponde a um caminho no sistema de arquivos                                            |
| ``last_forge_result()``	      |obtém o exit code do último processo de forja executado                                                        |
| ``forge()``	                  |invoca um outro projeto de forja                                                                               |
| ``byref()``	                  |atualiza os valores de um argumento de função para a variável externa que foi passada como esse argumento local|
| ``time()``	                  |retorna uma string representando o tempo do sistema de acordo com o formato passado                            |
| ``setenv()``	                  |cria uma variável de ambiente                                                                                  |
| ``unsetenv()``	              |remove uma variável de ambiente                                                                                |
| ``call_from_module()``	      |chama uma função implementada em uma biblioteca dinâmica                                                       |
| ``get_func_addr()``	          |obtém o endereço de um nome de função passado                                                                  |
| ``call_func_addr()``	          |tenta fazer uma chamada de função à partir do endereço passado                                                 |

#### Um guia prático para cada *Hefesto syscall*

Aqui nessa parte você pode explorar melhor como usar cada ``syscall``, vendo que argumentos recebem e o que retornam. Tudo isso de um modo mais prático.

##### ++replace_in_file()++

Substitui dados em um arquivo. O primeiro argumento deve ser o ``file path``, o segundo é o padrão de busca expresso por uma ``regex``, o terceiro é o texto de substituição desejado.

		function replace_in_file_sample() : result type none {
 			hefesto.sys.replace_in_file("test.txt", "^foo.*", "all initial foo to bar");
		}
##### ++lines_from_file()++

Filtra linhas de um arquivo. O primeiro argumento deve ser o ``file path``, o segundo é o padrão de busca expresso por uma ``regex``. A ``syscall`` retorna uma lista contendo as linhas filtradas do arquivo.

		function lines_from_file_sample() : result type none {
 			var retval type list;
 			$retval = hefesto.sys.lines_from_file("test.txt", "^foo.*");
		}

##### ++ls()++

Lista arquivos no diretório corrente. Recebe somente um argumento que deve ser o padrão de listagem em ``regex``.

		function ls_sample() : result type none {
 			if (hefesto.sys.ls(".*(c|cpp|s|asm)$") > 0) {
  				hefesto.sys.echo("Hey Beavis, Huh cool files to delete here!! Yeah yeah! Butt-head remove them! remove them!\n");
 			} else {
  				hefesto.sys.echo("ohh, nothing to screw up here.\n");
 			}
		}

##### ++pwd()++

Retorna o diretório corrente. É muito ``DIFÍCIL`` de usar essa função ``builtin``, dê uma olhada:

		function pwd_only_to_PHDs() : result type none {
 			hefesto.sys.echo("The current work directory is: " + hefesto.sys.pwd() + "\n");
		}

##### ++cd()++

Muda o ``cwd`` para o diretório fornecido via argumento.

		function cd_sample() : result type none {
 			hefesto.sys.cd("/home/rs");
		}

##### ++rm()++

Remove um arquivo.

		function rm_sample() : result type none {
 			hefesto.sys.rm("some.lint~");
		}

##### ++cp()++

Copia arquivo/diretórios para o diretório especificado. O primeiro argumento é o padrão regex correspondente ao source, o segundo é o destino.

		function cp_sample() : result type none {
 			hefesto.sys.cp(".*.(exe|dll|msi)", "wpkg");
		}

##### ++mkdir()++

Cria o diretório sob o diretório de trabalho.

		function mkdir_sample() : result type none {
 			hefesto.sys.mkdir("wpkg");
		}

##### ++rmdir()++

Remove um diretório vazio.

		function rmdir_sample() : result type none {
 			hefesto.sys.rmdir("stage");
		}


##### ++fopen()++

Abre um arquivo retornando o descritor que pode ser manipulado. O primeiro argumento é o ``file path``, o segundo é o modo de abertura. Os modos seguem o ``fopen`` clássico da ``libc`` ("r" = somente leitura, "w" = somente escrita, "a" = modo ``append``).

		function fopen_sample() : result type none {
 			var fp type file;
 			$fp = hefesto.sys.fopen("test.dat", "a");
		}


##### ++fwrite()++

Escreve dados em um descritor de arquivo aberto com modo de escrita ou ``append``. O primeiro argumento é o ``buffer`` de dados, o segundo é o quanto escrever desse ``buffer``, o terceiro é o descritor de arquivo.

Retorna a quantidade de dados escritos no arquivo.

		function fwrite_sample() : result type none {
 			var fp type file;
 			$fp = hefesto.sys.fopen("test.dat", "a");
 			var buf type string;
 			$buf = "that's all folks for fwrite!\n";
 			hefesto.sys.fwrite($buf, $buf.len(), $fp);
		}

##### ++fread()++

Lê dados de um descritor de arquivo (óbvio: aberto em modo de leitura) avançando o ponteiro de arquivo por *n* ``bytes`` lidos. O primeiro argumento é o ``buffer`` de dados, o segundo é a quantidade que deve ser lida à partir do arquivo, o terceiro é o descritor.

Retorna a quantidade de dados lidos do arquivo.

		function fread_sample() : result type none {
 			var fp type file;
 			$fp = hefesto.sys.fopen("test.dat", "r");
 			var byte type string
 			hefesto.sys.fread($byte, 1, $fp);
		}


##### ++fclose()++

Fecha/limpa os recursos internos associados ao descritor de arquivo. Recebe o descritor que deve ser fechado. É importante sempre chamar esta função depois de uma manipulação de arquivo. Seja higiêncio(a)!

		function fclose_sample() : result type none {
 			var fp type file;
 			$fp = hefesto.sys.fopen("test.dat", "w");
 			# TODO: do some manipulation here...
 			hefesto.sys.fclose($fp);
		}

##### ++feof()++

Retorna *1* se o final do arquivo foi alcançado de outra forma *0*. Recebe o descritor de arquivo que deve ser verificado.

		function feof_sample() : result type none {
 			var fp type file;
 			$fp = hefesto.sys.fopen("test.dat", "r");
 			if (hefesto.sys.feof($fp) == 1) {
  				hefesto.sys.echo("file end.\n");
			} else hefesto.sys.echo("NOT file end.\n");
 				hefesto.sys.fclose($fp);
			}
        }

##### ++fseek()++

Pula para um ``offset`` específico do arquivo do início ao fim. O primeiro argumento é o descritor de arquivo, o segundo é o ``offset`` desejado.

		function fseek_sample() : result type none {
 			var fp type file;
 			$fp = hefesto.sys.fopen("test.dat", "r");
 			hefesto.sys.fseek($fp, 10);
 			hefesto.sys.fclose($fp);
		}

##### ++fseek_to_begin()++

Pula para o início do arquivo. Recebe o descritor associado como argumento.

		function fseek_to_begin_sample() : result type none {
 			var fp type file;
 			$fp = hefesto.sys.fopen("test.dat", "r");
 			hefesto.sys.fseek_to_begin($fp); # yes this is a very useless dummy sample...
 			hefesto.sys.fclose($fp);
		}

##### ++fseek_to_end()++

Pula para o final do arquivo. Recebe o descritor associado como argumento.

		function fseek_to_end_sample() : result type none {
 			var fp type file;
 			$fp = hefesto.sys.fopen("test.dat", "r");
 			hefesto.sys.fseek_to_end($fp);
 			hefesto.sys.fclose($fp);
		}

##### ++fsize()++

Retorna os ``bytes`` totais de um arquivo associado ao descritor de arquivo passado.

		function fsize_sample() : result type none {
 			var fp type file;
 			$fp = hefesto.sys.fopen("test.dat", "r");
 			hefesto.sys.echo("The has " + hefesto.sys.fsize($fp) + "byte(s).\n");
 			hefesto.sys.fclose($fp);
		}

##### ++ftell()++

Retorna o ``offset`` corrente do descritor de arquivo.

		function ftell_sample() : result type none {
 			var fp type file;
 			$fp = hefesto.sys.fopen("test.dat", "r");
 			hefesto.sys.echo("The current offset should be 0 and is " + hefesto.sys.ftell($fp) + ".\n");
 			hefesto.sys.fclose($fp);
		}

##### ++run()++

Roda um processo externo retornando o ``exit code``. A chamada ``run`` é sempre bloqueante.

		function sync_run_sample() : result type none {
 			hefesto.sys.echo("ls /dev exits with " + hefesto.sys.run("ls /dev") + " exit code.\n");
		}

Você pode também rodar *n* processos se você passar para essa ``syscall`` uma lista contendo as linhas de execução. O número de processos concorrentes é controlado pela opção de linha de comando ``--qsize=n``. Nesse modo, a ``syscall run`` retorna a soma dos ``exit codes``.

		function async_run_sample() : result type none {
 			var wqueue type list;
 			$wqueue.add_item("ls /dev");
 			$wqueue.add_item("ls /home/rs");
 			$wqueue.add_item("echo \"duh!\"");
 			hefesto.sys.echo("The work queue exit code is " + hefesto.sys.run($wqueue) + ".\n");
		}

##### ++echo()++

Exibe uma ``string`` na tela. De fato é uma ``Hefesto syscall`` muito complicada.

		function echo_sample() : result type none {
 			hefesto.sys.echo("Hello world.\n");
		}

#### ++env()++

Obtém o conteúdo de uma variável de ambiente.

		function env_sample() : result type none {
 			hefesto.sys.echo("The current content of Windows PATH variable is = \"" + hefesto.sys.env("PATH") + "\"\n");
		}

##### ++prompt()++

Lê um valor do teclado. Retornando os dados lidos.

		function prompt_sample() : result type none {
 			var usr_data type string;
 			$usr_data = hefesto.sys.prompt("Type about an idea and then press enter to confirm this: \n");
 			hefesto.sys.echo("Hey I had an idea: " + $usr_data + "\nWhat do you think?");
		}

##### ++exit()++

Aborta a execução da ``HVM`` saindo com o código passado.

		function exit_sample() : result type none {
 			hefesto.sys.exit(1);
		}

##### ++os_name()++

Retorna o nome do sistema operacional onde seu ``HSL`` está correndo. O retorno é sempre o nome em ``lower-case`` e sem qualquer informação de versão.

		function env_sample() : result type none {
 			if (hefesto.sys.os_name() == "windows") {
  				hefesto.sys.echo("The current content of Windows PATH variable is = \"" + hefesto.sys.env("PATH") + "\"\n");
 			}
		}

##### ++get_option()++

Obtém um argumento da linha de comando. Retorna uma lista contendo um ou mais valores ou uma lista vazia.

		function get_option_sample() : result type none {
 			var cups_of_tea type list;
 			$cups_of_tea = hefesto.sys.get_option("cups-of-tea");
 			if ($cups_of_tea.size() == 0)
  				hefesto.sys.echo("--cups-of-tea option not supplied.\n");
 			else
  				hefesto.sys.echo("--cups-of-tea option supplied.\n");
		}

##### ++make_path()++

Cria uma ``string path``. O primeiro argumento é o ``root path``, o segundo é o *"addendum"* do ``path``. Retorna a combinação correta.

		function make_path_sample() : result type none {
 			hefesto.sys.echo("STRING PATH must be /home/rs \"" + hefesto.sys.make_path("/home", "/rs") + "\"\n.");
		}

##### ++last_forge_result()++

Obtém o último resultado de forja (este resultado pode ser alterado pela ``syscall exit``).

		function last_forge_result_sample() : result type none {
 			hefesto.sys.echo("The last forge result was " + hefesto.sys.last_forge_result() + ".\n");
		}

#### ++forge()++

Invoca um projeto de forja. O primeiro argumento é o nome do projeto contido no ``HSL`` informado via primeiro argumento. O terceiro argumento são as opções de usuário.

		function forge_sample() : result type none {
 			hefesto.sys.forge("foolib", "local_deps.hsl", "--user-includes-home=../alt_incs --pre-submit-me");
 			if (hefesto.sys.last_forge_result() == 0) {
  				hefesto.sys.echo("foolib built with success.\n");
 			} else {
  				hefesto.sys.echo("unable to build foolib.\n");
  				hefesto.sys.exit(1);
 			}
		}

##### ++byref()++

Atualiza os valores de um argumento local de função para uma variável externa que foi passada como esse argumento. Deve ser passada a variável local exata que deve ser atualizada para a variável externa.

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

##### ++time()++

Retorna uma ``string`` representando o tempo do sistema de acordo com o formato passado. Os formatos aceitos são os mesmos aceitos pela função ``strftime()`` contida na ``libc`` de seu ``OS``.

		function time_sample() : result type none {
 			hefesto.sys.echo("Current time: " + hefesto.sys.time("%H:%M") + "\n");
		}

##### ++setenv()++

Cria uma variável de ambiente. Após o término do processo a variável é perdida.

		function setenv_sample() : result type none {
 			hefesto.sys.setenv("mytempvar", "mytempval");
		}

No ``Windows`` você pode utilizar essa ``syscall`` para acessar o registro também, bastando informar o caminho completo onde deseja salvar o valor, precedendo essa informação pelo marcador ``WINREG:``.

		function setenv_sample() : result type none {
 			hefesto.sys.setenv("WINREG:HKCU\\Software\\Abc\\mytempvar:REG_SZ", "mytempval");
		}

##### ++unsetenv()++

Remove uma variável de ambiente. O processo de remoção somente afeta o processo referente a aplicação.

		function unsetenv_sample() : result type none {
 			hefesto.sys.unsetenv("VSCOMPILERPATH");
		}


No ``Windows`` você pode utilizar essa syscall para remover valores do registro também (essa remoção é permanente). Da seguinte forma:

		function unsetenv_sample() : result type none {
 			hefesto.sys.unsetenv("WINREG:HKLM\\Software\\Abc\\mytempvar");
		}

##### ++call_from_module()++

Chama uma função implementada numa biblioteca dinâmica.

O módulo precisa conter funções implementadas utilizando uma estrutura de dados especial. Mais informações sobre isso podem ser encontradas no documento destinado a especificar os detalhes de implementação de módulos para o hefesto.

		function call_from_module_sample() : result type none {
 			hefesto.sys.call_from_moduke("/usr/share/mymods/my_killer_mod.so", "killer_func", "arg1", "arg2", 3);
		}

Você pode usar indireção de paths para localizar um módulo, caso não queira indicar o ``path`` completo para ele em seu código ``HSL``:

		function call_from_module_sample() : result type none {
 			hefesto.sys.call_from_module("~/my_killer_mod.so", "killer_func", "arg1", "arg2", 3);
		}

Nesse exemplo apresentado, a biblioteca ``"my_killer_mod.so"`` será procurada nos ``paths`` configurados na variável de ambiente ``HEFESTO_MODULES_HOME``.

##### ++get_func_addr()++

Obtém o endereço de um nome de função informado.

		function get_func_addr_sample() : result type int {
 			result hefesto.sys.get_func_addr("dummy_clbk");
		}

##### ++call_func_addr()++

Tenta fazer uma chamada de função à partir de um endereço específico. Se qualquer erro ocorrer no processo um erro de tempo de execução é disparado.

		function call_func_addr_sample() : result type int {
 			var clbk_p type int;
 			$clbk_p = hefesto.sys.get_func_addr("abc_one_two_three");
 			result hefesto.sys.call_from_addr($clbk_p, "abc", 1, "two", 3);
		}

### Os facilitadores dos tipos *list* e *string*

Talvez você fique tentado(a) em chamar os facilitadores de métodos, porém, a ``HSL`` não é uma Linguagem orientada à objetos, de forma que os facilitadores podem ser entendidos como operações comuns de serem feitas sobre esses tipos de dados e que por serem comuns por motivos de performance foram implementadas de forma *builtin* na ``HSL``.

A **Tabela 5** traz uma listagem dos facilitadores presentes no tipo ``string``. A **Tabela 5** reúne os facilitadores implementados no tipo ``list``.

**Tabela 5**: Os facilitadores ``string-type``.

| **Facilitador** | **Utilidade**                                 |**Exemplo de chamada**         |
|:---------------:|----------------------------------------------:|:-----------------------------:|
|   ``at``        | Retorna o *byte* presente no índice fornecido | $str.at(0)                    |
|   ``len``       | Retorna o tamanho da *string*                 | $str.len()                    |
|   ``match``     | Busca por um padrão *regex* dentro da *string*| $str.match(".*")              |
|   ``replace``   | Substitui um padrão *regex* dentro da *string*| $str.replace(".*", "nothing") |

**Tabela 6**: Os facilitadores ``list-type``.

| **Facilitador**  | **Utilidade**                                                     | **Exemplo de chamada**                  |
|:----------------:|------------------------------------------------------------------:|:---------------------------------------:|
| ``item``         | Retorna o item presente no índice passado                         | $lst.item(0)                            |
| ``count``        | Retorna o total de itens presentes na lista                       | $lst.count()                            |
| ``add_item``     | Adiciona um item                                                  | $lst.add_item("1")                      |
| ``del_item``     | Remove um item baseado no conteúdo passado                        | $lst.del_item("1")                      |
| ``del_index``    | Remove o item presente no índice passado                          | $lst.del_index(1)                       |
| ``ls``           | Carrega os *paths* de arquivos que se encaixam na *regex* passada | $lst.ls(".*\\.c$")                      |
| ``clear``        | Limpa a lista                                                     | $lst.clear()                            |
| ``index_of``     | Retorna o índice de um conteúdo passado ou -1                     | $lst.index_of("1")                      |
| ``swap``         | Altera a posição de dois items baseados nos seus índices          | $lst.swap(0,9)                          |

#### Exemplos usando esses facilitadores

Seguem alguns:

	function get_filepaths_from_directories(dir type list, regex type string) :
    															return type list {
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

Exemplo de chamada:
>``var dir type list;``
>``var os_name type string;``
>``$os_name = hefesto.sys.os_name();``
>``$dir.add_item(".");``
>``$dir.add_item("native/" + $os_name);``
>``$dir.add_item("native/" + $os_name + "/fs");``
>``$dir.add_item("native/" + $os_name + "/net");``
>``$dir.add_item("native/" + $os_name + "/mm");``
>``$dir.add_item("native/" + $os_name + "/ps");``
>``$dir.add_item("common/");``
>``$dir.add_item("common/fs");``
>``$dir.add_item("common/net");``
>``$dir.add_item("common/mm");``
>``$dir.add_item("common/ps");``
>``var files type list;``
>``$files = get_filepaths_from_directories($dir, "*.\\.c$");``


	function piglatinize(said type string) : result type string {
    	var s type int;
        var etvalray type string;
        var punctoken type list;
        var curr_symbol type string;
        var curr_word type string;

		if ($said.match("[0123456789]") == 1) {
        	hefesto.sys.echo("ANICPAY: I can't handle numbers... aieeeeeeee!!\n");
            result "(ullnay)"; # now way: "nil" is not piglatin!!!! :D
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
                } else {}
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

Exemplo de chamada:

>``hefesto.sys.echo(piglatinize("I like to speak pig latin.") + "\n");``

### Tem uma forma fácil de rodar esses exemplos e brincar com a Linguagem para eu me habituar?!

Sim, claro! Quando você instala o ``Hefesto`` juntamente com os ``toolsets`` padrão ele instala um ``toolset`` oco, a função de forja apenas retona o número que você passa na declaração do projeto. Com isso, você pode chamar o que quiser dentro dos ``entry-points`` do projeto que criar baseado nesse ``toolset-stub``.

### Onde posso continuar explorando a *HSL*?

A ``HSL`` possui um repositório próprio de códigos chamado [Helios](https://github.com/rafael-santiago/helios.git), ele inclusive foi baixado junto com o repositório do ``Hefesto`` em sua máquina e uma parte que julgo básica dele já está instalada. Por padrão, ele traz documentações acerca de seus módulos. É uma boa dar uma lida nesses documentos caso queira saber mais sobre funções de apoio que você já possui à sua disposição. Em termos práticos, o ``Helios`` pode ser entendido como o diretório de ``includes`` do ``Hefesto`` criado na cópia que você instalou.

### Tirando as rodinhas: criando *Hefesto toolsets* e dominando completamente o *build-system*

Se você leu as outras seções e tentou rodar os exemplos, falta um passo para você se tornar um usuário avançado do ``Hefesto``, porque até o momento você está apto(a) em automatizar passos no sua própria *build-task* e usar os ``toolsets`` "de fábrica". O que te garante o ``status`` de usuário final estando ainda dependente de um ``toolset devel``.

Nisso, eu te convido aprender como se implementa um ``Hefesto toolset``. Se você até o momento está gostando da proposta do aplicativo, após as informações contidas nessa seção, talvez goste mais e se sinta motivado(a) em criar extensões para ele, levando em consideração suas próprias necessidades (o que é uma das principais linhas mestras desse projeto. *Liberdade para criar*. *Clareza em se expressar*. *Generalidade ao usar*.).

#### Mas antes: uma breve recaptulação em tudo que já foi visto

O que você já conferiu e/ou já sabe:

- O ``Hefesto`` é um *build system* que busca dar clareza e liberdade para escrever de forma organizada uma *build-task*.
- Ele implementa um *dsl* que é usada tanto nas extensões que podem ser feitas em uma *build-task* específica quanto nas generalidades que englobam qualquer *build-task* do mesmo tipo, neste caso tais generalidades nele são denominadas ``toolsets``.
- Foi apresentada a parte da ``HSL`` que recai sobre a declaração de um projeto.
- Foram apresentados códigos exemplificando o reuso de ``toolsets`` previamente escritos.
- Também foram apresentadas diferentes formas de disparar o *build* (forja) de um projeto.
- Foi visto como monitorar mudanças nos arquivos que compõem um projeto.
- Uma visão geral, no contexto de usuário, sobre as ``dep-chains`` foi apresentada.
- Foi mostrado como incluir passos adicionais ao processo de forja, sendo usado para isso o exemplo da necessidade de se disparar e avaliar o resultado de *unit tests*.
- A possibilidade de se criar um *Forgefile* mais maleável cuja a escolha de um ``toolset`` específico é feita em *runtime*, também foi vista.
- Aspectos gerais sobre a ``HSL`` e seu subsistema ``sys`` foram detalhados.

O que você ainda não conhece e depois de ler essa parte poderá dominar:

- Como declarar um ``toolset``.
- O subsistema ``toolset``.
- Como internamente o ``Hefesto`` sabe que um arquivo mudou.
- O subsistema ``project``.
- Boas práticas para se criar e manter novos ``toolsets``.

#### Nossa meta: um *toolset GCC* para compilar e gerar *libs* e *aplicativos* escritos em *C*

Escolhi a Linguagem *C* por se tratar de uma Linguagem "compilada" e com dependências a serem resolvidas de forma externa ao compilador. Algumas Linguagens mais contemporâneas detectam essas dependências entre arquivos automaticamente, já a Linguagem *C* não, isso vai nos fazer pensar em muitos detalhes o que será um ótimo **batismo de fogo no Hades** ;) e aí? Pronto(a)?!

##### Descobrindo quem depende de quem...

Sim, seria bom que você para essa segunda parte trouxesse uma noção de *C*, mesmo que *newbie*. Estou levando em conta que o leitor dessa seção tem esse perfil.

Imagine um projeto em *C* cuja a função ``main`` é essa:

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

Em *C* os ``includes`` dizem muito sobre as dependências diretas que um arquivo possui em relação aos outros. Geralmente, arquivos incluídos usando aspas duplas recaem sobre arquivos locais ao projeto e arquivos que utilizam o padrão ``< ... >``
são arquivos externos ao projeto.

À partir disso, vamos compor uma função que chegará à conclusão da ``depchain`` de qualquer projeto *C* que use esses moldes clássicos de inclusão de códigos. Convenhamos que indicar dependências manualmente é muito chato, contudo, esteja avisado(a) que a busca por dependências entre arquivos é algo heurístico de modo que não existe uma solução geral e completa. Eu pesquisei um pouco além da conta sobre isso durante a escrita do ``Hefesto`` e foi algo que até certo ponto tirou meu sono.

Qual é melhor forma na ``HSL`` de serializar o conteúdo de um arquivo em linhas? Eu voto na ``syscall lines_from_file()``. Vou apresentar o código completo do ``c-scanner`` de dependências e depois discorro sobre o que achar relevante:

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

Sim, *parser* é algo trabalhoso por mais bobo que seja pois envolve dar suporte para algumas características humanas: bagunça, desmanzelo e falta de padrão.

As linhas:

		$exts.add_item("h");
    	$exts.add_item("hpp");
    	$exts.add_item("c");
    	$exts.add_item("C");
    	$exts.add_item("CC");
    	$exts.add_item("cc");
    	$exts.add_item("cpp");
    	$exts.add_item("CPP");

São responsáveis por registrar as extensões de arquivo relevantes de serem escaneados. Pelo fato de haver milhões de possibilidade de se nomear ``headers`` e ``implementation files`` precisamos de uma lista... Se o desenvolvedor usar ``Hpp`` e/ou ``Cpp`` hahah ele já era. Poderíamos também converter o ``filepath`` de um arquivo totalmente para minúsculas ou maiúsculas mas não quis fazer isso, se quiser, sinta-se livre.

Depois para cada extensão registrada, buscamos no diretório corrente via a função de lista ``ls()`` o seguinte padrão ``Regex``:

	$str = ".*\\." + $exts.item($e) + "$";

O que nas iterações poderá ser:

> ".*\\\\.h$"...
> > ".*\\\\.hpp$"...
> > > ".*\\\\.c$"...
> > > > ".*\\\\.C$"...

Cada arquivo encontrado passa possuir um registro na ``dep-chain`` (lembra da sintaxe dela? então...). Dessa forma ele terá o conteúdo monitorado pelo ``Hefesto``:

	$str = $files.item($f);
	$dep_chain = $dep_chain + $str + ": ";

Depois o conteúdo desse arquivo é varrido em busca de diretivas includes:

	$includes = hefesto.sys.lines_from_file($str, "#include.*$");

Note que não é uma boa ancorar a ``regex`` com o ``^``, pois pessoas tendem ser relapsas e digitarem ``includes`` nesse formato organizado:

>      #includes "aqui.h"
>               #include "lah.h"
>        /*vou incluir ali também*/ #include "ali.h"

Com cada linha include encontrada, é extraído o nome do arquivo e gerado um ``fullpath`` com esse nome extraído precedido pelo ``cwd``. Se esse arquivo for realmente acessível ele entra como uma dependência para o arquivo anteriormente registrado na ``dep-chain``:

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

Para os ``headers`` essa função aplica uma busca secundária pelos arquivos de implementação deles. Se existirem e forem acessíveis sob o contexto do ``cwd`` as implementações também serão dependências, no caso, implícitas, de nosso arquivo atualmente escaneado:

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

Depois disso, o arquivo atual já terá todas as possíveis dependências devidamente listadas na ``dep-chain`` o que apenas resta fechar a declaração de suas dependências:

	$dep_chain = $dep_chain + ";\n";

Isso é repetido com cada arquivo presente no ``cwd``, em outras palavras o diretório do projeto, ou o diretório atual de onde esse ``c-scanner`` de dependências foi chamado, note que é possível mudar o ``cwd`` com a ``syscall cd`` e então chamar o ``scanner``.

Após todos os arquivos processados a ``dep-chain`` composta retorna para o chamador:

		result $dep_chain;

A qual provavelmente será usada na seção ``dependencies`` presente na declaração de um projeto.

Caso queira, volte na listagem total do código comentado e tente digerí-lo um pouco mais. Até se sentir confiante sobre o que cada linha faz.

##### Criando o toolset

Um ``toolset`` precisa ser declarado. Na declaração de um ``toolset`` não apenas definimos por qual nome esse será chamado, mas também reunimos em sua declaração de forma geral tudo que ele fará usando ferramentas externas.

O que um ``toolset`` *C* precisa fazer que depende de ``tools`` externas?

- Compilar os códigos;
- Linkeditá-los;

Veja a declaração desse ``toolset``:

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

A sintaxe básica de declaração é:

>``toolset <string-name> forge function <hsl-function-name> : [forge helpers <helpers-list>] : <command-templates> $``

No exemplo apresentado, o ``toolset`` se chama: ``gcc-c-app``. Sua função de forja é ``gcc_c_binary_forge``, isso explica a diretiva ``include`` no início do exemplo, pois é nele que está definida a função de forja em questão. Os ajudantes de forja (funções ``HSL``) são: ``gcc_compile_list``, ``gcc_link_ofiles`` e ``gcc_mk_compilation_command``. Uma declaração de ``toolset`` sempre termina com o símbolo de final de fita (``$``).

Os ``templates de comando`` que esse ``toolset`` implementa são: ``compile_r``, ``compile_d``, ``link_shared`` e ``link_static``. Um template de comando nada mais é do que uma abstração de um comando externo que o seu ``toolset`` irá fazer. Segue a sintaxe de declaração dessa parte:

>``command <string-name> : <string-argument-list> < <command-line template> >``

Importante notar que um ``comando de toolset`` **sempre** recebe *strings* como argumento e retorna uma *string*, baseada em seu ``command-line template`` com os argumentos passados expandidos nele.

Existe uma regra bem estrita quanto a acessibilidade desses comandos de ``toolset``:

>Somente podem ser acessados pela função de forja do ``toolset`` ou pelas funções ajudantes de forja.

Mas agora, como acessar esses comandos de ``toolset``?

Muito simples, você os acessa à partir do subsistema ``toolset``. Imagine o ``toolset`` apresentado, para acessar o comando ``compile_r`` é necessário:
>(...)
>``hefesto.toolset.compile_r($source_path, $outfile_path, $includes, $cflags);``
>(...)

Uma boa abordagem no início da criação de um ``toolset`` é se perguntar quais comandos externos esse ``toolset`` necessitará executar. Acho que essa é a gênese de qualquer ``toolset``.

##### A função de forja

Talvez essa seja a parte mais trabalhosa do trajeto, pois é na função de forja que deve ser incutida a lógica do *build*. Levando em consideração que um ``toolset`` deve ser uma generalização de tarefas que usem os comandos externos do ``toolset`` para produzirem alguma coisa...

Segue a função de forja, depois da listagem, comentarei as partes que julgar relevantes:

	function gcc_c_binary_forge(SOURCES type list,
    							INCLUDES type list, CFLAGS type list,
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

Note que a função de forja tem a seguinte definição:

	function gcc_c_binary_forge(SOURCES type list,
    							INCLUDES type list, CFLAGS type list,
                                LIBRARIES type list,
                                LDFLAGS type list,
                                APPNAME type string) : result type int

Isso explica a sequência/tipo dos argumentos passados na declaração do projeto:

	var sources type list;
    var includes type list;
    var cflags type list;
    var libraries type list;
    var ldflags type list;

	project sample : toolset "gcc-c-app" : $sources, $includes, $cflags,
                                           $libraries, $ldflags, "sample" ;

Esse ``toolset`` possui de cara uma conveniência:

	if ($SOURCES.count() == 0) {
        if (hefesto.project.toolset() == "gcc-c-app") {
        	$SOURCES.ls(".*\\.c$");
        } else {
        	$SOURCES.ls(".*\\.(cc|cpp|CC|Cpp|CPP)$");
    	}
    }

Se for passada uma lista de códigos vazia, ele escaneia o diretório corrente por códigos *C* ou *CC*. Pelo fato da mesma função de forja ser usada por outro ``toolset`` (``gcc-cc-app``) responsável por criar aplicativos *C++*, verificamos o nome do ``toolset`` atualmente carregado, via o subsistema ``project``, se for "gcc-c-app" listamos todos os arquivos que terminem com ".c" no nome para dentro da lista ``SOURCES``. Caso contrário, buscamos por códigos *C++*.

Se mesmo assim ``SOURCES`` estiver vazia:

	if ($SOURCES.count() == 0) {
    	hefesto.sys.echo("\t(empty source list)\n");
        result 1;
    }

Terminamos aqui, alertando sobre a lista vazia e saindo com ``1`` para sinalizar erro e fazer com que o ``Hefesto`` quebre o processo de forja para que o usuário tome conhecimento que algo deu errado.

Mais algumas conveniências... às vezes por motivos de higiene não desejamos poluir o diretório de onde o *build* roda com arquivos objeto:

	var obj_output_dir type list;
    var obj_dir type string;

    $obj_output_dir = hefesto.sys.get_option("obj-output-dir");
    if ($obj_output_dir.count() > 0) {
    	$obj_dir = $obj_output_dir.item(0);
        mktree($obj_dir);
    } else {
    	$obj_dir = hefesto.sys.pwd();
    }

Nisso, verificamos se o usuário passou a opção ``--obj-output-dir``. Lembrando que a ``syscall get_option`` retorna uma lista. Se a lista retornada contiver pelo menos um item é porque algo foi realmente passado e neste caso pegamos o primeiro item da lista (que é um *path*, *full* ou relativo) e passamos para a função ``mktree()``, definida em ``fsutil.hsl``. Essa função cria um ``path`` caso esse não exista. Agora, se não foi passada a opção ``--obj-output-dir``, o diretório corrente é assumido como o *output-dir* para os arquivos objeto, ``hefesto.sys.pwd()`` nos garante isso.

Algo similar é feito com o diretório de saída para o binário que será criado com a forja, confira:

	$bin_output_dir = hefesto.sys.get_option("bin-output-dir");
    if ($bin_output_dir.count() > 0) {
    	$bin_output = $bin_output_dir.item(0);
        mktree($bin_output);
        $bin_output = hefesto.sys.make_path($bin_output, $APPNAME);
    } else {
    	$bin_output = $APPNAME;
    }

Agora é chegada a hora de converter as opções do compilador em lista para *strings*:

		#  GCC's include list option string
        $includes = gcc_mk_gcc_incl_str_opt($INCLUDES);
        $cflags = gcc_mk_raw_str_opt($CFLAGS); #  GCC's compile options

Essas funções serão detalhadas posteriormente, por hora, só é importante conhecê-las superficialmente.

Também é conveniente poder escolher a arquitetura do *target*, no caso a leitura de opção que será apresentada permite escolher entre um binário *32-bit* ou *64-bit*, confira:

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

Se o usuário passar ``--cpu-arch=32`` ou ``--cpu-arch=64``, será adicionada a opção ``-m32`` ou ``-m64`` na linha de comando que será diretamente repassada ao *GCC*:

	$cflags = $cflags + " -m" + $chosen_arch;

Essa opção do *GCC* especifica a arquitetura do *target*. Caso seja passado um valor diferente de ``32`` ou ``64``, a forja será interrompida retornando ``1`` que significa erro.

Agora entregamos o controle para a função que propriamente compilará cada código presente no projeto:

	#  compiling...
    hefesto.sys.echo("*** compiling...\n");

    $exit_code = gcc_compile_source_list($SOURCES, $includes, $cflags, $obj_dir); 

Mais para frente a função ``gcc_compile_source_list()``, será detalhada.

Nesse ``toolset`` existe uma variável global chamada ``gcc_c_project_forge_dirty_files_nr``, que guarda a contagem de arquivos efetivamente processados. Se após o final da compilação não houver nenhum arquivo processado, é feita a verificação se o *target* (o binário propriamente dito) existe. Caso não exista, a variável global de contagem é setada para ``1``.

    if ($gcc_c_project_forge_dirty_files_nr == 0 && isfile($bin_output) == 0) {
    	$gcc_c_project_forge_dirty_files_nr = 1;
    }

O motivo de setar essa variável é por conta de garantir que a *linkedição* será feita (mesmo sem nenhum arquivo processado, porém sem o binário devemos *linkeditar*):

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

Note que além da contagem de arquivos processados, a *linkedição* também depende de não ter havido nenhum erro na compilação, o ``$exit_code == 0`` na ``if-clause`` nos  garante isso.

Se necessário *linkeditar*:

- Os *file paths* correspondentes aos arquivos objeto são reunidos numa *string*;
- O mesmo é feito com as opções de *linker* passadas pelo usuário, na declaração do projeto de forja;
- Se a arquitetura foi explicitada adiciona a opção *GCC* correspondente nas opções de *linker*;
- Serializa também os file paths adicionais onde podem ser encontradas bibliotecas (isso foi passado pelo usuário na declaração do projeto de forja);
- Chama a função que realiza efetivamente a *linkedição*: ``gcc_link_ofiles()``.

Depois apenas um relatório de *status* baseado no *exit code*, para orientar o usuário:

	if ($exit_code == 0) {
    	hefesto.sys.echo("*** success.\n");
    } else {
    	hefesto.sys.echo("*** failure.\n");
    }

E por fim o retorno do exit code, o que é importante para que o ``Hefesto`` saiba que rumo a forja teve:

	result $exit_code;

Volte na listagem integral do ``source-code`` da função de forja e veja se consegue entender cada porção de código dela. É importante para prosseguir entender realmente tudo que está sendo feito.

##### Detalhando cada forge helper

Anteriormente foi detalhada a função de forja, contudo, ela usa um conjunto de funções de apoio, as quais aqui serão melhor detalhadas.


###### gcc_compile_source_list()

Das ajudantes de forja, talvez a mais "complexa". Por isso, vamos começar com ela.
Nessa função você verá como as alterações dos arquivos são monitoradas e que é de responsabilidade do desenvolvedor do ``toolset`` manter a sanidade dessa ``feature``.

Existem dois *branches* de execução, um recai sobre a compilação ++síncrona++ o outro sobre a compilação ++assíncrona++. Poderiam ter sido quebradas em duas funções, nada impede, mas aqui estão bifurcadas em *if-branches*.

Uma compilação **síncrona** pode ser entendida como **uma compilação de arquivo por ciclo**. Já uma compilação **assíncrona** pode ser entendida como ***n* compilações de arquivos por ciclo**.

Essa é a definição da função:

	function gcc_compile_source_list(sources type list,
    								 includes type string,
                                     cflags type string,
                                     obj_dir type string) : result type int

Recebe a lista de códigos que serão compilados, uma lista de diretórios de *includes*, uma lista com opções que deverão ser usadas na compilação de cada código-fonte e o *path* para o diretório onde os arquivos objeto deverão ser gerados. Retorna zero para processamento sem erro e diferente de zero caso alguma coisa não ocorra conforme esperado.

O início da função é esse:

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

Se passado uma lista de códigos vazia retorna zero logo de cara. Após isso é feita a leitura de uma opção reconhecida internamente pelo *Hefesto* que é a ``--qsize=n``.

Essa opção indica o total de processos concorrentes que a ``syscall run`` é capaz de criar por chamada assíncrona. Para chamar a ``run`` de forma assíncrona é preciso passar uma lista de comandos.

Se a opção ``--qsize`` for passada, a função guarda o valor passado nela para uso posterior. Logo depois:

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

...Lemos a opção ``compile-model``, se o usuário passou essa opção verifica se é **release** ou **debug**, sendo outro conteúdo quebra a forja saindo com *1*. Caso seja um dos dois valores reconhecidos, seta uma *flag* indicando a escolha, para uso posterior.

Agora vem a compilação propriamente dita:

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
                $exit_code =
                	hefesto.sys.run(
                      gcc_mk_app_compilation_command($curr_src_file,
													 $ofile,
                                                     $includes,
                                                     $cflags, $is_release
                      )
                    );
                if ($exit_code == 0) {
                    $not_compiled.del_item($sources.item($i));
                    hefesto.sys.echo("\t*** " +
                    		$sources.item($i) +
                            ": no errors found.\n");
                } else {
                    hefesto.sys.echo("\t*** " +
                    		$sources.item($i) + ": errors found!\n");
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

O *else* refere-se a parte assíncrona, por hora vamos abstraí-la e nos atermos a parte síncrona. Temos a seguinte inicialização:

	$exit_code = 0;
	$i = 0;
    $gcc_c_project_forge_dirty_files_nr = 0;
    $not_compiled = lsdup($sources);

A variável que guarda o *exit code* das compilações é zerada. O contador que percorrerá a lista de códigos "começará pelo começo". A *flag* que indica que algum arquivo foi efetivamente processado é *unsetada*. Depois usamos uma função presente em ``lsutil.hsl``. Essa função retorna uma cópia da lista passada para ela.

A lista ``not_compiled`` tem uma utilidade especial. É por meio dela que o *Hefesto* conseguirá saber qual arquivo realmente mudou entre a última forja e a atual.

Existem duas funções *builtin* no ``subsystem toolset``:

- ``hefesto.toolset.file_has_change(<file-path>)``
- ``hefesto.toolset.base_refresh(<untouched-files>)``

A ``file_has_change`` é bem direta:

>Recebe o *path* de um arquivo e retorna **1** caso esse **arquivo tenha mudado**, de outra forma **0**.

Já ``base_refresh()``:

>Recebe uma lista de *paths*, mas essa lista deve apenas conter os *paths* dos arquivos que **ainda não foram processados**.

O *workflow* de uso dessas funções geralmente se resume em invocar ``file_has_change`` para saber se o arquivo corrente mudou, se mudou, se aplica o processamento necessário. Realiza isso para cada arquivo. Ao final, chama apenas uma única vez a ``base_refresh``. Com essas duas ``toolset-functions`` o *Hefesto* é capaz de saber exatamente quem mudou desde a última forja.

Voltemos ao código:

	if ($qsize <= 1) {
        while ($i < $size && $exit_code == 0) {
            $ofile = $sources.item($i);
            $ofile = gcc_mk_ofile_name(filenamefrompath($ofile), $obj_dir);
            if (hefesto.toolset.file_has_change($sources.item($i)) == 1 ||
            	isfile($ofile) == 0) {
                $gcc_c_project_forge_dirty_files_nr =
                	$gcc_c_project_forge_dirty_files_nr + 1;
                $curr_src_file = $sources.item($i);
                $exit_code =
                	hefesto.sys.run(
                      gcc_mk_app_compilation_command($curr_src_file,
													 $ofile,
                                                     $includes,
                                                     $cflags, $is_release
                      )
                    );
                if ($exit_code == 0) {
                    $not_compiled.del_item($sources.item($i));
                    hefesto.sys.echo("\t*** " +
                    		$sources.item($i) +
                            ": no errors found.\n");
                } else {
                    hefesto.sys.echo("\t*** " +
                    		$sources.item($i) + ": errors found!\n");
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

Se a forja tiver que ser síncrona, i.e: se ``qsize`` ser igual ou menor a *1*, para cada caminho de arquivo (código *C*) presente na lista, enquanto não houver nenhum erro de compilação:

> À partir do path do arquivo usamos a função ``gcc_mk_ofile_name()`` para criar o nome objeto do arquivo, de fato apenas substituímos o ``.c`` por ``.o`` e juntamos esse novo nome com o conteúdo do caminho para o diretório de arquivos objeto.
>
> Verificamos se o arquivo ``.c`` teve alteração ou se seu arquivo objeto não existe.
>> Com uma dessas duas condições satisfeitas, incrementamos o contador de arquivos processados e rodamos o comando de compilação. Se o arquivo for compilado com êxito, removemos o *path* dele da lista ``not_compiled``.
>>
>> Se o arquivo ``.c`` não mudou e o seu objeto existe, apenas informa que não houveram mudanças naquele arquivo.

Após todos os códigos-fonte processados, se houve efetivamente pelo menos uma compilação, realiza o ``base-refresh`` e retorna o *exit code* do processo, para que o chamador tome a medida necessária baseada nesse valor retornado.

Agora vamos ver a parte assíncrona da compilação:

	else {
        $not_compiled_tmp.clear();
        while ($i < $size) {
            $ofile = $sources.item($i);
            $ofile = gcc_mk_ofile_name(filenamefrompath($ofile), $obj_dir);
            if (hefesto.toolset.file_has_change($sources.item($i)) == 1 ||
            	isfile($ofile) == 0) {
                $gcc_c_project_forge_dirty_files_nr =
                	$gcc_c_project_forge_dirty_files_nr + 1;
                $curr_src_file = $sources.item($i);
                $run_list.add_item(
                	gcc_mk_app_compilation_command($curr_src_file,
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

O comportamento é similar ao da porção síncrona. Uma diferença é que os comandos de compilação não são imediatamente executados, sendo eles adicionados a uma lista para execução ao final de um ciclo. Outra diferença é que aqui há uma lista auxiliar chamada ``not_compiled_tmp`` que é preenchida por ciclos e o ``base-refreh`` é feito ao final desses ciclos. Um cíclo é completo quando o tamanho de ``run_list``, que é a lista que contém os comandos de compilação ainda não executados, contiver o total de elementos igual a ``--qsize``. Se alguma compilação quebrar ao final de um ciclo, o *loop* será interrompido. Se ao final do *loop* não havendo erro de compilação, ainda houverem códigos por compilar, eles serão compilados e o ``base-refresh`` feito.

###### gcc_mk_app_compilation_command()

Essa função é responsável por **montar** os comandos de compilação que serão executados:

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

Note que são passados para ela todos os parâmetros necessários para se compilar um fonte no *GCC* com a opção ``-c`` (confira o *template* dos comandos ``compile_r`` e ``compile_d`` na declaração do ``toolset``). Note que fora isso, é também informado o tipo de compilação, se é desejado incluir informações de *debug* ou não (em *release* não incluíra). Logo após, apenas é retornado a linha de comando formatada, pois um ``toolset-command`` não é executado, cabe ao desevolvedor do ``toolset`` escolher a melhor maneira de executá-lo via a ``syscall run`` e avaliar o *exit code* de tal comando. Lembrando que todos os ``toolset-commands`` estão disponíveis para a função de forja e suas ajudantes à partir do subsistema ``toolset`` (``hefesto.toolset.<command>``). Ainda, tudo o que foi passado para o comando se resumiu em argumentos do tipo ``string``. Se existir um lema para um ``toolset-command``, pode ser esse:

>Somente recebe *strings* **na ordem e quantidade** pré-definidas, somente retorna **uma** *string*.

###### gcc_link_ofiles()

Essa função é responsável por *linkeditar* os arquivos objeto produzindo a aplicação sob o nome que ``appname`` contiver.

Note que o argumento ``objects`` já é a lista de objetos no formato *string* que o *GCC* espera, i.e: *"foo.o bar.o baz.o"*.

As variáveis ``libraries`` e ``ldflags`` a mesma coisa, listas no formato *string* que o *GCC* espera.

Lembre que na função de forja nos ocupamos de converter as listas ``HSL`` passadas pelo usuário do ``toolset`` para o "formato *string* do *GCC*".

A primeira coisa que essa função de *linking* faz é verificar se a opção ``--link-model`` foi passada pelo usuário. Essa opção admite dois modos de *linking*: ``static`` ou ``shared``. Se for passado um modo de *linkagem* alienígena a forja quebrará com saída *1*.

Se um dos modos reconhecidos foi passado (note que o modo padrão é *shared*), essa função monta o comando de *linking* necessário à partir do ``command-template`` correspondente, definido no ``toolset`` e o executa com a ``syscall run``. Retornando ao final o ``exit code`` do processo de *linkedição*.

Confira a listagem do código e repasse o que leu:

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
        	$exit_code = hefesto.sys.run(
            	hefesto.toolset.link_static($appname,
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

É isso, um bom exercício é tentar criar um ``toolset`` para automatizar algo para o seu estado de coisas. Não precisa pensar apenas um compiladores, você pode tentar criar um ``toolset`` que faça o *deploy* do seu pacote, entre outras coisas. Seja criativo(a)!