- Developed by Jack Elton Bresenham in 1962 at IBM.
- Was presented at the 1963 ACM national convention in Denver, but was eventually published by the IBM Systems Journal in 1965.
- This paper is called **Algorithm for computer control of a digital plotter**
- The key point is that unlike DDA, Bresenham's algorithm has no floating point arithmetic, making it very cheap and efficient. 

### Basics:
- The equation of a line is $y = mx + b$.
	- $m$ is our slope, $\frac{\Delta y}{\Delta x}$.
	- $b$ is our $y$-intercept.
- This equation, while powerful, can not be used directly on our screen as we are dealing with a grid of discrete pixels, the *raster*.
- Thus, we must *rasterize* the line.

- In order to properly calculate $(x,y)$ using this equation, we must sample one of the values to get the other.
- For this case, we will sample the $x$-value as we march along the raster to find the corresponding $y$-value

### The Concept:
- Imagine we delve into a space that exists between the pixels on our screen. This obviously does not exist physically, but we will create it mathematically.
- We have our starting point at $(y_k, x_k)$, and our ending point at some other point.
- Our task is to determine what pixel we should draw to along every point of our vector line. This is easy if our line only goes through one pixel at a given pixel, but there are many instances where it will cross 2 pixels, and we must determine which pixel is "closer".
- The actual point that we are deciding for will be at $(x_k + 1, y)$
- ![[Pasted image 20240917005820.png]]
- As stated before, we will march along our raster by incrementing our $x$-value. Thus, we must determine where our $y$-value will be, and that is based on which pixel is closer to our line.
	- $x_{next}$ = $x_k + 1$
	- $y_{next}$ = $y_k$ OR $y_k + 1$
- First we establish 2 variables:
	- $d_1$: Distance from the point to the bottom pixel
		- $d_1 = y - y_k$
	- $d_2$: Distance from the point to the top pixel
		- $d_2 = y_k + 1 - y$
### Derivation:
$y = mx + b$
$y = m(x_k + 1) + b$
$d_1 = y - y_k = m(x_k + 1) + b - y_k$
$d_2 = y_k + 1 - y = y_k + 1 - m(x_k + 1) - c$

- if $d_1 - d_2 < 0$, we are closer to the bottom pixel, $y_k$
- if $d_1 - d_2 > 0$, we are closer to the top pixel, $y_k +1$

$d_1 - d_2 = (m(x_k + 1) + b - y_k) - (y_k + 1 - m(x_k + 1) - c)$
$d_1 - d_2 = m(x_k + 1) + b - y_k - y_k - 1 + m(x_k + 1) + c$
$d_1 - d_2 = 2m(x_k + 1) - 2y_k + 2b - 1$

- At this point, we still have a problem: $m$ is $\frac{\Delta y}{\Delta x}$. In other words, scary floating point division! To deal with this, we multiply by $\Delta x$.

$\Delta x(d_1 - d_2) = \Delta x(2\frac{\Delta y}{\Delta x}(x_k + 1) - 2y_k + 2b - 1)$
$\Delta x(d_1 - d_2) = 2\Delta y(x_k + 1) - 2\Delta xy_k + 2\Delta x b - \Delta x$
$\Delta x(d_1 - d_2) = 2\Delta yx_k - 2\Delta yx_k + 2\Delta y + 2\Delta x b - \Delta x$

- Here, something nice happens. The last 3 terms remain constant, as they are not directly related to $x_k$ or $y_k$. Thus, we will ignore it and move forward with the rest, naming it $P_k$. $P_k$ will act as our decision variable, sometimes referred to as the *error*.

$P_k = 2\Delta yx_k - 2\Delta xy_k$

- This equation will help us determine which pixel to choose, but we need an equation for the next pixel.

$P_{next} = 2\Delta yx_{next} - 2\Delta xy_{next}$

- How much our decision variable should change each time can be found if we just subtract $P_{next}$ and $P_k$

$P_{next} - P_k = (2\Delta yx_{next} - 2\Delta xy_{next}) - (2\Delta yx_k - 2\Delta xy_k)$
$P_{next} - P_k = 2\Delta y(x_{next} - x_k) - 2\Delta x(y_{next} - y_k)$

- if $P_{next} - P_k < 0$ 
	- $P_{next} = P_k + 2\Delta y (x_k + 1 - x_k) - 2\Delta x(y_k - y_k)$
	- $(y_k - y_k)$ cancels out, and $(x_k - x_k)$ cancel out, leaving:
	- $P_{next} = P_k + 2\Delta y$
- In this case, $y_{next} = y_k$

- if $P_{next} - P_k \geq 0$
	- $P_{next} = P_k + 2\Delta y - 2\Delta x$
- In this case, $y_{next} = y_k + 1$

- Now we need to find our *initial value*
$P_1 = 2\Delta yx_1 - 2\Delta yx_1 + 2\Delta y + 2\Delta x b - \Delta x$

- Solving for b, we get $b = y_1 - mx_1$
- Inserting b into our equation and solving, we get:

$P_1 = 2\Delta y - \Delta x$

- It should be noted that this algorithm, as laid out here, **only works for a specific octant**, the octant where the slope of our line is less than $45\degree$
- There are implementations that work for all octants


### The Algorithm (Pseudo code)

```c
Bresenham(x1, y1, x2, y2)
{
	x = x1;
	y = y1;
	dx = x2 - x1;
	dy = y2 - y1;
	p = 2dx - dy; //initial value
	while (x <= x2)
	{
		putpixel(x,y);
		x++;
		if (p < 0)
		{
			p = p + 2dy;			
		}

		else
		{
			p = p + 2dy - 2dx;
			y++;
		}
	}
}

```
