/**
 * @file read_only_intrusive_vector.h
 * @date Apr 21, 2014
 *
 * @author Milos Subotic <milos.subotic.sm@gmail.com>
 * @license LGPLv3
 *
 * @brief Read only intrusive vector.
 *
 * @version 1.0
 * Changelog:
 * 1.0 - Initial version.
 *
 */

#ifndef READ_ONLY_INTRUSIVE_VECTOR_H_
#define READ_ONLY_INTRUSIVE_VECTOR_H_

///////////////////////////////////////////////////////////////////////////////

#include <cstdlib>
#include <iterator>
#include <stdexcept>

///////////////////////////////////////////////////////////////////////////////

namespace stl_extensions {

	/**
	 * @class read_only_intrusive_vector
	 * @brief Read only intrusive vector.
	 */
	template <typename T>
	class read_only_intrusive_vector {
	public:
		typedef T value_type;
		typedef const value_type& const_reference;
		typedef const value_type* const_pointer;
		// TODO Maybe implement iterator.
		/*
		class random_input_iterator : public std::iterator<std::random_access_iterator_tag, T> {
			const T* _current;
		public:
			random_input_iterator() : _current(0) {}
			random_input_iterator(const T* p) : _current(t) {}

		};
		typedef random_input_iterator const_iterator;
		*/
		typedef const T* const_iterator;
		typedef ptrdiff_t difference_type;
		typedef size_t size_type;

		///////////////////////////////

	public:
		read_only_intrusive_vector()
				: _intrusive_memory(0), _size(0) {
		}
		/**
		 * Constructor.
		 * @param intrusive_memory Pointer to memory holding T elements.
		 * @param elements Elements of T in intrusive_memory;
		 */
		read_only_intrusive_vector(
				const void* intrusive_memory,
				size_type elements)
				: _intrusive_memory(
						reinterpret_cast<const_pointer>(intrusive_memory)),
						_size(elements) {
		}
		~read_only_intrusive_vector(){}

		///////////////////////////////

	public:
		const_iterator begin() const noexcept {
			return _intrusive_memory;
		}
		const_iterator cbegin() const noexcept {
			return _intrusive_memory;
		}
		const_iterator end() const noexcept {
			return _intrusive_memory + _size;
		}
		const_iterator cend() const noexcept {
			return _intrusive_memory + _size;
		}

		size_type size() const noexcept {
			return _size;
		}
		bool empty() const noexcept {
			return _size == 0;
		}

		const_reference operator[](size_type i) const noexcept {
			return _intrusive_memory[i];
		}
		const_reference at(size_type i) const {
			if(i >= _size){
				throw std::out_of_range(
						"read_only_intrusive_vector: index out of range");
			}else{
				return _intrusive_memory[i];
			}
		}
		const_reference front() const noexcept {
			return _intrusive_memory[0];
		}
		const_reference back() const noexcept {
			return _intrusive_memory[_size-1];
		}
		const_pointer data() const noexcept {
			return _intrusive_memory;
		}

		///////////////////////////////

	protected:
		const_pointer _intrusive_memory;
		size_type _size;

		///////////////////////////////

	};

} // namespace stl_extensions

///////////////////////////////////////////////////////////////////////////////

#endif // READ_ONLY_INTRUSIVE_VECTOR_H_
