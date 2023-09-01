#include "FileInterpret.h"


char File::read_char()
{
	char t;
	file.seekg(cursor);
	file.read(&t, 1);
	move_cursor(1);
	return t;
}

std::string File::readwrite_string(std::string s, std::string varname)
{
	if (mspecial == 2)
	{
		collected_names.push_back(varname);
		values_of.emplace(varname, s);
	}

	if (mread)
		return read_string();
	else if (mwrite)
		write_string(s);
	return s;
}

std::string File::getfileString()
{
	std::string s;
	size_t sz = size();
	for (size_t i = 0; i < sz; i++)
		s.push_back(read_char());
	return s;
}

std::string File::read_string()
{
	size_t sz = read<size_t>();
	std::string a;
	for (size_t i = 0; i < sz; i++)
		a.push_back(read_char());
	return a;
}

void File::write_string(std::string s)
{
	size_t sz = s.length();
	write(sz);

	for (size_t i = 0; i < sz; i++)
	{
		char c = s.at(i);
		write(c);
	}
}

void File::open_special(int s)
{
	//Logs::get<File>() << "<File> special opened " << s << std::endl;
	clear_modes();
	mspecial = s;
}

void File::clear_modes()
{
	if (file.is_open())
		file.close();
	cursor = 0;
	mread = false;
	mwrite = false;
	mspecial = 0;
}

void File::set_cursor() { file.seekg(cursor, std::ios::beg); }
File::File(std::string path) : path(path) { //Logs::get<File>() << "new <File>" << std::endl; 
}

File::~File()
{
	if (file.is_open())
		file.close();
	//Logs::get<File>() << "end <File>" << std::endl;
}
bool File::open_read()
{
	//Logs::get<File>() << "<File> open read" << std::endl;
	clear_modes();
	file.open(path, std::ios::in | std::ios::binary);
	mread = true;

	return (!file.fail());
}

void File::open_write()
{
	//Logs::get<File>() << "<File> open write" << std::endl;
	clear_modes();
	file.open(path, std::ios::out | std::ios::binary);
	mwrite = true;
}

void File::open_size() { open_special(1); }

void File::open_collect()
{
	collected_names.clear();
	open_special(2);
}
