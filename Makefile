all: 
	gcc -o finds main.c 
	gcc -o my_printf my_printf.c

clean: 
	rm -f finds 
	rm -f my_printf