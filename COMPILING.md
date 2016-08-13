# Building Hefesto

The easiest way to clone hefesto's repo is:

``git clone https://github.com/rafael-santiago/hefesto --recursive``

If you are a ``MINIX`` user you should read the remarks on ``Additional steps for cloning it on MINIX`` before going ahead.

If you are on ``Solaris`` maybe you should read the remarks on ``Pain avoidance when cloning it on Solaris``.

Now, in order to build Hefesto from the first time you need to use the "src/build.sh" or the "src/build.bat"

If you are on ``Linux``, ``FreeBSD``, ``MINIX`` or ``Solaris`` use: ./build.sh

If you are on ``Windows`` use: build.bat

Both commands will compile and linking the application, it will run the tests and so use the Hefesto to install the Hefesto ;)

The installation is pretty straightforward just follow the steps.

After installing you need to perform a new login in order to load some environment variables.

## Additional steps for cloning it on MINIX

If you are on ``MINIX``. You need to have a little more guts to accomplish the build...

The clone step is a little bit tricky because until now on MINIX 3.3.0 the ``git`` is compiled without ``https`` support.

This axiom will lead you to taste some pretty nice submodule cloning errors. But do not give up.

All you should do is to re-make the submodules using the ``git-protocol`` instead of ``https``.

So do the following, after cloning the main repo supposing that you have cloned it into a local directory called "hefesto":

```
# cd hefesto
# git submodule deinit helios
# git rm helios
# git submodule add git://github.com/rafael-santiago/helios helios
# git submodule deinit src/tests/unit/cutest
# git rm src/tests/unit/cutest
# git submodule add git://github.com/rafael-santiago/cutest src/tests/unit/cutest
# git submodule deinit src/here
# git rm src/here
# git submodule add git://github.com/rafael-santiago/here src/here
# cd src/here
# git submodule deinit src/test/cute
# git rm src/test/cute
# git submodule deinit src/test/cutest
# git submodule add git://github.com/rafael-santiago/cutest src/test/cutest
# cd ..
```

Yes, maybe I should include a ``shell script`` for doing this dull submodule re-making task. However, I think that this problem
on ``MINIX`` will be fixed soon. At least I expect. Besides, this "nice" script could result on nasty things like pull requests
with submodules using "git" protocol instead of ``http``. By the way, if you want to pull request me something, try to avoid
pulling it using a copy with the submodules schemas changed. And thank you in advance! ;)

Now returning to the main subject, once the submodules downloaded, call the ``./build.sh``. It will build and try to install
your Hefesto's copy. Congrats, well done, you deserve it! ;)

## Pain avoidance when cloning it on Solaris

I am assuming that you are using ``Solaris 10`` from factory. Yes, pretty old ``GCC``, no ``Git``.
If you have a newer ``Solaris`` version and ``git well-installed`` on your system, just go back to the start of this document
and follow the instructions from there. Otherwise, if you get an old ``Solaris``, instead of advising you to upgrade and patch
it by yourself, I will show you a way for cloning ``Hefesto's`` repo without ``git``, getting the used ``git-submodules``
without ``git`` and so on. Compiling and install the ``Hefesto`` without piss off anything in your stone age system.

In other words, the bootstrapping of the bootstrapping.

First of all you should have the following tools ``well-exported``:

- ``gcc``
- ``ar``
- ``wget``
- ``unzip``

and of course, your ``Solaris`` must have Internet access.

Now you should run everything under ``bash``. So, if you are not sure about being using it or not, into console call
``/bin/bash``.

The next step is to save the following data into a file, for example, let's call it ``sunglasses.sh``.

