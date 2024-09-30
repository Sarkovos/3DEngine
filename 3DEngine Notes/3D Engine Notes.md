
### Frame Buffer
- To make sure our colors are implemented properly, we define our values in our buffer as uint32_t, for all 32 bits of color info.
- we allocated the frame buffer (malloc) by taking the size of a color (sizeof(uint32_t)) and multiplying it by the total screen size (window_width * window_height)


### Coordinate System
- We start with a *left handed coordinate system*


### 3D Models
- Order of the vertices for faces are clockwise to determine the front face