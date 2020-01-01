complie:
	g++ olikkal_bank_server.cpp -o server -lm -lpthread
	g++ olikkal_bank_client.cpp -o client 

runbankserver:
	./server 8988

runbankclient:
	./client 127.0.0.1 8988 0.2 Transaction.txt

runmultiplebankclient:
	chmod 755 script.sh
	./script.sh

clean:
	rm server
	rm client

