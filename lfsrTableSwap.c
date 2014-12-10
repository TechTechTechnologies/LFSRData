#include <stdio.h>
#include <memory.h>

//Create table giving Tap configurations for each sequence length


int main()
{

  int i, j, k;
  int res;
  int length;
  int temp;
  int hit;
  int tableIndex;
  int index;

  FILE* inFile;

  const int tapNum = 4096;
  unsigned short *condTable[tapNum];  // condTable[taps] = {number of sequences, {sequence length_i, number of sequences_i}}
  unsigned short seqNum;

  unsigned short *invTable[tapNum]; // invTable[len] = {tap_i}
  float* invFreqTable[tapNum]; // invFreqTable = {len chance for tap_i}

  for(i = 0; i < tapNum; ++i)
  {
    invTable[i] = calloc(tapNum, sizeof(unsigned short));
    invFreqTable[i] = calloc(tapNum, sizeof(float));
  }

  inFile = fopen("condTable", "rb");

  for(i = 0; i < tapNum; ++i)
  {
    fread(&seqNum, sizeof(unsigned short), 1, inFile);
    condTable[i] = calloc(seqNum*2+2, sizeof(unsigned short));
    condTable[i][0] = seqNum;
    fread(condTable[i]+2, sizeof(unsigned short), seqNum*2, inFile);
  }

  fclose(inFile);

  for(i = 0; i < tapNum; ++i)
  {
    condTable[i][1] = 0;
    for(j = 0; j < condTable[i][0]; ++j)
    {
      condTable[i][1]+=condTable[i][2*j+3];
    }
  }

  for(i = 0; i < tapNum; ++i)
  {
    printf("%i\t", i);
    index = 0;
    for(j = 0; j < tapNum; ++j)
    {
      for(k = 0; k < condTable[j][0]; ++k)
      {
        if(condTable[j][2*k+2] == i)
        {
          invTable[i][index] = j;
          invFreqTable[i][index] = 100*((float)condTable[j][2*k+3])/condTable[j][1];
          printf("%4.4o, %2.0f\t", j, invFreqTable[i][index]);
          ++index;
        }
      }
    }
    printf("\n");

  }




  return 0;

}







