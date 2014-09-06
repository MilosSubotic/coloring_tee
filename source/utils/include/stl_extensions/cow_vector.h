/**
 * @file cow_vector.h
 * @date May 4, 2014
 *
 * @author Milos Subotic <milos.subotic.sm@gmail.com>
 * @license LGPLv3
 *
 * @brief Copy-on-write vector.
 *
 * @version 1.0
 * Changelog:
 * 1.0 - Initial version.
 *
 */

#ifndef COW_VECTOR_H_
#define COW_VECTOR_H_

///////////////////////////////////////////////////////////////////////////////

#include <vector>
#include <memory>

#include "type_traits.h"

///////////////////////////////////////////////////////////////////////////////

namespace stl_extensions {

	/**
	 * @class cow_vector
	 * @brief Copy-on-write vector.
	 */
	template <typename T, typename VectorType = std::vector<T> >
	class cow_vector {
	public:
		typedef typename VectorType::value_type value_type;
		typedef typename VectorType::pointer pointer;
		typedef typename VectorType::const_pointer const_pointer;
		typedef typename VectorType::reference reference;
		typedef typename VectorType::const_reference const_reference;
		typedef typename VectorType::iterator iterator;
		typedef typename VectorType::const_iterator const_iterator;
		typedef typename VectorType::const_reverse_iterator
				const_reverse_iterator;
		typedef typename VectorType::reverse_iterator reverse_iterator;
		typedef typename VectorType::size_type size_type;
		typedef typename VectorType::difference_type difference_type;
		typedef typename VectorType::allocator_type allocator_type;

		///////////////////////////////

	public:
		cow_vector()
			: _implicitly_shared_vector(std::make_shared<VectorType>()) {
		}

		cow_vector(const cow_vector& other)
			: _implicitly_shared_vector(other._implicitly_shared_vector) {
		}

		cow_vector(cow_vector&& other)
			: _implicitly_shared_vector(other._implicitly_shared_vector) {
		}

		/**
		 * Forwarding arguments to VectorType constructor.
		 * @param args parameters pack.
		 */
		template <typename... Args, typename
			= typename enable_if_unrelated<cow_vector, Args...>::type >
		cow_vector(Args&&... args)
			: _implicitly_shared_vector(std::make_shared<VectorType>(
					std::forward<Args>(args)...)) {
		}

		~cow_vector() {}

		///////////////////////////////

	public:
		void assign(size_type n, const value_type& val) {
			copy_on_write();
			_implicitly_shared_vector->assign(n, val);
		}

		template<typename InputIterator>
		void assign(InputIterator first, InputIterator last) {
			copy_on_write();
			_implicitly_shared_vector->assign(first, last);
		}

#if __cplusplus >= 201103L
		void assign(std::initializer_list<value_type> list) {
			copy_on_write();
			_implicitly_shared_vector->assign(list);
		}
#endif

		iterator begin() noexcept {
			copy_on_write();
			return _implicitly_shared_vector->begin();
		}

		const_iterator begin() const noexcept {
			return _implicitly_shared_vector->begin();
		}

		iterator end() noexcept {
			copy_on_write();
			return _implicitly_shared_vector->end();
		}

		const_iterator end() const noexcept {
			return _implicitly_shared_vector->end();
		}

		reverse_iterator rbegin() noexcept {
			copy_on_write();
			return _implicitly_shared_vector->rbegin();
		}

		const_reverse_iterator rbegin() const noexcept {
			return _implicitly_shared_vector->rbegin();
		}

		reverse_iterator rend() noexcept {
			copy_on_write();
			return _implicitly_shared_vector->rend();
		}

		const_reverse_iterator rend() const noexcept {
			return _implicitly_shared_vector->rend();
		}

#if __cplusplus >= 201103L
		const_iterator cbegin() const noexcept {
			return _implicitly_shared_vector->begin();
		}

		const_iterator cend() const noexcept {
			return _implicitly_shared_vector->end();
		}

		const_reverse_iterator crbegin() const noexcept {
			return _implicitly_shared_vector->rbegin();
		}

		const_reverse_iterator crend() const noexcept {
			return _implicitly_shared_vector->rend();
		}
#endif

		size_type size() const noexcept {
			return _implicitly_shared_vector->size();
		}

		size_type max_size() const noexcept {
			return _implicitly_shared_vector->max_size();
		}

#if __cplusplus >= 201103L
		void resize(size_type new_size) {
			copy_on_write();
			_implicitly_shared_vector->resize(new_size);
		}

