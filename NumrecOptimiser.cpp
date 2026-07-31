// NumrecOptimiser.cpp: implementation of the CNumrecOptimiser class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Texton.h"
#include "NumrecOptimiser.h"
#ifndef ANSI
#define ANSI
#endif
#include "nrutil.h"
#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/* amoeba definitions */
#define AM_TINY 1.0e-10 // A small number.
#define NMAX 5000 // Maximum allowed number of function evaluations.
#define GET_PSUM \
	for (j=1;j<=ndim;j++) {\
	for (sum=0.0,i=1;i<=mpts;i++) sum += p[i][j];\
psum[j]=sum;}
#define SWAP(a,b) {swap=(a);(a)=(b);(b)=swap;}

/* powell's definitions */
#define PO_TINY 1.0e-25 // A small number.
#define ITMAX 200 // Maximum allowed iterations.
#include "nrutil.h"
#define TOL 2.0e-4 // Tolerance passed to brent.

/* mnbrak's definitions */
#define GOLD 1.618034
#define GLIMIT 100.0
#define MNB_TINY 1.0e-20
#define SHFT(a,b,c,d) (a)=(b);(b)=(c);(c)=(d);
// Here GOLD is the default ratio by which successive intervals are magnified; GLIMIT is the
// maximum magnification allowed for a parabolic-fit step.

/* brent's definitions */
#define BR_ITMAX 100
#define CGOLD 0.3819660
#define ZEPS 1.0e-10
// Here ITMAX is the maximum allowed number of iterations; CGOLD is the golden ratio; ZEPS is
// a small number that protects against trying to achieve fractional accuracy for a minimum that
// happens to be exactly zero.
#define SHFT(a,b,c,d) (a)=(b);(b)=(c);(c)=(d);

CNumrecOptimiser::CNumrecOptimiser()
{

}

CNumrecOptimiser::~CNumrecOptimiser()
{

}

int ncom; // for powell's fidim/linmin
float *pcom,*xicom,(*nrfunc)(float []); // for powell's fidim/linmin
float f1dim(float x);

void CNumrecOptimiser::amoeba(float * * p, float y [ ], int ndim, float ftol, 
							  float ( * funk ) ( float [ ] ), int * nfunk)
{

/*
Multidimensional minimization of the function funk(x) where x[1..ndim] is a vector in ndim
dimensions, by the downhill simplex method of Nelder and Mead. The matrix p[1..ndim+1]
[1..ndim] is input. Its ndim+1 rows are ndim-dimensional vectors which are the vertices of
the starting simplex. Also input is the vector y[1..ndim+1], whose components must be preinitialized
to the values of funk evaluated at the ndim+1 vertices (rows) of p; and ftol the
fractional convergence tolerance to be achieved in the function value (n.b.!). On output, p and
y will have been reset to ndim+1 new points all within ftol of a minimum function value, and
nfunk gives the number of function evaluations taken.
*/

	int i,ihi,ilo,inhi,j,mpts=ndim+1;
	float rtol,sum,swap,ysave,ytry,*psum;
	psum=vector(1,ndim);
	*nfunk=0;
	GET_PSUM
		for (;;) {
			ilo=1;
			// First we must determine which point is the highest (worst), 
			// next-highest, and lowest (best), by looping over the points 
			// in the simplex.
			ihi = y[1]>y[2] ? (inhi=2,1) : (inhi=1,2);
			for (i=1;i<=mpts;i++) {
				if (y[i] <= y[ilo]) ilo=i;
				if (y[i] > y[ihi]) {
					inhi=ihi;
					ihi=i;
				} else if (y[i] > y[inhi] && i != ihi) inhi=i;
			}
			rtol=2.0*fabs(y[ihi]-y[ilo])/(fabs(y[ihi])+fabs(y[ilo])+AM_TINY);
			// Compute the fractional range from highest to lowest and return if 
			// satisfactory.
			if (rtol < ftol) { 
				//If returning, put best point and value in slot 1.
				SWAP(y[1],y[ilo])
					for (i=1;i<=ndim;i++) SWAP(p[1][i],p[ilo][i])
						break;
			}
			if (*nfunk >= NMAX) nrerror("NMAX exceeded");
			*nfunk += 2;
			//Begin a new iteration. First extrapolate by a factor -1 through the face of the simplex
			//	across from the high point, i.e., reflect the simplex from the high point.
			ytry=amotry(p,y,psum,ndim,funk,ihi,-1.0);
			if (ytry <= y[ilo])
				//	Gives a result better than the best point, so try an additional extrapolation by a
				//	factor 2.
				ytry=amotry(p,y,psum,ndim,funk,ihi,2.0);
			else if (ytry >= y[inhi]) {
				//	The reflected point is worse than the second-highest, so look for an intermediate
				//		lower point, i.e., do a one-dimensional contraction.
				ysave=y[ihi];
				ytry=amotry(p,y,psum,ndim,funk,ihi,0.5);
				if (ytry >= ysave) 
				{ 
					// Can't seem to get rid of that high point. Better
					// contract around the lowest (best) point. 
					
					for (i=1;i<=mpts;i++) {
						if (i != ilo) {
							for (j=1;j<=ndim;j++)
								p[i][j]=psum[j]=0.5*(p[i][j]+p[ilo][j]);
							y[i]=(*funk)(psum);
						}
					}
					*nfunk += ndim; // Keep track of function evaluations.
					GET_PSUM // Recompute psum.
				}
			} else --(*nfunk); // Correct the evaluation count.
		} // Go back for the test of doneness and the next
		//iteration. 
		free_vector(psum,1,ndim);
}


