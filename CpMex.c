/*
 
 By Sebastien Gros
 Assistant Professor
 
 Department of Signals and Systems
 Chalmers University of Technology
 SE-412 96 Göteborg, SWEDEN
 
 Compute the evaluation of a surface B-spline (order 3), using the cox-deBoor/Böhm formula
 Provides 1st and second-order derivatives (function EvalSpline) or 1st and 2nd (function EvalSpline2)
 The blending functions are code-generated in Blender.h (for-looped version commented in this code, can be used with splines of different order than 3)
 
 */


#include <stdio.h>
#include "mex.h"

int findspan(float xi, const float knots[], const int lenght_knots)
{
    // Binary search over the knots
    int index_up   =  lenght_knots;
    int index_low  =  0;
    int index_middle;

    while (index_up - index_low > 1)
    {
        index_middle = ((index_up+index_low)/2);
        if (xi < knots[index_middle])
        {
            index_up = index_middle;
        }
        else
        {
            index_low = index_middle;
        }
    }
    return index_low;
}

// Unrolled blending function
#include "Blender.h"

// Original blending function
/*
 float Pijeval(const float mat[],int i,int j,const int n)
 {
 // Line major "matrix" P -> take out i,j entry
 
 //printf("index = %d\n", i*n + j);
 return mat[i*n + j];
 }
 
 
 float Blend(float x_basis[], float y_basis[], int i_x, int i_y, const float P[], const int n, int length_x_basis, int length_y_basis)
 {
 //printf("Blending \n");
 float S = 0;
 float x_basis_k1;
 for (int k1 = 0; k1 < length_x_basis; k1++)
 {
 x_basis_k1 = x_basis[k1];
 for (int k2 = 0; k2 < length_y_basis; k2++)
 {
 S += Pijeval(P,i_x-k1,i_y-k2,n)*x_basis_k1*y_basis[k2];
 }
 }
 
 return S;
 }
*/

//#include "BasisFunc.h"




int basisFuncs(float basis[], float xi, const int order, const float U[], int i)
{
    //Minimal implementation of the Cox-deBoor formula

    
    // UNROLLING THESE LOOPS IMPROVES THE SPEED BY ONLY A FEW %
    
    int iplus = i+1;
    int pminus;
    int ipluspminusk;
    
    float Uiplus = U[iplus];
    float Ui     = U[i];
    float Den    = Uiplus - U[i];
    
    int k = 0;
    int p = 0;
    
    //Compute the first step (special branch, p=1, Ni,0 = 1)
    basis[0] = (xi     - Ui) / Den;
    basis[1] = (Uiplus -   xi) / Den;
    
    
    //Clear out the remaining values of basis
    for (k = 2; k < order+1; k++)
    {
        basis[k] = 0;
    }
    
    //These loops could be unrolled into two different functions, depending on the order (2 or 3)
    for (p = 2; p < order+1; p++)
    {
        pminus   = p-1;
        
        //Update of Ni-p,p is standalone (cross arrow):
        Den  = Uiplus - U[i-pminus];
        basis[p] = (Uiplus - xi)*basis[pminus] / Den;

        for (k=p-1; k > 0; k--)
        {
            //Flat arrow
            basis[k]  = (xi - U[i-k])*basis[k] / Den;
            
            //Cross arrow
            ipluspminusk = iplus+p-k;
            Den   =  U[ipluspminusk] - U[iplus-k];
            basis[k] += (U[ipluspminusk] - xi)*basis[k-1] / Den;

        }
        // Update of Ni,p is standalone (flat arrow):
        basis[0] = (xi - Ui)*basis[0] / Den;
        
    }
    return 0;
}


int EvalSpline0(float x, float y, float out[])
{
    
    #include "Cp.h"
    
    //interpolation points >= 0
    x -= x_shift;
    y -= y_shift;
    
    // Eval Spline
    int ix = findspan(x, knots_x, length_knots_x);
    int iy = findspan(y, knots_y, length_knots_y);
    
    float basis_x[p+1];
    float basis_y[q+1];
    
    basisFuncs(basis_x, x, p, knots_x, ix);
    basisFuncs(basis_y, y, q, knots_y, iy);
    
    out[0] = Blend44(basis_x, basis_y, ix, iy, P, n);
    
    return 0;
}


