#include<iostream>
#include<math.h>

#define GL_MODELVIEW                      0x1700
#define GL_PROJECTION                     0x1701

//#define OTHER_IMPLE
#define SELF_IMPLE
using namespace std;

const double PI = 3.1415926;

static const int STACK_SIZE = 32;
static const int MATRIX_SIZE = 16;
static int s_choice = 0; // 0->model view stack, 1->prejection stack
static int topNum = 0; //topNum is one above the top of the stack
static double stack[2][STACK_SIZE][MATRIX_SIZE];

//identity matrix
static const double I[16] = {1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1};



void push(double *matrix) {
	if (topNum < STACK_SIZE) {
		/*
		for (int i = 0; i < MATRIX_SIZE; i++)
			stack[topNum][i] = *(matrix + i);
		topNum++;
		*/
		memcpy(stack[s_choice][topNum++], matrix, sizeof(double) * MATRIX_SIZE);
		int i = 0;
	} else
		cout << "Stack overflow" <<endl;
}

double* pop() {
	if (topNum > 0) {
		/*
		topNum--;
		double *temp = stack[topNum];
		return temp;
		*/
		return stack[s_choice][--topNum];
	}
	return NULL;
}

extern "C" double* top() {
	if (topNum > 0)
		return stack[s_choice][topNum - 1];
	return NULL;
}


//matrix multiplication
//the matrix is stored by column in one-dimension array
//e.g.{{1,2},{3,4}} is stored as {1,3,2,4} in openGL
void matrix_multiply(double *m1, const double *m2) {
	static double result_buffer[16];

	//i/4 is column
	//i%4 is row
	for (int i = 0; i < MATRIX_SIZE; i++) {
		double sum = 0;

		for (int j = 0; j < 4; j++)
			sum += m1[j*4 + i%4] * m2[i/4 * 4 + j];

		*(result_buffer + i) = sum;
	}
	memcpy(m1, result_buffer, sizeof(double)* MATRIX_SIZE);
	//delete result;
	
} 

//just used for test, can be deleted
void display(double *m) {
	for (int i = 0; i < MATRIX_SIZE; i++) {
		cout << *(m + i) << "  ";
		if (i % 4 == 3)
			cout << endl;
	}
}

int size() {
	return topNum;
}

//replace current matrix with identity matrix
extern "C" void myGL_loadIdentity() {
	if (topNum == 0)
		topNum++;

	//for (int i = 0; i < MATRIX_SIZE; i++)
		//stack[topNum - 1][i] = *(I + i);

	memcpy(stack[s_choice][topNum - 1], I, sizeof(double)* MATRIX_SIZE);
}

//replace current matrix with m
extern "C" void myGL_loadMatrix(const double* m) {
	if (topNum == 0)
		topNum++;

	//for (int i = 0; i < MATRIX_SIZE; i++)
		//stack[topNum - 1][i] = *(m + i);
	memcpy(stack[s_choice][topNum - 1], m, sizeof(double)* MATRIX_SIZE);
}

//multiply the current matrix by m, and replace the current matrix with product
//i.e. current = current * m
extern "C" void myGL_multMatrix(const double *m) {
	if (topNum > 0) {
		//double *temp = matrix_multiply(pop(), m);
		//push(temp);
		matrix_multiply(top(), m);
	}
}

//push the current matrix down by one, duplicating the current matrix
extern "C" void myGL_pushMatrix() {
	if (topNum > 0) {
		//for (int i = 0; i < MATRIX_SIZE; i++)
			//stack[s_choice][topNum][i] = stack[s_choice][topNum - 1][i];
		memcpy(stack[s_choice][topNum], stack[s_choice][topNum - 1], sizeof(double)* MATRIX_SIZE);
		topNum++;
	}
}

//pop the current matrix, replacing the current matrix with the one below it
extern "C" void myGL_popMatrix() {
	if (topNum > 0)
		topNum--;
}

// Set matrix mode
extern "C" void myGL_glMatrixMode(int flag){ 
	if (flag == GL_MODELVIEW) {
		s_choice = 0;
	}
	else if (flag == GL_PROJECTION) {
		s_choice = 1;
	}
}

// TODO: remove the following TEST CODE
void glhFrustumf2(float *matrix, float left, float right, float bottom, float top,
	float znear, float zfar)
{
	float temp, temp2, temp3, temp4;
	temp = 2.0 * znear;
	temp2 = right - left;
	temp3 = top - bottom;
	temp4 = zfar - znear;
	matrix[0] = temp / temp2;
	matrix[1] = 0.0;
	matrix[2] = 0.0;
	matrix[3] = 0.0;
	matrix[4] = 0.0;
	matrix[5] = temp / temp3;
	matrix[6] = 0.0;
	matrix[7] = 0.0;
	matrix[8] = (right + left) / temp2;
	matrix[9] = (top + bottom) / temp3;
	matrix[10] = (-zfar - znear) / temp4;
	matrix[11] = -1.0;
	matrix[12] = 0.0;
	matrix[13] = 0.0;
	matrix[14] = (-temp * zfar) / temp4;
	matrix[15] = 0.0;
}

