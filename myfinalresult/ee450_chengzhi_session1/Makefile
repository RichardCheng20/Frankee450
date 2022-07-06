CC = g++ 
CFLAGS = -g -Wall -std=c++0x 

all: scheduler.cpp hospitalA.cpp hospitalB.cpp hospitalC.cpp client.cpp

	$(CC) $(CFLAGS) -o scheduler scheduler.cpp

	$(CC) $(CFLAGS) -o hospitalA hospitalA.cpp

	$(CC) $(CFLAGS) -o hospitalB hospitalB.cpp

	$(CC) $(CFLAGS) -o hospitalC hospitalC.cpp

	$(CC) $(CFLAGS) -o client client.cpp

scheduler:
	./scheduler

hospitalA:
	./hospitalA

hospitalB:
	./hospitalB

hospitalC:
	./hospitalC

client:
	./client
	
clean:
	rm -f scheduler hospitalA hospitalB hospitalC client
