#pragma once

#ifndef __ExbJson

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstring>
#include <iostream>
#include <sstream>
#include <utility>
#include <vector>
#include <string>
#include <iostream>
#include <memory>
#include <unordered_map>
#include <algorithm>
#include <stack>
#include <queue>
#include <string.h>
#include <stdlib.h>
#include <list>
#include <stdio.h>
#include <set>
#include <math.h>
#include <complex>
#include <map>
#include <any>

namespace ExbJson
{
#define Debug__Exb

	using namespace std;
	using doubledata = double;
	using intdata = int_fast64_t;
	using std::any;
	using ValueObject = std::any;
	using SearchList = vector<string>;

	class JsonElement;

	JsonElement* __InitJsonElement(JsonElement* target, const string& source);


	class Any_list_Obj
	{
	protected:
		map<string, any> _vector_obj;
		unsigned long long index = 0;
	public:
		Any_list_Obj() = default;
		Any_list_Obj(string source)
		{
			Deserialization(source);
		}
		Any_list_Obj(map<string, any> from) noexcept
		{
			_vector_obj = from;
		};
		Any_list_Obj(const Any_list_Obj& from)noexcept
		{
			_vector_obj = from._vector_obj;
		};

		decltype(_vector_obj.begin()) begin()
		{
			return _vector_obj.begin();
		}
		decltype(_vector_obj.end()) end()
		{
			return _vector_obj.end();
		}

		template<typename T> void Add(T v)
		{
			if (_vector_obj.empty())_vector_obj[to_string(index++)] = v;
			else if (typeid(T) == (*_vector_obj.begin()).second.type())_vector_obj[to_string(index++)] = v;
			else return;
		}
		template<typename T> void Add(string key, T v)
		{
			if (_vector_obj.empty())_vector_obj[key] = v;
			else if (typeid(T) == (*_vector_obj.begin()).second.type())_vector_obj[key] = v;
			else return;
		}

		template<typename T> void Remove(string key)
		{
			if (_vector_obj.empty())return;
			else if (typeid(T) == (*_vector_obj.begin()).second.type())_vector_obj.erase(key);
			else return;
		}

		inline const type_info& _type()
		{
			return (*_vector_obj.begin()).second.type();
		}


		void Deserialization(string source)
		{
			string current;
			int layer = -1;
			if (source.find(',') == string::npos)//single value
			{
				_Value_Deserialization(current);
			}
			else for (auto i = source.begin(), b = source.begin(), e = source.end(); i != e; i++)
			{
				switch (*i)
				{
				case '{':
				{
					if (current.empty() && layer == 0)
					{
						layer++;
						current += (*i++);
						while (layer > 0 && i != e)
						{
							if (*i == '{')layer++;
							if (*i == '}')layer--;
							current += (*i++);
						}
						JsonElement* newobj = nullptr;
						Add<JsonElement*>(__InitJsonElement(newobj, current));
						current.clear();
					}
					else throw "ErrorJson";
				}
				break;
				case ',':
					if (current.empty()) throw "ErrorJson";
					else
					{
						_Value_Deserialization(current);
						current.clear();
					}
					break;
				case '\n':
				case '\t':
				case ' ':
				case '[':
					if (layer > 0)
					{
						while (layer > 0 && i != e)
						{
							if (*i == '[')layer++;
							if (*i == ']')layer--;
							current += (*i++);
						}
						Any_list_Obj* newobj = new Any_list_Obj(current);
						Add<Any_list_Obj*>(newobj);
					}
					else layer++;
					break;
				case ']':
					_Value_Deserialization(current);
					return;
				default:
					current += (*i);
					break;
				}

			}
		}
	private:
		void _Value_Deserialization(std::string& current)
		{
			if (current.empty())return;
			size_t b = current.find_first_of('\"'), e = current.find_last_of('\"');
			if (b != e) Add<string>(string(current.substr(b + 1, e - b - 1)));
			else if (current == "false" || current == "False")Add<bool>(false);
			else if (current == "true" || current == "True")Add<bool>(true);
			else if (current == "null" || current == "Null")Add<JsonElement*>(nullptr);
			else if (current.find('.') == string::npos)Add<intdata>((intdata)stoi(current));
			else Add<doubledata>((doubledata)stod(current));
		}
	};

