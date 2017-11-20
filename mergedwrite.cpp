#include <algorithm>
#include <array>
#include <cstdlib>

#include "wav_file.h"
#include "wav_direct_write.h"

using namespace std;

struct sample_t {
	array<uint8_t, 3> val;
	sample_t(int32_t to_write = 0) {
		uint8_t *init = reinterpret_cast<uint8_t*>(&to_write);
		copy(init, init+3, val.data());
	}
};

int main(int argc, char **argv) {
	wav_t<uint32_t> inp = read_wav<uint32_t>(argv[1]);
	wav_header_t header(1,44100,3);
	wav_writer_t<sample_t> outp(argv[2], header);

	int psize = atoi(argv[3]), msize=atoi(argv[4]);
	for (int n_=0; n_<200; ++n_) {
		for (int i=48000; i<48000 + msize; ++i) {
			int32_t val1 = int32_t(int16_t(inp.samples[psize+i] & 0xffffU))<<8, val2 = int32_t(int16_t(inp.samples[i] & 0xffffU))<<8;
			outp.add_sample(val1 + int64_t(i-48000)*(val2 - val1)/msize);
		}
		for (int i=48000 + msize; i<48000 + psize; ++i) {
			auto val = inp.samples[i];
			outp.add_sample((val & 0xffffU) << 8);
		}
	}

	return 0;
}
