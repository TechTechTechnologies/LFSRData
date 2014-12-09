#include <stdio.h>
#include <memory.h>

int getLength(int start, int taps, int length)
{

  int* found;
  
  int done;
  int counter;
  int next;
  int bits;
  int result;
  int lfsr;
  int last;

  done = 0;
  found = (int*)malloc(10000*sizeof(int));
  memset(found, 0, 10000*sizeof(int));
  result = 0;
  lfsr = start;

  do
  {
    last = lfsr;
    next = 1;
    bits = lfsr&taps;
    while(bits)
    {
      next ^= (bits&1);
      bits>>=1;
    }
    lfsr <<=1;
    lfsr |= next;
    ++result;    

    if(found[lfsr&length] == 0)
    {
      found[lfsr&length] = result;
    }
    else
    {
      result -= found[lfsr&length];
      done = 1;
    }


  } while(!done);

  free(found);

  return result;

}

int main()
{

  int i, j, k;
  int res;
  int length;
  int temp;
  int hit;
  int tableIndex;
  int unique;

  FILE* outFile;

  const int tapNum = 4096;
  unsigned short *lenTable[tapNum];  //lenTable[taps][ic] = length

  for(i = 0; i < tapNum; ++i)
  {
    lenTable[i] = malloc(tapNum*sizeof(unsigned short));
    memset(lenTable[i], 0, (tapNum)*sizeof(unsigned short));
  }


  for(i = 0; i < tapNum; ++i)
  {
    printf("\r%3.1f%%", 100*((float)i)/tapNum);
    hit = 0;
    length = 0;
    temp = i;
    while(i)
    {
      i>>=1;
      length |= i;
    }
    i = temp;

    for(j = 0; j <= length; ++j)
    {
      res = getLength(j, i, length);
      lenTable[i][j] = res;
    }
  }

  outFile = fopen("tapsTable", "wb");

  for(i = 0; i < tapNum; ++i)
  {
    fwrite(lenTable[i], sizeof(unsigned short), i, outFile);
  }

  fclose(outFile);

  return 0;

}







