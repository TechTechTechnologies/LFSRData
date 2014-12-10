#include <stdio.h>
#include <memory.h>

//Computes and writes LFSR sequence length table to a binary file as unsigned short

int* found;
#define tapNum 4096
//const int tapNum = 4096;

int getLength(int start, int taps, int length)
{
 
  int done;
  int counter;
  int next;
  int bits;
  int result;
  int lfsr;
  int last;

  done = 0;
  memset(found, 0, tapNum*sizeof(int));
  result = 0;
  lfsr = start;

  do
  {
    last = lfsr;
    next = 1;
    bits = lfsr&taps;

    while(bits) //Compute LFSR feedback
    {
      next ^= (bits&1);
      bits>>=1;
    }
    lfsr <<=1;
    lfsr |= next;
    lfsr &= length;
    ++result;    

    if(found[lfsr] == 0) //Look for previous instances of the current value
    {
      found[lfsr] = result;
    }
    else                       //Stop and compute result if there was one
    {
      result -= found[lfsr];
      done = 1;
    }


  } while(!done);

  return result;

}

int main()
{

  unsigned short i, j, k;
  int res;
  unsigned short length;
  int temp;
  int hit;
  int tableIndex;
  int unique;

  FILE* outFile;

  found = malloc(sizeof(int)*tapNum);

  unsigned short *lenTable[tapNum];  //lenTable[taps][ic] = length

  for(i = 0; i < tapNum; ++i)
  {
    lenTable[i] = malloc(tapNum*sizeof(unsigned short));
    memset(lenTable[i], 0, (tapNum)*sizeof(unsigned short));
  }

  length = 0;
  for(i = 0; i < tapNum; ++i)
  {
    printf("\r%3.1f%%\t%4x", 100*((float)i)/tapNum, length);
    hit = 0;

    length |=i; // Ensure that length masks i, with assumptions that i always increases

    for(j = 0; j <= length; ++j)
    {
      res = getLength(j, i, length);
      lenTable[i][j] = res;
    }
  }


  for(i = 0; i < 4; ++i)
  {
    for(j = 0; j < 4; ++j)
    {
      printf("%4i\t", lenTable[i][j]);
    }
    printf("\n");
  }

  outFile = fopen("tapsTable", "wb");

  for(i = 0; i < tapNum; ++i)
  {
    fwrite(lenTable[i], sizeof(unsigned short), tapNum, outFile);
  }

  fclose(outFile);

  return 0;

}







