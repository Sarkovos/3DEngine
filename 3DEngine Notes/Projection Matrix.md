![[Pasted image 20240923215312.png]]
- Our projection matrix will be responsible for:
	- Aspect Ratio: adjust x and y values based on the screen width and height values
	- Field of view: adjust x and y values based on the FOV angle
	- Normalization: adjust x,y,z values to sit between -1 and 1
 ![[Pasted image 20240923215648.png]]
 - Normalization creates *image space* in *normalized device coordinates*


- *Aspect ratio*: $a = \frac{h}{w}$, where $h$ is height of screen and $w$ is width of screen
![[Pasted image 20240923220159.png]]

*Field Of View*:![[Pasted image 20240923220233.png]]
![[Pasted image 20240923220923.png]]
- Our field of view scaling factor : $f = \frac{1}{\tan(\theta/2)}$
 ![[Pasted image 20240923221355.png]]

- Normalizing $z$: we have zfar, a plane at the far end of our scene, and we have znear, a plane where our monitor sits in our scene.
- ![[Pasted image 20240923221940.png]]
 
![[Pasted image 20240923222053.png]]


## From Scratch A Pixel
- Projection matrices are specialized 4x4 matrices designed to transform a 3D point in camera space into its projected counterpart on the canvas. Essentially, when you multiply a 3D point by a projection matrix, you determine its 2D coordinates within Normalized Device Coordinates space.