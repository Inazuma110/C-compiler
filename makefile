cc: cc.c

test: cc
	./test.sh

print: cc
	./print.sh

clean:
	rm -f cc *.o *~ tmp*
