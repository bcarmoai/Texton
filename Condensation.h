
//*******************************************************************
//* iTools for Video *
//* *
//* Filter Wizard Created file *
//* *
//* by Benny Lo *
//* August 5, 2002 *
//*******************************************************************
// Condensation.h: interface for the CCondensation class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_Condensation_H__224AB4B9_1B2A_4EBB_AAF2_A5573BC0D349__INCLUDED_)
#define AFX_Condensation_H__224AB4B9_1B2A_4EBB_AAF2_A5573BC0D349__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//--------------------------------------------------------------------------------------------------------------
// The following codes are added automatically by the iTools for Video. Do not modified the following manually
//<!--iTools for Video  - Filter Wizard - DATA_STRUCT_DEFINE
#ifndef DEFINE_2D_Array
#define DEFINE_2D_Array
typedef struct iv_type_struct_2D_Array
{
	int X_size;
	int Y_size;
	LPVOID Data;
	int Data_size;

} iv_type_2D_Array;
#endif
//DATA_STRUCT_DEFINE -->
//--------------------------------------------------------------------------------------------------------------
#define DEFAULT_SIGMA 10
#define DEFAULT_SCALLING 1.0
#define DEFAULT_PRIOR_MEAN 0

#define DEFAULT_NO_SAMPLE 500
typedef struct CondensationModelstruct
{
	double sigma; //the standard deviation of the model variable
	double scalling; //the damping ratio for prediction from one state to the next
	double priormean; //mean of the initial probability (ex inital position)
	
	double *sample; //sample points
	double *oldsample; //previous sample points
	double *cumul_array;//array of cumulative probabilities
	double max_cumul; //largest cumulative probabilty for normalization purpose
	double *weight; //weight
	
	double observed;
	double est; //estimated result
} CondensationModelVariable;

class CCondensation
{
public:
	CCondensation(int no_points, int no_samples, 
				  double mean, double scaling, double sigma,
				  LPVOID inputPoints, LPVOID observationPoints);
	~CCondensation();
	
	bool Process();//process the image
	
	//**********************************************
	//Add your filter variable names in here
	//**********************************************
	double observation_sigma;
	int novariables;
	int nosample;
	CondensationModelVariable *variables;
	
	
	bool SetVariableParameters(int pvariable,double sigma,
		double scalling,double priormean);
	void Observation(CPoint *pbuf);
	void Predict();
	void Update();
	
	int nopoints;
	CPoint *inputbuf;
	DWORD bufsize;
	CPoint *buffer;
	CPoint *loopbackbuf;
	bool firstframe;
	int framecount;
protected:
	double uniform_random(void);
	double gaussian_random();//generate gaussian random variables
	double evaluate_gaussian(double val,double sigma);
	double iterate_first_order_auto_regressiveprocess(double previous,double mean,double scaling,double sigma);
	int pick_base_sample(double *cumul_prob_array,double largest_cumul_prob);
	void predict_sample_position(double *newpos,int new_sample,double *oldpos,int old_sample,
		double mean,double scalling,double sigma);
	double evaluate_observation_density(double estpos,double observedpos);
	
	int next_gaussian;
	double saved_gaussian; 
	//--------------------------------------------------------------------------------------------------------------
	// The following codes are added automatically by the iTools for Video. Do not modified the following manually
	//<!--iTools for Video  - Filter Wizard - DEFINE_VARIABLES
	//Input
	iv_type_2D_Array iv_i_Input;
	iv_type_2D_Array iv_i_Observation;
	//Output
	iv_type_2D_Array iv_o_Output;
	//Parameter
	int iv_p_No_Points;
	int iv_p_No_Samples;
	double iv_p_Mean;
	double iv_p_Scaling;
	double iv_p_Sigma;
	//DEFINE_VARIABLES -->
	//--------------------------------------------------------------------------------------------------------------
};
#endif // !defined(AFX_Condensation_H__224AB4B9_1B2A_4EBB_AAF2_A5573BC0D349__INCLUDED_)