int EvalSpline1(float x, float y, float out[])
{
    
    #include "Cp.h"
    
    //interpolation points >= 0
    x -= x_shift;
    y -= y_shift;
    
    // Eval Spline
    int ix = findspan(x, knots_x, length_knots_x);
    int iy = findspan(y, knots_y, length_knots_y);
     
    float basis_x[p+1];      
    float basis_y[q+1];
    
    basisFuncs(basis_x, x, p, knots_x, ix);
    basisFuncs(basis_y, y, q, knots_y, iy);
    
    out[0] = Blend44(basis_x, basis_y, ix, iy, P, n);

    // Eval derivatives
    int ix_tilde = findspan(x, Ux, length_Ux);
    int iy_tilde = findspan(y, Uy, length_Uy);
    
    float basis_x_tilde[p];
    float basis_y_tilde[q];
    
    basisFuncs(basis_x_tilde, x, p-1, Ux, ix_tilde);
    basisFuncs(basis_y_tilde, y, q-1, Uy, iy_tilde);

    out[1] = Blend34(basis_x_tilde, basis_y,       ix_tilde, iy,       Px, n   );
    out[2] = Blend43(basis_x      , basis_y_tilde, ix,       iy_tilde ,Py, n-1 );
    
    return 0;
}


int EvalSpline2(float x, float y, float out[])
{
    
    #include "Cp.h"
    
    //interpolation points >= 0
    x -= x_shift;
    y -= y_shift;
    
    // Eval Spline
    int ix = findspan(x, knots_x, length_knots_x);
    int iy = findspan(y, knots_y, length_knots_y);
    
    float basis_x[p+1];
    float basis_y[q+1];
    
    basisFuncs(basis_x, x, p, knots_x, ix);
    basisFuncs(basis_y, y, q, knots_y, iy);
    
    out[0] = Blend44(basis_x, basis_y, ix, iy, P, n);
    
    // Eval derivatives
    int ix_tilde = findspan(x, Ux, length_Ux);
    int iy_tilde = findspan(y, Uy, length_Uy);
    
    float basis_x_tilde[p];
    float basis_y_tilde[q];
    
    basisFuncs(basis_x_tilde, x, p-1, Ux, ix_tilde);
    basisFuncs(basis_y_tilde, y, q-1, Uy, iy_tilde);
    
    out[1] = Blend34(basis_x_tilde, basis_y,       ix_tilde, iy,       Px, n   );
    out[2] = Blend43(basis_x      , basis_y_tilde, ix,       iy_tilde ,Py, n-1 );
    
    //Eval curvature
    int ixx = findspan(x, Uxx, length_Uxx);
    int iyy = findspan(y, Uyy, length_Uyy);
     
    float basis_xx[p-1];
    float basis_yy[q-1];
     
    basisFuncs(basis_xx,   x,   p-2,   Uxx,    ixx);
    basisFuncs(basis_yy,   y,   q-2,   Uyy,    iyy);
     
    out[3] = Blend24(basis_xx,      basis_y,            ixx,       iy,  Pxx,    n );
    out[4] = Blend42(basis_x,       basis_yy,            ix,      iyy,  Pyy,  n-2 );
    out[5] = Blend33(basis_x_tilde, basis_y_tilde, ix_tilde, iy_tilde,  Pxy,  n-1 );
    
    return 0;
}

void mexFunction(
                 int nlhs, mxArray *plhs[],
                 int nrhs, const mxArray *prhs[])
{

    double *ptr_x;
    double *ptr_y;
    float out[6];
    int i;
    
    ptr_x = mxGetPr(prhs[0]);
    ptr_y = mxGetPr(prhs[1]);
    
    plhs[0] = mxCreateDoubleMatrix(6, 1, mxREAL);

    double *Out;
    Out = mxGetPr(plhs[0]);
    
    EvalSpline2(*ptr_x,*ptr_y,out);
    
    //mexPrintf("Beta  :%f \n ",*ptr_x);
    //mexPrintf("Lambda:%f \n ",*ptr_y);
    
    
    for (i=0;i<6;i++)
    {
        Out[i] = out[i];
    }
   
        
}


