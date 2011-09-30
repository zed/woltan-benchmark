plot: Test.csv
	gnuplot -persist Plot.gplt
	
Test.csv:
	g++ -ldl main.cpp -o bench
	python main.py
		
clean:
	rm -f *.csv bench