// TODO: remove the following TEST CODE
void glhPerspectivef2(float *matrix, float fovyInDegrees, float aspectRatio,
	float znear, float zfar)
{
	float ymax, xmax;
	//float temp, temp2, temp3, temp4;
	ymax = znear * tanf(fovyInDegrees * PI / 360.0);
	//ymin = -ymax;
	//xmin = -ymax * aspectRatio;
	xmax = ymax * aspectRatio;
	glhFrustumf2(matrix, -xmax, xmax, -ymax, ymax, znear, zfar);
}

extern "C" void myGL_gluPerspective(double fovy, double aspect, double zNear, double zFar) {
#ifdef SELF_IMPLE
	double f = 1 / tan(fovy / 2.0  * PI / 180.0 );
	double m[] = { f / aspect, 0.0, 0.0, 0.0,
		0.0, f, 0.0, 0.0,
		0.0, 0.0, (zFar + zNear) / (zNear - zFar), -1.0 ,
		0.0, 0.0, (2 * zFar* zNear) / (zNear - zFar), 0.0 };
	myGL_multMatrix(m);
#endif

#ifdef OTHER_IMPLE
	float f[16];
	double mat[16];
	glhPerspectivef2(f, fovy, aspect, zNear, zFar);
	for (int i = 0; i < MATRIX_SIZE; i++) {
			mat[i] = f[i];
		}
	myGL_multMatrix(mat);
#endif
}

//multiply current matrix by translation matrix given by parameters x, y, z
extern "C" void myGL_translate(double x, double y, double z) {
	 double m[] = {1,0,0,0,0,1,0,0,0,0,1,0,x,y,z,1};

	myGL_multMatrix(m);
}

//multiply current matrix by scale matrix given by parameters x, y, z
extern "C" void myGL_scale(double x, double y, double z) {
	 double m[] = {x,0,0,0,0,y,0,0,0,0,z,0,0,0,0,1};

	myGL_multMatrix(m);
}


//multiply current matrix by rotation matrix given by parameters x, y, z
extern "C" void myGL_rotated(double angle, double x, double y, double z) {
	//normalize
	double l = sqrt(x*x + y*y + z*z);
	x = x / l;
	y = y / l;
	z = z / l;
	double c = cos(angle * PI / 180.0);
	double s = sin(angle * PI / 180.0);

	double m[] = {x*x*(1-c)+c,y*x*(1-c)+z*s,x*z*(1-c)-y*s,0,
		x*y*(1-c)-z*s,y*y*(1-c)+c,y*z*(1-c)+x*s,0,
		x*z*(1-c)+y*s,y*z*(1-c)-x*s,z*z*(1-c)+c,0,
		0,0,0,1};


	myGL_multMatrix(m);
}


extern "C" void myGL_lookAt(double eyeX, double eyeY, double eyeZ, double centerX, double centerY, double centerZ,
	double upX, double upY, double upZ) {
		double f[] = {centerX - eyeX, centerY - eyeY, centerZ - eyeZ};
		double l = sqrt(f[0] * f[0] + f[1] * f[1] + f[2] * f[2]);
		f[0] /= l;
		f[1] /= l;
		f[2] /= l;

		l = sqrt(upX * upX + upY * upY + upZ * upZ);
		double up[] = {upX / l, upY / l, upZ / l};
		double s[] = {f[1] * up[2] - f[2] * up[1],
			f[2] * up[0] - f[0] * up[2],
			f[0] * up[1] - f[1] * up[0]};
		double u[] = {s[1] * f[2] - s[2] * f[1],
			s[2] * f[0] - s[0] * f[2],
			s[0] * f[1] - s[1] * f[0]};

		double m[] = {s[0],u[0],-f[0],0,s[1],u[1],-f[1],0,s[2],u[2],-f[2],0,0,0,0,1};

		myGL_multMatrix(m);
		myGL_translate(-eyeX, -eyeY, -eyeZ);
}


/*
int main() {
	double a[] = {1,2,3,4,5,6,7,8,8,7,6,5,4,3,2,1};
	double b[] = {8,7,6,5,4,3,2,1,1,2,3,4,5,6,7,8};

	myGL_loadIdentity();

	cout << size() << endl;

	display(top());

	pop();

	cout << size() << endl;

	myGL_loadMatrix(a);

	myGL_multMatrix(b);

	display(top());

	cout << size() << endl;

	myGL_pushMatrix();

	myGL_translate(1,2,3);

	display(top());

	cout << size() << endl;

	myGL_rotated(30,0,5,5);

	display(top());
	
}
*/