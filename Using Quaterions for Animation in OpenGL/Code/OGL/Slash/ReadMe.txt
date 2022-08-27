Quaternion in OpenGL Demonstration Program  Feb 5, 1998
--------------------------------------------------------
v. 1.0

This is the sample application that accompanies the March 98
Game Developer magazine.  It is meant as a demonstration of
how use Quaternions in OpenGL.  This is the first article
of the two part series.  I really only prints out the values
in the status bar and allows you to toggle between Quaternions
and Euler angles.  The second part will deal more
with interpolating between Quaternions.  The Quaternion
SLERP code is in here if you want to play around with it.

Write to me if you have problems or questions and check
the web site or Game Developer's web site for updates.

Jeff Lander
jeffl@darwin3d.com
www.darwin3d.com/gamedev.htm				Feb 5, 1998
-----------------------------------------------------------

I know this code could be optimized for maximum performance
but it was written to be a clean example without a lot of
tricks.  It should be easy to learn and build from.

Here are the details.

I compiled the code with Visual C++ 5.0.  It has been tested
with Microsoft OpenGL, SGI OpenGL, Permidia 1 and 2 OpenGL
Drivers, Riva 128 (New Beta OGL Drivers), and Symetra Ultra FX Pro.

It should run on any OpenGL fully complient driver.  This
DOES NOT include the mini-QuakeGL driver for 3DFX.  3DFX
OpenGL does not support OpenGL in a window so will not work
with this application.

There are instructions in the Help/About dialog.

By default it is in "Use Quaternions" mode.  In this mode,
The current Euler angles of a bone are converted to Quaternions
then to Axis/Angle for use in OpenGL.  All the Quaternion code
is in Quaternion.cpp/.h.

So, for now it really doesn't do much, just shows that there is
no difference between using Quaternions and Euler angles in
OpenGL.  It also prints out the values at the bottom status
bar.

It also has an example of working with Vertex Arrays as well as
Display Lists.  I have been playing around with different variations
of interleaved arrays and found this a pretty good way to do things.

Have some fun.

Jeff