#include <iostream>
#include <chrono>
#include <algorithm>

#pragma intrinsic(_umul128)

uint64_t a = 42;
uint64_t c;

uint64_t lehmer64() {
	a = _umul128(a, 0xda942042e4dd58b5, &c);
	return c;
}

int main() {

	size_t KB = 1024;

	// from 1k to 128MB  
	for (int i = 0, size = 1; i < 18; ++i, size <<= 1) {
		size_t intsInSize = (size * KB) / sizeof(int);

		// data init
		auto* data = new int[intsInSize];

		// data fill
		for (int r = 0; r < intsInSize; ++r) {
			data[r] = lehmer64() % INT_MAX;
		}

		std::chrono::steady_clock::time_point start, stop;

		int c = 0;
		int counts = intsInSize * 10;

		int cL = 0;
		int tmp = 0;

		const unsigned mesCount = 3;
		size_t mes[mesCount];

		for (int m = 0; m < mesCount; ++m) {

			start = std::chrono::high_resolution_clock::now();

			for (c = 0; c < counts; c++) {
				// random access to data array
				tmp += data[cL & (intsInSize - 1)];

				// compute next access ptr that is further away than 1024
				cL += 1024 + (tmp + lehmer64()) & 1023;
			}

			stop = std::chrono::high_resolution_clock::now();
			mes[m] = (stop - start).count() / intsInSize;
		}

		// median of measure
		std::sort(mes, &mes[mesCount]);

		// do not remove tmp from output, it gets optimized away if you do ;)
		std::cout << mes[mesCount / 2] << " " << tmp << std::endl;

		// data destruct
		delete[] data;
	}
}
