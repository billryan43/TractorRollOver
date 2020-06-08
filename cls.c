//
//	clear screen -- must be run in PUTTY
//

#include "cls.h"
#include <stdio.h>
#include <stdlib.h>

static char clearData[128];
static int  clearLen=0;

void cls()
{

  if (clearLen==0) { 
    FILE *pipe=popen("clear", "r");
    for (char c=fgetc(pipe); !feof(pipe); c=fgetc(pipe)) { 
      if (clearLen>sizeof(clearData)) {
        fprintf(stderr,"error reading clear screen data, size > %d\n", sizeof(clearData));
        break;
      }
      clearData[clearLen++]=c;
    }
    fclose(pipe);
  }

  fwrite(clearData,clearLen,1,stdout);   
  
}
