#!/bin/bash
for i in {1..1500}
do 
	./client $[ $RANDOM %2 ] $[ $RANDOM %2 ] $[ $RANDOM %2 ] $[ $RANDOM %2 ]
	#echo $[ $RANDOM %2 ] $[ $RANDOM %2 ] $[ $RANDOM %2 ] $[ $RANDOM %2 ]
done

