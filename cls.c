//
//	clear screen -- must be run in PUTTY
//

#include "cls.h"

void cls()
{

char FF[5]={27,'[','2','J'}; 		// the character to clear the terminal screen
char FG[6]={27,'[',1,1,'f'};    // move the curser to the upper left of the screen


printf("%s", FF);
printf("%s", FG);


}
