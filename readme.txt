=== CS606 Assignment ===

Graphical application that reads in a .ply file, renders a polygonal mesh and implements lighting models on it


=== Youtube link ===

ttp://youtu.be/DPpfIbHWMFo

=== Installation ===

Install free-glut libraries. To do the same, at terminal punch in

$apt-get install freeglut3-dev

=== List of files ===

main.cpp
Utility.cpp
Utility.h
Makefile

=== How to run ===

1. Open your termial. 
2. Move into the src directory.
3. Type: make

$ make

4. Type: ./output to fire up the window

$ ./output

5. On the window opened, hover your mouse curser and click on any part window screen, you can observe position of light moving in that direction.

6. We have following features:

--Scaling--

Scaling is done using following keys   -

i – to scale up the scene
o – to  scale down the scene

--Translation--

Translation is done using following keys   -

w – to translate upward
s  – to translate to translate downward
a – to translate to left side
d – to translate to ride side 

--Rotation--

Rotation is done using following keys   -

r – to start/stop animated rotation (implemented using Quaternions)
t – to toggles model between clock wise direction and anti-clockwise direction
1, 2 - x-axis rotations control
3, 4 - y-axis rotations control
5, 6 - z-axis rotations control


-- DiscoLight --

v - to get flicker of light from the 4 corners.

-- CircularLight --

c - to get circular movement of light source.

-- Reset --

b - to reset.

-- Lighting Mode Selection -- 

p - to toggle between perVertex and perPloygon lighting.


=== ScreenShots ===

ScreenShots are avaliable in the screenshots folder.

=== References/help ===

1. Interactive Computer Graphics - A Top-Down Approach Using OpenGL by Edward Angel
2. www.stackoverflow.com
3. www.opengl.org
4. Tutorials of openGL on www.youtube.com
