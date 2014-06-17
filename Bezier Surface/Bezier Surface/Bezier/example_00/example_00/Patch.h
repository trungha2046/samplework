/*
 * Patch.h
 *
 *  Created on: Nov 2, 2013
 *      Author: everardobarriga
 */

#ifndef PATCH_H_
#define PATCH_H_
class Patch{
	Vector patches[3][3];
public:

	Patch();
	Patch(Vector, Vector, Vector, Vector,
		  Vector, Vector, Vector, Vector,
		  Vector, Vector, Vector, Vector,
		  Vector, Vector, Vector, Vector);
	void set_vertex(int, int, Vector);
	Vector get_vertex(int, int);
};
Patch::Patch() {
}
Patch::Patch(Vector a, Vector b, Vector c, Vector d,
		  Vector e, Vector f, Vector g,
		  Vector h, Vector i, Vector j,
		  Vector k, Vector l, Vector m, Vector n, Vector o,Vector p) {
	patches[0][0] = a;
	patches[0][1] = b;
	patches[0][2] = c;
	patches[0][3] = d;
	patches[1][0] = e;
	patches[1][1] = f;
	patches[1][2] = g;
	patches[1][3] = h;
	patches[2][0] = i;
	patches[2][1] = j;
	patches[2][2] = k;
	patches[2][3] = l;
	patches[3][0] = m;
	patches[3][1] = n;
	patches[3][2] = o;
	patches[3][3] = p;
}
void Patch::set_vertex(int u, int v, Vector value) {
	patches[u][v] = value;
}
Vector Patch::get_vertex(int u, int v) {
	return patches[u][v];
}


#endif /* PATCH_H_ */
