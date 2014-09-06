/**
 * @file mmapped_file.cpp
 * @date Apr 21, 2014
 *
 * @author Milos Subotic <milos.subotic.sm@gmail.com>
 * @license LGPLv3
 *
 * @brief mmaped file.
 *
 * @version 1.0
 * Changelog:
 * 1.0 - Initial version.
 *
 */

///////////////////////////////////////////////////////////////////////////////

#include "stl_extensions/mmapped_file.h"

#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>

#include "Exceptions.h"

///////////////////////////////////////////////////////////////////////////////

namespace stl_extensions {

	class mmapped_file_error : public Exception {
	public:
		explicit mmapped_file_error()
				: Exception("mmapped_file_error") {
		}
	};

#define MMF_TAG "mmapped_file: "
#define MMF_POS "Error while mmapping file \"" << _state->file_name \
	<< "\":" << endl
#define MMF_ERR MMF_TAG << MMF_POS

#define THROW_MMF_ERROR() \
	throw (mmapped_file_error() << MMF_POS)

	mmapped_file::mmapped_file(){
		_state = std::make_shared<State>();
	}

	mmapped_file::mmapped_file(const char* file_name){
		open(file_name);
	}

	void mmapped_file::open(const char* file_name) {
		using namespace std;

		_state = std::make_shared<State>();
		_state->file_name = file_name;

		// Open file.
		int fd = ::open(file_name, O_RDONLY);
		if(fd < 0){
			THROW_MMF_ERROR() << "Cannot open file!" << endl;
		}

		// Get file size.
		struct stat sb;
		if(fstat(fd, &sb) == -1){
			THROW_MMF_ERROR() << "Cannot get file size!" << endl;
		}
		_state->file_size = sb.st_size;

		// mmap file.
		_state->mapped_memory = reinterpret_cast<uint8_t*>(mmap(
				NULL,
				_state->file_size,
				PROT_READ,
				MAP_PRIVATE,
				fd,
				0));
		if(_state->mapped_memory == MAP_FAILED){
			THROW_MMF_ERROR() << "Failed to map file to memory! errno = "
					<< errno << endl;
		}
	}

	mmapped_file::State::State() {
		file_size = 0;
		mapped_memory = nullptr;
	}

	mmapped_file::State::~State() {
		if(mapped_memory){
			if(munmap(mapped_memory, file_size)){
				throw mmapped_file_error()
						<< "Error while mmapping file \"" << file_name
						<< "\":" << endl
						<< "Failed to unmap file to memory! errno = "
						<< errno << endl;
			}
		}
	}
} // namespace stl_extensions

///////////////////////////////////////////////////////////////////////////////
