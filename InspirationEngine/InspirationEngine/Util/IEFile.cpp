#include <fstream>
#include "Core/MacroDefine.h"
#include "IEFile.h"

IEFile::IEFile()
{
	Reset();
}

IEFile::~IEFile()
{
	Reset();
}

void IEFile::Reset()
{
	m_data.clear();
	m_cursor = 0;
}

bool IEFile::ReadFile(const char* filename)
{
	Reset();
	std::ifstream in(filename, std::ifstream::binary | std::ios::in);

	if (!in)
		return false;

	in.seekg(0, std::ifstream::end);
	std::streamsize size = in.tellg();
	in.seekg(0, std::ifstream::beg);

	m_data.resize(static_cast<size_t>(size));
	in.read(m_data.data(), size);

	return true;
}
