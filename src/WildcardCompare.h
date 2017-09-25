#ifndef _PieDock_WildardCompare_
#define _PieDock_WildardCompare_

namespace PieDock {
class WildcardCompare {
public:
	virtual ~WildcardCompare() {}
	static const bool match(const char *, const char *, bool = false);

private:
	WildcardCompare() {}
};
}

#endif
