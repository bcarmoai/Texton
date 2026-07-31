/* 3d.h: A small 3-d matrix vector class */

#if !defined __3D_H__
#define __3D_H__

/* vector3d: */

#include <iostream>

class vector3d {
public:
  vector3d() {}
  vector3d(double x, double y, double z) {v[0]=x; v[1]=y; v[2]=z;}
  vector3d(const double* rhs) {v[0]=rhs[0]; v[1]=rhs[1]; v[2]=rhs[2];}
  vector3d(const vector3d& rhs) {v[0]=rhs[0]; v[1]=rhs[1]; v[2]=rhs[2];}
  vector3d operator=(const vector3d& rhs) {return vector3d(rhs);}
  double operator[](int i) const {return v[i];}
  double& operator()(int i) {return v[i];}
  const double* pt() const {return v;}
  double norm() const {return sqrt(v[0]*v[0]+v[1]*v[1]+v[2]*v[2]);}
  vector3d& operator*=(double x) {v[0]*=x; v[1]*=x; v[2]*=x; return *this;}
  vector3d& operator/=(double x) {*this *= 1.0/x; return *this;}
  vector3d& operator+=(const double* rhs) {
    v[0]+=rhs[0]; v[1]+=rhs[1]; v[2]+=rhs[2]; return *this;}
  vector3d& operator+=(const vector3d& rhs) {*this+=rhs.pt(); return *this;}
  vector3d& operator -=(const double* rhs) {
    v[0]-=rhs[0]; v[1]-=rhs[1]; v[2]-=rhs[2]; return *this;}
  vector3d& operator-=(const vector3d& rhs) {*this-=rhs.pt(); return *this;}
  void write(double* pt) const {pt[0]=v[0]; pt[1]=v[1]; pt[2]=v[2];}
  vector3d mult(double t);
private:
  double v[3];
};

inline
double dot(const vector3d& x, const vector3d& y)
{
  return x[0]*y[0] + x[1]*y[1] + x[2]*y[2];
}

inline
vector3d operator+(const vector3d& x, const vector3d& y)
{
  return vector3d(x[0]+y[0], x[1]+y[1], x[2]+y[2]);
}

inline
vector3d operator-(const vector3d& x, const vector3d& y)
{
  return vector3d(x[0]-y[0], x[1]-y[1], x[2]-y[2]);
}

inline
std::ostream& operator<<(std::ostream& out, const vector3d& v)
{
  out << '(' << v[0] << ',' << v[1] << ',' << v[2] << ')';
  return out;
}

/* diag_matrix3d: */

class diag_matrix3d {
public:
  diag_matrix3d(double x, double y, double z) {
    diag[0] = x;
    diag[1] = y;
    diag[2] = z;
  }
  diag_matrix3d& operator*=(double x) {
    diag[0] *= x;
    diag[1] *= x;
    diag[2] *= x;
    return *this;
  }
  diag_matrix3d& operator/=(double x) {
    diag[0] *= x;
    diag[1] *= x;
    diag[2] *= x;
    return *this;
  }
  diag_matrix3d inverse() const {
    if (fabs(diag[0]*diag[1]*diag[2])<1.0e-30) {
      std::cerr << "diagonal matrix is singular: can't invert\n";
      if(!(fabs(diag[0]*diag[1]*diag[2])>=1.0e-30))
		  AfxMessageBox("Error: diag[0]*diag[1]*diag[2]>=1.0e-30");
    }
    return diag_matrix3d(1.0/diag[0], 1.0/diag[1], 1.0/diag[2]);
  }
  const double* pt() const {return diag;}
  double trace() const {
    return diag[0]+diag[1]+diag[2];
  }    
private:
  double diag[3];
};

inline
std::ostream& operator<<(std::ostream& out, const diag_matrix3d& mat)
{
  const double* diag = mat.pt();
  out << '\n';
  out << '(' << diag[0] << ',' << 0 << ',' << 0 << ')' << '\n';
  out << '(' << 0 << ',' << diag[1] << ',' << 0 << ')' << '\n';
  out << '(' << 0 << ',' << 0 << ',' << diag[2] << ')' << '\n';
  return out;
}


/* matrix3d: */

