int my_printf (const char* format, ...); 
void my_itoa (char* buf, int base, int d);
int tags_number(const char* format); 

#define MOV_WIDTH 7 
#define MOVL_WIDTH 8 
#define BUFFER_SIZE 80


int main(int argc, char** argv) 
{ 
    // const char* test = "%d and %c and %x and %u";
    // int tags = tags_number(test);
    int a = 2;
    int b = 3;
    char* test = "Hello";
    int last = -9;
    my_printf("%d %d %d %s %s %c %x %u %u \n", a,5,0, "asdf", test,'c',10, -5, last);
    printf("%d %d %d %s %s %c %x %u %u \n", a,5,0, "asdf", test,'c',10, -5,last);
    
    
    my_printf("%d %d %d \n", 4,6); 
    my_printf("%d %c %s \n", 0, 'R', "ufat");
    return 0;
}



/* Convert the integer D to a string and save the string in BUF. If
   BASE is equal to 'd', interpret that D is decimal, and if BASE is
   equal to 'x', interpret that D is hexadecimal. 
*/

void my_itoa (char *buf, int base, int d)
{
    char *p = buf;
    char *p1, *p2;
    unsigned long ud = d;
    int divisor = 10;
    
    /* If %d is specified and D is minus, put `-' in the head. */
    if (base == 'd' && d < 0)
    {
	*p++ = '-';
	buf++;
	ud = -d;
    }
    else if (base == 'x')
	divisor = 16;
    
    /* Divide UD by DIVISOR until UD == 0. */
    do
    {
	int remainder = ud % divisor;
	
	*p++ = (remainder < 10) ? remainder + '0' : remainder + 'a' - 10;
    }
    while (ud /= divisor);
    
    /* Terminate BUF. */
    *p = 0;
    
    /* Reverse BUF. */
    p1 = buf;
    p2 = p - 1;
    while (p1 < p2)
    {
	char tmp = *p1;
	*p1 = *p2;
	*p2 = tmp;
	p1++;
	p2--;
    }
}



/* Code from Quest libc */ 

int my_printf (const char *format, ...) {
    
    /* This is probably the laziest code I've ever written, but I'll come back to it if I have time */
    
    unsigned long ebp; 
    asm("\t movl %%ebp,%0" : "=r"(ebp));
    unsigned long* ret = ebp+4;   // pointer to the next instruction
    
    int i =0; 
  
    unsigned long start  = *ret-12;
    while (1)
    {
	unsigned long* ar_1 = start-MOV_WIDTH;
	char b[BUFFER_SIZE]; 
	my_itoa(b,'x',*ar_1);
	
	int len = strlen(b);
	if (b[len-2] == '8' && b[len-1] == 'b' && b[len-4] == '4' && b[len-3]=='5')
	{ 
	    start = start - MOV_WIDTH;
	    i++; 
	    continue;
	}
	else 
	{ 
	    char b2[BUFFER_SIZE];
	    ar_1 = start-MOVL_WIDTH; 
	    my_itoa(b2,'x',*ar_1);
	    int len2 = strlen(b2);
	    if (b2[len2-2] == 'c' && b2[len2-1]=='7' && b2[len2-4]=='4' && b2[len2-3]=='4') 
	    { 
		start = start-MOVL_WIDTH; 
		i++; 
		continue;
	    }
	    else 
		break;
	}
	
    }
    int tag_num = tags_number(format);
    if (tag_num!= i) 
    { 
	my_printf("%s \n", "Number of format tags doesnt match the number of arguments"); 
	return 0;
    }
    
    
    

    char **arg = (char **) &format;
    
    
    
    int c;
    char buf[20];
    int count = 0;
    
    arg++;
    
    while ((c = *format++) != 0)
    {
	if (c != '%') {
	    putchar (c);
        count++;
	}
	else
	{
	    char *p;
	    
	    c = *format++;
	    switch (c)
	    {
	    case 'd':
	    case 'u':
	    case 'x':
		my_itoa (buf, c, *((int *) arg++));
		p = buf;
		goto string;
		break;
		
	    case 's':
		p = *arg++;
		if (! p)
		    p = "(null)";
		
	    string:
		while (*p) {
		    putchar (*p++);
		    count++;
		}
		break;
		
	    default:
		putchar (*((int *) arg++));
		count++;
		break;
	    }
	}
    }
    return count;
}


int tags_number(const char* format) 
{ 
    int count = 0; 
    while (*format != '\0') 
    { 
	if (*format++ == '%')
	    count++;
	
    }
    return count;
} 

