/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Class for reading from and writing to WAVE files. Supports any alignment  *
 * of data (not necessarily num_channels * byte_depth).                      *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <cassert>
#include <vector>

#include "util.h"

template <typename sample_t>
struct wav_t {
	wav_header_t const header;
	std::vector<sample_t> samples;

	void write_to_file(char const *filename) {
		uint32_t data_size = header.block_align * samples.size();
		std::ofstream outfile(filename, std::ios::binary);

		outfile << "RIFF";
		binary_write<uint32_t>(outfile, 4                         /*remaining data in this chunk*/
		                                    + (8 + sizeof header) /*fmt chunk*/
		                                    + (8 + data_size) /*data chunk*/);
		outfile << "WAVE";

		outfile << "fmt ";
		binary_write<uint32_t>(outfile, sizeof header);
		binary_write(outfile, header);

		outfile << "data";
		binary_write<uint32_t>(outfile, data_size);
		for (sample_t const &sample : samples) {
			binary_write(outfile, sample, header.sample_bytes());
			for (int i = header.sample_bytes(); i < header.block_align; ++i)
				outfile.put(0);
		}

		outfile.close();
	}

	wav_t(const wav_header_t &hdr) : header(hdr) {
		assert(sizeof samples[0] >= header.block_align);
	}
};

template <typename sample_t>
wav_t<sample_t> read_wav(char const fname[]) {
	wav_header_t header;
	std::ifstream reader(fname);

	while (true) {
		auto chunkInfo = readChunkInfo(reader);
		if (chunkInfo.first == "fmt ") {
			reader.read(reinterpret_cast<char *>(&header), chunkInfo.second);
		} else if (chunkInfo.first == "data") {
			wav_t<sample_t> wav(header);
			wav.samples.resize(chunkInfo.second / header.block_align);
			for (auto &smpl : wav.samples) {
				reader.read(reinterpret_cast<char *>(&smpl), header.sample_bytes());
				reader.seekg(header.block_align - header.sample_bytes(), std::ios::cur);
			}

			return wav;
		} else {
			reader.seekg(chunkInfo.second, std::ios::cur);
		}
	};
}
