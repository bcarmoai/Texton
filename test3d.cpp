/* Test 3d matrix vector class */

#include <stdafx.h>
#include <math.h>
#include <iostream>
#include <stdlib.h>
#include "3d.h"

using namespace std;

/* VelocityField: */

inline
diag_matrix3d operator*(double x, const diag_matrix3d mat)
{
  const double* diag = mat.pt();
  return diag_matrix3d(x*diag[0], x*diag[1], x*diag[2]);
}

inline
diag_matrix3d operator*(const diag_matrix3d mat, double x)
{
  return x*mat;
}

inline
diag_matrix3d operator/(const diag_matrix3d mat, double x)
{
  return (1.0/x)*mat;
}

inline
vector3d operator*(double x, const vector3d& y)
{
  return vector3d(x*y[0], x*y[1], x*y[2]);
}

inline
vector3d operator*(const vector3d& y, double x)
{
  return vector3d(x*y[0], x*y[1], x*y[2]);
}

inline
vector3d operator/(const vector3d& y, double x)
{
  double rx = 1.0/x;
  return vector3d(rx*y[0], rx*y[1], rx*y[2]);
}

vector3d vector3d::mult(double t)
{
	return t*(*this);
}

inline
matrix3d operator+(const matrix3d m1, const matrix3d m2)
{
  const double* x = m1.pt();
  const double* y = m2.pt();
  return matrix3d(x[0]+y[0],x[1]+y[1],x[2]+y[2],
		  x[3]+y[3],x[4]+y[4],x[5]+y[5],
		  x[6]+y[6],x[7]+y[7],x[8]+y[8]);
}

inline
matrix3d operator-(const matrix3d m1, const matrix3d m2)
{
  const double* x = m1.pt();
  const double* y = m2.pt();
  return matrix3d(x[0]-y[0],x[1]-y[1],x[2]-y[2],
		  x[3]-y[3],x[4]-y[4],x[5]-y[5],
		  x[6]-y[6],x[7]-y[7],x[8]-y[8]);
}

inline
matrix3d operator*(const matrix3d m1, const matrix3d m2)
{
  const double* x = m1.pt();
  const double* y = m2.pt();
  return matrix3d(x[0]*y[0]+x[1]*y[3]+x[2]*y[6],
		  x[0]*y[1]+x[1]*y[4]+x[2]*y[7],
		  x[0]*y[2]+x[1]*y[5]+x[2]*y[8],
		  x[3]*y[0]+x[4]*y[3]+x[5]*y[6],
		  x[3]*y[1]+x[4]*y[4]+x[5]*y[7],
		  x[3]*y[2]+x[4]*y[5]+x[5]*y[8],
		  x[6]*y[0]+x[7]*y[3]+x[8]*y[6],
		  x[6]*y[1]+x[7]*y[4]+x[8]*y[7],
		  x[6]*y[2]+x[7]*y[5]+x[8]*y[8]);
}

inline
matrix3d operator*(double x, const matrix3d m1)
{
  const double* m = m1.pt();
  return matrix3d(x*m[0], x*m[1], x*m[2],
		  x*m[3], x*m[4], x*m[5], 
		  x*m[6], x*m[7], x*m[8]);
}

inline
matrix3d operator*(const matrix3d m1, double x)
{
  const double* m = m1.pt();
  return matrix3d(x*m[0], x*m[1], x*m[2],
		  x*m[3], x*m[4], x*m[5], 
		  x*m[6], x*m[7], x*m[8]);
}

inline
matrix3d operator/(const matrix3d m1, double y)
{
  double x = 1.0/y;
  const double* m = m1.pt();
  return matrix3d(x*m[0], x*m[1], x*m[2],
		  x*m[3], x*m[4], x*m[5], 
		  x*m[6], x*m[7], x*m[8]);
}


inline
vector3d operator*(const matrix3d m1, const vector3d v1)
{
  const double* m = m1.pt();
  const double* v = v1.pt();
  return vector3d(m[0]*v[0]+m[1]*v[1]+m[2]*v[2],
		  m[3]*v[0]+m[4]*v[1]+m[5]*v[2],
		  m[6]*v[0]+m[7]*v[1]+m[8]*v[2]);
}

