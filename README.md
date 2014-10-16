hand-gesture-recognition
========================

Hand Gesture Recognition using a Modification of the Growing Neural Gas Algorithm

### How to use the application

When the application starts, three windows are opened. The "track" window is displaying the current view of the camera. Once the target is selected, the GNG-T mesh will be displayed in this window. The "bin" window is displaying the probability for each pixel to belong to the target (it is inititally entirely black). Finally, the "Parameters" window contains trackbars that can be used to tune the parameters of the algorithm.

To select a target, raise your hand and select (with the mouse) a recangle INSIDE the hand. This will allow the program to compute an histogram of the target. This histogram is then used to compute the probability of each pixel to belong to the target, this probability is shown in the "bin" window and the tracking start immediately. You can repete the selection procedure as many time as you want.


### Compilation

There are multiple ways to run the above program:

1.  Type the following commands in the command line:
    
    g++ -std=c++11 -o classifier classifier.cpp gesture.cpp gestureexamples.cpp
    g++ -std=c++11 -O3 main.cpp tracking.cpp `pkg-config --cflags --libs opencv`
    ./a.out
    Two windows appear,'track' and 'skin'. In the 'track' window, perform the gesture, select a region of your skin(alternatively the code can also detect skin automatically), and press q.
    ./classifier ToBeClassified.csv features1.csv features2.csv features3.csv features4.csv features5.csv features6.csv features7.csv features8.csv features9.csv
    This will give the output as which feature(predefined gesture), the gesture you performed most closely resembles.

2. Type the following commands in the command line:
    
    g++ -std=c++11 -o classifier classifier.cpp gesture.cpp gestureexamples.cpp
    ./classifier Example.csv features1.csv features2.csv features3.csv features4.csv features5.csv features6.csv features7.csv features8.csv features9.csv
    The Example.csv already contains the features of a number of gestures that were performed by us,and the above command gives the output as the feature(predefined gesture) that most closely resembles the gestures in the Examples.csv file.

3. Type the following commands in the command line:

    cd recorders/
    g++ -std=c++11 -O3 recordFrames.cpp `pkg-config --cflags --libs opencv`
    ./a.out Trial
    A window will appear called 'img'. Perform the various gestures you wish to identify and press 's' to select one frame for every gesture. Press q when you have completed all the gestures.
    g++ -std=c++11 -O3 learning.cpp tracking.cpp `pkg-config --cflags --libs opencv`
    ./a.out recorders/Trial
    ./classifier recorders/Trail/features.csv features1.csv features2.csv features3.csv features4.csv features5.csv features6.csv features7.csv features8.csv features9.csv
    This will give the output as which features(predefined gestures), the gestures you performed most closely resemble.
    

    
