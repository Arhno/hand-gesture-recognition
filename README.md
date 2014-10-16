hand-gesture-recognition
========================

Hand Gesture Recognition using a Modification of the Growing Neural Gas Algorithm

### Files
Classification: Files related to classification module.

 - classifier.cpp
 - gesture.cpp
 - gesture.h 
 - gestureexamples.cpp
 - gestureexamples.h

Segmentation: Files related to the segmentation module.
 
 - haarcascade_frontalface_alt.xmlhandDetector.h
 - handDetector.h (range version)
 - handDetectorHist.h (histogram version, selection is a square)
 - handDetectorHist2.h (histogram version, selection is more precise)
 
Learning: File that use the segmentation and extraction module to extract features from sample gesture image and store them in csv files.

 - learning.cpp
 
Example application: Launch the camera so that you can see the gng-t graph on your hand. (use all the module except the classification one)
 
 - main.cpp
 
GNG-T: Files related to the features extraction module.

 - gngt.h
 - tracking.cpp
 - tracking.h
 - unionfind.h
 - param.txt
 - param_fast.txt

### Compilation
You can compile the different program using:

 - `make video` for the Example application
 - `make classifier` for the classification module
 - `make recorder` for the recoreder program
 - `make learning`for the learning pogram
 
`make clean` erase the executables.

We use the Boost graph library as well as opencv so you will need those library to be able to compile.

### Use the program

After compilation there are multiple ways to run the above program:

1.  `./video`

    Two windows appear,'track' and 'skin'. In the 'track' window, perform the gesture, select a region of your skin(alternatively the code can also detect skin automatically), and press q.
    
    `./classifier ToBeClassified.csv features1.csv features2.csv features3.csv features4.csv features5.csv features6.csv features7.csv features8.csv features9.csv`
    
    This will give the output as which feature(predefined gesture), the gesture you performed most closely resembles.

2. `./classifier Example.csv features1.csv features2.csv features3.csv features4.csv features5.csv features6.csv features7.csv features8.csv features9.csv`

    The Example.csv already contains the features of a number of gestures that were performed by us, and the above command gives the output as the feature(predefined gesture) that most closely resembles the gestures in the Examples.csv file.

3. `./recorders/recorder ./recorders/Trial`

    A window will appear called 'img'. Perform the various gestures you wish to identify and press 's' to select one frame for every gesture. Press q when you have completed all the gestures.
    
    `./learning recorders/Trial`
    
    `./classifier recorders/Trail/features.csv features1.csv features2.csv features3.csv features4.csv features5.csv features6.csv features7.csv features8.csv features9.csv`
    
    This will give the output as which features(predefined gestures), the gestures you performed most closely resemble.
    

    
