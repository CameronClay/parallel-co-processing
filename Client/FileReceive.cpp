#include "FileReceive.h"
#include "StreamExt2.h"

FileReceive::FileReceive()
{}

FileReceive::~FileReceive()
{}


bool FileReceive::HaveFileList(MsgStreamReader& streamReader)
{
	streamReader.Read(fileList);
	streamReader.Read(dir);

	if (!FileMisc::Exists(dir.c_str()))
	{
		FileMisc::CreateFolder(dir.c_str());

		return InitFirstFile();
	}

	auto localList = FileMisc::GetFileNameList(dir.c_str());
	for (auto& f : fileList)
	{
		bool found = false;
		for (auto& fl : localList)
		{
			if (f.fileName == fl.fileName && memcmp(&f.dateModified, &fl.dateModified, sizeof(SYSTEMTIME)) == 0)
			{
				found = true;
				break;
			}
		}
		if (!found)
		{
			return InitFirstFile();
		}
	}

	return true;
}

bool FileReceive::ReadFiles(MsgStreamReader& streamReader)
{
	const UINT size = streamReader.GetDataSize();
	file.Write(streamReader.GetData(), size);
	curFile->size -= size;

	if (AdvanceFile())
		return true;

	return false;
}

bool FileReceive::AdvanceFile()
{
	while (!curFile->size)
	{
		file.ChangeDate(curFile->dateModified);
		if (curFile != fileList.end() - 1)
		{
			++curFile;
			file.Close();

			if (dir.empty())
				file.Open(curFile->fileName.c_str(), GENERIC_WRITE);
			else
				file.Open((dir + curFile->fileName).c_str(), GENERIC_WRITE);
		}
		else
		{
			OnCompletion();
			return true;
		}
	}
	return false;
}

bool FileReceive::InitFirstFile()
{
	dir += _T("\\");
	curFile = fileList.begin();

	if (dir.empty())
		file.Open(curFile->fileName.c_str(), GENERIC_WRITE);
	else
		file.Open((dir + curFile->fileName).c_str(), GENERIC_WRITE);

	if (AdvanceFile())
		return true;

	return false;
}

void FileReceive::OnCompletion()
{
	fileList.clear();
	file.Close();
}


