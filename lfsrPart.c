#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include "lfsrUtils.h"

//Computes and writes LFSR sequences to a binary file as unsigned short
//MSB indicates non-transient component, lower 15 bits give sequence id starting at 1
//Note that this does not generate meaningful information for a taps configuration of 0

//Another note: at present this is not guarunteed to handle transients correctly. For example consider the sequence:
//1230456[789789...]
//The first run will mark 0456 as a transient belonging to sequence 1 and 789 as the repeating part of sequence 1,
//then, the second run will start at 1 and mark 123 as the transient part of sequence 2, then mark 0456 as repeating parts of sequence 1

#define tapNum 4096


int main()
{

  unsigned short i, j, k;

  FILE* outFile;


  unsigned short **seqTable;  //seqTable[taps][val] = sequence id containing val

  seqTable = getAllSequences(tapNum);

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







