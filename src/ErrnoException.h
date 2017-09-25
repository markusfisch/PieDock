#ifndef _PieDock_ErrnoException_
#define _PieDock_ErrnoException_

#include <errno.h>
#include <string.h>

#include <stdexcept>

namespace PieDock {
class ErrnoException : public std::runtime_error {
public:
	ErrnoException() : runtime_error(strerror(errno)) {}
};
}

#endif
