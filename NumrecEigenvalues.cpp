// NumrecEigenvalues.cpp: implementation of the CNumrecEigenvalues class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "texton.h"
#include "NumrecEigenvalues.h"
#ifndef ANSI
#define ANSI
#endif
#include "nrutil.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#include <math.h>
#define RADIX 2.0

CNumrecEigenvalues::CNumrecEigenvalues()
{

}

CNumrecEigenvalues::~CNumrecEigenvalues()
{

}

void CNumrecEigenvalues::balanc(float **a, int n)
// Given a matrix a[1..n][1..n], this routine replaces it by a balanced matrix with identical
// eigenvalues. A symmetric matrix is already balanced and is unafected by this procedure. The
// parameter RADIX should be the machine's floating-point radix.
{
	int last,j,i;
	float s,r,g,f,c,sqrdx;
	sqrdx=RADIX*RADIX;
	last=0;
	while (last == 0) {
		last=1;
		for (i=1;i<=n;i++) { // Calculate row and column norms.
			r=c=0.0;
			for (j=1;j<=n;j++)
				if (j != i) 
				{
					c += fabs(a[j][i]);
					r += fabs(a[i][j]);
				}
				if (c && r) 
				{ // If both are nonzero,
					g=r/RADIX;
					f=1.0;
					s=c+r;
					while (c<g) 
					{ 
						//Find the integer power of the machine radix that
						//comes closest to balancing the matrix. 
						f *= RADIX;
						c *= sqrdx;
					}
					g=r*RADIX;
					while (c>g) 
					{
						f /= RADIX;
						c /= sqrdx;
					}
					if ((c+r)/f < 0.95*s) 
					{
						last=0;
						g=1.0/f;
						for (j=1;j<=n;j++) a[i][j] *= g; // Apply similarity transformation.
						for (j=1;j<=n;j++) a[j][i] *= f;
					}
				}
		}
	}
}

#define SWAP(g,h) {y=(g);(g)=(h);(h)=y;}
void CNumrecEigenvalues::elmhes(float **a, int n)
// Reduction to Hessenberg form by the elimination method. The real, nonsymmetric matrix
// a[1..n][1..n] is replaced by an upper Hessenberg matrix with identical eigenvalues. Recommended,
// but not required, is that this routine be preceded by balanc. On output, the
// Hessenberg matrix is in elements a[i][j] with i . j+1. Elements with i > j+1 are to be
// thought of as zero, but are returned with random values.
{
	int m,j,i;
	float y,x;
	for (m=2;m<n;m++) 
	{ 
		// m is called r + 1 in the text.
		x=0.0;
		i=m;
		for (j=m;j<=n;j++) 
		{ 
			// Find the pivot.
			if (fabs(a[j][m-1]) > fabs(x)) {
				x=a[j][m-1];
				i=j;
			}
		}
		if (i != m) 
		{ 
			// Interchange rows and columns.
			for (j=m-1;j<=n;j++) SWAP(a[i][j],a[m][j])
				for (j=1;j<=n;j++) SWAP(a[j][i],a[j][m])
		}
		if (x) { 
			// Carry out the elimination.
			for (i=m+1;i<=n;i++) 
			{
				if ((y=a[i][m-1]) != 0.0) 
				{
					y /= x;
					a[i][m-1]=y;
					for (j=m;j<=n;j++)
						a[i][j] -= y*a[m][j];
					for (j=1;j<=n;j++)
						a[j][m] += y*a[j][i];
				}
			}
		}
	}
}

