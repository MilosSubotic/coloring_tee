/**
 * @file log_ostream.cpp
 * @date May 11, 2012
 *
 * @brief
 * @version 1.0
 * @author Milos Subotic milos.subotic.sm@gmail.com
 *
 * @license LGPLv3
 *
 */

#include <ostream_color_log/log_ostream.h>

namespace ostream_color_log{

		log_ostream& log_ostream::flush(){
			_Iter iter = ostreams.begin(), end = ostreams.end();
			for(; iter != end; iter++)
			    switch(iter->type){
                case OSTREAM:
                    iter->os->flush();
                    break;
			    case OFSTREAM:
                    dynamic_cast<std::ofstream*>(iter->os)->flush();
                    break;
			    case HTML_OFSTREAM:
                    dynamic_cast<html_ofstream*>(iter->os)->flush();
                    break;
			    }
			return *this;
		}

		log_ostream& log_ostream::put(char c){
			_Iter iter = ostreams.begin(), end = ostreams.end();
			for(; iter != end; iter++)
			    switch(iter->type){
                case OSTREAM:
                    iter->os->put(c);
                    break;
			    case OFSTREAM:
                    dynamic_cast<std::ofstream*>(iter->os)->put(c);
                    break;
			    case HTML_OFSTREAM:
                    dynamic_cast<html_ofstream*>(iter->os)->put(c);
                    break;
			    }
			return *this;
		}


		log_ostream& log_ostream::operator<<(const ostream_colors data){
			_Iter iter = ostreams.begin(), end = ostreams.end();
			for(; iter != end; iter++)
				if(iter->enableColoring)
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

		log_ostream& log_ostream::operator<<(const ostream_attributes data){
			_Iter iter = ostreams.begin(), end = ostreams.end();
			for(; iter != end; iter++)
				if(iter->enableColoring)
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

		log_ostream& log_ostream::operator<<(const _OstreamAttributeAndColorFormat data){
			_Iter iter = ostreams.begin(), end = ostreams.end();
			for(; iter != end; iter++)
				if(iter->enableColoring)
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

} // namespace ostream_color_log{
