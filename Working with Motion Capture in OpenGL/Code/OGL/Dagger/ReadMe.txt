Dagger Motion Capture File Viewer		Jan 10, 1997
----------------------------------------------------
v. 1.02

Well, I got some time to work on this today so I merged in
my code that loaded the ASF Acclaim skeletal file.  I still need
to bring in the AMC motion file but that is pretty easy.  I hope
to do one evening this week so check the site later in the week.

Still need to merge in the BVH loader to the sample app.  That
is real easy, though and should only take a few hours when I
find the time.

The ASF code needed to have the OGL draw routine re-arranged a
bit to handle the hierarchy.  Pretty simple though.  See the
drawSkeleton routine in OGLView.cpp to see what I did.

Vast difference in scale of the BVA vs. Acclaim sample files made
it necessary to change the default camera positions and axis scales
for the two files.  I used the scale channel in the bone structure
for this since I wasn't using it for anything else.

That's it for today.  Someone ambitious may want to add a
Curve plotter for the animation channels.  It would be cool
to see all the animated channels plotted in an editable window.
That would be the basis for a motion smoother/editor.  Not
far from a real F-curve editor then.

Let me know if you want to do this but don't quite get it.


Dagger Motion Capture File Viewer		Dec 16, 1997
-----------------------------------------------------

Well the issue release snuck up on me fast.

This is the sample application that accompanies the Jan 98
Game Developer magazine.  It is meant as a demonstration of
how to load and display Motion Capture data via standard
file formats.

First off, I am pulling out this code from my existing tools
and converting them to a nice clean OpenGL application.  I am
a little behind in finishing this.  I have only converted the
BVA loader code.  I will convert my BVH and AMC loaders as
soon as I can and upload them to my website as well as
Game Developer.  In any case, this should be a great start to
working with motion data.

Thanks again to House of Moves and Biovision for providing
sample motion files to work with.  Write to me if you have
problems or questions and check the web site for updates.

Jeff Lander
jeffl@darwin3d.com
www.darwin3d.com/gamedev.htm				Dec. 18, 1997
-----------------------------------------------------------

I know this code could be optimized for maximum performance
but it was written to be a clean example without a lot of
tricks.  It should be easy to learn and build from.

Here are the details.

I compiled the code with Visual C++ 5.0.  It has been tested
with Microsoft OpenGL, SGI OpenGL, Permidia 1 and 2 OpenGL
Drivers, and Symetra Ultra FX Pro.

It should run on any OpenGL fully complient driver.  This
DOES NOT include the mini-QuakeGL driver for 3DFX.  3DFX
OpenGL does not support OpenGL in a window so will not work
with this application.

There are instructions in the Help/About dialog.

Use Frame/Load Animation to bring in a BVA file.  You can
use the VCR style controls to play it back.  Hold the SHIFT
with the mouse buttons to rotate the view or CTRL to
translate the view around.

The Hierarchy window (HierWin.CPP/H) can be used to select a Bone.
Double click on it to bring up the edit window.  This allows you
to change the bone transformation settings.  Playing the animation
loses those changes.

I started implementing an Add bone command but it is not complete
and commented out.

In the article I described using Windows Timers to play the animation.
When I finished my production tool, I found that it could not play
back as fast as I thought it should.  It turned out the WinTimer was
a real slowdown to the system.  So, I yanked it out and grabbed the
message handler and run the animation from there.  That improved
things greatly.  I know I could even get it a bit faster but it
wouldn't be as windows friendly.

Have some fun.

