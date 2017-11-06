echo -e "\033[32m Please select region\033[0m"
echo -e "\033[32m 1:US(default)\033[0m"
echo -e "\033[32m 2:EU\033[0m"
echo -e "\033[32m 3:JP\033[0m"
echo -e "\033[32m 4:AU\033[0m"

read -p "" REGION

echo "loraNodeLib/" > .mbedignore
rm lib*.a

if [ "$REGION" == "1" ]; then
	echo "Ok, build US"
	cp loraNodeLib/*US*.a .
elif [ "$REGION" == "2" ]; then
	echo "Ok, build EU"
	cp loraNodeLib/*EU*.a .
elif [ "$REGION" == "3" ]; then
	echo "Ok, build JP"
	cp loraNodeLib/*JP*.a .
elif [ "$REGION" == "4" ]; then
	echo "Ok, build AU"
	cp loraNodeLib/*AU*.a .
else
	echo "Default: build US"
	cp loraNodeLib/*US*.a .
fi

#mbed compile -t GCC -m NUCLEO_L443RC  -c
#mbed compile -t GCC -m NUCLEO_L443RC  -c
mbed compile -c
