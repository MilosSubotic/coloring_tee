/**
 * @file foo_ostream.h
 * @date Aug 6, 2012
 *
 * @brief Everything written to this ostream is discarded.
 * @version 1.0
 * @author Milos Subotic milos.subotic.sm@gmail.com
 *
 * @license GPLv3
 *
 */

#ifndef FOO_OSTREAM_H_
#define FOO_OSTREAM_H_
/*
#include <ostream_color_log/ostream_coloring.h>
#include <ostream>

namespace ostream_color_log {

	class foo_ostream;

	/////////////////////////////
	// Manipulator interfaces for foo_ostream.

	inline foo_ostream& operator<<(foo_ostream& fos,
			ostream_colors foreground){ return fos; }

	inline foo_ostream& operator<<(foo_ostream& fos,
			ostream_attributes attribute){ return fos; }

	inline foo_ostream& operator<<(foo_ostream& fos,
			_OstreamAttributeAndColorFormat format){ return fos; }

	//////////////////////////////
	// class

	class foo_ostream: public std::ofstream{

		friend foo_ostream& operator<<(foo_ostream& fos,
				ostream_colors foreground);
		friend foo_ostream& operator<<(foo_ostream& fos,
				ostream_attributes attribute);
		friend foo_ostream& operator<<(foo_ostream& fos,
				_OstreamAttributeAndColorFormat format);

	public:

		foo_ostream(const char* name, _OstreamAttributeAndColorFormat format){}

		foo_ostream(const char* name, ostream_colors foreground = white,
				ostream_colors background = black,
				ostream_attributes attribute = nothing){}

		void open(){}

		void close(){}

		~foo_ostream(){}

		template<typename DataType>
		foo_ostream& operator<<(const DataType& data){
			return *this;
		}


	};

} // namespace ostream_color_log{
*/
#endif /* FOO_OSTREAM_H_ */