		void resize(size_type new_size, const value_type& x) {
			copy_on_write();
			_implicitly_shared_vector->resize(new_size, x);
		}
#else
		void resize(size_type new_size, const value_type& x = value_type()) {
			copy_on_write();
			_implicitly_shared_vector->resize(new_size, x);
		}
#endif

#if __cplusplus >= 201103L
		void shrink_to_fit() {
			copy_on_write();
			_implicitly_shared_vector->shrink_to_fit();
		}
#endif

		size_type capacity() const noexcept {
			return _implicitly_shared_vector->capacity();
		}

		bool empty() const noexcept {
			return _implicitly_shared_vector->empty();
		}

		void reserve(size_type n) {
			copy_on_write();
			_implicitly_shared_vector->reserve(n);
		}

		reference operator[](size_type i) {
			copy_on_write();
			return _implicitly_shared_vector->operator[](i);
		}
		const_reference operator[](size_type i) const {
			return _implicitly_shared_vector->operator[](i);
		}

		reference at(size_type i) {
			copy_on_write();
			return _implicitly_shared_vector->at(i);
		}

		const_reference at(size_type i) const {
			return _implicitly_shared_vector->at(i);
		}

		reference front() {
			copy_on_write();
			return _implicitly_shared_vector->front();
		}

		const_reference front() const {
			return _implicitly_shared_vector->front();
		}

		reference back() {
			copy_on_write();
			return _implicitly_shared_vector->back();
		}

		const_reference back() const {
			return _implicitly_shared_vector->back();
		}

		pointer data() noexcept{
			copy_on_write();
			return _implicitly_shared_vector->data();
		}

		pointer data() const noexcept{
			return _implicitly_shared_vector->data();
		}

		void push_back(const value_type& x) {
			copy_on_write();
			_implicitly_shared_vector->push_back(x);
		}

#if __cplusplus >= 201103L
		void push_back(const value_type&& x) {
			copy_on_write();
			_implicitly_shared_vector->push_back(x);
		}

		template<typename... Args>
		void emplace_back(Args&&... args) {
			copy_on_write();
			_implicitly_shared_vector->emplace_back(
					std::forward<Args>(args)...);
		}
#endif
		void pop_back() {
			copy_on_write();
			_implicitly_shared_vector->pop_back();
		}

#if __cplusplus >= 201103L
		template<typename... Args>
		iterator emplace(iterator position, Args&&... args) {
			copy_on_write();
			return _implicitly_shared_vector->emplace(position,
					std::forward<Args>(args)...);
		}
#endif

		iterator insert(iterator position, const value_type& x) {
			copy_on_write();
			return _implicitly_shared_vector->insert(position, x);
		}

#if __cplusplus >= 201103L
		iterator insert(iterator position, value_type&& x) {
			copy_on_write();
			return _implicitly_shared_vector->insert(position, x);
		}

		void insert(iterator position,
				std::initializer_list<value_type> list) {
			copy_on_write();
			_implicitly_shared_vector->insert(position, list);
		}
#endif

		void insert(iterator position, size_type n, const value_type& x) {
			copy_on_write();
			_implicitly_shared_vector->insert(position, n, x);
		}

		template<typename InputIterator>
		void insert(iterator position, InputIterator first,
				InputIterator last) {
			copy_on_write();
			_implicitly_shared_vector->insert(position, first, last);
		}

		iterator erase(iterator position) {
			copy_on_write();
			return _implicitly_shared_vector->erase(position);
		}

		iterator erase(iterator first, iterator last) {
			copy_on_write();
			return _implicitly_shared_vector->erase(first, last);
		}


		void swap(cow_vector& x) {
			typedef std::shared_ptr<VectorType> sp_t;

			if(_implicitly_shared_vector.use_count() > 1) {
				copy_on_write();
			}
			if(x._implicitly_shared_vector.use_count() > 1) {
				x.copy_on_write();
			}

			std::swap(_implicitly_shared_vector, x._implicitly_shared_vector);
		}

		void swap(VectorType& x) {
			copy_on_write();
			_implicitly_shared_vector->swap(x);
		}

		void clear() noexcept {
			copy_on_write();
			_implicitly_shared_vector->clear();
		}

		///////////////////////////////

	private:
		std::shared_ptr<VectorType> _implicitly_shared_vector;

		void copy_on_write() {
			if(_implicitly_shared_vector.use_count() > 1) {
				_implicitly_shared_vector = std::make_shared<VectorType>(
						*_implicitly_shared_vector);
			}
		}

		///////////////////////////////

	};
} // namespace stl_extensions

///////////////////////////////////////////////////////////////////////////////

#endif // COW_VECTOR_H_