class matrix3d {
public:
  matrix3d() {for(int i=0;i<9;i++) m[i]=0;}
  matrix3d(double a, double b, double c, 
	   double d, double e, double f,
	   double g, double h, double i) {
    m[0] = a; m[1] = b; m[2] = c; 
    m[3] = d; m[4] = e; m[5] = f; 
    m[6] = g; m[7] = h; m[8] = i;} 
  matrix3d(const matrix3d& rhs) {
    const double* ptr=rhs.pt();
    for(int i=0;i<9;i++) m[i]=ptr[i];}
  matrix3d operator=(const matrix3d& rhs) {return matrix3d(rhs);}
  double operator()(int r, int c) const {return m[3*r+c];}
  const double* pt() const {return m;}
  matrix3d& operator*=(double x) {for(int i=0;i<9;i++) m[i]*=x; return *this;}
  matrix3d& operator/=(double x) {(*this)*=1.0/x; return *this;}
  matrix3d& operator+=(const matrix3d& rhs) {
    const double* ptr=rhs.pt();
    for(int i=0;i<9;i++) m[i]+=ptr[i]; return *this;}
  matrix3d& operator-=(const matrix3d& rhs) {
    const double* ptr=rhs.pt();
    for(int i=0;i<9;i++) m[i]-=ptr[i]; return *this;}
  matrix3d& operator+=(const diag_matrix3d& rhs) {
    const double* diag=rhs.pt();
    m[0] += diag[0];
    m[4] += diag[1];
    m[8] += diag[2];
    return *this;}
  matrix3d& operator-=(const diag_matrix3d& rhs) {
    const double* diag=rhs.pt();
    m[0] -= diag[0];
    m[4] -= diag[1];
    m[8] -= diag[2];
    return *this;}
  matrix3d& plus_outer(const vector3d& x, const vector3d& y) {
    m[0] += x[0]*y[0]; m[1] += x[0]*y[1]; m[2] += x[0]*y[2];
    m[3] += x[1]*y[0]; m[4] += x[1]*y[1]; m[5] += x[1]*y[2];
    m[6] += x[2]*y[0]; m[7] += x[2]*y[1]; m[8] += x[2]*y[2];
    return *this;}
  double trace() const {return m[0]+m[4]+m[8];}
  double det() const {
    return m[0]*(m[4]*m[8]-m[5]*m[7])-m[1]*(m[3]*m[8]-m[5]*m[6])
      +m[2]*(m[3]*m[7]-m[4]*m[6]);}
  double invar3() const {
    return m[4]*m[8]-m[5]*m[7]+m[0]*m[8]-m[2]*m[6]+m[0]*m[4]-m[1]*m[3];}
  matrix3d transpose() const {
    return matrix3d(m[0], m[3], m[6],
		    m[1], m[4], m[7],
		    m[2], m[5], m[8]);
  }
  matrix3d inverse() const {
    double determ = det();
    if (fabs(determ)<1.0e-10) {
      std::cerr << "Singular martix\n";
      if(!(fabs(determ)>=1.0e-10))
		  AfxMessageBox("Error: fabs(determ)>=1.0e-10");
    }
    double norm = 1.0/det();
    return matrix3d(norm*(m[4]*m[8]-m[5]*m[7]),
		    norm*(m[2]*m[7]-m[1]*m[8]),
		    norm*(m[1]*m[5]-m[2]*m[4]),
		    norm*(m[5]*m[6]-m[3]*m[8]),
		    norm*(m[0]*m[8]-m[2]*m[6]),
		    norm*(m[2]*m[3]-m[0]*m[5]),
		    norm*(m[3]*m[7]-m[4]*m[6]),
		    norm*(m[1]*m[6]-m[0]*m[7]),
		    norm*(m[0]*m[4]-m[1]*m[3]));}
  vector3d eigenvector(double x) {
    double d = (m[0]-x)*(m[4]-x)-m[1]*m[3];
    if (fabs(d)<1.0e-10) {
      d = (m[0]-x)*(m[8]-x)-m[6]*m[2];
      if (fabs(d)<1.0e-10) {
	d = (m[4]-x)*(m[8]-x)-m[5]*m[7];
	vector3d v(1.0, (m[5]*m[6]-m[3]*(m[8]-x))/d,
		   (m[3]*m[7]-m[6]*(m[4]-x))/d);
	v /= v.norm();
	return(v);
      }
      vector3d v((m[7]*m[2]-m[1]*(m[8]-x))/d, 1.0, 
		 (m[1]*m[6]-m[7]*(m[0]-x))/d);
      v /= v.norm();
      return(v);
      if(!(d!=0.0))
		  AfxMessageBox("d!=0.0");
    }
    vector3d v((m[5]*m[1]-m[2]*(m[4]-x))/d, (m[3]*m[2]-m[5]*(m[0]-x))/d, 1.0);
    v /= v.norm();
    return(v);
  }
private:
  double m[9];
};

/* eig3d: */

inline
double esgn(double x)
{
  return (x>=0.0)? 1.0:-1.0;
}

class eig3d {
public:
  eig3d(const matrix3d& m): flag(false) {
    a = -1.0*m.trace();
    b = m.invar3();
    c = -1.0*m.det();
    if (fabs(a)<1.0e-10) {
      offset = 0.0;
      Q = -b/3.0;
      R = 0.5*c;
    } else {
      offset = -a/3.0;
      Q = (a*a-3.0*b)/9.0;
      R = (2.0*a*a*a-9.0*a*b+27.0*c)/54.0;
    }
    discrim = R*R-Q*Q*Q;
  }
  bool complex_eigenvalues() {return discrim >= 0.0;}
  double real_eigenvalue() {
    set();
    return (A+B) + offset;
  }
  double complex_eigenvalue_re() {
    set();
    return -0.5*(A+B) + offset;
  }
  double complex_eigenvalue_im() {
    set();
    return 0.5*sqrt(3.0)*(A-B);
  }
private:
  void set() {
    if (!flag) {
      if (!complex_eigenvalues()) {
	std::cerr << "all eigenvalues are real\n";
	if(!complex_eigenvalues())
		AfxMessageBox("complex_eigenvalues()");
      }
      flag=true;
      A = -esgn(R)*pow(fabs(R)+sqrt(discrim),1.0/3.0);
      if (A != 0.0)
	B = Q/A;
      else
	B = 0;
    }
  }
  double a;
  double b;
  double c;
  double R;
  double Q;
  double offset;
  double discrim;
  bool flag;
  double A;
  double B;
};



class VelocityField {
public:
  VelocityField(int l, int m, int n);
  ~VelocityField() {delete [] vel;}
  double* operator()(int x, int y, int z) {return vel+3*(z+zsize*(y+ysize*x));}
  int x() const {return xsize;}
  int y() const {return ysize;}
  int z() const {return zsize;}
private:
  int xsize;
  int ysize;
  int zsize;
  double* vel;
  double drand();
  vector3d rand_vec(double x);
  vector3d solve(const matrix3d& mat, const vector3d& b);
  vector3d solve1(const matrix3d& mat, const vector3d& b);
  //main();
};


#endif /*  __3D_H__ */
