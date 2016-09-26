# Hefesto

``Hefesto`` is a multi platform build system which currently runs on ``Linux``, ``FreeBSD``, ``MINIX``, ``Solaris``, ``NetBSD``, ``OpenBSD`` and ``Windows``.

The main ``Hefesto's`` feature is allow a more programmatic build scripting.

In order to do it ``Hefesto`` uses his own language (more procedural than declarative).

Seeking:

- Clarity
- Organization
- Expansibility

In practice, you can avoid doing build tasks like this:

![Rube Goldberg Machine / Public Domain](https://upload.wikimedia.org/wikipedia/commons/a/a9/Rube_Goldberg%27s_%22Self-Operating_Napkin%22_%28cropped%29.gif "Collier's, September 26 1931 Rube Goldberg's Machine for Self-Operating Napkin / Public Domain")

This application can help not only a small amount of programmers who use some specific languages and toolsets. ``Hefesto`` helps on automating several tasks that go beyond just compiling and linking programs. It depends on your necessity and disposition to extend ``Hefesto``.

When you extend ``Hefesto`` it does not mean that you need to recompile the application. All is done via ``HSL``, the domain specific language which ``Hefesto`` implements.

The ``HSL`` was developed to be simple but robust.

In order to build this project follow the steps in [COMPILING.md](https://github.com/rafael-santiago/hefesto/blob/master/COMPILING.md).
