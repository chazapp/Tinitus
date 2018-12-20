// Decibel.cpp
//
#include "pch.h"
#include "Decibel.h"

int main() {
	int time = 10;
	DBReader dBReader;
	if (dBReader.SetCurrentDevice() == false) {
		std::cout << "[Decibel]" << "No current devices found. Aborting" << std::endl;
		return 0;
	}
	for (int i = 0; i < time; i++) {
		float val = dBReader.GetCurrentdBLevel();
		std::cout << "[Decibel]: Current db level: " << val << std::endl;
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
	return 0;
}