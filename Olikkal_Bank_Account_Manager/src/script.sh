#! /bin/bash

for ((animalclient = 1; animalclient <= 100; animalclient++))
do
	./client 127.0.0.1 8988 0.2 Transaction.txt;
done
