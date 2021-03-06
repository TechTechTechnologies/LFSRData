#include <stdio.h>
#include <memory.h>

//Base Code for reading condensed LFSR sequence length data


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


  return 0;

}







