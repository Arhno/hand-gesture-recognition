hand-gesture-recognition
========================

CS-6601-AI Team 14 project 1 code

### How to use the application

When the application starts, three windows are opened. The "track" window is displaying the current view of the camera. Once the target is selected, the GNG-T mesh will be displayed in this window. The "bin" window is displaying the probability for each pixel to belong to the target (it is inititally entirely black). Finally, the "Parameters" window contains trackbars that can be used to tune the parameters of the algorithm.

To select a target, raise your hand and select (with the mouse) a recangle INSIDE the hand. This will allow the program to compute an histogram of the target. This histogram is then used to compute the probability of each pixel to belong to the target, this probability is shown in the "bin" window and the tracking start immediately. You can repete the selection procedure as many time as you want.

### The parameters

 - Number of epochs: controls how many time the GNG-T algorithm will be updated each frame. The higher the better ... at the price of computationnal time.
 
 - Taget: controls the density of node in the graph. Low target allows less "error" and induces a dense graph.
 
 - Number of samples: control the number of samples sent to the GNG-T algorithm at each epoch. Too low and the target may not be sufficiently sample, but higher values increase the computational time.
 
### Compilation

You need to have both opencv and boost (graph) installed.
Then 
```
g++ -std=c++11 -O3 -o testGngT main.cpp tracking.cpp `pkg-config --cflags --libs opencv`
```
Should do the trick.
