# glfw-tutorial
Files for my little GLFW-tutorial series

Introduction
====
I'll write some sensible stuff here later but these are files related to a tutorial series on GLFW and related topics that I'm currently working on.
Note that these files will progress as the tutorial progresses. 
In the archive folder I'm placing zip files containing the source code as is after each tutorial.

Dependencies
====
This work is dependend on a number of other bits of source code.

First and foremost: http://www.glfw.org
GLFW is the cross platform framework we're using here. For convenience I've included the main header and compiled binaries in this repository but obviously I recommend you download the most recent source and compile it.

Another project I use is: http://glew.sourceforge.net/
GLEW is a library that handles calls to parts of OpenGL that are dependent on the OpenGL version or any differences between platforms and vendors.

And then: https://github.com/nothings/stb
STB is a collection of single file C libraries that do nice things like image loading. 

And finally (for now): https://github.com/memononen/fontstash
Fontstash is a single file C library using the font rendering logic in STB to output text to screen in OpenGL.
Note my own fork of this project as I've added support for rendering using VAOs and VBOs which is a prerequisit with OpenGL 3 on a Mac.

Links to articles
====
http://bastiaanolij.blogspot.com.au/p/glfw-tutorials.html
