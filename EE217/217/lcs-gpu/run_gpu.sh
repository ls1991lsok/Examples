
echo "-------- Compiling ---------"
make 
echo "-------- Compile done ---------\n\n"

echo "-------- Run random input from (2000,2000) to (12000,12000) ---------"
./LCS 2000 2000 > ./results/2000.out
./LCS 3000 3000 > ./results/3000.out
./LCS 4000 4000 > ./results/4000.out
./LCS 5000 5000 > ./results/5000.out
./LCS 6000 6000 > ./results/6000.out
./LCS 7000 7000 > ./results/7000.out
./LCS 8000 8000 > ./results/8000.out
./LCS 9000 9000 > ./results/9000.out
./LCS 10000 10000 > ./results/10000.out
./LCS 11000 11000 > ./results/11000.out
./LCS 12000 12000 > ./results/12000.out



