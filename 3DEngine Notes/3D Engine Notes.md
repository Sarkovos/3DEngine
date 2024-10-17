
### Frame Buffer
- To make sure our colors are implemented properly, we define our values in our buffer as uint32_t, for all 32 bits of color info.
- we allocated the frame buffer (malloc) by taking the size of a color (sizeof(uint32_t)) and multiplying it by the total screen size (window_width * window_height)


### Coordinate System
- We start with a *left handed coordinate system*
- Our origin is in the top left of our screen, so y grows down


### 3D Models
- Order of the vertices for faces are wound clockwise to determine the front face

### Projections
- We have 2 modes of projection, *perspective* and *orthographic*, with 2 forms of perspective projection.
- There is one *horizontal FOV* implementation and one *vertical FOV* implementation of perspective projection.
	- In one case, it only needs the FOV in radians and the aspect ratio. 