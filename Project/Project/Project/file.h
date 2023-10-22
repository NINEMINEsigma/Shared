#pragma once
#pragma once

#ifndef __FileC

#include"mastgl.h"
#include"json.h"
#include <comutil.h>  
#pragma comment(lib, "comsuppw.lib")

std::string wstring2string(const std::wstring& ws)
{
	_bstr_t t = ws.c_str();
	char* pchar = (char*)t;
	std::string result = pchar;
	return result;
}

std::wstring string2wstring(const std::string& s)
{
	_bstr_t t = s.c_str();
	wchar_t* pwchar = (wchar_t*)t;
	std::wstring result = pwchar;
	return result;
}

namespace io
{
	using std::string;
	using std::wstring;
	using std::array;
	using ExbJson::JsonElement;
	using std::map;

	using OriClassList = map<wstring, std::any>;

	class FileC
	{
	public:
		static bool TryMatchIOStreamFile(const wstring& filepath);
		static bool TryMatchIOStreamFile();
		static wstring GetIOStream(const wstring& lpAppName, const wstring& lpKeyName);
		static void WriteIOStream(const wstring& lpAppName, const wstring& lpKeyName, const wstring& lpValue);

		static JsonElement* GetOriginalClass(const string& source);
		static JsonElement* GetOriginalClass(const wstring& source);

		static const DWORD GenerateOriginalClass(const wstring& lpAppName, const wstring& lpClassName, map<wstring, std::any> OriginalSource, const wstring& path);
		static JsonElement* GetOriginalClass(const wstring& lpAppName, const wstring& lpClassName);
		static const DWORD GenerateOriginalClass(const wstring& lpAppName, const wstring& lpClassName, JsonElement& OriginalSource, const wstring& path);
		static const DWORD GenerateOriginalClass(const wstring& lpAppName, const wstring& lpClassName, JsonElement&& OriginalSource, const wstring& path);

		static wstring ReadFrom(const string& path);
		static wstring ReadFrom(const wstring& path);
		static wstring WriteFrom(const string& path, const string& source);
		static wstring WriteFrom(const wstring& path, const string& source);
		static wstring WriteFrom(const string& path, const wstring& source);
		static wstring WriteFrom(const wstring& path, const wstring& source);

		static wstring IOStreamFilePath;
	private:
		FileC() {}
	};

	bool FileC::TryMatchIOStreamFile()
	{
		return TryMatchIOStreamFile(IOStreamFilePath);
	}

	bool FileC::TryMatchIOStreamFile(const wstring& filepath)
	{
		try
		{
			FILE* fp = nullptr;											//文件指针
			auto err = _wfopen_s(&fp, filepath.c_str(), L"r");			//只读
			if (err == 0) IOStreamFilePath = filepath;
			else
			{
				err = _wfopen_s(&fp, filepath.c_str(), L"w");			// 创建文件
				if (err == 0) IOStreamFilePath = filepath;
				else
				{
					fclose(fp);
					return false;
				}
			}
			fclose(fp);
			return true;
		}
		catch (...)
		{
			return false;
		}
	}

	wstring FileC::GetIOStream(const wstring& lpAppName, const wstring& lpKeyName)
	{
		array<wchar_t, 256> wptr{ L'\0' };
		const DWORD d = GetPrivateProfileString(lpAppName.c_str(), lpKeyName.c_str(), L".",
			&wptr.at(0), 256, IOStreamFilePath.c_str());

		return &wptr.at(0);
	}

	void FileC::WriteIOStream(const wstring& lpAppName, const wstring& lpKeyName, const wstring& lpValue)
	{
		bool a = WritePrivateProfileString(lpAppName.c_str(), lpKeyName.c_str(), lpValue.c_str(), IOStreamFilePath.c_str());
	}

	JsonElement* FileC::GetOriginalClass(const string& source)
	{
		JsonElement* cat = new JsonElement();
		try
		{
			cat->Deserialization(source);
		}
		catch (...)
		{
			delete cat;
			cat = nullptr;
		}
		return cat;
	}
	JsonElement* FileC::GetOriginalClass(const wstring& source)
	{
		return GetOriginalClass(wstring2string(source));
	}

