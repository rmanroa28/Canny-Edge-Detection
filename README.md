# Canny-Edge-Detection
Canny Edge Detection Pipeline in C++ (without using OpenCV). Requires that the input image is in P3 (ASCII) ppm format.

The program first loads the input PPM into a 2D vector. 

![Initial image.](empire_pngs/empire%20-%20Copy.png)

It then converts the image to a grayscale:

![Grayscaled image.](empire_pngs/empire_grayscale%20-%20Copy.png)

After that, it uses edgeDetector2() to compute gradients, magnitudes, threshold, angle quantization, and non-max suppression:

![Non-max suppressed image.](empire_pngs/empire_nonmax%20-%20Copy.png)

It then uses changeWeak() to perform Hysteresis (connect weak edges to strong edges):

![Hystersis image.](empire_pngs/empire_hystheresis%20-%20Copy.png)

Finally, it combines NMS and hysteresis to make the final image and renders the results to ppm files:

![Final image.](empire_pngs/empire_final%20-%20Copy.png)

Here's an example of how to run the program in the command line: 

``` g++ -std=c++17 -O2 -o canny_edge canny_edge.cpp ```

``` ./canny_edge -f empire.ppm -lt 150 -ht 200 -fg empire_gray.ppm -of empire_hystheresis.ppm -f2 empire_nonmax.ppm -ff empire_final.ppm ```

This is what each flag means:

-f   -  input file (default: "empire.ppm")
-lt  -  low threshold (defualt: 150)
-ht  -  high threshold (default: 200)
-fg  -  grayscale image output (default: "empire_gray.ppm")
-of  -  hystheresis image output (default: "empire_hystheresis.ppm")
-f2  -  nonmax image output (default: "empire_nonmax.ppm")
-ff  -  final image output (default: "empire_final.ppm")

All flags are optional.