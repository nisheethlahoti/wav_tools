#include "wav_file.h"

using namespace std;

int main(int, char **argv) {
	wav_t<uint32_t> inp = read_wav<uint32_t>(argv[1]);
	wav_header_t header(1, 44100, 3);
	header.block_align = 4;
	wav_t<uint32_t> outp(header);

	for (auto const &val : inp.samples) {
		outp.samples.push_back((val & 0xffffU) << 8);
	}

	outp.write_to_file(argv[2]);
	return 0;
}