	class JsonElement
	{
	protected:

	public:
		JsonElement() = default;
		JsonElement(string n) :name(n) {}
		JsonElement(const map<string, ValueObject>& from_carrier)
		{
			_carrier = from_carrier;
		}
		JsonElement(const JsonElement&& from)noexcept
		{
			_carrier = from._carrier;
		}
		JsonElement(const JsonElement& from)noexcept
		{
			_carrier = from._carrier;
		}
		JsonElement& operator=(const JsonElement& from) noexcept
		{
			_carrier = from._carrier;
			name = from.name;
			return *this;
		}
		JsonElement& operator=(const JsonElement&& from) noexcept
		{
			_carrier = from._carrier;
			name = from.name;
			return *this;
		}
		bool operator==(const JsonElement& from) noexcept
		{
			return Equal(from);
		}
		bool operator==(const JsonElement&& from) noexcept
		{
			return Equal(from);
		}

		void Init()
		{
			_carrier.clear();
		}
		JsonElement GenerateSame()
		{
			JsonElement cat;
			for (auto i = _carrier.begin(), e = _carrier.end(); i != e; i++)
			{
				cat.Add(i->first, i->second);
			}
			return cat;
		}

		bool Equal(const JsonElement& from)
		{
			if (_carrier.size() != from._carrier.size())return false;
			for (auto i = _carrier.begin(), e = _carrier.end(); i != e; i++)
			{
				if (from._carrier.count(i->first) == 0)return false;
			}
			return true;
		}
		bool Equal(const JsonElement&& from)
		{
			if (_carrier.size() != from._carrier.size())return false;
			for (auto i = _carrier.begin(), e = _carrier.end(); i != e; i++)
			{
				if (from._carrier.count(i->first) == 0)return false;
			}
			return true;
		}
		bool Empty()
		{
			return _carrier.empty();
		}
		void Test(string name)
		{
			if (_carrier.count(name) == 0)
			{
				cout << "NULL_ValueObject_" + name;
				throw "NULL_ValueObject_" + name;
			}
		}
		intdata Size()
		{
			return sizeof(*this);
		}
		intdata Count()
		{
			return _carrier.size();
		}
		ValueObject& operator[](string key)
		{
			return _carrier[key];
		}
		template<typename R> pair<R, bool> Get(string key)
		{
			if (_carrier.count(key) > 0 && _carrier[key].type() == typeid(R))
				return { any_cast<R>(_carrier[key]),true };
			else
				return { R(), false };
		}
		template<class T> T _(string key)
		{
			if (Get<T>(key).second)
				return Get<T>(key).first;
			else throw "Key-Value Error";
		}
		template<class T> void _(string key, T& input)
		{
			if (_carrier.count(key) > 0 && _carrier[key].type() == typeid(T))
				any_cast<T&>(_carrier[key]) = input;
			else throw "Key-Value Error";
		}
		template<class T> void _(SearchList keys, const T& input)
		{
			JsonElement* _current = this;
			for (auto i = keys.begin(), b = keys.begin(), e = keys.end(); i != e; )
			{
				auto& key = *i++;
				if (i != e)
				{
					if (!(_carrier.count(key) > 0 && _carrier[key].type() == typeid(JsonElement*))) throw "Key-Value Error";
					_current = any_cast<JsonElement*>((*_current)[key]);
				}
				else
					any_cast<T&>((*_current)[key]) = input;
			}
		}
		template<class T> void _(string* keys, size_t size, T input)
		{
			SearchList cat(keys, keys + size);
			_(cat, input);
		}
		pair<ValueObject&, bool> Getref(string key)
		{
			if (_carrier.count(key) > 0)
				return { _carrier[key],true };
			else
				return { (*_carrier.begin()).second, false };
		}

