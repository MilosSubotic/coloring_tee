/**
 * @file range.h
 * @date Jan 14, 2013
 *
 * @brief
 * @version 1.0
 * @author Milos Subotic milos.subotic.sm@gmail.com
 *
 * @license LGPLv3
 *
 */

#ifndef RANGE_H_
#define RANGE_H_

///////////////////////////////////////////////////////////////////////////////

#include <utility>
#include <iterator>
#include <limits>
#include <cassert>

///////////////////////////////////////////////////////////////////////////////

namespace GLUtils {

	template<typename Type>
	class range{
	public:

		typedef const Type& reference;
		typedef const Type* pointer;

		range(Type start, Type end = std::numeric_limits<Type>::max(),
				Type step = 1)
				: _value(start), _end(end), _step(step){
		}

		bool done() const{
			if(_value + _step > _value){
				return _value >= _end;
			}else{
				return _value <= _end;
			}
		}

		reference operator*() const{
			return _value;
		}
		pointer operator->() const{
			return &_value;
		}

		/**
		 * Prefix increment operator
		 * @return this
		 */
		range& operator++(){
			assert(!done() && "Cannot increment if done.");
			_value += _step;
			return *this;
		}

		/**
		 * Suffix increment operator
		 * @return this before increment
		 */
		range operator++(int){
			const range tmp(*this);
			++*this;
			return tmp;
		}

	private:

		Type _value;
		Type _end;
		Type _step;

	private:
		typedef void (range::*BoolLike)();
		void non_comparable(){}
	public:
		// Safe Bool idiom
		operator BoolLike() const{
			return done() ? nullptr : &range::non_comparable;
		}

	};

} // namespace GLUtils

///////////////////////////////////////////////////////////////////////////////

#endif /* RANGE_H_ */