float CNumrecOptimiser::amotry(float * * p, float y [ ], float psum [ ], int ndim, 
							   float ( * funk ) ( float [ ] ), int ihi, float fac)
{

/*
Extrapolates by a factor fac through the face of the simplex across from the high point, tries
it, and replaces the high point if the new point is better.
*/

	int j;
	float fac1,fac2,ytry,*ptry;
	ptry=vector(1,ndim);
	fac1=(1.0-fac)/ndim;
	fac2=fac1-fac;
	for (j=1;j<=ndim;j++) ptry[j]=psum[j]*fac1-p[ihi][j]*fac2;
	ytry=(*funk)(ptry); // Evaluate the function at the trial point.
	if (ytry < y[ihi]) { // If it's better than the highest, then replace the highest.
		y[ihi]=ytry;
		for (j=1;j<=ndim;j++) {
			psum[j] += ptry[j]-p[ihi][j];
			p[ihi][j]=ptry[j];
		}
	}
	free_vector(ptry,1,ndim);
	return ytry;
}

void CNumrecOptimiser::powell(float p [ ], float *xi[], int n, float ftol, 
							  int * iter, float * fret, float ( * func ) ( float []))
{

/*
Minimization of a function func of n variables. Input consists of an initial starting point
p[1..n]; an initial matrix xi[1..n][1..n], whose columns contain the initial set of directions
(usually the n unit vectors); and ftol, the fractional tolerance in the function value
such that failure to decrease by more than this amount on one iteration signals doneness. On
output, p is set to the best point found, xi is the then-current direction set, fret is the returned
function value at p, and iter is the number of iterations taken. The routine linmin is used.
*/
	int i,ibig,j;
	float del,fp,fptt,t,*pt,*ptt,*xit;
	pt=vector(1,n);
	ptt=vector(1,n);
	xit=vector(1,n);
	*fret=(*func)(p);
	for (j=1;j<=n;j++) pt[j]=p[j]; // Save the initial point.
	for (*iter=1;;++(*iter)) {
		fp=(*fret);
		ibig=0;
		del=0.0; // Will be the biggest function decrease.
		for (i=1;i<=n;i++) { // In each iteration, loop over all directions in the set.
			for (j=1;j<=n;j++) xit[j]=xi[j][i]; // Copy the direction,
			fptt=(*fret);
			linmin(p,xit,n,fret,func); // minimize along it,
			if (fptt-(*fret) > del) { // and record it if it is the largest decrease
				// so far. 
				del=fptt-(*fret);
				ibig=i;
			}
		}
		if (2.0*(fp-(*fret)) <= ftol*(fabs(fp)+fabs(*fret))+PO_TINY) {
			free_vector(xit,1,n); // Termination criterion.
			free_vector(ptt,1,n);
			free_vector(pt,1,n);
			return;
		}
		if (*iter == ITMAX) nrerror("powell exceeding maximum iterations.");
		for (j=1;j<=n;j++) { // Construct the extrapolated point and the
			// average direction moved. Save the
			// old starting point.
			ptt[j]=2.0*p[j]-pt[j];
			xit[j]=p[j]-pt[j];
			pt[j]=p[j];
		}
		fptt=(*func)(ptt); // Function value at extrapolated point.
		if (fptt < fp) {
			t=2.0*(fp-2.0*(*fret)+fptt)*SQR(fp-(*fret)-del)-del*SQR(fp-fptt);
			if (t < 0.0) {
				linmin(p,xit,n,fret,func); // Move to the minimum of the new direction,
				// and save the new direction. 
				for (j=1;j<=n;j++) {
					xi[j][ibig]=xi[j][n];
					xi[j][n]=xit[j];
				}
			}
		}
	} // Back for another iteration.
}