void CNumrecEigenvalues::hqr(float **a, int n, float wr[], float wi[])
// Finds all eigenvalues of an upper Hessenberg matrix a[1..n][1..n]. On input a can be
// exactly as output from elmhes x11.5; on output it is destroyed. The real and imaginary parts
// of the eigenvalues are returned in wr[1..n] and wi[1..n], respectively.
{
	int nn,m,l,k,j,its,i,mmin;
	float z,y,x,w,v,u,t,s,r,q,p,anorm;
	anorm=0.0; // Compute matrix norm for possible use in locating
	// single small subdiagonal element. 
	for (i=1;i<=n;i++)
		for (j=IMAX(i-1,1);j<=n;j++)
			anorm += fabs(a[i][j]);
	nn=n;
	t=0.0; // Gets changed only by an exceptional shift.
	while (nn >= 1) 
	{ // Begin search for next eigenvalue.
		its=0;
		do 
		{
			for (l=nn;l>=2;l--) 
			{ // Begin iteration: look for single small subdiagonal element. 
				s=fabs(a[l-1][l-1])+fabs(a[l][l]);
				if (s == 0.0) s=anorm;
				if ((float)(fabs(a[l][l-1]) + s) == s) break;
			}
			x=a[nn][nn];
			if (l == nn) 
			{ // One root found.
				wr[nn]=x+t;
				wi[nn--]=0.0;
			} 
			else 
			{
				y=a[nn-1][nn-1];
				w=a[nn][nn-1]*a[nn-1][nn];
				if (l == (nn-1)) 
				{ 
					// Two roots found...
					p=0.5*(y-x);
					q=p*p+w;
					z=sqrt(fabs(q));
					x += t;
					if (q >= 0.0) 
					{ 
						// ...a real pair.
						z=p+SIGN(z,p);
						wr[nn-1]=wr[nn]=x+z;
						if (z) wr[nn]=x-w/z;
						wi[nn-1]=wi[nn]=0.0;
					} 
					else 
					{ // ...a complex pair.
						wr[nn-1]=wr[nn]=x+p;
						wi[nn-1]= -(wi[nn]=z);
					}
					nn -= 2;
				} 
				else 
				{ // No roots found. Continue iteration.
					if (its == 30) nrerror("Too many iterations in hqr");
					if (its == 10 || its == 20) 
					{ // Form exceptional shift.
						t += x;
						for (i=1;i<=nn;i++) a[i][i] -= x;
						s=fabs(a[nn][nn-1])+fabs(a[nn-1][nn-2]);
						y=x=0.75*s;
						w = -0.4375*s*s;
					}
					++its;
					for (m=(nn-2);m>=l;m--) 
					{ 
						// Form shift and then look for 2 consecutive small subdiagonal elements.
						z=a[m][m];
						r=x-z;
						s=y-z;
						p=(r*s-w)/a[m+1][m]+a[m][m+1]; // Equation (11.6.23).
						q=a[m+1][m+1]-z-r-s;
						r=a[m+2][m+1];
						s=fabs(p)+fabs(q)+fabs(r); // Scale to prevent overflow or underflow. 
						p /= s;
						q /= s;
						r /= s;
						if (m == l) break;
						u=fabs(a[m][m-1])*(fabs(q)+fabs(r));
						v=fabs(p)*(fabs(a[m-1][m-1])+fabs(z)+fabs(a[m+1][m+1]));
						if ((float)(u+v) == v) break; // Equation (11.6.26).
					}
					for (i=m+2;i<=nn;i++) 
					{
						a[i][i-2]=0.0;
						if (i != (m+2)) a[i][i-3]=0.0;
					}
					for (k=m;k<=nn-1;k++) 
					{
						// Double QR step on rows l to nn and columns m to nn.
						if (k != m) 
						{
							p=a[k][k-1]; // Begin setup of Householder vector. 
							q=a[k+1][k-1];
							r=0.0;
							if (k != (nn-1)) r=a[k+2][k-1];
							if ((x=fabs(p)+fabs(q)+fabs(r)) != 0.0) {
								p /= x; // Scale to prevent overflow or underflow.
								q /= x;
								r /= x;
							}
						}
						if ((s=SIGN(sqrt(p*p+q*q+r*r),p)) != 0.0) 
						{
							if (k == m) 
							{
								if (l != m)
									a[k][k-1] = -a[k][k-1];
							} 
							else
								a[k][k-1] = -s*x;
							p += s; // Equations (11.6.24).
							x=p/s;
							y=q/s;
							z=r/s;
							q /= p;
							r /= p;
							for (j=k;j<=nn;j++) 
							{ // Row modification.
								p=a[k][j]+q*a[k+1][j];
								if (k != (nn-1)) {
									p += r*a[k+2][j];
									a[k+2][j] -= p*z;
								}
								a[k+1][j] -= p*y;
								a[k][j] -= p*x;
							}
							mmin = nn<k+3 ? nn : k+3;
							for (i=l;i<=mmin;i++) 
							{ // Column modification.
								p=x*a[i][k]+y*a[i][k+1];
								if (k != (nn-1)) 
								{
									p += z*a[i][k+2];
									a[i][k+2] -= p*r;
								}
								a[i][k+1] -= p*q;
								a[i][k] -= p;
							}
						}
					}
				}
			}
		} while (l < nn-1);
	}
}

