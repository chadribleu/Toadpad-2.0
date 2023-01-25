#include "utils/documenthandler.h"

// TODO: Reallocate memory if its too big

DocumentHandler::DocumentHandler(const std::filesystem::path path) : m_dirty(false)
{
	if (openFile(path.string())) {
		m_path = path;
	}
}

DocumentHandler::DocumentHandler(DocumentHandler const& other) 
{
	m_path = other.m_path;
	if (openFile(m_path.string())) {
		m_state = other.m_state;
		m_eol = other.m_eol;
	}
}

DocumentHandler& DocumentHandler::operator=(DocumentHandler const& other) 
{
	m_path = other.m_path;
	if (openFile(m_path.string())) {
		m_state =other.m_state;
		m_eol = other.m_eol;
	}
	return *this;
}

bool DocumentHandler::openFile(const std::filesystem::path filename) 
{
	std::fstream fs(filename, std::ios::in);
	if (!fs.is_open()) {
		return false;
	}
	m_path = filename;
	m_eol = End_of_Line(fs);
	m_encoding = static_cast<UINT>(getEncodingMode(fs));
	return true;
}

std::wstring DocumentHandler::ReadFile()
{
	std::ifstream ifs(m_path.string(), std::ios::binary | std::ios::in);
	if ((!ifs)) {
		return CANNOT_OPEN_FILE;
	}

	std::string buffer;
	char previousChara;
	char c;

	while (ifs.get(c) && ifs.good()) {
		if (c == L'\n' && previousChara != L'\r') {
			buffer.push_back(L'\r');
		}
		buffer.push_back(c);
		previousChara = c;
	}
	return convertStringTo(buffer, m_encoding);
}

bool DocumentHandler::WriteOnFile(const std::wstring_view content) 
{
	const size_t stringSize = content.length();
	if (stringSize <= 0) {
		return true;
	}
	// Convert wstring to string
	int iSize = WideCharToMultiByte(CP_UTF8, 0, content.data(), stringSize, nullptr, 0, 0, 0);
	std::string conversion_string(iSize, 0);
	WideCharToMultiByte(CP_UTF8, 0, content.data(), content.length(), conversion_string.data(), conversion_string.length(), 0, 0);
	std::ofstream ofs(m_path, std::ios::binary);
	if (!ofs.is_open()) {
		return false;
	}
	size_t strSize = content.length();
	ofs.write(&conversion_string.at(0), iSize);
	return true;
}

bool DocumentHandler::createFile(const std::filesystem::path filename)
{
	std::ofstream ofs(filename);
	if (ofs.is_open()) {
		m_path = filename;
		return true;
	}
	return false;
}

DocumentHandler::EOLMode DocumentHandler::End_of_Line(std::fstream& file) 
{
	file.seekg(std::ios::beg);
	char c;
	while (file.get(c) && file.good()) {
		switch (c) {
		case '\r':
			if (file.get() == L'\n') {
				return EOLMode::CRLF;
			}
			else {
				return EOLMode::CR;
			}
		case L'\n':
			return EOLMode::LF;
		default:
			continue;
		}
	}
	return EOLMode::Undefined; // Use default : CRLF
}

std::wstring DocumentHandler::End_of_Line_String() const
{
	switch (m_eol) {
	case EOLMode::CR:
		return L"CR";
	case EOLMode::LF:
		return L"LF";
	case EOLMode::CRLF:
		return L"CRLF";

	default:
		return L"CRLF";
	}
}

DocumentHandler::EncodingMode DocumentHandler::getEncodingMode(std::fstream& file)
{ 
#define BOM_SIZE 3
#define MULTIBYTE_LIMIT 0x080
	// Test for Byte Order Mask // TODO: use memcmp (maybe faster)
	/*file.seekg(0, std::ios::beg);
	char byte;
	char data[BOM_SIZE];
	for (int i = 0; i != BOM_SIZE; ++i) {
		file.get(byte);
		data[i] = byte;
	}
	if (data[0] == '\xEF' && data[1] == '\xBB' && data[2] == '\xBF')
		return EncodingMode::UTF8_BOM;
	else if (data[0] == '\xFE' && data[1] == '\xFF')
		return EncodingMode::UTF16BE;
	else if (data[0] == '\xFF' && data[1] == '\xFE')
		return EncodingMode::UTF16LE;
	// test for invalid UTF8 character
	file.seekg(0);
	file.clear();*/
	return EncodingMode::UTF8;
}

std::wstring DocumentHandler::convertStringTo(const std::string_view str, UINT codepage)
{
	// Win32
#ifdef _WIN32 
	const int length = static_cast<int>(str.length());
	const int cchWide = MultiByteToWideChar(codepage, 0, str.data(), length, nullptr, 0);
	std::wstring output(cchWide, 0);
	MultiByteToWideChar(codepage, 0, str.data(), length, output.data(), cchWide);
	if (codepage == 1201) {
		int a = 0;
		// Not implemented yet, swap each bytes
	}
	return output;
#endif
}