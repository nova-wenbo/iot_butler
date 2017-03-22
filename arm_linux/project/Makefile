objects = iot.o iot_tty.o
cc = gcc
iot:$(objects)
	cc -o iot $(objects) -I. -L. -let-client3a
iot.o:iot.c ETClient.h
	cc -c iot.c 
iot_tty.o:iot_tty.c iot_tty.h
	cc -c iot_tty.c


.PHONY : clean
clean :
	-rm iot $(objects)

