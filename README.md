PieDock
=======

PieDock is a task bar and application launcher in shape of a pie menu.
It feels a little bit like the famous OS X dock in a circle.

Demo
----

To experience the idea in action, try the WebPie
[demo](http://markusfisch.de/WebPie) or watch this
[video](http://vimeo.com/21360384).

Requirements
------------

Requires a C++ compiler, headers and libraries of X11 and libpng.
Xft and the X Rendering Extension (to make use of a compositing manager)
are optional.

Runs on Linux, BSD and Solaris.

Installation
------------

Manual configure, compile and install:

	$ ./configure

Alternatively, for Gnome integration, run configure with:

	$ ./configure --enable-gtk

Or for KDE use:

	$ ./configure --enable-kde

To compile:

	$ make

Then as root:

	# make install-strip

Finally, you may want to invoke the setup script with normal privileges:

	$ sh/setup

Or manually copy res/piedockrc.sample to ~/.piedockrc and modify it to
meet your needs.

License
-------

PieDock is open source and licensed under the
[MIT license](http://www.opensource.org/licenses/mit-license.php).
