tty:example.o iot_tty.o
	cc -o tty example.o iot_tty.o
main.o:example.c
	cc -c example.c 
iot_tty.o:iot_tty.c iot_tty.h
	cc -c iot_tty.c

clean:
	rm example.o  iot_tty.o tty  *~
