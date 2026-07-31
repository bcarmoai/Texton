
//*******************************************************************
//* iTools for Video *
//* *
//* Filter Wizard Created file *
//* *
//* by Benny Lo *
//* August 5, 2002 *
//*******************************************************************
// Condensation.cpp: implementation of the CCondensation class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Condensation.h"
#include <math.h>
#define PI 3.141592654

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCondensation::CCondensation(int no_points, int no_samples, 
							double mean, double scaling, double sigma,
							LPVOID inputPoints, LPVOID observationPoints)
{
	//----------------------------------------------------------------------------------------------------------------
	// The following codes are added automatically by the iTools for Video. Do not modified the following manually
	//<!--iTools for Video  - Filter Wizard - INITIALIZE_VARIABLES
	//Input port variables
	iv_i_Input.X_size = 0;
	iv_i_Input.Y_size = 0;
	iv_i_Input.Data = inputPoints; // from 0 by BSC 270303
	iv_i_Input.Data_size = 0;
	iv_i_Observation.X_size = 0;
	iv_i_Observation.Y_size = 0;
	iv_i_Observation.Data = observationPoints; // from 0 by BSC 270303
	iv_i_Observation.Data_size = 0;
	//Output port variables
	iv_o_Output.X_size = 0;
	iv_o_Output.Y_size = 0;
	iv_o_Output.Data = 0;
	iv_o_Output.Data_size = 0;
	//Parameter variables
	iv_p_No_Points = no_points;
	iv_p_No_Samples = no_samples;
	iv_p_Mean = mean;
	iv_p_Scaling = scaling;
	iv_p_Sigma = sigma;

	buffer=NULL;

	firstframe=false;
	framecount=0;
	inputbuf = (CPoint *)iv_i_Input.Data;
	nopoints = iv_p_No_Points;
	novariables= iv_p_No_Points*2;//number of variables in the model
	nosample = iv_p_No_Samples;
	observation_sigma=iv_p_Sigma;//the diffusion the observation
	next_gaussian=0;
	saved_gaussian=0;
	variables = new CondensationModelVariable[novariables];
	//initialize the parameters
	for (int i=0;i<novariables;i++)
	{
		variables[i].sigma=iv_p_Sigma;
		variables[i].scalling=iv_p_Scaling;
		variables[i].sample=new double[nosample];
		variables[i].oldsample=new double[nosample];
		variables[i].cumul_array=new double[nosample];
		variables[i].max_cumul=nosample;
		variables[i].weight=new double[nosample]; 
		variables[i].est=0;
		variables[i].observed=0;
		for (int j=0;j<nosample;j++)
		{
			variables[i].cumul_array[j] =j;
			variables[i].weight[j] =1.0;
			variables[i].sample[j] =0;
			variables[i].oldsample[j] = DEFAULT_PRIOR_MEAN+(DEFAULT_SIGMA*gaussian_random());
		}
	} 
	//store the parameters
	bufsize=iv_p_No_Points * nosample;
	buffer = new CPoint[bufsize];
	for (i=0;i<(int)bufsize;i++)
	{
		buffer[i].x=0;
		buffer[i].y=0;
	} 
}

CCondensation::~CCondensation()
{
	//----------------------------------------------------------------------------------------------------------------
	// The following codes are added automatically by the iTools for Video. Do not modified the following manually
	//<!--iTools for Video  - Filter Wizard - RELEASE_VARIABLES
	//RELEASE_VARIABLES -->
	//----------------------------------------------------------------------------------------------------------------
	
	//**********************************************
	//Free the memory objects in here
	//**********************************************
	delete buffer;
	for (int i=0;i<novariables;i++)
	{
		delete variables[i].sample;
		delete variables[i].oldsample;
		delete variables[i].cumul_array;
		delete variables[i].weight;
	}
	delete variables;
}


bool CCondensation::Process()
{
	
	loopbackbuf = (CPoint *) iv_i_Observation.Data;
	if (firstframe && loopbackbuf)
	{ 
		if (framecount>2)
		{
			Predict();//predict 
			Observation(loopbackbuf);
			Update();//update the weights 
		} 
		else 
		{
			for (int i=0;i<nopoints;i++)
			{
				variables[i*2].est=inputbuf[i].x;
				variables[i*2].observed=inputbuf[i].x;
				variables[i*2+1].est=inputbuf[i].y;
				variables[i*2+1].observed=inputbuf[i].y;
			} 
		} 
		int i1,i2;
		double est1,est2;
		int ipos;
		for (int i=0;i<nopoints;i++)
		{
			i1=i*2;
			i2=i*2+1;
			est1=variables[i1].est;
			est2=variables[i2].est;
			for (int j=0;j<nosample;j++)
			{
				ipos = i*nosample+j;
				buffer[ipos].x=(int)(variables[i1].sample[j]+est1);
				buffer[ipos].y=(int)(variables[i2].sample[j]+est2);
			} 
		} 
		//if there is any changes in the output value
		//iv_o_Output.Data_changed = true;  // commented out by BSC 260303
	}
	framecount++;
	firstframe= true;

	return true;
}