inline
vector3d operator*(const vector3d v1, const matrix3d m1)
{
  const double* m = m1.pt();
  const double* v = v1.pt();
  return vector3d(m[0]*v[0]+m[3]*v[1]+m[6]*v[2],
		  m[1]*v[0]+m[4]*v[1]+m[7]*v[2],
		  m[2]*v[0]+m[5]*v[1]+m[8]*v[2]);
}

inline
std::ostream& operator<<(std::ostream& out, const matrix3d& mat)
{
  const double* m = mat.pt();
  out << '\n';
  out << '(' << m[0] << ',' << m[1] << ',' << m[2] << ')' << '\n';
  out << '(' << m[3] << ',' << m[4] << ',' << m[5] << ')' << '\n';
  out << '(' << m[6] << ',' << m[7] << ',' << m[8] << ')' << '\n';
  return out;
}



inline
matrix3d operator+(const matrix3d m1, const diag_matrix3d m2)
{
  const double* m = m1.pt();
  const double* diag = m2.pt();
  return matrix3d(m[0]+diag[0], m[1], m[2],
		  m[3], m[4]+diag[1], m[5],
		  m[6], m[7], m[8]+diag[2]);
}

inline
matrix3d operator-(const matrix3d m1, const diag_matrix3d m2)
{
  const double* m = m1.pt();
  const double* diag = m2.pt();
  return matrix3d(m[0]-diag[0], m[1], m[2],
		  m[3], m[4]-diag[1], m[5],
		  m[6], m[7], m[8]-diag[2]);
}

inline
matrix3d operator+(const diag_matrix3d m1, const matrix3d m2)
{
  return m2+m1;
}

inline
matrix3d operator-(const diag_matrix3d m1, const matrix3d m2)
{
  return m2-m1;
}

matrix3d operator*(const matrix3d m1, const diag_matrix3d m2)
{
  const double* m = m1.pt();
  const double* diag = m2.pt();
  return matrix3d(m[0]*diag[0], m[1]*diag[1], m[2]*diag[2],
		  m[3]*diag[0], m[4]*diag[1], m[5]*diag[2],
		  m[6]*diag[0], m[7]*diag[1], m[8]*diag[2]);
}

matrix3d operator*(const diag_matrix3d m1, const matrix3d m2)
{
  const double* m = m1.pt();
  const double* diag = m2.pt();
  return matrix3d(m[0]*diag[0], m[1]*diag[0], m[2]*diag[0],
		  m[3]*diag[1], m[4]*diag[1], m[5]*diag[1],
		  m[6]*diag[2], m[7]*diag[2], m[8]*diag[2]);
}

diag_matrix3d operator*(const diag_matrix3d m1, const diag_matrix3d m2)
{
  const double* diag1 = m1.pt();
  const double* diag2 = m2.pt();
  return diag_matrix3d(diag1[0]*diag2[0],
		       diag1[1]*diag2[1],
		       diag1[2]*diag2[2]);
}

VelocityField::VelocityField(int l, int m, int n): xsize(l), ysize(m), zsize(m)
{
  vel = new double[l*m*n*3];
}

double VelocityField::drand()
{
  return rand()/(RAND_MAX+1.0);
}

vector3d VelocityField::rand_vec(double x)
{
  return vector3d(x*drand(), x*drand(), x*drand());
}

