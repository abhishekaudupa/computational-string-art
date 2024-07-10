# String Art Generation Algorithm

Copyright 2024 Abhishek A Udupa

This is a simple string-art generation algorithm I made with basic knowledge of mathematics and computer graphics and implemented using the C programming language, from scratch. I made a [series of videos](https://www.youtube.com/playlist?list=PLlJolu6iXPxzqXUK4kP6z5DFTGtxxpbhy) on YouTube explaining the process in detail and the code in this repository stays true to that tutorial.

![string-art-representative](https://raw.githubusercontent.com/abhishekaudupa/computational-string-art/main/string-art.png)

## What's here?

There are a couple of C files in the main directory and the archive corresponding to various YouTube videos in the series.

### 1. Grayscale Converter

The code in [`archive/grayscale.c`](https://github.com/abhishekaudupa/computational-string-art/blob/main/archive/grayscale.c) converts a given color BMP image to grayscale BMP image of the same size. I've explained it in detail [here](https://www.youtube.com/watch?v=tW110h9lacA&list=PLlJolu6iXPxzqXUK4kP6z5DFTGtxxpbhy&index=1&pp=iAQB).

### 2. Blank Image Generator

The code in [`archive/canvas_maker.c`](https://github.com/abhishekaudupa/computational-string-art/blob/main/archive/canvas_maker.c) takes an input BMP image and spits out another BMP image that is completely whitewashed. This corresponds to the [this](https://www.youtube.com/watch?v=Kqe-6xX3CoI&list=PLlJolu6iXPxzqXUK4kP6z5DFTGtxxpbhy&index=2&pp=iAQB) video.

### 3. Thread and Nail Generator

The code in [`archive/thread-generator.c`](https://github.com/abhishekaudupa/computational-string-art/blob/main/archive/thread-generator.c) takes an input BMP image (of same width and height. The program doesn't check this though) and generates another BMP image of the same size which has all possible threads strung between all possible nail-pairs. This file has various functions whose working is explained in details in [part-2](https://www.youtube.com/watch?v=Kqe-6xX3CoI&list=PLlJolu6iXPxzqXUK4kP6z5DFTGtxxpbhy&index=2&pp=iAQB), [part-3](https://www.youtube.com/watch?v=DGwA0A0k_9k&list=PLlJolu6iXPxzqXUK4kP6z5DFTGtxxpbhy&index=3&pp=iAQB) and [part-4](https://www.youtube.com/watch?v=6zLvAqVu0jQ&list=PLlJolu6iXPxzqXUK4kP6z5DFTGtxxpbhy&index=4&pp=iAQB) of the series.

### 4. The String Art Generator

The code in [`string-art.c`](https://github.com/abhishekaudupa/computational-string-art/blob/main/string-art.c) is of our primary interest as it contains the algorithm for generating the string art. This takes a BMP input image (of equal width and height) and outputs its string art representation. This file corresponds to this [video](https://www.youtube.com/watch?v=lFmN3hi2ji0&list=PLlJolu6iXPxzqXUK4kP6z5DFTGtxxpbhy&index=6&pp=iAQB), in which I've given a thorough explanation of its working.

This part is still a work in-progress: I'm yet to show how to optimize the program and to make it converge faster and better. I'm planning to do that in part-6.

## More Info

None of the programs in this repo follow safe programming techniques such as bounds checking, null checking etc and *will* crash upon encountering unexpected inputs (if you do not pass the name of the image as an argument, for example). Since I've wrote these programs with the intent to demonstrate the working of the algorithm, I did not bother writing secure code. If you're planning to use this in an unsafe environment, I highly suggest you fortify your programs.
