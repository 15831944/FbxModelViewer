#include "stdafx.h"
#include "MyUtil.h"

#include "DebugNew.h"


namespace MyUtils
{
	template<typename T> void LoadBinaryFromFileImpl(LPCWSTR pFilePath, std::vector<T>& outBuffer)
	{
		//static_assert((sizeof(T) == 1), "Unsupported size!!");

		// ���炩���߃N���A���Ă����B
		outBuffer.clear();

		// �t�@�C�� �T�C�Y�̎擾�ɂ� Windows API �� GetFileSizeEx() �� FindFirstFile() ���g�����@�����邪�A�� OS �ւ̈ڐA���ɗ��̂Ŏg��Ȃ��B
		// fseek(), ftell() �͘_�O�B
		// https://www.jpcert.or.jp/sc-rules/c-fio19-c.html
		// POSIX �x�[�X�� stat ���g�����Ƃɂ���B
		// _stat32(), _wstat32(), _stat32i64(), _wstat32i64() �̓^�C���X�^���v�� 2038-01-18 23:59:59 �܂ł��������Ȃ��̂Ř_�O�B
		// _stat64i32(), _wstat64i32() �� 0x7FFFFFFF [bytes] (2GB - 1B) �܂ł̃t�@�C����������Ɉ����Ȃ��B
		// 2GB �ȏ�̃t�@�C��������Ɉ�����悤�ɂ���ɂ� _stat64(), _wstat64() ���g���K�v������B

#if defined(_M_X64) || defined(_M_ARM64)
		// 64bit �r���h�̔���� _WIN64 �ł��悳�����B
		// https://msdn.microsoft.com/en-us/library/b0084kay.aspx
		// ���Ȃ݂� x86 �� ARM �ł� _WIN64 �͒�`����Ȃ����Ax64 �� ARM64 �ł� _WIN64 �� _WIN32 �̗�������`�����̂Œ��ӁB
#elif defined(_M_IX86) || defined(_M_ARM)
#else
#error Not supported platform!!
#endif
		// �\���̂Ɠ����̊֐������݂���̂ŁAC++ �ł� struct �ŏC������K�v������B
		struct _stat64 fileStats = {};
		const auto getFileStatFunc = _wstat64;

		// �t�@�C�� �T�C�Y���擾�ł��Ȃ��A�������̓T�C�Y�����������ꍇ�̓G���[�B
		if (getFileStatFunc(pFilePath, &fileStats) != 0 || fileStats.st_size < 0)
		{
			throw std::exception("Cannot get the file stats for the file!!");
		}

		if (fileStats.st_size % sizeof(T) != 0)
		{
			throw std::exception("The file size is not a multiple of the expected size of element!!");
		}

		const auto fileSizeInBytes = static_cast<uint64_t>(fileStats.st_size);

		// MFC �� GDI+ ���g���ꍇ�� NOMINMAX ���`�ł����A�ז��Ƃ������׈��� min/max �}�N���𖳌����ł��Ȃ��B
		// Windows SDK �� <intsafe.h> �Œ�`����Ă��� SIZE_T_MAX �Ȃǂ��ւƂ��Ďg�����@�����邪�A
		// �R�[�h���x���ł̈ڐA�������߂邽�߂ɁA���Z���g���ă}�N���W�J���������B
		// http://d.hatena.ne.jp/yohhoy/20120115/p1

		// size_t �ŕ\���ł��Ȃ��ꍇ�̓G���[�B
		if (sizeof(size_t) < 8 && (std::numeric_limits<size_t>::max)() < fileSizeInBytes)
		{
			throw std::exception("The file size is over the capacity on this platform!!");
		}

		if (fileStats.st_size == 0)
		{
			return;
		}

		const auto numElementsInFile = static_cast<size_t>(fileStats.st_size / sizeof(T));

		outBuffer.resize(numElementsInFile);

#if 0
		// std::fstream �̏ꍇ�AMSVC �̃f�o�b�O �r���h�ł́A�� MB �̃t�@�C����ǂݍ��ނƂ���20�b�ȏ�̎��Ԃ�������B
		// �]���� CRT �t�@�C�����o�͊֐��ɂ́A�f�o�b�O �r���h�ł����Ă������܂ł̒v���I�ȃI�[�o�[�w�b�h�͂Ȃ��B
		std::basic_ifstream<T> ifs(pFilePath, std::ios::in | std::ios::binary);

		if (ifs.fail())
		{
			throw std::exception("Cannot open the file!!");
		}

		ifs.seekg(0, std::fstream::end);
		const auto endPos = ifs.tellg();
		ifs.clear();
		ifs.seekg(0, std::fstream::beg);
		const auto begPos = ifs.tellg();
		const auto fileSize = endPos - begPos; // std::streamoff �^�BVC2012 �ł� 32bit �łł� long long �ɂȂ�炵���B
		static_assert(sizeof(fileSize) >= 8, "Size of pos_type must be greater than or equal to 8 bytes!!");
		static_assert(sizeof(fpos_t) >= 8, "Size of fpos_t must be greater than or equal to 8 bytes!!");

		ifs.read(&outBuffer[0], numElementsInFile);
#else
		FILE* pFile = nullptr;
		const auto retCode = _wfopen_s(&pFile, pFilePath, L"rb");
		if (retCode != 0 || pFile == nullptr)
		{
			throw std::exception("Cannot open the file!!");
		}
		fread_s(&outBuffer[0], numElementsInFile * sizeof(T), sizeof(T), numElementsInFile, pFile);
		fclose(pFile);
		pFile = nullptr;
#endif
	}

	template<typename T> bool LoadBinaryFromFileImpl2(LPCWSTR pFilePath, std::vector<T>& outBuffer)
	{
		try
		{
			LoadBinaryFromFileImpl(pFilePath, outBuffer);
			return true;
		}
		catch (const std::exception& ex)
		{
			const CStringW strMsg(ex.what());
			ATLTRACE(L"%s <%s>\n", strMsg.GetString(), pFilePath);
			return false;
		}
	}

	bool LoadBinaryFromFile(LPCWSTR pFilePath, std::vector<char>& outBuffer)
	{
		return LoadBinaryFromFileImpl2(pFilePath, outBuffer);
	}

	bool LoadBinaryFromFile(LPCWSTR pFilePath, std::vector<int8_t>& outBuffer)
	{
		return LoadBinaryFromFileImpl2(pFilePath, outBuffer);
	}

	bool LoadBinaryFromFile(LPCWSTR pFilePath, std::vector<uint8_t>& outBuffer)
	{
		return LoadBinaryFromFileImpl2(pFilePath, outBuffer);
	}

	bool LoadBinaryFromFile(LPCWSTR pFilePath, std::vector<int32_t>& outBuffer)
	{
		return LoadBinaryFromFileImpl2(pFilePath, outBuffer);
	}

	bool LoadBinaryFromFile(LPCWSTR pFilePath, std::vector<uint32_t>& outBuffer)
	{
		return LoadBinaryFromFileImpl2(pFilePath, outBuffer);
	}

	bool LoadBinaryFromFile(LPCWSTR pFilePath, std::vector<float>& outBuffer)
	{
		return LoadBinaryFromFileImpl2(pFilePath, outBuffer);
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
			return std::move(std::string(&buff[0]));
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
			return std::move(std::wstring(&buff[0]));
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