void CNumrecOptimiser::linmin(float p [ ], float xi [ ], int n, float * fret, float ( * func ) ( float [ ] ))
{
/*
Given an n-dimensional point p[1..n] and an n-dimensional direction xi[1..n], moves and
resets p to where the function func(p) takes on a minimum along the direction xi from p,
and replaces xi by the actual vector displacement that p was moved. Also returns as fret
the value of func at the returned location p. This is actually all accomplished by calling the
routines mnbrak and brent.
*/
	int j;
	float xx,xmin,fx,fb,fa,bx,ax;
	ncom=n; // Define the global variables.
	pcom=vector(1,n);
	xicom=vector(1,n);
	nrfunc=func;
	for (j=1;j<=n;j++) {
		pcom[j]=p[j];
		xicom[j]=xi[j];
	}
	ax=0.0; // Initial guess for brackets.
	xx=1.0;
	mnbrak(&ax,&xx,&bx,&fa,&fx,&fb,f1dim);
	*fret=brent(ax,xx,bx,f1dim,TOL,&xmin);
	for (j=1;j<=n;j++) { // Construct the vector results to return.
		xi[j] *= xmin;
		p[j] += xi[j];
	}
	free_vector(xicom,1,n);
	free_vector(pcom,1,n);
}

float f1dim(float x)
{
	int j;
	float f,*xt;
	xt=vector(1,ncom);
	for (j=1;j<=ncom;j++) xt[j]=pcom[j]+x*xicom[j];
	f=(*nrfunc)(xt);
	free_vector(xt,1,ncom);
	return f;
}

void CNumrecOptimiser::mnbrak(float * ax, float * bx, float * cx, float * fa, float * fb, float * fc, float ( * func ) ( float ))
{
/*
Given a function func, and given distinct initial points ax and bx, this routine searches in
the downhill direction (defined by the function as evaluated at the initial points) and returns
new points ax, bx, cx that bracket a minimum of the function. Also returned are the function
values at the three points, fa, fb, and fc.
	*/
	
	float ulim,u,r,q,fu,dum;
	*fa=(*func)(*ax);
	*fb=(*func)(*bx);
	if (*fb > *fa) { // Switch roles of a and b so that we can go
		//downhill in the direction from a to b. 
		SHFT(dum,*ax,*bx,dum)
			SHFT(dum,*fb,*fa,dum)
	}
	*cx=(*bx)+GOLD*(*bx-*ax); // First guess for c.
	*fc=(*func)(*cx);
	while (*fb > *fc) { // Keep returning here until we bracket.
		r=(*bx-*ax)*(*fb-*fc); // Compute u by parabolic extrapolation from
		// a; b; c. TINY is used to prevent any possible
		// division by zero.
		q=(*bx-*cx)*(*fb-*fa);
		u=(*bx)-((*bx-*cx)*q-(*bx-*ax)*r)/
			(2.0*SIGN(FMAX(fabs(q-r),MNB_TINY),q-r));
		ulim=(*bx)+GLIMIT*(*cx-*bx);
		// We won't go farther than this. Test various possibilities:
		if ((*bx-u)*(u-*cx) > 0.0) { // Parabolic u is between b and c: try it.
			fu=(*func)(u);
			if (fu < *fc) { // Got a minimum between b and c.
				*ax=(*bx);
				*bx=u;
				*fa=(*fb);
				*fb=fu;
				return;
			} else if (fu > *fb) { // Got a minimum between between a and u.
				*cx=u;
				*fc=fu;
				return;
			}
			u=(*cx)+GOLD*(*cx-*bx); // Parabolic fit was no use. Use default magnification. 
			fu=(*func)(u);
		} else if ((*cx-u)*(u-ulim) > 0.0) { // Parabolic fit is between c and its allowed limit. 
			fu=(*func)(u);
			if (fu < *fc) {
				SHFT(*bx,*cx,u,*cx+GOLD*(*cx-*bx))
					SHFT(*fb,*fc,fu,(*func)(u))
			}
		} else if ((u-ulim)*(ulim-*cx) >= 0.0) { // Limit parabolic u to maximum allowed value. 
			u=ulim;
			fu=(*func)(u);
		} else { // Reject parabolic u, use default magnication.
			u=(*cx)+GOLD*(*cx-*bx);
			fu=(*func)(u);
		}
		SHFT(*ax,*bx,*cx,u) // Eliminate oldest point and continue.
			SHFT(*fa,*fb,*fc,fu)
	}
}

