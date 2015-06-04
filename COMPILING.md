Building Hefesto
----------------

After done a git clone from "https://github.com/rafael-santiago/hefesto", some submodules is also needed... so:

        git submodule update --init

Or in the moment of the hefesto's master cloning operation you should try:

        git clone --recursive https://github.com/rafael-santiago/hefesto

Now, in order to build Hefesto from the first time you need to use the "src/build.sh" or the "src/build.bat"

If you're on Linux/FreeBSD use: ./build.sh

If you're on Windows use: build.bat

Ok, I forgot... Before edit the header src/types.h, adjusting the macro HEFESTO_TGT_OS to your current platform.

The both commands will compile and linking the application, it will run the tests and so use the Hefesto to install the Hefesto ;)

The installation is pretty straightforward just follow the steps.

Construindo o Hefesto
---------------------

Depois de feito um git clone à partir de "https://github.com/rafael-santiago/hefesto", alguns submódulos são também necessários... então:

        git submodule init

E ainda...

        git submodule update

Ou no momento em que você for clonar o master do hefesto... pode tentar:

        git clone --recursive https://github.com/rafael-santiago/hefesto

Agora, para fazer o build do Hefesto da primeira vez você precisa usar o "src/build.sh" ou o "src/build.bat".

Se você está no Linux/FreeBSD use: ./build.sh

Se você está no Windows use: build.bat

Ok, eu esqueci... Antes edite o header src/types.h, ajustando a macro HEFESTO_TGT_OS para sua plataforma atual.

Ambos os comandos irão compilar e linkeditar a aplicação, irão rodar os testes e então usará o Hefesto para instalar o Hefesto ;)

A instalação é bem direta apenas siga os passos.