void CCondensation::Observation(CPoint *pbuf)
{
	for (int i=0;i<nopoints;i++)
	{
		variables[i*2].observed = pbuf[i].x;
		variables[i*2+1].observed = pbuf[i].y;
	}
} 

bool CCondensation::SetVariableParameters(int pvariable, double sigma,double scalling,
										  double priormean)
{
	if (pvariable>=novariables)
		return false;
	variables[pvariable].sigma=sigma;
	variables[pvariable].scalling=scalling;
	variables[pvariable].est=priormean;
	variables[pvariable].observed=priormean;
	return true;
}

double CCondensation::uniform_random(void)
{//obtain normalized random number
	return (double) rand()/(double) RAND_MAX;
}

double CCondensation::gaussian_random()
{//generate gaussian random variables
	double fac,rsq,v1,v2;
	if (next_gaussian ==0) 
	{
		do {
			v1=2.0*uniform_random()-1.0;
			v2=2.0*uniform_random()-1.0;
			rsq=v1*v1+v2*v2;
		}
		while (rsq>=1.0 || rsq==0.0);
		fac=sqrt(-2.0*log(rsq)/rsq);
		saved_gaussian=v1*fac;
		next_gaussian=1;
		return v2*fac;
	}
	else {
		next_gaussian=0;
		return saved_gaussian;
	}
}

double CCondensation::evaluate_gaussian(double val,double sigma)
{//Gaussian distance from the observation
	return 1.0/(sqrt(2.0*PI)*sigma)*exp(-0.5*(val*val/(sigma*sigma)));
}

//The process model for a first-order auto-regressive process is 
//x-(t+1) - mean = (xt-mean)*scaling +sigma*wt
//where wt is unit iid Gaussian noice
double CCondensation::iterate_first_order_auto_regressiveprocess(double previous,double mean,double scaling,double sigma)
{
	return mean+(previous-mean)*scaling+sigma*gaussian_random();
}

//Binary search using cumulative probabilities to pick a base sample. The use of this routine
//makes CONDENSATION O(NlogN) where N is the number of samples.  It is probably better to
//pick base samples deterministically, since then the algorithm is O(N) and probabily marginaly more efficient,
int CCondensation::pick_base_sample(double *cumul_prob_array,double largest_cumul_prob)
{
	double choice = uniform_random()*largest_cumul_prob;
	int low,middle,high;
	low=0;
	high=nosample;
	while (high>(low+1))
	{
		middle = (high+low)/2;
		if (choice > cumul_prob_array[middle])
			low=middle;
		else high = middle;
	}
	return low;
}

double CCondensation::evaluate_observation_density(double estpos,double observedpos)
{
	return evaluate_gaussian(estpos-observedpos,observation_sigma);
}

//predict the t+1 value for each sample from its previous state using stochastic approximation with Gaussian noise
//Note scalling is the weight for from the previous state to the next state, note
// it will have a damping effect on the result and may speed up the process
// sigma is the uncertainty in the prediction, 
// mean is set to "0" for the reason that the prediction is made upon the previous state,
// and it is not from a offset of the mean
void CCondensation::predict_sample_position(double *newpos,int new_sample,
											double *oldpos,int old_sample,
											double mean,double scalling,double sigma)
{ //note mean is not used and set to 0
	newpos[new_sample] = iterate_first_order_auto_regressiveprocess(oldpos[old_sample],mean,scalling,sigma);
}

void CCondensation::Predict()
{//predict the next state of the samples of each variable 
	for (int i=0;i<novariables;i++)
	{//perform prediction on each variable in the model
		for (int j=0;j<nosample;j++)
		{
			predict_sample_position(variables[i].sample,j,variables[i].oldsample,
				//pick the base sample through cumulative probabilities
				pick_base_sample(variables[i].cumul_array,variables[i].max_cumul),
				0,variables[i].scalling,variables[i].sigma);
		}
	}
}

void CCondensation::Update()
{
	for (int i=0;i<novariables;i++)
	{
		double cumul_total=0;
		//update the weight with the observation
		for (int j=0;j<nosample;j++)
		{
			variables[i].weight[j] = evaluate_gaussian(variables[i].sample[j]+variables[i].est-variables[i].observed,observation_sigma); 
			cumul_total +=variables[i].weight[j];
			variables[i].cumul_array[j] = cumul_total;
		}
		variables[i].max_cumul=cumul_total;
		
		double aggregate=0;
		double temp;
		for (j=0;j<nosample;j++)
		{
			aggregate+=variables[i].sample[j]*variables[i].weight[j];
			temp = variables[i].oldsample[j];
			variables[i].oldsample[j] = variables[i].sample[j];
			variables[i].sample[j] = temp;
		}
		aggregate/=variables[i].max_cumul;
		variables[i].est = variables[i].observed;//aggregate+variables[i].est;
	}
}

