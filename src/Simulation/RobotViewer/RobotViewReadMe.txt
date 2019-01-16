Robot Viewer:

This is actually the robot simulation code that has been slightly altered to become a remote of the new simulation.

Currently its under development and a bit buggy, so you must launch and test the simulation code first (This ensures the heading and X and Y coordinates are put before this program gets it).  I'll fix this later.

After running and testing the simulation, Launch the robot viewer, it will not do anything but should open the OSG viewing window and a command prompt, type test tankrobot

In its current state the wheels can still turn to joystick and keyboard, but the position and attitude are now controlled by the test program.  Also some keys can control like 'w' forward 's' backwards 'a' rotate left 'd' rotate right... but these will soon be phased out of the GUI, once the encoders are online.

If you type test t... it will list all the tests currently available, you can see robots of the past here, but I have no idea what will work or not.   The real cool one is curivator... just type test by itself... now you really shouldn't run more than one robot at a time, as it may crash, I haven't been keeping up with instance managment in a long time... so just rerun if you want to swap robots.

For curivator, it shows ideas for simulation on the manipulator... use 'j' to bring bucket in 'k' to put it out 'l' to bring it down and ';' to bring it up (some serious trig to do this haha).  'u' and 'i' rotate the bucket angle and 'o' and 'p' open and close the clasp.  Surprisingly, the feel of this happens on the robot itself... well not Surprisingly, because so much time was spent ensuring the sensors match the virtual world with the real world!

---
OSG commands:
f3 toggle full screen--- there is a dang 4:3 16:9 stretch... I hate it, but still if you want full screen you can.
f5 toggle various OSG performance benchmark overlays
f9 some legacy debug out... prior to using smart dashboard
----

My plans for this are to get more remote access from the simulation to this viewer on the drive, and perhaps add some UI for this years robot as time allows.

As always type quit when done using it.

