// -------------------------------------------------------------------------------------------
// DocumentHandler.h - contains some code to manage files at a higher level
// -------------------------------------------------------------------------------------------

#ifndef DOCUMENT_HANDLER_H
#define DOCUMENT_HANDLER_H

#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING

#include "common.h"

// Errors
#define MEM_ALLOC_FAILED 0
#define CANNOT_OPEN_FILE 0

#define MAXBUFF 4096
#define MAXPATH 260

class DocumentHandler {
private:
	enum EOLMode {
		LF, // Unix
		CR, // MacOS
		CRLF, // Windows
		Undefined
	};
	enum EncodingMode : UINT {
		ANSI = CP_ACP,
		UTF8 = CP_UTF8,
		UTF8_BOM = CP_UTF8,
		UTF16LE = 1200,
		UTF16BE = 1201,
	};

	enum State {
		Opened,
		PendingChanges,
		Closed,
		Saved,
		NoFile
	};

private:
	EncodingMode getEncodingMode(std::fstream& file);
	EOLMode End_of_Line(std::fstream& file);

private:
	std::filesystem::path m_path;
	State m_state;
	EOLMode m_eol;
	UINT m_encoding;
	bool m_dirty;

public:
	DocumentHandler() = default;
	DocumentHandler(DocumentHandler const& documentHandler);
	DocumentHandler(const std::filesystem::path path);
	DocumentHandler(DocumentHandler&&) noexcept = default;

	DocumentHandler& operator=(DocumentHandler const& other);
	
	bool openFile(const std::filesystem::path filename);
	bool createFile(const std::filesystem::path filename);
	bool WriteOnFile(const std::wstring_view buffer);
	bool Dirty() { return m_dirty; }

	void SetDirty(bool dirty) { m_dirty = dirty; }
	std::wstring ReadFile();
	std::wstring GetFileName() const { return m_path.filename().wstring(); }
	std::wstring GetPath() const { return m_path; };
	std::wstring End_of_Line_String() const;
	std::wstring convertStringTo(const std::string_view str, UINT codepage);
};

#endif