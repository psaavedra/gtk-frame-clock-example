# gtk-frame-clock-example

This code provides a foundation for understanding the integration of GTK with the Wayland display protocol for creating graphical applications. It demonstrates how to manage timing, frame synchronization, and rendering in a GTK-based application. Developers can build upon this code to create more complex graphical interfaces with enhanced features.

This example initializes a GTK window and a drawing area widget. Signal handlers are connected to the drawing area's `realize` and `draw` signals. Additionally, a tick callback is added to the drawing area for animation control.

It also gets the `GdkFrameClock` object, which is used for synchronization and timing of graphical operations. Various signal handlers are connected to this frame clock to monitor different stages of the rendering process.

## Usage Instructions:

1. Compile the code using a C compiler with GTK and Wayland development libraries installed.
2. Run the executable to launch the application.
3. The application window will display a continuously updating display of random colored rectangles.
4. Observe the console output for timing and synchronization information related to the rendering process.

## How to build:

```sh
mkdir build
cd build
cmake ..
make
```

## How to run the code

```
gdb ./gtk-frame-clock-example
```

## How to debug

```sh
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Debug  ..
make
```

```
gdb ./gtk-frame-clock-example
```

