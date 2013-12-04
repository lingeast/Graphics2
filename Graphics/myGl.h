#ifndef __MYGL_H
#define __MYGL_H

#ifdef __cplusplus
extern "C" {
#endif

double* top();

void myGL_gluPerspective(double, double, double, double);
void myGL_glMatrixMode(int flag);
void myGL_loadIdentity();
void myGL_loadMatrix(const double* m);
void myGL_multMatrix(const double *m);
void myGL_pushMatrix();
void myGL_popMatrix();
void myGL_translate(double x, double y, double z);
void myGL_scale(double x, double y, double z);
void myGL_rotated(double angle, double x, double y, double z);
void myGL_lookAt(double eyeX, double eyeY, double eyeZ, double centerX, double centerY, double centerZ, double upX, double upY, double upZ);

#ifdef __cplusplus
}
#endif

#endif