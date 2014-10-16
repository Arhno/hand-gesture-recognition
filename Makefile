video:
	g++ -std=c++11 -O3 -o video main.cpp tracking.cpp `pkg-config --cflags --libs opencv`

classifier:
	g++ -std=c++11 -o classifier classifier.cpp gesture.cpp gestureexamples.cpp

recorder:
	g++ -std=c++11 -O3 -o recorders/recorder recorders/recordFrames.cpp `pkg-config --cflags --libs opencv`

learning:
	g++ -std=c++11 -O3 -o learning learning.cpp tracking.cpp `pkg-config --cflags --libs opencv`

clean:
	rm video
	rm classifier
	rm recorders/recorder
	rm learning