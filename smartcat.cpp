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

	int32_t as_int() const {
		int32_t ret = 0;
		copy(val.begin(), val.end(), reinterpret_cast<uint8_t*>(&ret));
		return ret;
	}

	sample_t merge(sample_t const& other, double ratio) const {
		return sample_t(static_cast<int32_t>(as_int()*(1-ratio) + other.as_int()*ratio));
	}	
};

constexpr int psize = 24000;

int main(int argc, char **argv) {
	wav_t<int32_t> inp = read_wav<int32_t>(argv[1]);
	wav_header_t header(1,44100,3);
	wav_writer_t<sample_t> outp(argv[2], header);

	int first = atoi(argv[3]), second = atoi(argv[4]);
	for (int i=first-psize; i<first; ++i) {
		auto val = inp.samples[i];
		outp.add_sample((val & 0xffffU) << 8);
	}

	int merge_size = atoi(argv[5]);
	for (int i=0; i<merge_size; ++i) {
		int32_t val1 = int32_t(int16_t(inp.samples[first+i] & 0xffff))<<8, val2 = int32_t(int16_t(inp.samples[i] & 0xffffU))<<8;
		outp.add_sample((i*val2 + (merge_size-i)*val1)/merge_size);
	}

	for (int i=second+merge_size; i<second+psize; ++i) {
		auto val = inp.samples[i];
		outp.add_sample((val & 0xffffU) << 8);
	}

	return 0;
}
