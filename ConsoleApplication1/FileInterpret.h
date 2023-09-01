#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <typeinfo>
#include <type_traits>
#include <cassert>
#include <map>

//macro evil don't use >:(

//*proceeds to use macros anyways because am lazy*
#define AP22_readwrite(file, var) file.readwrite(var, #var)
#define AP22_readwrite_ptr(file, var) file->readwrite(var, #var)
#define AP22_readwrite_s(var) f.readwrite(var, #var)

class File
{
private:
	size_t cursor = 0;
	std::fstream file;
	std::string path;
	bool mread = false;
	bool mwrite = false;
	int mspecial = 0;
	//special mode is for getting variable type and name in a readwrite

	//readwrite combines reading and writing into one to remove redundancy
	//since data will always be in the same order for files for certain types for files
	//ex: int, int, string, ..., etc

	//some type specific read/writes
	char read_char();

	std::string read_string();
	void write_string(std::string s);

	void open_special(int s); //2 for collecting names 1 for size
	void clear_modes();

	void set_cursor();

	std::vector<std::string> collected_names;
	std::map<std::string, std::string> values_of;
	//std::map<std::string, std::function<std::string()>> values_of_f;

public:
	File(std::string path);
	~File();

	template<typename T> void readwrite(T& o); //returns the name of the type
	template<typename T> void readwrite(T& o, std::string varname); //returns the name of the variable / custom name
	template<typename T> void fake_readwrite(T& o, std::string varname); //usefull for being able to get a value but not output it to a file

	std::string readwrite_string(std::string s, std::string varname);
	std::string getfileString();

	bool open_read();
	void open_write();
	void open_size(); //open to get the size of something
	void open_collect(); //open to get names and types of variables

	size_t get_cursor() { return cursor; }
	void move_cursor_to(size_t pos) { cursor = pos; }
	void move_cursor(size_t pos) { cursor += pos; }
	template<typename T> void move_cursor() { move_cursor(sizeof(T)); }

	bool isRead() { return mread; }
	bool isWrite() { return mwrite; }

	template<typename T> T read();
	template<typename T> void write(T o);

	/*SDL_Texture* read_texture(std::string dumpath, SDL_Renderer* ren)
	{
		size_t size = read<size_t>();

		char* data = new char[size];
		file.read(data, size);

		std::fstream f;
		f.open(dumpath, std::ios::out | std::ios::binary);
		f.write(data, size);

		SDL_Surface* sur = IMG_Load(dumpath.c_str());
		move_cursor(size);

		return SDL_CreateTextureFromSurface(ren, sur);
	}*/

	void write_texture(std::string path)
	{
		std::fstream f;
		f.open(path, std::ios::in | std::ios::binary);
		f.seekg(0, std::ios::end);
		size_t t = f.tellg();
		char* data = new char[t];
		f.seekg(0, 0);
		f.read(data, t);

		write<size_t>(t);
		file.write(data, t);
		move_cursor(t);
	}

	size_t size()
	{
		struct stat stat_buf;
		int rc = stat(path.c_str(), &stat_buf);
		return rc == 0 ? stat_buf.st_size : -1;;
	}
	std::vector<std::string> get_collected_names() { return collected_names; }
	std::string get_value_of(std::string n) { return values_of[n]; }
	//std::function<std::string()> get_value_of_f(std::string n) { return values_of_f[n]; }
};

template<typename T>
inline T File::read()
{
	T t;

	set_cursor();
	file.read((char*)&t, sizeof(T));
	move_cursor<T>();

	//Logs::get<File>() << "<File> reading " << path.c_str() << " at " << cursor << std::endl;

	if (file.fail())
	{
		//std::cerr << "Error while reading file: check path (" << path.c_str() << ")" << cursor << SDL_GetError() << std::endl;
		assert(0);
	}

	return std::forward<T>(t);
}

template<typename T>
inline void File::write(T o)
{
	file.write(reinterpret_cast<const char*>(&o), sizeof(T));
	move_cursor<T>();

	if (file.fail())
	{
		std::cerr << "Error while writing file: check path (" << path.c_str() << ")" << cursor << std::endl;
		assert(0);
	}

	//Logs::get<File>() << "<File> writing " << path.c_str() << " at " << cursor << std::endl;
}

template<typename T>
inline void File::readwrite(T& o)
{
	readwrite(o, typeid(T).name());
}

template<typename T>
inline void File::readwrite(T& o, std::string varname)
{
	//Logs::get<File>() << "<File> readwrite " << varname.c_str() << std::endl;
	if (mspecial != 0)
	{
		if (mspecial == 1)
			move_cursor<T>();
		else if (mspecial == 2)
		{
			collected_names.push_back(varname);
			//std::string s = AP22::stringify(o);
			//values_of.emplace(varname, s);
		}
	}
	else if (mread)
	{
		if (std::is_same<T, char>())
			read_char();
		else
			o = read<T>();
	}
	else if (mwrite)
		write(o);
}

