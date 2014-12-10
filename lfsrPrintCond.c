#include <stdio.h>
#include <memory.h>

//Code for printing condensed table


int main()
{

  int i, j, k;
  int res;
  int length;
  int temp;
  int hit;
  int tableIndex;
  int unique;

  FILE* inFile;

  const int tapNum = 4096;
  unsigned short *condTable[tapNum];  // condTable[taps] = {number of sequences, {sequence length_i, number of sequences_i}}
  unsigned short seqNum;
  float total;

  for(i = 0; i < tapNum; ++i)
  {
    condTable[i] = malloc(tapNum*sizeof(unsigned short));
    memset(condTable[i], 0, (tapNum)*sizeof(unsigned short));
  }

  inFile = fopen("condTable", "rb");

  for(i = 0; i < tapNum; ++i)
  {
    fread(&seqNum, sizeof(unsigned short), 1, inFile);
    condTable[i] = calloc(seqNum*2+1, sizeof(unsigned short));
    condTable[i][0] = seqNum;
    fread(condTable[i]+1, sizeof(unsigned short), seqNum*2, inFile);
  }

  fclose(inFile);

  for(i = 0; i < tapNum; ++i)
  {
    printf("%4.4o, %i\t", i, condTable[i][0]);
    total = 0;

    for(j = 0; j < condTable[i][0]; ++j)
    {
      total+=condTable[i][2*j+2];
    }
    for(j = 0; j < condTable[i][0]; ++j)
    {
      printf("%4i, %2.0f\t", condTable[i][2*j+1], 100*condTable[i][2*j+2]/total);
    }
    printf("\n");
  }

  return 0;

}







