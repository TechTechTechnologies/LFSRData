#ifndef LFSRUTILS_H
#define LFSRUTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <time.h>

//some functions for computing LFSR data



//getSequenceAtTaps, getAllSequences:
//Computes LFSR sequences
//MSB indicates non-transient component, lower 15 bits give sequence id starting at 1
//Note that this does not generate meaningful information for a taps configuration of 0

//Another note: at present this is not guarunteed to handle transients correctly. For example consider the sequence:
//1230456[789789...]
//The first run will mark 0456 as a transient belonging to sequence 1 and 789 as the repeating part of sequence 1,
//then, the second run will start at 1 and mark 123 as the transient part of sequence 2, then mark 0456 as repeating parts of sequence 1
//This does not appear to occur for 12-bit LFSRs

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

  start = 0;

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