```bash
#!/bin/bash

# sunglasses.sh

LCM=".caveman-from-the-sun" # Larry, Curl and Moe
WGET="wget"
UNZIP="unzip"
HEFESTOREPOTREE="../hefesto"

${WGET} --version >/dev/null 2>&1

if [ $? -ne 0 ]
then
	echo "Unable to find WGET. What a mess you are in... omg!"
	exit 1
fi

${UNZIP} -v >/dev/null 2>&1

if [ $? -ne 0 ]
then
	echo "Unable to find ${UNZIP}. What a mess you are in... omg!"
	exit 1
fi

if [ -d ${LCM} ]
then
	rm -rf ${LCM}
fi

mkdir ${LCM}

cd ${LCM}

echo "blah: okay..."

${WGET} https://github.com/rafael-santiago/hefesto/archive/master.zip -O hefesto.zip --no-check-certificate > /dev/null 2>&1

if [ $? -ne 0 ]
then
	echo "wget ERROR"
	exit $?
fi

echo "blah: so..."

${WGET} https://github.com/rafael-santiago/helios/archive/master.zip -O helios.zip --no-check-certificate > /dev/null 2>&1

if [ $? -ne 0 ]
then
	echo "wget ERROR"
	exit $?
fi

echo "blah: Hocus pocus..."

${WGET} https://github.com/rafael-santiago/cutest/archive/master.zip -O cutest.zip --no-check-certificate > /dev/null 2>&1

if [ $? -ne 0 ]
then
	echo "wget ERROR"
	exit $?
fi

echo "blah: Abracadabra..."

${WGET} https://github.com/rafael-santiago/here/archive/master.zip -O here.zip --no-check-certificate > /dev/null 2>&1

if [ $? -ne 0 ]
then
	echo "ERROR"
	exit $?
fi

echo "blah: <Some lisp statement goes here>..."

${UNZIP} hefesto.zip -d hefesto > /dev/null 2>&1

if [ $? -ne 0 ]
then
	echo "unzip ERROR"
	exit $?
fi

${UNZIP} helios.zip -d helios > /dev/null 2>&1

if [ $? -ne 0 ]
then
	echo "unzip ERROR"
	exit $?
fi

${UNZIP} cutest.zip -d cutest > /dev/null 2>&1

if [ $? -ne 0 ]
then
	echo "unzip ERROR"
	exit $?
fi

echo "blah: Tchummmm!"

${UNZIP} here.zip -d here > /dev/null 2>&1

if [ $? -ne 0 ]
then
	echo "unzip ERROR"
	exit $?
fi

rm -rf ${HEFESTOREPOTREE}

cp -rf hefesto/hefesto-master ${HEFESTOREPOTREE}

if [ $? -ne 0 ]
then
	echo "hefesto copying ERROR"
	exit $?
fi

rm -rf ${HEFESTOREPOTREE}/helios
rm -rf ${HEFESTOREPOTREE}/src/here
rm -rf ${HEFESTOREPOTREE}/src/tests/unit/cutest

cp -rf helios/helios-master ${HEFESTOREPOTREE}/helios

if [ $? -ne 0 ]
then
	echo "helios copying ERROR"
	exit $?
fi

cp -rf here/here-master ${HEFESTOREPOTREE}/src/here

if [ $? -ne 0 ]
then
	echo "src/here copying ERROR"
	exit $?
fi

cp -rf cutest/cutest-master ${HEFESTOREPOTREE}/src/tests/unit/cutest

if [ $? -ne 0 ]
then
	echo "src/tests/unit/cutest copying ERROR"
	exit $?
fi

rm -rf ${HEFESTOREPOTREE}/src/here/src/test/cutest
cp -rf cutest/cutest-master ${HEFESTOREPOTREE}/src/here/src/test/cutest

if [ $? -ne 0 ]
then
	echo "src/here/src/test/cutest copying ERROR"
	exit $?
fi

cd ..

rm -rf ${LCM}

echo "blah: All download stuff is done your repo's copy is under 'hefesto' sub-directory."
```

After saving the ``shell script`` shown above, you need to give execution permission for it. Supposing that you save this
script in the ``CWD`` named as ``sunglasses.sh``:

```
# chmod +x sunglasses.sh
```

Now all you should do is to run it:

```
# ./sunglasses.sh
```

If no errors have occurred during the ``shitty-way-cloning`` process, you should jump to ``hefesto/src`` sub-directory and
run ``./build.sh``. Finally, follow the steps for installing and you will have got the ``Hefesto`` installed on your
``Solaris`` without losing your sanity. What by the way is the most important thing. I guess ;)

Well, if you are a such lazy to copy and paste the ``shitty-way-cloning`` script but not so lazy to type the following smaller
script onto your ``bash``:

```
l=1; ini=0; fini=0; cat COMPILING.md | while read LINE ; do if [ ${ini} = 0 ] && [ "${LINE}" = "\`\`\`bash" ];\
then ini=$[${l} +1]; elif [ "${LINE}" = "\`\`\`" ] && [ ${l} -gt ${ini} ] && [ ${ini} != 0 ];\
then fini=$[${l} -1]; sed -n "${ini},${fini}p" COMPILING.md > sunglasses.sh; chmod +x sunglasses.sh; exit 0;\
fi; l=$[${l} +1]; done
```

Lazy...

On this way, you can extract the ``sunglasses.sh`` from the ``COMPILING.md`` without copy and paste besides feeling like a
[circus bear on a bike](https://www.google.com/search?q=circus+bear+on+a+bike).

Crazy.
