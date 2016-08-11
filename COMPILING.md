# Building Hefesto

The easiest way to clone hefesto's repo is:

``git clone https://github.com/rafael-santiago/hefesto --recursive``

If you are a ``MINIX`` user you should read the remarks on ``Additional steps for cloning it on MINIX`` before going ahead.

Now, in order to build Hefesto from the first time you need to use the "src/build.sh" or the "src/build.bat"

If you are on ``Linux``, ``FreeBSD`` or ``MINIX`` use: ./build.sh

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
