/**
 * @file log.h
 * @date Sep 19, 2012
 *
 * @brief
 * @version 1.0
 * @author Milos Subotic milos.subotic.sm@gmail.com
 *
 * @license LGPLv3
 *
 */

#ifndef LOG_H_
#define LOG_H_

///////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <iomanip>

///////////////////////////////////////////////////////////////////////////////

extern std::ostream& verboseLog;
extern std::ostream& debugLog;
extern std::ostream& infoLog;
extern std::ostream& warningLog;
extern std::ostream& errorLog;

#ifdef __ANDROID__
void log_setLogTag(const std::string& logTag);
#else
void log_setColoringEnabled(bool coloringEnabled);
void log_setBold(bool bold);
#endif

using std::endl;

template<typename CharType, typename TraitsType>
inline std::basic_ostream<CharType, TraitsType>&
tab(std::basic_ostream<CharType, TraitsType>& __os){
	return std::flush(__os.put(__os.widen('\t')));
}

#define PRINT(x) #x << " = " << x
#define POSITION __PRETTY_FUNCTION__ << " @ " << __LINE__ << ": "
#define DEBUG(x) do{ debugLog << PRINT(x) << endl; }while(0)
#define TRACE() do{ debugLog << POSITION << endl; }while(0)
#define PRINT_ADDRESS(x) std::hex << x \
		<< std::resetiosflags(std::ios::hex)

///////////////////////////////////////////////////////////////////////////////

#endif /* LOG_H_ */
