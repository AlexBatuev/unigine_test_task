#include "filesystem.h"

#include <iostream>
#include <string>
#include <algorithm>

#ifdef _WIN32
#include <direct.h>
#include <windows.h>
#else
#include <unistd.h>
#endif

const char *getCurrentDir()
{
	static char buf[1024];
	memset(buf, 0, sizeof(buf));
#ifdef _WIN32
	static wchar_t wbuf[1024];
	if (_wgetcwd(wbuf, sizeof(wbuf) / sizeof(wbuf[0])) != nullptr)
	{
		wbuf[sizeof(wbuf) / sizeof(wbuf[0]) - 1] = '\0';
		auto dir_from_unicode = [](char *dest, const wchar_t *src, size_t size)
		{
			WideCharToMultiByte(CP_UTF8, 0, src, -1, dest, (int)size, nullptr, nullptr);
			dest[size - 1] = '\0';
			for (char *d = dest; *d != '\0'; d++)
			{
				if (*d == '\\')
					*d = '/';
			}
		};
		dir_from_unicode(buf, wbuf, sizeof(buf));
	}
#else
	if (getcwd(buf, sizeof(buf)) != nullptr)
		buf[sizeof(buf) - 1] = '\0';
#endif
	return buf;
}

std::string unicodeToUtf8(const wchar_t *src)
{
	std::string ret;
	const wchar_t *s = src;
	while (*s)
	{
		unsigned int code = *s++;
		if (code < 0x80)
			ret += (unsigned char)(code & 0x7f);
		else if (code < 0x800)
		{
			ret += (unsigned char)(0xc0 | (code >> 6));
			ret += (unsigned char)(0x80 | (code & 0x3f));
		} else if (code < 0x10000)
		{
			ret += (unsigned char)(0xe0 | (code >> 12));
			ret += (unsigned char)(0x80 | ((code >> 6) & 0x3f));
			ret += (unsigned char)(0x80 | (code & 0x3f));
		}
	}
	return ret;
}

std::string pathname(const char *str)
{
	std::string ret;
	const char *s = str;
	while (*s)
	{
		// Note: let's not merge first two slashes because it could be a network path
		if (*s == '/' && ret.size() > 1 && ret[ret.size() - 1] == '/')
			s++;
		else if (!strncmp(s, "./", 2))
			s += 2;
		else if (!strncmp(s, "../", 3))
		{
			s += 3;
			if (ret.size() > 0 && ret[ret.size() - 1] == '/' && (ret.size() < 3 || strncmp(ret.data() + ret.size() - 3, "../", 3)))
			{
#ifdef _WIN32
				if (ret.size() != 3 || isalpha(ret[0]) == 0 || ret[1] != ':' || ret[2] != '/')
				{
#else
				if (ret.size() != 1 || ret[0] != '/')
				{
#endif
					ret.pop_back();
					size_t pos = ret.rfind('/');
					if (pos == std::string::npos)
						ret.clear();
					else
						ret.erase(pos + 1, ret.size() - pos - 1);
				}
				} else
			{
#ifdef _WIN32
				if (ret.size() < 3 || isalpha(ret[0]) == 0 || ret[1] != ':' || ret[2] != '/')
					ret.append("../");
#else
				if (ret.size() < 1 || ret[0] != '/')
					ret.append("../");
#endif
			}
			} else
		{
			const char *slash = strchr(s, '/');
			if (slash == nullptr)
				break;
			ret.append(s, (int)(slash - s) + 1);
			s = slash + 1;
		}
		}
	return ret;
}

const char *getAppPath()
{
	static std::string app_path;

#ifdef _WIN32
	wchar_t wbuf[1024] = {0};
	if (!GetModuleFileNameW(nullptr, wbuf, sizeof(wbuf) / sizeof(wchar_t)))
	{
		std::cout << "GetModuleFileNameW() failed" << std::endl;
		return "";
	}
	wbuf[sizeof(wbuf) / sizeof(wbuf[0]) - 1] = '\0';

	app_path = unicodeToUtf8(wbuf);
	std::replace(app_path.begin(), app_path.end(), '\\', '/');
	if (app_path.size())
		app_path[0] = (char)toupper(app_path[0]);
#elif defined(_LINUX)
	char buf[1024] = {0};
	if (readlink("/proc/self/exe", buf, sizeof(buf)) == -1)
	{
		std::cout << "readlink() failed" << std::endl;
		return "";
	}
	buf[sizeof(buf) - 1] = '\0';
	app_path = buf;
#endif

	// remove executable filename
	app_path = pathname(app_path.c_str());

	return app_path.c_str();
}