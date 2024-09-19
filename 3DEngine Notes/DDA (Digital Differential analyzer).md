- Very basic line drawing algorithm
- We start with 2 points, $(x_1, y_1)$ and $(x_2, y_2)$, and for the sake of this explanation, we will assume that we are marching along the $x$-axis
- We have the equation of the line, $y = mx + b$, and we can have our one point to sample, in this case $x$
- Our equation of a line does not work in our *raster*, so we must draw our best approximation when the line intersects multiple pixels.
- Now, we simply plug in $x$, and round up or down to determine where to draw our pixel.


## DDA (pseudo code)
```c
/*This code works purely for a situation where deltaX is greater than deltaY*/

int deltaX = (x1 - x0);
int deltaY = (y1 - y0);

int side_length = abs(deltaX);

//Find how much we should increment in both x and y steps
float x_inc = deltaX / (float)side_length;
float y_inc = deltaY / (float)side_length;

float currentX = x0;
float currentY = y0;

for (int i = 0; i <= side_length; i++)
{
	draw_pixel(round(currentX), round (currentY));
	current_x += x_inc;
	current_y += y_inc;
}
```