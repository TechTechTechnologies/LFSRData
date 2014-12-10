#include <stdio.h>
#include <memory.h>

//Condense sequence length to unique sequence lengths and their numbers of instances per tap configuration


int main()
{

  int i, j, k;
  int res;
  int length;
  int temp;
  int hit;
  int tableIndex;
  int unique;
  int seqNum;

  FILE* inFile;
  FILE* outFile;

  const int tapNum = 4096;
  unsigned short *lenTable[tapNum];  //lenTable[taps][ic] = length

  unsigned short *freqTable[tapNum]; // freqTable[taps][len] = number of sequences of length [len], freqTable[taps][0] = total number of sequences

  unsigned short *condTable[tapNum]; // condTable[taps] = {number of sequences, {sequence length_i, number of sequences_i}}

  for(i = 0; i < tapNum; ++i)
  {
    lenTable[i] = malloc(tapNum*sizeof(unsigned short));
    memset(lenTable[i], 0, (tapNum)*sizeof(unsigned short));

    freqTable[i] = malloc(tapNum*sizeof(unsigned short));
    memset(freqTable[i], 0, (tapNum)*sizeof(unsigned short));

  }

  inFile = fopen("tapsTable", "rb");

  for(i = 0; i < tapNum; ++i)
  {
    fread(lenTable[i], sizeof(unsigned short), tapNum, inFile);
  }

  fclose(inFile);

  //compute sparse freq table

  for(i = 0; i < 4; ++i)
  {
    for(j = 0; j < 4; ++j)
    {
      printf("%4i\t", lenTable[i][j]);
    }
    printf("\n");
  }


  for(i = 0; i < tapNum; ++i)
  {
    printf("\r%3.1f%%", 100*((float)i)/tapNum);

    for(j = 0; j < tapNum; ++j)
    {
      if(lenTable[i][j] !=0 && lenTable[i][j] < tapNum)
      {
        ++freqTable[i][lenTable[i][j]];
        ++freqTable[i][0];
      }
    }  

  }

  //compute condensed frequency table

  outFile = fopen("condTable", "wb");

  printf("\n");
  for(i = 0; i < tapNum; ++i)
  {
    printf("\r%3.1f%%", 100*((float)i)/tapNum);
//printf("%x\t", i);    
    
    seqNum = 0;
    for(j = 1; j < tapNum; ++j)
    {
      if(freqTable[i][j] != 0) ++seqNum;
    }

    condTable[i] = calloc(1+seqNum*2, sizeof(unsigned short));
    condTable[i][0] = seqNum;

    seqNum = 0;
    for(j = 1; j < tapNum; ++j)
    {
      if(freqTable[i][j] != 0)
      {
        condTable[i][1+2*seqNum] = j;
        condTable[i][2+2*seqNum] = freqTable[i][j];
        ++seqNum;
//printf("%4i, %2f\t", condTable[i][2*j+1], 100*condTable[i][2*j+2]/(float)freqTable[i][0]);

      }
    }
//printf("\n");
    fwrite(condTable[i], sizeof(unsigned short), condTable[i][0]*2+1, outFile);

  }


  fclose(outFile);

  return 0;

}







