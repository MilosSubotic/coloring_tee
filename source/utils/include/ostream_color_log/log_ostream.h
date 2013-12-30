/**
 * @file log_ostream.h
 * @date May 3, 2012
 *
 * @brief Ostream proxy for writing to mutiple files.
 * @version 1.0
 * @author Milos Subotic milos.subotic.sm@gmail.com
 *
 * @license LGPLv3
 *
 */

#ifndef LOG_OSTREAM_H_
#define LOG_OSTREAM_H_

#include <sstream>
#include <vector>
#include <ostream_color_log/ostream_coloring.h>
#include <ostream_color_log/html_ofstream.h>

namespace ostream_color_log{


	class log_ostream {

	protected:
        enum OstreamType{ OSTREAM, OFSTREAM, HTML_OFSTREAM };
        class Ostream{
        public:
            std::ostream* os;
            OstreamType type;
            bool enableColoring;

            Ostream(std::ostream& os_, OstreamType type_,
            		bool enableColoring_ = true)
                :os(&os_), type(type_), enableColoring(enableColoring_){}
        };
		/**
		 * Container of ostream.
		 * Everything that is written to instance of this class
		 * is going to be written to these ostreams.
		 */
		std::vector<Ostream> ostreams;
		typedef std::vector<Ostream>::iterator _Iter;

	public:
		// "Overridden" methods form ostream.
		log_ostream(){}
		log_ostream(const log_ostream& los)
			:ostreams(los.ostreams){}
		log_ostream& operator=(const log_ostream& los){
			if( this != &los ){
				ostreams = los.ostreams;
			}
			return *this;
		}

		void close(){
			// Flush if anything left.
			flush();
			// Clear ostreams so if someone want to write after this point
			// it wont affect on ostreams.
			ostreams.clear();
			/// @todo maybe throw some exception
		}
		~log_ostream(){
			// Don't close or flush or do anything else with ostreams
			// in destructor because instance of this class
			// could be destructed after ostreams (whose pointers
			// this instance holds).
		}

		log_ostream& flush();

		log_ostream& put(char c);

		template<typename DataType>
		log_ostream& operator<<(const DataType& data){
			_Iter iter = ostreams.begin(), end = ostreams.end();
			for(; iter != end; iter++)
			    switch(iter->type){
                case OSTREAM:
                    (*iter->os) << data;
                    break;
			    case OFSTREAM:
                    (*dynamic_cast<std::ofstream*>(iter->os)) << data;
                    break;
			    case HTML_OFSTREAM:
                    (*dynamic_cast<html_ofstream*>(iter->os)) << data;
                    break;
			    }
			return *this;
		}

		log_ostream& operator<<(const ostream_colors data);

		log_ostream& operator<<(const ostream_attributes data);

		log_ostream& operator<<(const _OstreamAttributeAndColorFormat data);

		/**
		 * Interface for manipulators.
		 *  Manipulators such as @c std::endl and @c std::hex use these
		 *  functions in constructs like "std::cout << std::endl".
		 * @param pf manipulator.
		 * @return this
		 */
		log_ostream& operator<<(log_ostream& (*pf)(log_ostream&)){
			return pf(*this);
		}




		// Methods specific for log_ostream.
		/**
		 * Add ostream for logging.
		 * Everything that is written to instance of this class
		 * is going to be written to added ostreams.
		 * @param os ostream to add.
		 * @return this
		 */
		log_ostream& add_ostream(std::ostream& os,
				bool enableColoring = true){
				ostreams.push_back(Ostream(os, OSTREAM, enableColoring));
			return *this;
		}

        log_ostream& add_ostream(std::ofstream& os,
				bool enableColoring = true){
			ostreams.push_back(Ostream(os, OFSTREAM, enableColoring));
			return *this;
		}

        log_ostream& add_ostream(html_ofstream& os,
				bool enableColoring = true){
			ostreams.push_back(Ostream(os, HTML_OFSTREAM, enableColoring));
			return *this;
		}


	};

	// "Overridden" functions from ostream.
	// Ostream manipulators.
	inline log_ostream& flush(log_ostream& os){
		return os.flush();
	}

	inline log_ostream& endl(log_ostream& os){
		os.put('\n');
		return os.flush();
	}

	inline log_ostream& ends(log_ostream& os){
		return os.put('\0');
	}

	// "Overridden" functions from ostream_coloring.h.
	// TODO implement this

} // namespace ostream_color_log{



#endif /* LOG_OSTREAM_H_ */
