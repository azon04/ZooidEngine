#include "FileTest.h"

#include "Utils/ZEngineHelper.h"

#include "FileSystem/FileWriter.h"
#include "FileSystem/FileReader.h"

void FileTest::RunTest()
{
	char buffer[128];
	ZE::FileReader file("Resources/TestFile.txt");
	
	ZASSERT(file.size() == 42, "Check file size Failed!");
	
	size_t resultSize = file.readToBuffer(buffer, file.size());
	ZASSERT(resultSize <= 42, "Read file Failed!");

	buffer[resultSize] = '\0';
	ZEINFO("Buffer : %s", buffer);

	file.close();

	ZE::FileWriter fileWriter("Resources/WriteTestFile.txt");
	fileWriter.writeFromBuffer(buffer, resultSize);

	fileWriter.close();
}
