#include "Environment.h"

#include <sys/types.h>
#include <pwd.h>
#include <stdlib.h>
#include <unistd.h>

using namespace PieDock;

/**
 * Return home directory
 */
const std::string Environment::getHome() {
	static std::string home;

	if (!home.empty()) {
		return home;
	}

	// try getenv()
	{
		const char *h;

		if ((h = getenv("HOME"))) {
			return (home = h);
		}
	}

	// try getpwuid/getuid
	{
		struct passwd *pw = getpwuid(getuid());

		if (pw) {
			return (home = pw->pw_dir);
		}
	}

	// out of options
	return "";
}
