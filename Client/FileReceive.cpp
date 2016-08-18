#include "FileReceive.h"
#include "StreamExt2.h"

FileReceive::FileReceive()
{}

FileReceive::~FileReceive()
{}

bool FileReceive::ReadFiles(MsgStreamReader& streamReader)
{
	if (fileList.empty())
	{
		streamReader.Read(fileList);
		streamReader.Read(dir);
		if (!dir.empty())
		{
			dir.append(_T("\\"));
			if (!FileMisc::Exists(dir.c_str()))
				FileMisc::CreateFolder(dir.c_str());
		}

		curFile = fileList.begin();

		if (dir.empty())
			file.Open(curFile->fileName.c_str(), GENERIC_WRITE);
		else
			file.Open((dir + curFile->fileName).c_str(), GENERIC_WRITE);

		if (AdvanceFile())
			return true;
	}
	else
	{
		const UINT size = streamReader.GetDataSize();
		file.Write(streamReader.GetData(), size);
		curFile->size -= size;

		if (AdvanceFile())
			return true;
	}
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


void FileReceive::OnCompletion()
{
	fileList.clear();
	file.Close();
}