		void Add(string name, intdata v)
		{
			_carrier[name] = v;
		}
		void Add(string name, doubledata v)
		{
			_carrier[name] = v;
		}
		void Add(string name, string v)
		{
			_carrier[name] = v;
		}
		void Add(string name, bool v)
		{
			_carrier[name] = v;
		}
		void Add(string name, JsonElement* v)
		{
			_carrier[name] = v;
		}
		void Add(string name, char const* __ptr64 v)
		{
			_carrier[name] = string(v);
		}
		void Add(string name, Any_list_Obj* v)
		{
			_carrier[name] = v;
		}
		void Add(string name, ValueObject v)
		{
			_carrier[name] = v;
		}

		void remove(string name)
		{
			_carrier.erase(name);
		}

		operator string()
		{
			if (Empty())return "null";

			string out("{\n");
			unsigned int countline = linet;

			for (auto it = _carrier.begin(), b = _carrier.begin(), e = _carrier.end(); it != e; it++)
			{

				if (it != b)out += ",\n";
				auto& cat = *it;
#ifdef Debug__Exb
				cout << cat.first + "->" << cat.second.type().name() << "\n";
#endif // Debug__Exb
				for (unsigned int i = 0; i < countline; i++)out += "\t";
				out += "\"" + cat.first + "\" : ";

				auto& type = cat.second.type();
				{
					_OutPutStr__(type, out, cat, countline);
				}
			}

			out += "\n";
			for (unsigned int i = 0; i < countline; i++)out += "\t";
			out += "}";

			return out;
		}
		string ToString()
		{
			return *this;
		}
		string ToString_OneLine()
		{
			if (Empty())return "null";

			string out("{");
			unsigned int countline = linet;

			for (auto it = _carrier.begin(), b = _carrier.begin(), e = _carrier.end(); it != e; it++)
			{

				if (it != b)out += ",";//\n";
				auto& cat = *it;
#ifdef Debug__Exb
				cout << cat.first + "->" << cat.second.type().name() << "\n";
#endif // Debug__Exb
				for (unsigned int i = 0; i < countline; i++)out += "\t";
				out += "\"" + cat.first + "\" : ";

				auto& type = cat.second.type();
				{
					_OutPutStr__(type, out, cat, countline, true);
				}
			}

			//out += "\n";
			for (unsigned int i = 0; i < countline; i++)out += "\t";
			out += "}";

			return out;
		}
		string Serialization()
		{
			return *this;
		}
		void Deserialization(string source)
		{
			unsigned int layer = -1;
			bool state_key = false, state_value = false, state_sub = false;
			if (layer == -1)
			{
				auto begin = source.find_first_of('{'), end = source.find_last_of('}');
				if (begin == string::npos)throw "NoJson";
				if (end == string::npos)throw "NoJson";

				source.erase(0, begin);
				source.erase(source.find_last_of('}') + 1, source.size() - 1);
			}

			string current;
			string key, value;
			for (auto it = source.begin(), it2 = source.begin(), b = source.begin(), e = source.end(); it != e; )
			{
				//Determine the current state that should be processed and decide the subsequent processing method
				switch ((*it))
				{
				case ':':
					state_key = false;
					state_value = true;
					__fallthrough;
				case ' ':
				case '\n':
				case '\t':
					it++;
					break;
				case '[':
				{
					_List_Deserialization(layer, state_value, current, it, it2, e, key, state_key);
				}
				state_key = true;
				state_value = false;
				break;
				case '{':
				{
					_Recursive_Deserialization(layer, state_value, current, it, it2, e, key, state_key);
				}
				break;
				case '}':
					layer--;
					__fallthrough;
				case ',':
				{
					_Values_Deserialization(key, state_value, current, state_key, it);
				}
				break;
				case '\"':
				{
					_Strings_Deserialization(it2, it, state_key, key, e, state_value, value);
				}
				break;
				default:
					current += (*it++);
					break;
				}
				if (layer == -1)return;
			}
		}

