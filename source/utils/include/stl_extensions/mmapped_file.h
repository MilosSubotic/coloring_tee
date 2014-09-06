/**
 * @file mmapped_file.h
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

#ifndef MMAPPED_FILE_H_
#define MMAPPED_FILE_H_

///////////////////////////////////////////////////////////////////////////////

#include <stdint.h>
#include <string>
#include <memory>

///////////////////////////////////////////////////////////////////////////////

namespace stl_extensions {

	/**
	 * @class mmapped_file
	 * @brief mmaped file.
	 */
	class mmapped_file {
	public:
		mmapped_file();
		mmapped_file(const char* file_name);
		~mmapped_file(){}

		///////////////////////////////

	public:
		void open(const char* file_name);

		uint8_t* get_mapped_memory() const noexcept {
			return _state->mapped_memory;
		}
		size_t get_file_size() const noexcept {
			return _state->file_size;
		}

		///////////////////////////////

	private:
		class State {
		public:
			State();
			~State();
		public:
			uint8_t* mapped_memory;
			size_t file_size;
			std::string file_name;

		};

		std::shared_ptr<State> _state;
	};

} // namespace stl_extensions

///////////////////////////////////////////////////////////////////////////////

#endif // MMAPPED_FILE_H_
