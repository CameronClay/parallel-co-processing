#pragma once
#include <Includes.h>
//#include <MsgStream.h>
#include <File.h>

template<>
class StreamWriter::Helper<FileMisc::FileData> : public HelpBase<FileMisc::FileData>
{
public:
	Helper(StreamWriter& stream) : HelpBase<FileMisc::FileData>(stream) {}
	void Write(const FileMisc::FileData& t)
	{
		stream.Write(t.fileName);
		stream.Write(t.dateModified);
		stream.Write(t.size);
	}
	static constexpr std::size_t SizeType(const FileMisc::FileData& t)
	{
		return Helper<std::tstring>::SizeType(t.fileName) + sizeof(SYSTEMTIME) + sizeof(DWORD64);
	}
};

template<>
class StreamReader::Helper<FileMisc::FileData> : public HelpBase<FileMisc::FileData>
{
public:
	Helper(StreamReader& stream) : HelpBase<FileMisc::FileData>(stream) {}
	FileMisc::FileData Read()
	{
		FileMisc::FileData fd;
		stream.Read(fd.fileName);
		stream.Read(fd.dateModified);
		stream.Read(fd.size);
		return fd;
	}
};