void CNumrecEigenvalues::ComputeEigenvalues(float **a, float *wr, float *wi, int n)
{
	balanc(a, n);
	elmhes(a, n);
	hqr(a, n, wr, wi);
}

#define SWAP2(a,b) {temp=(a);(a)=(b);(b)=temp;}
int CNumrecEigenvalues::gaussj(float **a, int n, float **b, int m)
/* return 0 for error, 1 for success */
/*Linear equation solution by Gauss-Jordan elimination, equation (2.1.1) above. a[1..n][1..n]
is the input matrix. b[1..n][1..m] is input containing the m right-hand side vectors. On
output, a is replaced by its matrix inverse, and b is replaced by the corresponding set of solution
vectors.*/
{
	int *indxc,*indxr,*ipiv;
	int i,icol,irow,j,k,l,ll;
	float big,dum,pivinv,temp;
	indxc=ivector(1,n); // The integer arrays ipiv, indxr, and indxc are
	// used for bookkeeping on the pivoting. 
	indxr=ivector(1,n);
	ipiv=ivector(1,n);
	for (j=1;j<=n;j++) ipiv[j]=0;
	for (i=1;i<=n;i++) { //This is the main loop over the columns to be reduced. 
		big=0.0;
		for (j=1;j<=n;j++) // This is the outer loop of the search for a pivot element. 
			if (ipiv[j] != 1)
				for (k=1;k<=n;k++) {
					if (ipiv[k] == 0) {
						if (fabs(a[j][k]) >= big) {
							big=fabs(a[j][k]);
							irow=j;
							icol=k;
						}
					} else if (ipiv[k] > 1) /*nrerror("gaussj: Singular Matrix-1");*/return 0;
				}
		++(ipiv[icol]);

		/* 
		We now have the pivot element, so we interchange rows, if needed, to put the pivot
		element on the diagonal. The columns are not physically interchanged, only relabeled:
		indxc[i], the column of the ith pivot element, is the ith column that is reduced, while
		indxr[i] is the row in which that pivot element was originally located. If indxr[i] 6=
		indxc[i] there is an implied column interchange. With this form of bookkeeping, the
		solution b's will end up in the correct order, and the inverse matrix will be scrambled
		by columns. */

		if (irow != icol) {
			for (l=1;l<=n;l++) SWAP2(a[irow][l],a[icol][l])
				for (l=1;l<=m;l++) SWAP2(b[irow][l],b[icol][l])
		}
		indxr[i]=irow; // We are now ready to divide the pivot row by the
		// pivot element, located at irow and icol. 
		indxc[i]=icol;
		if (a[icol][icol] == 0.0) /*nrerror("gaussj: Singular Matrix-2");*/return 0;
		pivinv=1.0/a[icol][icol];
		a[icol][icol]=1.0;
		for (l=1;l<=n;l++) a[icol][l] *= pivinv;
		for (l=1;l<=m;l++) b[icol][l] *= pivinv;
		for (ll=1;ll<=n;ll++) // Next, we reduce the rows...
			if (ll != icol) { // ...except for the pivot one, of course.
				dum=a[ll][icol];
				a[ll][icol]=0.0;
				for (l=1;l<=n;l++) a[ll][l] -= a[icol][l]*dum;
				for (l=1;l<=m;l++) b[ll][l] -= b[icol][l]*dum;
			}
	}

	/*This is the end of the main loop over columns of the reduction. It only remains to unscramble
	the solution in view of the column interchanges. We do this by interchanging pairs of
	columns in the reverse order that the permutation was built up.*/

	for (l=n;l>=1;l--) {
		if (indxr[l] != indxc[l])
			for (k=1;k<=n;k++)
				SWAP2(a[k][indxr[l]],a[k][indxc[l]]);
	} // And we are done.
	free_ivector(ipiv,1,n);
	free_ivector(indxr,1,n);
	free_ivector(indxc,1,n);
	return 1;
}
