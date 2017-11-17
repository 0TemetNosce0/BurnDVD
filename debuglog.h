/*
 * debuglog.h
 *
 *  Created on: Dec 17, 2012
 *      Author: dxy
 */

#ifndef DEBUGLOG_H_
#define DEBUGLOG_H_

#ifdef	__cplusplus
extern "C" {
#endif

void debuglog(int line, const wchar_t* format, ...);

#ifdef	__cplusplus
}
#endif

#define DEBUGLOG(format, ...) debuglog(__LINE__, format, ## __VA_ARGS__)

#endif /* DEBUGLOG_H_ */