		ValueObject& operator[](const char* key)
		{
			if (_carrier.count(key) == 0)throw exception("no key");
			return _carrier[key];
		}
		ValueObject& operator[](const string& key)
		{
			if (_carrier.count(key) == 0)throw exception("no key");
			return _carrier[key];
		}
		ValueObject& operator[](const string&& key)
		{
			if (_carrier.count(key) == 0)throw exception("no key");
			return _carrier[key];
		}

#define NULLNAME "NULLNAME"
		string name = NULLNAME;
	private:
		map<string, ValueObject> _carrier;
		unsigned int linet = 0;
		void _OutPutStr__(const type_info& type, std::string& out, std::pair<const std::string, std::any> cat, unsigned int countline, bool isoneline = false)
		{

			if (type == typeid(bool))
			{
				out += (any_cast<bool>(cat.second)) ? "true" : "false";
			}
			else if (type == typeid(intdata))
			{
				out += to_string(any_cast<intdata>(cat.second));
			}
			else if (type == typeid(int))
			{
				out += to_string(any_cast<int>(cat.second));
			}
			else if (type == typeid(unsigned))
			{
				out += to_string(any_cast<unsigned>(cat.second));
			}
			else if (type == typeid(unsigned long))
			{
				out += to_string(any_cast<unsigned long>(cat.second));
			}
			else if (type == typeid(unsigned long long))
			{
				out += to_string(any_cast<unsigned long long>(cat.second));
			}
			else if (type == typeid(doubledata) || type == typeid(float))
			{
				out += to_string(any_cast<doubledata>(cat.second));
			}
			else if (type == typeid(JsonElement*))
			{
				JsonElement* pointer = any_cast<JsonElement*>(cat.second);
				if (pointer == nullptr)
				{
					out += "null";
				}
				else
				{
					(*pointer).linet = countline + 1;
					out += (isoneline) ? pointer->ToString_OneLine() : pointer->ToString();
				}
			}
			else if (type == typeid(string))
			{
				out += "\"" + any_cast<string>(cat.second) + "\"";
			}
			else if (type == typeid(char const* __ptr64))
			{
				out += "\"" + string(any_cast<char const* __ptr64>(cat.second)) + "\"";
			}
			else if (type == typeid(Any_list_Obj*))
			{
				out += "[";
				auto icat = any_cast<Any_list_Obj*>(cat.second);
				for (auto i = icat->begin(), b = icat->begin(), e = icat->end(); i != e; i++)
				{
					if (!isoneline && i != b)out += "\n,";
					_OutPutStr__(icat->_type(), out, *i, countline, isoneline);
				}
				out += "]";
			}
			else out += type.name();
		}
		void _OutPutStr__(const type_info& type, std::string& out, std::any& cat, unsigned int countline, bool isoneline = false)
		{
			if (type == typeid(bool))
			{
				out += (any_cast<bool>(cat)) ? "true" : "false";
			}
			else if (type == typeid(intdata))
			{
				out += to_string(any_cast<intdata>(cat));
			}
			else if (type == typeid(int))
			{
				out += to_string(any_cast<int>(cat));
			}
			else if (type == typeid(unsigned))
			{
				out += to_string(any_cast<unsigned>(cat));
			}
			else if (type == typeid(unsigned long))
			{
				out += to_string(any_cast<unsigned long>(cat));
			}
			else if (type == typeid(unsigned long long))
			{
				out += to_string(any_cast<unsigned long long>(cat));
			}
			else if (type == typeid(doubledata) || type == typeid(float))
			{
				out += to_string(any_cast<doubledata>(cat));
			}
			else if (type == typeid(JsonElement*))
			{
				JsonElement* pointer = any_cast<JsonElement*>(cat);
				if (pointer == nullptr)
				{
					out += "null";
				}
				else
				{
					(*pointer).linet = countline + 1;
					out += (isoneline) ? pointer->ToString_OneLine() : pointer->ToString();
				}
			}
			else if (type == typeid(string))
			{
				out += "\"" + any_cast<string>(cat) + "\"";
			}
			else if (type == typeid(char const* __ptr64))
			{
				out += "\"" + string(any_cast<char const* __ptr64>(cat)) + "\"";
			}
			else if (type == typeid(Any_list_Obj*))
			{
				out += "[";
				auto icat = any_cast<Any_list_Obj*>(cat);
				for (auto i = icat->begin(), b = icat->begin(), e = icat->end(); i != e; i++)
				{
					if (!isoneline && i != b)out += ",";
					_OutPutStr__(icat->_type(), out, *i, countline);
				}
				out += "]";
			}
			else out += type.name();
		}
		void _Strings_Deserialization(std::string::iterator& it2, std::string::iterator& it, bool& state_key, std::string& key,
			std::string::iterator& e, bool& state_value, std::string& value)
		{

			it2 = ++it;
			if (state_key)
			{
				key.clear();
				while ((*it2) != '\"')
				{
					if (it2 == e)throw "ErrorJson";
					key += (*it2++);
				}
				if (key.empty())throw "ErrorJson";
				it = it2;
				while ((*it++) == ':') if (it == e)throw "ErrorJson";
				state_key = false;
				state_value = true;
			}
			else if (state_value)
			{
				value.clear();
				while ((*it2) != '\"')
				{
					if (it2 == e)throw "ErrorJson";
					value += (*it2++);
				}
				it = it2;
				while ((*it++) == ',') if (it == e)throw "ErrorJson";
				_carrier[key] = value;
				state_key = true;
				state_value = false;
			}
		}
		void _Recursive_Deserialization(unsigned int& layer, bool& state_value, std::string& current, std::string::iterator& it,
			std::string::iterator& it2, std::string::iterator& e, std::string& key, bool& state_key)
		{

			layer++;
			if (layer > 0)
			{
				if (!state_value)throw "ErrorJson";
				current.clear();
				current += (*it);
				it2 = ++it;
				while (layer > 0)
				{
					if (it2 == e)throw "ErrorJson";
					switch ((*it2))
					{
					case '\n':
					case '\t':
					case ' ':
						break;
					case '{':
						layer++;
						current += (*it2);
						break;
					case '}':
						layer--;
						current += (*it2);
						break;
					default:
						current += (*it2);
						break;
					}
					it2++;
				}
				JsonElement* newobj = new JsonElement(key);
				newobj->Deserialization(current);
				_carrier[key] = newobj;
				it = it2;
				while ((*it++) != ',')if (it == e)break;
			}
			else it++;
			state_key = true;
			state_value = false;
			current.clear();
		}
		void _Values_Deserialization(std::string& key, bool& state_value, std::string& current, bool& state_key, std::string::iterator& it)
		{

			if (key.size() == 0)throw "ErrorJson";
			if (state_value)
			{
				if (current == "false" || current == "False")_carrier[key] = false;
				else if (current == "true" || current == "True")_carrier[key] = true;
				else if (current == "null" || current == "Null")_carrier[key] = new JsonElement(key);
				else if (current.find('.') == string::npos)_carrier[key] = (intdata)stoi(current);
				else _carrier[key] = (doubledata)stod(current);
			}
			state_key = true;
			state_value = false;
			current.clear();
			it++;
		}
		void _List_Deserialization(unsigned int& layer, bool& state_value, std::string& current, std::string::iterator& it,
			std::string::iterator& it2, std::string::iterator& e, std::string& key, bool& state_key)
		{

			layer++;
			if (!state_value)throw "ErrorJson";
			current.clear();
			current += (*it);
			it2 = ++it;
			while (layer > 0 && it2 != e)
			{
				switch ((*it2))
				{
				case '\n':
				case '\t':
				case ' ':
					break;
				case '[':
					layer++;
					current += (*it2);
					break;
				case ']':
					layer--;
					current += (*it2);
					break;
				default:
					current += (*it2);
					break;
				}
				it2++;
			}
			Any_list_Obj* newobj = new Any_list_Obj(current);
			_carrier[key] = newobj;
			it = it2;
			current.clear();
		}


	};


