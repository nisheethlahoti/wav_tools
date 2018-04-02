#include <iostream>
#include "wav_file.h"

int main(int, char **argv) {
	wav_t<int16_t> inp = read_wav<int16_t>(argv[1]);
	wav_header_t header(1, inp.header.sample_rate, 2);
	wav_t<int16_t> outp(header);

	int16_t base_level = 0x200u;
	int64_t wrongness = 0, stop_level = base_level * header.sample_rate / 8;
	for (auto const &val : inp.samples) {
		wrongness = std::min(stop_level, std::max(0l, wrongness + base_level - std::abs(val)));
		if (wrongness < stop_level)
			outp.samples.push_back(val);
	}
	std::clog << inp.samples.size() << ' ' << outp.samples.size() << std::endl;

	outp.write_to_file(argv[2]);
	return 0;
}
