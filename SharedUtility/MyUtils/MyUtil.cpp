#include "stdafx.h"
#include "MyUtil.h"

#include "DebugNew.h"


namespace MyUtil
{
	template<typename T> bool LoadBinaryFromFileImpl(LPCWSTR pFileName, std::vector<T>& buffer)
	{
		static_assert((sizeof(T) == 1), "Unsupported size!!");

		// �O�̂��߃N���A���Ă����B
		buffer.clear();

		// ARM64 �͂Ƃ肠�����ΏۊO�B
//#ifdef WIN64
#ifdef _M_X64
		struct _stat64 fileStats = {};
		const auto getFileStatFunc = _wstat64;
#else
		struct _stat64i32 fileStats = {};
		const auto getFileStatFunc = _wstat64i32;
#endif
		// _stat64i32 �� (2GB-1B) �܂ł̃t�@�C����������Ɉ����Ȃ��B2GB �ȏ�̃t�@�C���ɑ΂��Ă� _stat64 ���g���B

		// �t�@�C�� �T�C�Y���擾�ł��Ȃ��A�������̓T�C�Y�����������ꍇ�� false ��Ԃ��B
		if (getFileStatFunc(pFileName, &fileStats) != 0 || fileStats.st_size < 0)
		{
			ATLTRACE(L"Cannot get the file stats for the file!! <%s>\n", pFileName);
			return false;
		}

		buffer.resize(fileStats.st_size);

		if (fileStats.st_size == 0)
		{
			return true;
		}

#if 0
		// std::fstream �̏ꍇ�A32bit �łł� 2GB �܂ł̃t�@�C�����������Ȃ��̂Œ��ӁB
		// �f�o�b�O �r���h�ł́A�� MB �̃t�@�C����ǂݍ��ނƂ���20�b�ȏ�̎��Ԃ�������B
		std::basic_ifstream<T> ifs(pFileName, std::ios::in | std::ios::binary);

		if (ifs.fail())
		{
			ATLTRACE(L"Cannot open the file!! <%s>\n", pFileName);
			return false;
		}

		ifs.read(&buffer[0], fileStats.st_size);
#else
		FILE* pFile = nullptr;
		const auto retCode = _wfopen_s(&pFile, pFileName, L"rb");
		if (retCode != 0 || pFile == nullptr)
		{
			ATLTRACE(L"Cannot open the file!! <%s>\n", pFileName);
			return false;
		}
		fread_s(&buffer[0], buffer.size(), 1, fileStats.st_size, pFile);
		fclose(pFile);
		pFile = nullptr;
#endif

		return true;
	}

	bool LoadBinaryFromFile(LPCWSTR pFileName, std::vector<char>& buffer)
	{
		return LoadBinaryFromFileImpl(pFileName, buffer);
	}

	bool LoadBinaryFromFile(LPCWSTR pFileName, std::vector<int8_t>& buffer)
	{
		return LoadBinaryFromFileImpl(pFileName, buffer);
	}

	bool LoadBinaryFromFile(LPCWSTR pFileName, std::vector<uint8_t>& buffer)
	{
		return LoadBinaryFromFileImpl(pFileName, buffer);
	}

	// �R�s�[�R���X�g���N�^���� C++11 ���[�u �R���X�g���N�^���D�悳��邱�Ƃ��l�����A�����Ĉ����ł͂Ȃ��߂�l�ŕԂ��B
	std::string ConvertUtf16toUtf8(const wchar_t* srcText)
	{
		_ASSERTE(srcText != nullptr);
		const int textLen = static_cast<int>(wcslen(srcText));
		if (textLen <= 0)
		{
			return std::string();
		}
		const int reqSize = ::WideCharToMultiByte(CP_UTF8, 0, srcText, textLen, nullptr, 0, nullptr, nullptr);
		if (reqSize > 0)
		{
			std::vector<char> buff(reqSize + 1); // �Ō�� + 1 �͕K�{�炵���B�I�[ null ���܂܂Ȃ��T�C�Y���Ԃ�炵���B
			//std::vector<char> buff(reqSize);
			::WideCharToMultiByte(CP_UTF8, 0, srcText, textLen, &buff[0], reqSize, nullptr, nullptr);
			return std::string(&buff[0]);
		}
		else
		{
			// ���S�ɃG���[�Ȃ̂Ŗ{���͗�O�𓊂���ׂ��H
			return std::string();
		}
	}

	std::wstring ConvertUtf8toUtf16(const char* srcText)
	{
		_ASSERTE(srcText != nullptr);
		const int textLen = static_cast<int>(strlen(srcText));
		if (textLen <= 0)
		{
			return std::wstring();
		}
		const int reqSize = ::MultiByteToWideChar(CP_UTF8, 0, srcText, textLen, nullptr, 0);
		if (reqSize > 0)
		{
			std::vector<wchar_t> buff(reqSize + 1); // �Ō�� + 1 �͕K�{�炵���B�I�[ null ���܂܂Ȃ��T�C�Y���Ԃ�炵���B
			//std::vector<wchar_t> buff(reqSize);
			::MultiByteToWideChar(CP_UTF8, 0, srcText, textLen, &buff[0], reqSize);
			return std::wstring(&buff[0]);
		}
		else
		{
			// ���S�ɃG���[�Ȃ̂Ŗ{���͗�O�𓊂���ׂ��H
			return std::wstring();
		}
	}

	bool GetUtf8FullPath(const wchar_t* inRelFilePath, std::string& outFullPath)
	{
		const int RequiredStringBufferSize = 1024;
		wchar_t inFullPath[RequiredStringBufferSize] = {};
		char outFullPathBuf[RequiredStringBufferSize] = {};

		if (_wfullpath(inFullPath, inRelFilePath, RequiredStringBufferSize) == nullptr)
		{
			return false;
		}

		if (::WideCharToMultiByte(CP_UTF8, 0, inFullPath, -1, outFullPathBuf, RequiredStringBufferSize, nullptr, nullptr) == 0)
		{
			return false;
		}

		outFullPath = outFullPathBuf;
		return true;
	}

} // end of namespace
