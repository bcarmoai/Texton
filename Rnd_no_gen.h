/* Copyright (c) 2001 S.E. Grigorescu */

#ifndef _RND_NO_GEN_H_
#define _RND_NO_GEN_H_

/** \ingroup misc 
*/                                                                
void SetGenRand(unsigned long seed);
/*! \fn void SetGenRand(unsigned long seed); 

  Sets the seed of the random number generator. SetGenRand(seed) must be called at least once before any other function from this library that generates random numbers.
*/

/** \ingroup misc 
*/                                                                
double GenRand();
/*! \fn double GenRand();

  Draws one pseudo-random real number (double) in the range [0, 1] from a uniform distribution. 
*/

/** \ingroup misc 
*/                                                                
double GenRand(double lo, double hi);
/*! \fn double GenRand(double lo, double hi);

  Draws one pseudo-random real number (double) in the range [lo, hi] from a uniform distribution. 
*/

/** \ingroup misc 
*/                                                                
double GaussRand();
/*! \fn double GaussRand();

  Draws one pseudo-random real number (double) from a normal distribution with mean 0 and standard deviation 1. 
*/

/** \ingroup misc 
*/                                                                
double GaussRand(double mean, double stdev);
/*! \fn double GaussRand(double mean, double stdev);

  Draws one pseudo-random real number (double) from a normal distribution with the mean \a mean and standard deviation \a stdev. 
*/

#endif
