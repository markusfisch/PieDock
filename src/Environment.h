#ifndef _PieDock_Environment_
#define _PieDock_Environment_

#include <string>

namespace PieDock {
class Environment {
public:
	virtual ~Environment() {}
	static const std::string getHome();

private:
	Environment() {}
};
}

#endif
