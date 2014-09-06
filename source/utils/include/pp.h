/**
 * @file pp.h
 * @date Jan 20, 2014
 *
 * @author Milos Subotic <milos.subotic.sm@gmail.com>
 * @license LGPLv3
 *
 * @brief Pool pointer.
 *
 * @version 2.0
 * Changelog:
 * 1.0 - Initial version.
 * 2.0 - Garbage collector.
 *
 */

#ifndef PP_H_
#define PP_H_

///////////////////////////////////////////////////////////////////////////////

#include <cstddef>
#include <type_traits>

///////////////////////////////////////////////////////////////////////////////

/**
 * Internal function for registering new pool per thread so it could be later
 * deleted.
 * @param pointerToPool
 */
void _pp_registerNewPool(void* pointerToPool);

/**
 * Garbage collector. Collect pool of dead threads.
 */
void pp_gc();

///////////////////////////////////////////////////////////////////////////////

/**
 * @class pp
 * @brief Pool pointer.
 * Have pool of preallocated objects from which new objects
 * are took when pool pointer is initialized and to which are recycled
 * objects that are not used any more by any pool pointer. Have reference
 * count to track how much pool pointers use same object.
 * @warning It is not safe to use same pool pointer across multiple threads.
 */
template<typename T>
class pp {
public:
	friend void pp_gc();
public:
	pp() {
		_Node*& pool = _pool();
		if(!pool){
			_node = new _Node(pool);
			_p = new T();
		}else{
			_node = pool;
			_p = static_cast<T*>(_node->pool.p);
			pool = _node->pool.next;
			_node->active.count = 1;
			_node->active.pool = &pool;
		}
	}

	~pp() {
		if(--_node->active.count == 0){
			_Node*& pool = _pool();
			_node->pool.p = const_cast<void*>(
					reinterpret_cast<const void*>(_p));
			_node->pool.next = pool;
			pool = _node;
		}
	}

	///////////////////////////////////

	pp(const pp& other) {
		_p = other._p;
		_node = other._node;
		_node->active.count++;
	}

	pp& operator=(const pp& other) {
		if(this != &other){
			_p = other._p;
			_node = other._node;
			_node->active.count++;
		}
		return *this;
	}

	template<typename U, typename = typename
			std::enable_if<std::is_convertible<U*, T*>::value>::type>
	pp(const pp<U>& other) {
		// FIXME Better way to do this.
		_p = reinterpret_cast<const pp<T>&>(other)._p;
		_node = reinterpret_cast<const pp<T>&>(other)._node;
		_node->active.count++;
	}

	template<typename U, typename = typename
			std::enable_if<std::is_convertible<U*, T*>::value>::type>
	pp& operator=(const pp<U>& other) {
		if(this != &other){
			// FIXME Better way to do this.
			_p = reinterpret_cast<const pp<T>&>(other)._p;
			_node = reinterpret_cast<const pp<T>&>(other)._node;
			_node->active.count++;
		}
		return *this;
	}

	///////////////////////////////////

	pp(pp&& other) {
		_p = other._p;
		_node = other._node;
		_node->active.count++;
	}

	pp& operator=(pp&& other) {
		if(this != &other){
			_p = other._p;
			_node = other._node;
			_node->active.count++;
		}
		return *this;
	}

	template<typename U, typename = typename
			std::enable_if<std::is_convertible<U*, T*>::value>::type>
	pp(pp<U>&& other) {
		// FIXME Better way to do this.
		_p = reinterpret_cast<pp<T>&&>(other)._p;
		_node = reinterpret_cast<pp<T>&&>(other)._node;
		_node->active.count++;
	}

	template<typename U, typename = typename
			std::enable_if<std::is_convertible<U*, T*>::value>::type>
	pp& operator=(pp<U>&& other) {
		if(this != &other){
			// FIXME Better way to do this.
			_p = reinterpret_cast<pp<T>&&>(other)._p;
			_node = reinterpret_cast<pp<T>&&>(other)._node;
			_node->active.count++;
		}
		return *this;
	}

	///////////////////////////////////

	T& operator*() const noexcept {
		return *_p;
	}

	T* const operator->() const noexcept {
		return _p;
	}

	T* const get() const {
		return _p;
	}

	///////////////////////////////////

	/**
	 * Resize pool.
	 * @param newSizeOfPool This do not counts object that are allocated
	 * from pool.
	 */
	static void resizePool(size_t newSizeOfPool) {
		_Node*& pool = _pool();
		_Node* node = pool;
		while(newSizeOfPool--){
			node = new _Node(new T(), node);
		}
		pool = node;
	}

	static void cleanUpPool() {
		_Node*& pool = _pool();
		_Node* node = pool;
		while(node){
			_Node* t = node;
			node = t->pool.next;
			delete t->pool.p;
			delete t;
		}
		pool = nullptr;
	}

	///////////////////////////////////

private:
	class _Node {
	public:
		/**
		 * Making node for pool.
		 * @param p
		 * @param next
		 */
		_Node(void* p, _Node* next) {
			pool.p = p;
			pool.next = next;
		}

		/**
		 * Making node for active use.
		 * @param pool
		 */
		_Node(_Node*& pool) {
			active.count = 1;
			active.pool = &pool;
		}

		///////////////////////////////

	public:
		union{
			/// For pool.
			struct {
				/// Pointer to object.
				void* p;
				/// Pointer to next Node.
				/// If nullptr then this is last node in pool.
				_Node* next;
			} pool;
			/// For active use.
			struct {
				/// Reference counter.
				size_t count;
				/// Pointer to pool.
				_Node** pool;
			} active;
		};

		///////////////////////////////

	};

	///////////////////////////////////

private:
	/**
	 * Object pointer.
	 */
	T* _p;
	/**
	 * Node. Have reference counter which is same for all pool pointer
	 * instances that have same object pointer.
	 */
	_Node* _node;

	///////////////////////////////////

private:
	static _Node*& _pool() {
		// FIXME When thread is destroyed this will be memory leak.
		// Try with pthread_cleanup_push.
		static bool initialized = false;
		#pragma omp threadprivate(initialized)
		static _Node* pool = nullptr;
		#pragma omp threadprivate(pool)
		if(!initialized){
			_pp_registerNewPool(&pool);
			initialized = true;
		}
		return pool;
	}

	///////////////////////////////////

};

///////////////////////////////////////////////////////////////////////////////

#endif // PP_H_
