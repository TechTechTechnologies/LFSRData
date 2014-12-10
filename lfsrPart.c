#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

//Computes and writes LFSR sequences to a binary file as unsigned short
//MSB indicates non-transient component, lower 15 bits give sequence id starting at 1
//Note that this does not generate meaningful information for a taps configuration of 0

//Another note: at present this is not guarunteed to handle transients correctly. For example consider the sequence:
//1230456[789789...]
//The first run will mark 0456 as a transient belonging to sequence 1 and 789 as the repeating part of sequence 1,
//then, the second run will start at 1 and mark 123 as the transient part of sequence 2, then mark 0456 as repeating parts of sequence 1

#define tapNum 4096

void getSequences(int taps, int mask, unsigned short* data)
{

  //given starting value, taps config, and masking value: run LFSR and record distinct sequences in an array

  int done;
 
  int start;
  int next;
  int bits;
  int lfsr;

  unsigned short part;

  part = 1;

  start = 0;

  while(start <= mask)
  {

    lfsr = start;
    done = 0;
    do             //Record the transient and repeating sequence for the current value of start
    {

      if(data[lfsr] == 0)                //Look for previous instances of the current value
      {
        data[lfsr] = (part&0x7FFF);
      }
      else if((data[lfsr]&0x8000) == 0)  //Check for loop bit
      {
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

    ++part;

    while(start <= mask && data[start] != 0) ++start;
  }

}

int main()
{

  unsigned short i, j, k;
  int res;
  unsigned short mask;
  int temp;
  int hit;
  int tableIndex;
  int unique;

  FILE* outFile;


  unsigned short *seqTable[tapNum];  //seqTable[taps][val] = sequence id containing val

  for(i = 0; i < tapNum; ++i)
  {
    seqTable[i] = calloc(tapNum, sizeof(unsigned short));
  }

  //run getSequences on every taps configuration

  mask = 0;
  for(i = 0; i < tapNum; ++i)
  {
    printf("\r%3.1f%%\t", 100*((float)i)/tapNum);
    hit = 0;

    mask |=i; // Ensure that length masks i, with assumptions that i always increases

    getSequences(i, mask, seqTable[i]);

  }
  printf("\n");

  //print sample for sanity check

  for(i = 0; i < 8; ++i)
  {
    for(j = 0; j < 8; ++j)
    {
      printf("%4.4x\t", seqTable[i][j]);
    }
    printf("\n");
  }

  //write sequence table to file seqsTable

  outFile = fopen("seqsTable", "wb");

  for(i = 0; i < tapNum; ++i)
  {
    fwrite(seqTable[i], sizeof(unsigned short), tapNum, outFile);
  }

  fclose(outFile);

  return 0;

}