	const DWORD FileC::GenerateOriginalClass(const wstring& lpAppName, const wstring& lpClassName, map<wstring, std::any> OriginalSource, const wstring& path)
	{
		WriteIOStream(lpAppName, lpClassName, path);
		JsonElement cat;
		for (auto i = OriginalSource.begin(), e = OriginalSource.end(); i != e; i++)
		{
			cat.Add(wstring2string(i->first), i->second);
		}
		const DWORD d = WritePrivateProfileString(lpClassName.c_str(), L"OriginalClass", string2wstring(cat.ToString_OneLine()).c_str(), path.c_str());
		return d;
	}
	JsonElement* FileC::GetOriginalClass(const wstring& lpAppName, const wstring& lpClassName)
	{
		wstring path = GetIOStream(lpAppName, lpClassName);
		static array<wchar_t, 10384>* wptr = nullptr;
		delete wptr;
		wptr = new array<wchar_t, 10384>{ L'\0' };
		const DWORD d = GetPrivateProfileString(lpClassName.c_str(), L"OriginalClass", L".",
			&wptr->at(0), 10384, path.c_str());
		return GetOriginalClass((wstring)&wptr->at(0));
	}

	const DWORD FileC::GenerateOriginalClass(const wstring& lpAppName, const wstring& lpClassName, JsonElement& OriginalSource, const wstring& path)
	{
		WriteIOStream(lpAppName, lpClassName, path);
		const DWORD d = WritePrivateProfileString(lpClassName.c_str(), L"OriginalClass", string2wstring(OriginalSource.ToString_OneLine()).c_str(), path.c_str());
		return d;
	}
	const DWORD FileC::GenerateOriginalClass(const wstring& lpAppName, const wstring& lpClassName, JsonElement&& OriginalSource, const wstring& path)
	{
		WriteIOStream(lpAppName, lpClassName, path);
		const DWORD d = WritePrivateProfileString(lpClassName.c_str(), L"OriginalClass", string2wstring(OriginalSource.ToString_OneLine()).c_str(), path.c_str());
		return d;
	}

	wstring FileC::ReadFrom(const string& path)
	{
		return ReadFrom(string2wstring(path));
	}
	wstring FileC::ReadFrom(const wstring& path)
	{
		FILE* fp = nullptr;											//文件指针
		auto err = _wfopen_s(&fp, path.c_str(), L"r");			//只读
		try
		{
			wstring s;
			if (err == 0)
			{
				//fputws(musicPathName.at(0).c_str(), fp);
				array<wchar_t, 256> wMusic{ L'\0' };
				while (!feof(fp))
				{
					fgetws(&wMusic.at(0), 256, fp);
					s += &wMusic.at(0);
				}
			}
			else throw 0;
			fclose(fp);
			return s;
		}
		catch (...)
		{
			fclose(fp);
			return L"";
		}

	}
	wstring FileC::WriteFrom(const string& path, const string& source)
	{
		return WriteFrom(string2wstring(path), string2wstring(source));
	}
	wstring FileC::WriteFrom(const wstring& path, const string& source)
	{
		return WriteFrom(path, string2wstring(source));
	}
	wstring FileC::WriteFrom(const string& path, const wstring& source)
	{
		return WriteFrom(string2wstring(path), source);
	}
	wstring FileC::WriteFrom(const wstring& path, const wstring& source)
	{
		FILE* fp = nullptr;											//文件指针
		auto err = _wfopen_s(&fp, path.c_str(), L"w");
		try
		{
			if (err == 0)
			{
				fputws(source.c_str(), fp);
			}
			else throw 0;
			fclose(fp);
			return source;
		}
		catch (...)
		{
			fclose(fp);
			return L"";
		}
	}

	wstring FileC::IOStreamFilePath = L".\\DefaultIOFile.ini";//L"F:\\openGL_ExbObject\\Object_0\\ExbGraphicCore_0\\ExbGraphicCore_0\\DefaultIOFile.ini";
}

#define __Filec

#endif // !__FileC
