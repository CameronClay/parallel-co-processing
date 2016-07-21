#pragma once
#include <Includes.h>
//#include <MsgStream.h>
#include <File.h>

template<>
class StreamWriter::Helper<FileMisc::FileData> : public HelpBase<FileMisc::FileData>
{
public:
	Helper(StreamWriter& stream) : HelpBase(stream) {}
	void Write(const FileMisc::FileData& t)
	{
		stream.Write(t.fileName);
		stream.Write(t.dateModified);
		stream.Write(t.size);
	}
	static UINT SizeType(const FileMisc::FileData& t)
	{
		return StreamWriter::SizeType(t.fileName) + sizeof(SYSTEMTIME) + sizeof(DWORD64);
	}
};

template<>
class StreamReader::Helper<FileMisc::FileData> : public HelpBase<FileMisc::FileData>
{
public:
	Helper(StreamReader& stream) : HelpBase(stream) {}
	FileMisc::FileData Read()
	{
		FileMisc::FileData fd;
		stream.Read(fd.fileName);
		stream.Read(fd.dateModified);
		stream.Read(fd.size);
		return fd;
	}
};

//template<typename T>
//void StreamReader::Read(std::vector<T>& vec)
//{
//	const UINT size = stream.Read<UINT>();
//	vec.reserve(size);
//	for (UINT i = 0; i < size; i++)
//	{
//		vec.push_back();
//		stream.Read<T>(vec.back());
//	}
//}