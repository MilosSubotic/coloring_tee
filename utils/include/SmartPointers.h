/**
 * @file SmartPointers.h
 * @date Mar 31, 2013
 *
 * @brief Smart pointers.
 * @version 1.0
 * @author Milos Subotic milos.subotic.sm@gmail.com
 *
 * @license GPLv3
 *
 */

#ifndef SMARTPOINTERS_H_
#define SMARTPOINTERS_H_

///////////////////////////////////////////////////////////////////////////////

#include "CommonMacros.h"

#if defined(__GXX_EXPERIMENTAL_CXX0X__)

#include <memory>
#include <cstddef>

template<typename T>
class wp;

template<typename T>
class sp : public std::shared_ptr<T> {
public:

	// Default.
	constexpr
	sp() noexcept{
	}

	// For null pointer.
	constexpr
	sp(std::nullptr_t){
	}

	// For pointer.
	template<class U>
	explicit sp(U* p)
			: std::shared_ptr<T>(p){
	}

	// With deleter.
	template<class U, class D>
	sp(U* p, D del)
			: std::shared_ptr<T>(p, del){
	}

	template<class D>
	sp(std::nullptr_t p, D del)
			: std::shared_ptr<T>(p, del){

	}

	// With allocator.
	template<class U, class D, class Alloc>
	sp(U* p, D del, Alloc alloc)
			: std::shared_ptr<T>(p, del, alloc){

	}

	template<class D, class Alloc>
	sp(std::nullptr_t p, D del, Alloc alloc)
			: std::shared_ptr<T>(p, del, alloc){

	}

	// Copy.
	sp(const sp& x) noexcept
			: std::shared_ptr<T>(x){
	}

	template<class U>
	sp(const sp<U>& x) noexcept
			: std::shared_ptr<T>(x){
	}

	// Copy from weak.
	template<class U>
	explicit sp(const wp<U>& x)
			: std::shared_ptr<T>(x){
	}

	// Move.
	sp(sp&& x) noexcept
			: std::shared_ptr<T>(x){
	}

	template <class U>
	sp(sp<U>&& x) noexcept
			: std::shared_ptr<T>(x){
	}

	// Move from managed.
	template <class U>
	sp(std::auto_ptr<U>&& x)
			: std::shared_ptr<T>(x){
	}

	template <class U, class D>
	sp(std::unique_ptr<U, D>&& x)
			: std::shared_ptr<T>(x){
	}

	// Aliasing.
	// T is element_type.
	template <class U>
	sp(const sp<U>& x, T* p) noexcept
			: std::shared_ptr<T>(x, p){

	}

	// Copy.
	sp& operator=(const sp& x) noexcept{
		std::shared_ptr<T>::operator=(x);
		return *this;
	}
	
	template <class U> 
	sp& operator=(const sp<U>& x) noexcept{
		std::shared_ptr<T>::operator=(x);
		return *this;
	}

	// Move.
	sp& operator=(sp&& x) noexcept{
		std::shared_ptr<T>::operator=(x);
		return *this;
	}

	template <class U>
	sp& operator=(sp<U>&& x) noexcept{
		std::shared_ptr<T>::operator=(x);
		return *this;
	}

	// Move from.
	template <class U>
	sp& operator=(std::auto_ptr<U>&& x){
		std::shared_ptr<T>::operator=(x);
		return *this;
	}

	template <class U, class D>
	sp& operator=(std::unique_ptr<U, D>&& x){
		std::shared_ptr<T>::operator=(x);
		return *this;
	}

};

template<typename T>
class wp : public std::weak_ptr<T> {
public:

	// Default.
	constexpr
	wp() noexcept{
	}

	// Copy.
	wp(const wp& x) noexcept
			: std::weak_ptr<T>(x){

	}

	template<class U>
	wp(const wp<U>& x) noexcept
			: std::weak_ptr<T>(x){
	}

	// From shared.
	template<class U>
	wp(const sp<U>& x) noexcept
			: std::weak_ptr<T>(x){
	}

	// Copy.
	wp& operator=(const wp& x) noexcept {
		std::weak_ptr<T>::operator=(x);
		return *this;
	}

	template <class U>
	wp& operator=(const wp<U>& x) noexcept{
		std::weak_ptr<T>::operator=(x);
		return *this;
	}

	// Copy from shared.
	template <class U>
	wp& operator=(const sp<U>& x) noexcept{
		std::weak_ptr<T>::operator=(x);
		return *this;
	}
};

#else

#error "Not implemented."

#endif

///////////////////////////////////////////////////////////////////////////////

#endif /* SMARTPOINTERS_H_ */
