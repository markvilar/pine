#include <fstream>
#include <iostream>
#include <vector>
#include <string>

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/filter/zlib.hpp>

#include <sl/Camera.hpp>

#include <zedutils/io.hpp>
#include <zedutils/serialization.hpp>

void save_string()
{
	namespace io = boost::iostreams;
	
	const char* ptr = "hello world!";
	auto s = sl::String(ptr);
	
	std::ofstream ofs("string.txt", std::ios::out | std::ios::binary);

	// Use scope to ensure archive goes out of scope before stream.
	{
		// Create output archive.
		boost::archive::text_oarchive oa(ofs);

		// Send string to archive.
		oa << s;
		std::cout << "Save string to archive!\n" << s << "\n";
	}

	ofs.close();
}

void load_string()
{
	namespace io = boost::iostreams;

	sl::String s;

	std::ifstream ifs("string.txt", std::ios::out | std::ios::binary);

	// Use scope to ensure archive goes out of scope before stream.
	{
		// Create input archive.
		boost::archive::text_iarchive ia(ifs);

		// Load string from archive.
		ia >> s;
		std::cout << "Loaded string from archive!\n" << s << "\n";
	}

	ifs.close();
}

void save_matrix()
{
	namespace io = boost::iostreams;

	auto m = sl::Mat(100, 150, sl::MAT_TYPE::F32_C4);
	m.setTo(sl::float4(1.1, 2.1, 3.1, 4.1));
	m.setValue(50, 50, sl::float4(5.1, 5.1, 5.1, 5.1));

	std::ofstream ofs("matrix.txt", std::ios::out | std::ios::binary);

	// Use scope to ensure archive goes out of scope before stream.
	{
		// Create output archive.
		boost::archive::text_oarchive oa(ofs);
		
		// Send matrix to archive.
		oa << m;
		std::cout << "Saved matrix to archive!\n" << m << "\n";

		// Get values from matrix.
		sl::float4 vs;
		m.getValue(50, 50, &vs);
		std::cout << vs << "\n";
		m.getValue(0, 0, &vs);
		std::cout << vs << "\n";
	}
	
	ofs.close();
}

void load_matrix()
{
	namespace io = boost::iostreams;

	sl::Mat m;

	std::ifstream ifs("matrix.txt", std::ios::out | std::ios::binary);

	// Use scope to ensure archive goes out of scope before stream.
	{
		// Create input archive
		boost::archive::text_iarchive ia(ifs);

		// Load matrix from archive.
		ia >> m;
		std::cout << "Loaded matrix from archive!\n" << m << "\n";
		sl::float4 vs;

		// Get values from matrix
		m.getValue(50, 50, &vs);
		std::cout << vs << "\n";
		m.getValue(0, 0, &vs);
		std::cout << vs << "\n";
	}
	
	ifs.close();
}

int main(int argc, char* argv[])
{
	// Test serialization for matrices.
	save_matrix();
	load_matrix();

	// Test serialization for strings.
	save_string();
	load_string();
	return 0;
}