/*
int VelocityField::main()
{
  int n = 5;

  // Generate a vortex
  matrix3d A(1.0, 0.0, 0.0,
	     0.0, 0.0, 5.0,
	     0.0, -5.0, -1.0);
  vector3d x0(1.0,2.0,3.0); // Vortex Centre

  // Velocity Field
  VelocityField vf(n,n,n);
  // initialise velocity field v = A(x-x0)
  vector3d x;
  int i;
  for(i=0; i<n; i++) 
  {
	  x(0) = i - x0[0];
	  for(int j=0; j<n; j++) 
	  {
		  x(1) = j - x0[1];
		  for(int k=0; k<n; k++) 
		  {
			  x(2) = k - x0[2];
			  vector3d v = A*x;
			  //	v += rand_vec(0.1);
			  v.write(vf(i,j,k));
		  }
	  }
  }

  // Calculate covariance V
  vector3d x_av(2.0, 2.0, 2.0);
  double tau=0;
  for(i=0; i<n; i++)
    tau += (i-0.5*(n-1))*(i-0.5*(n-1));
  tau /= n;
  diag_matrix3d V(tau,tau,tau);
  cerr << "V= " << V << endl;
  
  diag_matrix3d Vinv = V.inverse();
  double Vinv_trace = Vinv.trace();

  // Calculate v_av;
  vector3d v_av(0,0,0);
  int cnt = 0;
  for(i=0; i<n; i++) 
  {
	  for(int j=0; j<n; j++) 
	  {
		  for(int k=0; k<n; k++) 
		  {
			  v_av += vf(i,j,k);
			  cnt++;
		  }
	  }
  }
  v_av /= cnt;

  // Calculate Wtr
  matrix3d Wtr;
  for(i=0; i<n; i++) 
  {
	  x(0) = i - x_av[0];
	  for(int j=0; j<n; j++) 
	  {
		  x(1) = j - x_av[1];
		  for(int k=0; k<n; k++) 
		  {
			  x(2) = k - x_av[2];
			  Wtr.plus_outer(vector3d(vf(i,j,k))-v_av,x);
		  }
	  }
  }
  Wtr /= cnt;

  matrix3d A_est(Wtr/tau);
  A_est -= (A.trace()/Vinv_trace)*Vinv;
  cerr << "A= " << A;
  cerr << "A_est= " << A_est;
  vector3d x0_est(x_av);
  x0_est -= solve(A_est, v_av);
  cerr << "x0= " << x0 << '\n';
  cerr << "x0_est= " << x0_est << '\n';

  eig3d eig(A_est);
 
  if (eig.complex_eigenvalues()) {
    double lambda = eig.real_eigenvalue();
    cerr << "lambda= " << lambda << endl;
    vector3d w(A_est.eigenvector(lambda));
    cerr << "w= " << w << endl;
    vector3d dx = x0_est-x_av;
    double t = dot(dx,w);
    dx -= t*w;
    cerr << "distance = " << dx.norm() << endl;
    cerr << "lambda_complex= " << eig.complex_eigenvalue_re();
    cerr << " +/- i " << eig.complex_eigenvalue_im() << endl;
  }

  return 0;
}
*/

vector3d VelocityField::solve1(const matrix3d& mat, const vector3d& b)
{
  matrix3d minv(mat.inverse());
  return minv*b;
}

vector3d VelocityField::solve(const matrix3d& mat, const vector3d& b)
{
  const double* m = mat.pt();
  if (fabs(m[0])<1.0e-5) {
    return solve1(mat,b);
  }
  double m3 = m[3]/m[0];
  double m4 = m[4]-m[1]*m3;
  if (fabs(m4)<1.0e-5) {
    return solve1(mat,b);
  }
  double m5 = m[5]-m[2]*m3;
  double m6 = m[6]/m[0];
  double m7 = (m[7]-m[1]*m6)/m4;
  double m8 = m[8]-m[2]*m6-m5*m7;
  if (fabs(m8)<1.0e-10) {
    std::cerr << "singular matrix\n";
    if(!(fabs(m8)>=1.0e-10))
		AfxMessageBox("Error: fabs(m8)>=1.0e-10");
  }
  double y1 = b[0];
  double y2 = b[1]-m3*y1;
  double y3 = b[2]-m6*y1-m7*y2;
  double x3 = y3/m8;
  double x2 = (y2-m5*x3)/m4;
  return vector3d((y1-m[1]*x2-m[2]*x3)/m[0], x2, x3);
}
