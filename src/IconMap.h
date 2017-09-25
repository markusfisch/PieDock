#ifndef _PieDock_IconMap_
#define _PieDock_IconMap_

#include "Icon.h"

#include <string>
#include <vector>
#include <map>

namespace PieDock {
class IconMap {
public:
	typedef std::vector<std::string> Paths;

	IconMap() :
		missingSurface(0),
		fillerSurface(0) {}
	virtual ~IconMap();
	virtual inline void addPath(const std::string p) {
		paths.push_back(p);
	}
	virtual inline const Paths &getPath() const {
		return paths;
	}
	virtual inline void setFileForMissing(const std::string f) {
		fileForMissing = f;
	}
	virtual inline const std::string &getFileForMissing() const {
		return fileForMissing;
	}
	virtual inline void setFileForFiller(const std::string f) {
		fileForFiller = f;
	}
	virtual inline const std::string &getFileForFiller() const {
		return fileForFiller;
	}
	virtual void reset();
	virtual void addNameAlias(std::string, std::string);
	virtual void addClassAlias(std::string, std::string);
	virtual void addTitleAlias(std::string, std::string);
	virtual Icon *getIcon(std::string, std::string, std::string);
	virtual Icon *getIconByName(std::string);
	virtual Icon *getIconByClass(const std::string);
	virtual Icon *getIconByTitle(const std::string);
	virtual Icon *getMissingIcon(const std::string);
	virtual Icon *getFillerIcon();
	virtual Icon *createIcon(const ArgbSurface *, const std::string,
		Icon::Type);
	virtual void saveIcon(const ArgbSurface *, const std::string) const;

protected:
	typedef std::map<std::string, std::string> AliasToFile;
	typedef std::map<std::string, Icon *> FileToIcon;

	virtual void freeIcons();

private:
	Paths paths;
	AliasToFile nameToFile;
	AliasToFile classToFile;
	AliasToFile titleToFile;
	FileToIcon cache;
	static const char fallbackPng[];
	ArgbSurface *missingSurface;
	ArgbSurface *fillerSurface;
	std::string fileForMissing;
	std::string fileForFiller;
};
}

#endif
