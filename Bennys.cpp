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
#include "CondensationProj.h"
#include "Condensation.h"
#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCondensation::CCondensation(LPSTR pdisplayname,LPSTR parameterStr,LPSTR resultparam,int resultbufsize):BaseOperator()
{
	//----------------------------------------------------------------------------------------------------------------
	// The following codes are added automatically by the iTools for Video. Do not modified the following manually
	//<!--iTools for Video  - Filter Wizard - INITIALIZE_VARIABLES
	//Input port variables
	iv_i_Input.Field_size = 0;
	iv_i_Input.X_size = 0;
	iv_i_Input.Y_size = 0;
	iv_i_Input.Data = 0;
	iv_i_Input.Data_size = 0;
	iv_i_Observation.Field_size = 0;
	iv_i_Observation.X_size = 0;
	iv_i_Observation.Y_size = 0;
	iv_i_Observation.Data = 0;
	iv_i_Observation.Data_size = 0;
	//Output port variables
	iv_o_Output.Field_size = 0;
	iv_o_Output.X_size = 0;
	iv_o_Output.Y_size = 0;
	iv_o_Output.Data = 0;
	iv_o_Output.Data_size = 0;
	//Parameter variables
	iv_p_No_Points = 0;
	iv_p_No_Samples = 0;
	iv_p_Mean = 0;
	iv_p_Scaling = 0;
	iv_p_Sigma = 0;
	//INITIALIZE_VARIABLES -->
	//----------------------------------------------------------------------------------------------------------------
	
	//*******************************
	//Initialize your pointers here
	//*******************************
	buffer=NULL;
	//----------------------------------------------------------------------------------------------------------------
	// The following codes are added automatically by the iTools for Video. 
	if (!parameterStr) return;//for constructing the object for own setup
	InitializeFilter(parameterStr); 
	//----------------------------------------------------------------------------------------------------------------
	
	
	//**********************************************
	//Add your filter initializiation code in here
	//**********************************************
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
	//----------------------------------------------------------------------------------------------------------------
	// The following codes are added automatically by the iTools for Video. Do not modified the following manually
	//You should set the output port's parameter using the structures outputport_XXX
	//<!--iTools for Video  - Filter Wizard - SET_OUTPUT_VARIABLES 
	//Initialize Output port variables
	iv_o_Output.Field_size = 0;
	iv_o_Output.Field_size_changed = true;
	iv_o_Output.X_size = 0;
	iv_o_Output.X_size_changed = true;
	iv_o_Output.Y_size = 0;
	iv_o_Output.Y_size_changed = true;
	iv_o_Output.Data = 0;
	iv_o_Output.Data_size = 0;
	iv_o_Output.Data_changed = true;
	//SET_OUTPUT_VARIABLES -->
	//----------------------------------------------------------------------------------------------------------------
	
	iv_o_Output.Field_size = sizeof (CPoint);
	iv_o_Output.X_size = bufsize;
	iv_o_Output.Y_size = bufsize;
	iv_o_Output.Data = buffer;
	iv_o_Output.Data_size = bufsize*2*sizeof(CPoint);
	//----------------------------------------------------------------------------------------------------------------
	// The following codes are added automatically by the iTools for Video. 
	//write the XML string for the output connection to another other
	CString str= CreateOutputXMLString();
	if (str.GetLength())
		strcpy(resultparam,(LPSTR)(LPCSTR)str);
	else strcpy(resultparam,"");
	//----------------------------------------------------------------------------------------------------------------
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


bool CCondensation::Process(LPSTR xml,int xmlbufsize)
{
	if (xml)InitializeFilter(xml);//read the port changes 
	
	//**********************************************
	//put your video processing functions in here
	//**********************************************
	//put your video processing functions in here
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
		iv_o_Output.Data_changed = true;
	}
	framecount++;
	firstframe= true;
	
	CString str= CreateOutputXMLString();
	if (str.GetLength())
		strcpy(xml,(LPSTR)(LPCSTR)str);
	else strcpy(xml,"");
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

bool CCondensation::Setup(LPSTR resultxml,int resultbufsize)
{
	//**********************************************
	//Put your own set up function in here
	//**********************************************
	return true;
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

//----------------------------------------------------------------------------------------------------------------
// The following codes are added automatically by the iTools for Video. Do not modified the following manually
void CCondensation::InitializeFilter(LPSTR parameterStr)
{
	if (!parameterStr) return;
	XMLObject *xmlobj = new XMLObject(CString(parameterStr));
	XMLObject *inputxml = xmlobj->FindObject("Input");//find the input port xml string 
	//read the input port values
	XMLObject *portxml;
	CString portname;
	if (inputxml) 
	{//look for the the value for each input port
		portxml = inputxml->Childs;
		while (portxml)
		{ 
			portname = portxml->GetAttributeValue("Name");
			//<!--iTools for Video  - Filter Wizard - EXTRACT_INPUT_XML_STRING
			if (!portname.Compare("Input"))
			{
				XMLObject *portxml_0 = portxml->Childs;
				while(portxml_0)
				{
					portname = portxml_0->GetAttributeValue("Name");
					if (!portname.Compare("Field size"))
						iv_i_Input.Field_size =  GetShortParameter(portxml_0);
					if (!portname.Compare("X size"))
						iv_i_Input.X_size =  GetIntParameter(portxml_0);
					if (!portname.Compare("Y size"))
						iv_i_Input.Y_size =  GetIntParameter(portxml_0);
					if (!portname.Compare("Data"))
						iv_i_Input.Data = GetBinaryParameter(portxml_0,iv_i_Input.Data_size);
					portxml_0=portxml_0->next;
				}
			}
			if (!portname.Compare("Observation"))
			{
				XMLObject *portxml_0 = portxml->Childs;
				while(portxml_0)
				{
					portname = portxml_0->GetAttributeValue("Name");
					if (!portname.Compare("Field size"))
						iv_i_Observation.Field_size =  GetShortParameter(portxml_0);
					if (!portname.Compare("X size"))
						iv_i_Observation.X_size =  GetIntParameter(portxml_0);
					if (!portname.Compare("Y size"))
						iv_i_Observation.Y_size =  GetIntParameter(portxml_0);
					if (!portname.Compare("Data"))
						iv_i_Observation.Data = GetBinaryParameter(portxml_0,iv_i_Observation.Data_size);
					portxml_0=portxml_0->next;
				}
			}
			//EXTRACT_INPUT_XML_STRING -->
			portxml = portxml->next;
		}
	} 
	//read the parameter values
	XMLObject *paramxml = xmlobj->FindObject("Parameter");//find the parameter string
	if (paramxml)
	{
		portxml = paramxml->Childs;
		while (portxml)
		{
			portname=portxml->GetAttributeValue("Name");
			//<!--iTools for Video  - Filter Wizard - EXTRACT_PARAMETER_XML_STRING
			if (!portname.Compare("No Points"))
				iv_p_No_Points =  GetIntParameter(portxml);
			if (!portname.Compare("No Samples"))
				iv_p_No_Samples =  GetIntParameter(portxml);
			if (!portname.Compare("Mean"))
				iv_p_Mean =  GetDoubleParameter(portxml);
			if (!portname.Compare("Scaling"))
				iv_p_Scaling =  GetDoubleParameter(portxml);
			if (!portname.Compare("Sigma"))
				iv_p_Sigma =  GetDoubleParameter(portxml);
			//EXTRACT_PARAMETER_XML_STRING -->
			portxml= portxml->next;
		}
	} 
	delete xmlobj;
}

CString CCondensation::CreateOutputXMLString()
{
	CString result=""; 
	XMLObject *resultxml = new XMLObject();
	resultxml->Name= CString("xml");
	resultxml->root=true;
	resultxml->AddAttributeValue("version","1.0");
	XMLObject *filterxml = resultxml->AddChild(new XMLObject());
	filterxml->Name = CString("Filter");
	filterxml->AddAttributeValue("Name","Condensation"); //filter's name
	XMLObject *outputxml = filterxml->AddChild("Output");//output port 
	bool anychanges=false;
	
	//<!--iTools for Video  - Filter Wizard - CREATE_OUTPUT_XML_STRING 
	//Update Output ports
	XMLObject *outputxml_0=outputxml->AddChild("Item");
	outputxml_0->AddAttributeValue("Name","Output");
	outputxml_0->AddAttributeValue("Type","2D Array");
	if (iv_o_Output.Field_size_changed)
	{
		if (iv_o_Output.Field_size_changed)
		{
			XMLObject *outputxml_0_0=outputxml_0->AddChild("Item");
			outputxml_0_0->AddAttributeValue("Name","Field size");
			outputxml_0_0->AddAttributeValue("Type","short");
			outputxml_0_0->AddAttributeValue("Value",iv_o_Output.Field_size);
			anychanges=true;
			iv_o_Output.Field_size_changed = false;
		}
	}
	if (iv_o_Output.X_size_changed)
	{
		if (iv_o_Output.X_size_changed)
		{
			XMLObject *outputxml_0_1=outputxml_0->AddChild("Item");
			outputxml_0_1->AddAttributeValue("Name","X size");
			outputxml_0_1->AddAttributeValue("Type","int");
			outputxml_0_1->AddAttributeValue("Value",iv_o_Output.X_size);
			anychanges=true;
			iv_o_Output.X_size_changed = false;
		}
	}
	if (iv_o_Output.Y_size_changed)
	{
		if (iv_o_Output.Y_size_changed)
		{
			XMLObject *outputxml_0_2=outputxml_0->AddChild("Item");
			outputxml_0_2->AddAttributeValue("Name","Y size");
			outputxml_0_2->AddAttributeValue("Type","int");
			outputxml_0_2->AddAttributeValue("Value",iv_o_Output.Y_size);
			anychanges=true;
			iv_o_Output.Y_size_changed = false;
		}
	}
	if (iv_o_Output.Data_changed)
	{
		if (iv_o_Output.Data_changed)
		{
			XMLObject *outputxml_0_3=outputxml_0->AddChild("Item");
			outputxml_0_3->AddAttributeValue("Name","Data");
			outputxml_0_3->AddAttributeValue("Type","Binary");
			outputxml_0_3->AddAttributeValue("Value",(DWORD)iv_o_Output.Data);
			outputxml_0_3->AddAttributeValue("Size",(DWORD)iv_o_Output.Data_size);
			anychanges=true;
			iv_o_Output.Data_changed = false;
		}
	}
	//CREATE_OUTPUT_XML_STRING -->
	if (anychanges) result = resultxml->WriteXMLString();
	delete resultxml;
	return result;
}
//----------------------------------------------------------------------------------------------------------------


--------------------------------------------------------------------------------


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
#include "DriverCommon.h"
//--------------------------------------------------------------------------------------------------------------
// The following codes are added automatically by the iTools for Video. Do not modified the following manually
//<!--iTools for Video  - Filter Wizard - DATA_STRUCT_DEFINE
#ifndef DEFINE_2D_Array
#define DEFINE_2D_Array
typedef struct iv_type_struct_2D_Array
{
	short Field_size;
	bool Field_size_changed;
	int X_size;
	bool X_size_changed;
	int Y_size;
	bool Y_size_changed;
	LPVOID Data;
	int Data_size;
	bool Data_changed;
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

class CCondensation :public BaseOperator 
{
public:
	CCondensation(LPSTR pdisplayname,LPSTR parameterStr,LPSTR resultparam,int resultbufsize);
	virtual ~CCondensation();
	
	bool Process(LPSTR xml,int xmlbufsize);//process the image
	bool Setup(LPSTR resultxml,int resultbufsize);
	
	void InitializeFilter(LPSTR paramStr);
	CString CreateOutputXMLString();
	
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
