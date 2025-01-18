To review, this is the order of our transformations:
- Model space
	- Local coordinate system, as they are defined in your DCC
- World Space
	- Model is translated, rotated, scaled
- View/Camera Space
	- Multiply vertices by view matrix
- Screen Space
	- Where we end up after multiplying by projection matrix

"The camera has a location in world space and a direction, which are used to
place and aim the camera. To facilitate projection and clipping, the camera and all the
models are transformed with the view transform. The purpose of the view transform
is to place the camera at the origin and aim it, to make it look in the direction of the
negative z-axis, with the y-axis pointing upward and the x-axis pointing to the right.
We use the −z-axis convention; some texts prefer looking down the +z-axis."
![[Pasted image 20250110003758.png]]
## Camera Look At Model
- Create a Look_at() function
- The function returns a 4x4 that can be used to multiply the vertices to convert them to view space
- In order to set a camera position and orientation, all you really need is a point to set the camera position in space (the eye point) and a point that defines where the camera is looking at (the target point)
- Function consists of 2 transformations:
	- *Translating* the whole scene inversely from the camera eye position to the origin
	- *Rotating* the scene with reverse orientation so the camera is positioned at the origin and facing the positive Z axis (left handed coordinate system)

 $M_{view} = M_R * M_T$
- To invert any orthogonal matrix, all we need to do is transpose the matrix.
- 