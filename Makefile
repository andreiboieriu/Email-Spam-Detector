CFLAGS=-Wall -Wextra

SRCS=$(wildcard *.c)
OBJS=$(SRCS:%.c=%.o)

build: $(OBJS) -lm
	gcc $(CFLAGS) -o spam_detector $(OBJS) -lm

pack:
	zip -FSr 312CA_Boieriu_Andrei_Teodor_SpamDetector.zip README Makefile *.c *.h additional_keywords

run:
	./spam_detector

clean:
	rm -f spam_detector *.gch $(OBJS)

.PHONY: pack clean