	template<class T> bool TryGetValue(JsonElement& from, T& target, const string& key)
	{
		pair<T, bool> cat_ref = from.Get<T>(key.c_str());
		if (!cat_ref.second)return false;
		target = any_cast<T>(cat_ref.first);
		return cat_ref.second;
	}
	template<class T> bool TryGetValue(JsonElement& from, T& target, const string&& key)
	{
		pair<T, bool> cat_ref = from.Get<T>(key.c_str());
		if (!cat_ref.second)return false;
		target = any_cast<T>(cat_ref.first);
		return cat_ref.second;
	}
	template<class T> bool TryGetValue(JsonElement& from, T& target, const char* key)
	{
		pair<T, bool> cat_ref = from.Get<T>(key);
		if (!cat_ref.second)return false;
		target = any_cast<T>(cat_ref.first);
		return cat_ref.second;
	}

	bool TryGetNumber(JsonElement& from, doubledata& target, const char* key)
	{
		bool a = TryGetValue(from, target, key);
		if (!a)
		{
			intdata b = 0;
			a = TryGetValue(from, b, key);
			if (a)
			{
				target = (doubledata)b;
				return true;
			}
			else return false;
		}
		else return true;
	}
	bool TryGetNumber(JsonElement& from, doubledata& target, const string& key)
	{
		bool a = TryGetValue(from, target, key);
		if (!a)
		{
			intdata b = 0;
			a = TryGetValue(from, b, key);
			if (a)
			{
				target = (doubledata)b;
				return true;
			}
			else return false;
		}
		else return true;
	}
	bool TryGetNumber(JsonElement& from, doubledata& target, const string&& key)
	{
		bool a = TryGetValue(from, target, key);
		if (!a)
		{
			intdata b = 0;
			a = TryGetValue(from, b, key);
			if (a)
			{
				target = (doubledata)b;
				return true;
			}
			else return false;
		}
		else return true;
	}
	bool TryGetNumber(JsonElement& from, intdata& target, const string&& key)
	{
		bool a = TryGetValue(from, target, key);
		if (!a)
		{
			doubledata b = 0;
			a = TryGetValue(from, b, key);
			if (a)
			{
				target = (intdata)b;
				return true;
			}
			else return false;
		}
		else return true;
	}
	bool TryGetNumber(JsonElement& from, intdata& target, const string& key)
	{
		bool a = TryGetValue(from, target, key);
		if (!a)
		{
			doubledata b = 0;
			a = TryGetValue(from, b, key);
			if (a)
			{
				target = (intdata)b;
				return true;
			}
			else return false;
		}
		else return true;
	}
	bool TryGetNumber(JsonElement& from, intdata& target, const char* key)
	{
		bool a = TryGetValue(from, target, key);
		if (!a)
		{
			doubledata b = 0;
			a = TryGetValue(from, b, key);
			if (a)
			{
				target = (intdata)b;
				return true;
			}
			else return false;
		}
		else return true;
	}
	template<typename Ty> bool TryGetNumber(JsonElement& from, Ty& target, const string& key)
	{
		doubledata b = 0;
		bool a = TryGetNumber(from, b, key);
		if (a)target = (Ty)b;
		else return false;
		return true;
	}
	template<typename Ty> bool TryGetNumber(JsonElement& from, Ty& target, const string&& key)
	{
		doubledata b = 0;
		bool a = TryGetNumber(from, b, key);
		if (a)target = (Ty)b;
		else return false;
		return true;
	}
	template<typename Ty> bool TryGetNumber(JsonElement& from, Ty& target, const char* key)
	{
		doubledata b = 0;
		bool a = TryGetNumber(from, b, key);
		if (a)target = (Ty)b;
		else return false;
		return true;
	}

	JsonElement* __InitJsonElement(JsonElement* target, const string& source)
	{
		JsonElement* newobj = new JsonElement();
		newobj->Deserialization(source);
		return newobj;
	}
}


#define __ExbJson

#endif // !__ExbJson
