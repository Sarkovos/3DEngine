## Algorithm overview:
- Define a triangle
- Surround the triangle into a bounding box defined by the maxes of the vertices
- Loop all pixels in the bounding box

#### Cross Product
- To tell if a point is inside our triangle, if we say our triangles are defined *clockwise*, then for each edge of the triangle, the point has to exist to the right of that edge, and this must hold for each edge.
- To find this, we can use the cross product
- The cross product takes two *3D* vectors, $\vec{a}$ and $\vec{b}$, and generates a new vector perpendicular to the two starting vectors
- The magnitude of this vector is equal to the size of the parallelogram generated by the addition of $\vec{a}$ and $\vec{b}$, sort of like the determinant
- magnitude of the cross product is $||\vec{a}||||\vec{b}||\sin\theta$
- Cross product is not commutative
- We will use if the cross product is either positive or negative to determine where the point lies
- Technically cross product is only defined in 3 and 7 dimensions (don't ask) but we can just think of a 2D cross product as the length of the z component of the the imaginary vector perpendicular to the screen plane
- This is much more like the determinant
- So,
	- $cross(v_1, v_2, p) \leq 0$
	- $cross(v_0, v_1, p) \leq 0$
	- $cross(v_2, v_0, p) \leq 0$
	- If these are all true, point $p$ is in the triangle

#### Fill Convention
- Top-Left Rule: A pixel center is defined to lie inside of a triangle if it lies on a flat top edge or a left edge of a triangle