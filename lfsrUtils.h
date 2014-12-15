#ifndef LFSRUTILS_H
#define LFSRUTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <time.h>
#include <math.h>

//some functions for computing LFSR data

//Complex math stuff

struct Complex
{
  float r,i;
};

struct Complex cMult(struct Complex a, struct Complex b)
{
  struct Complex result;
  result.r = a.r*b.r - a.i*b.i;
  result.i = a.r*b.i + a.i*b.r;
  return result;
}

struct Complex cAdd(struct Complex a, struct Complex b)
{
  struct Complex result;
  result.r = a.r+b.r;
  result.i = a.i+b.i;
  return result;
}

struct Complex iExp(float i)
{
  struct Complex result;
  result.r = cos(i);
  result.i = sin(i);
  return result;
}

//getSeriesAtTaps
//Computes fourier series amplitude of each sequences
//range = number of series coeffs to computer

float** getSeriesAtTaps(unsigned short* seqData, unsigned short tapNum, unsigned int range)
{

  int i,j;

  int k,n,N;

  unsigned short seqNum;           //Num of sequences
  int lenCounter;

  struct Complex accum;
  struct Complex coeff;
  float temp;

  float** specTable;    //specTable[sequenceID][n] = {N, c_0, c_1, ..., c_{N-1}}
                        // specTable[0][0] = number of sequences

  float* seqVals;       //For holding the LFSR feedback sequence values


  for(i = 0; i < tapNum; ++i) //Count the number of sequences by finding the largest value in the sequence array
  {
    if(seqData[i] > seqNum)
    {
      seqNum = seqData[i];
    }
  }

  seqNum &= 0x7FFF;

  specTable = calloc(seqNum+1, sizeof(float*));

  specTable[0] = calloc(1, sizeof(float)); //set number of seqeunces
  specTable[0][0] = seqNum;

  for(i = 1; i <= seqNum; ++i)  
  {
    lenCounter = 0;                 //count length of sequences and allocate 
    for(j = 0; j < tapNum; ++j)
    {
      if(seqData[j] == (i|0x8000))
      {
        ++lenCounter;
      }
    }

    specTable[i] = calloc(range+1, sizeof(float));
    specTable[i][0] = lenCounter;

    seqVals = calloc(lenCounter, sizeof(float));

    N = lenCounter;
    n = 0;
    for(j = 0; j < tapNum; ++j)     //load lfsr sequence values
    {
      if(seqData[j] == (i|0x8000))
      {
        seqVals[n] = (j&0x0001);
        ++n;
      }
    }

    // c_k = sinc(k/N)/N * e^{-j*pi*k/N} * \summat_{n=0}^{N-1} L_n * e^{-j*2*pi*kn/N}
    //where L_N is the LFSR output of 1 or 0
    for(k = 0; k < range; ++k) //computer series coefficients
    {

      if(k == 0)           //sinc function stuff
      {
        temp = 1/(float)N;
      }
      else
      {
        temp = sin(M_PI*k/N)/(M_PI*k);
      }

      coeff.r = cos(M_PI*k/N)*temp;
      coeff.i = -sin(M_PI*k/N)*temp;

      accum.r = 0;
      accum.i = 0;

      for(n = 0; n < N; ++n)         //summmation
      {
        if(seqVals[n] != 0)
        {
          temp = -2*M_PI*k*n/N;
          accum = cAdd(accum, iExp(temp));
        }
      }
      accum = cMult(accum, coeff);  //result

      specTable[i][k+1] = sqrt(accum.r*accum.r+accum.i*accum.i);

    }

  }

  return specTable;

}



//getSequenceAtTaps, getAllSequences:
//Computes LFSR sequences
//MSB indicates non-transient component, lower 15 bits give sequence id starting at 1
//Note that this does not generate meaningful information for a taps configuration of 0

unsigned short* getSequencesAtTaps(unsigned short taps, unsigned short mask, unsigned short tapNum)
{

  //given starting value, taps config, and masking value: run LFSR and record distinct sequences in an array

  unsigned short i;
  int done;
  int revise;
 
  int start;
  int next;
  int bits;
  int lfsr;

  unsigned short part;
  unsigned short* data;


  data = calloc(tapNum, sizeof(unsigned short));

  part = 1;

  start = 0;struct Complex
{
  float r,i;
};

  while(start <= mask)
  {

    lfsr = start;
    done = 0;
    revise = 0;
    do             //Record the transient and repeating sequence for the current value of start
    {

      if(data[lfsr] == 0)                //Look for previous instances of the current value
      {
        data[lfsr] = (part&0x7FFF);
      }
      else if((data[lfsr]&0x8000) == 0)  //Check for loop bit
      {
        if((data[lfsr]&0x7FFF) != (part&0x7FFF))  //On collision, set revise flasg to replacement seqeunce id and break. This probably never happens
        {
          printf("Boner\n");
          revise = (data[lfsr]&0x7FFF);
          break;
        }

        data[lfsr] |= 0x8000;
      }
      else                                //Loop complete, exit
      {
        done = 1;
      }

      next = 1;
      bits = lfsr&taps;

      while(bits)         //Compute LFSR feedback
      {
        next ^= (bits&1);
        bits>>=1;
      }

      lfsr <<=1;          //Shift LFSR
      lfsr |= next;

      lfsr &= mask;       //Mask LFSR


    } while(!done);

    if(revise != 0)
    {
      for( i = 0; i < mask; ++i)
      {
        if(data[i] == part)
          data[i] = revise;
      }
    }
    else
    {
      ++part;
    }

    while(start <= mask && data[start] != 0) ++start;
  }

  return data;

}

unsigned short** getAllSequences(unsigned short tapNum)
{

  unsigned short i, j, k;
  int res;
  unsigned short mask;
  int temp;
  int hit;
  int tableIndex;
  int unique;

  clock_t startTime;
  clock_t endTime;

  unsigned short **seqTable;  //seqTable[taps][val] = sequence id containing val

  seqTable = calloc(tapNum, sizeof(unsigned short*));

  startTime = clock();

  //run getSequences on every taps configuration

  mask = 0;
  for(i = 0; i < tapNum; ++i)
  {
    printf("\r%3.1f%%\t", 100*((float)i)/tapNum);
    hit = 0;

    mask |=i; // Ensure that length masks i, with assumptions that i always increases

    seqTable[i] = getSequencesAtTaps(i, mask, tapNum);

  }
  printf("\n");

  endTime = clock();
  printf("Operation completed in %f s\n", (float)(endTime-startTime)/CLOCKS_PER_SEC);

  return seqTable;

}


#endif