float CNumrecOptimiser::brent(float ax, float bx, float cx, float ( * f ) ( float ), float tol, float * xmin)
{
/*
Given a function f, and given a bracketing triplet of abscissas ax, bx, cx (such that bx is
between ax and cx, and f(bx) is less than both f(ax) and f(cx)), this routine isolates
the minimum to a fractional precision of about tol using Brent's method. The abscissa of
the minimum is returned as xmin, and the minimum function value is returned as brent, the
returned function value.
*/
	
	int iter;
	float a,b,d,etemp,fu,fv,fw,fx,p,q,r,tol1,tol2,u,v,w,x,xm;
	float e=0.0; // This will be the distance moved on the step before last.
	a=(ax < cx ? ax : cx); // a and b must be in ascending order,
	// but input abscissas need not be. 
	b=(ax > cx ? ax : cx);
	x=w=v=bx; // Initializations...
	fw=fv=fx=(*f)(x);
	for (iter=1;iter<=BR_ITMAX;iter++) { // Main program loop.
		xm=0.5*(a+b);
		tol2=2.0*(tol1=tol*fabs(x)+ZEPS);
		if (fabs(x-xm) <= (tol2-0.5*(b-a))) { // Test for done here.
			*xmin=x;
			return fx;
		}
		if (fabs(e) > tol1) { // Construct a trial parabolic fit.
			r=(x-w)*(fx-fv);
			q=(x-v)*(fx-fw);
			p=(x-v)*q-(x-w)*r;
			q=2.0*(q-r);
			if (q > 0.0) p = -p;
			q=fabs(q);
			etemp=e;
			e=d;
			if (fabs(p) >= fabs(0.5*q*etemp) || p <= q*(a-x) || p >= q*(b-x))
				d=CGOLD*(e=(x >= xm ? a-x : b-x));
			// The above conditions determine the acceptability of the parabolic fit. Here we
			// take the golden section step into the larger of the two segments.
			else {
				d=p/q; // Take the parabolic step.
				u=x+d;
				if (u-a < tol2 || b-u < tol2)
					d=SIGN(tol1,xm-x);
			}
		} else {
			d=CGOLD*(e=(x >= xm ? a-x : b-x));
		}
		u=(fabs(d) >= tol1 ? x+d : x+SIGN(tol1,d));
		fu=(*f)(u);
		// This is the one function evaluation per iteration.
		if (fu <= fx) { // Now decide what to do with our function
			// evaluation. 
			if (u >= x) a=x; else b=x;
			SHFT(v,w,x,u) // Housekeeping follows:
				SHFT(fv,fw,fx,fu)
		} else {
			if (u < x) a=u; else b=u;
			if (fu <= fw || w == x) {
				v=w;
				w=u;
				fv=fw;
				fw=fu;
			} else if (fu <= fv || v == x || v == w) {
				v=u;
				fv=fu;
			}
			} // Done with housekeeping. Back for another iteration. 
	}
	nrerror("Too many iterations in brent");
	*xmin=x; // Never get here.
	return fx;
}
