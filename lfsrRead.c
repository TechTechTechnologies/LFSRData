#include <stdio.h>
#include <memory.h>

//Base Code for reading LFSR sequence length data


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
  unsigned short *lenTable[tapNum];  //lenTable[taps][ic] = length

  for(i = 0; i < tapNum; ++i)
  {
    lenTable[i] = malloc(tapNum*sizeof(unsigned short));
    memset(lenTable[i], 0, (tapNum)*sizeof(unsigned short));
  }

  inFile = fopen("tapsTable", "rb");

  for(i = 0; i < tapNum; ++i)
  {
    fread(lenTable[i], sizeof(unsigned short), i, inFile);
  }

  fclose(inFile);

  return 0;

}







