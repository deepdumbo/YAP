#include "stdafx.h"
#include "NiumagFidWriter.h"
#include "Implement/LogUserImpl.h"

#include <time.h>
#include <stdio.h>
#include <fstream>
#include <iosfwd>
#include "Client\DataHelper.h"

using namespace Yap;
using namespace std;

NiumagFidWriter::NiumagFidWriter(void) :
	ProcessorImpl(L"NiumagFidWriter")
{
	AddInput(L"Input", YAP_ANY_DIMENSION, DataTypeComplexFloat);
	AddOutput(L"Output", YAP_ANY_DIMENSION, DataTypeComplexFloat);

	AddProperty<wstring>(L"ExportFolder", L"", L"Set folder used to write FID.");
	AddProperty<wstring>(L"FileName", L"", L"Set file name.");
	AddProperty<wstring>(L"SavePath", L"", L"Full Path (folder and file name) used to write FID");
}

NiumagFidWriter::NiumagFidWriter(const NiumagFidWriter& rhs) :
	ProcessorImpl(rhs)
{
}

NiumagFidWriter::~NiumagFidWriter()
{
}

bool Yap::NiumagFidWriter::Input(const wchar_t * name, IData * data)
{
	assert((data != nullptr) && (GetDataArray<complex<float>>(data) != nullptr));

	auto file_path = GetProperty<wstring>(L"SavePath");
	if (file_path.empty())
	{
		auto output_folder = GetProperty<wstring>(L"ExportFolder");
		auto output_name = GetProperty<wstring>(L"FileName");
		file_path = GetFilePath(output_folder.c_str(), output_name.c_str()).c_str();
	}

	// write data
	int file_version = 1;
	int section1size = 100;
	int section2size = 100;
	int section3size = 100;
	int section4size = 100;
	int section5size = 100; // fid
	
	int section5_offset = 4 * 6 + 400;

	DataHelper data_helper(data);

	auto dimension_count = data_helper.GetDimensionCount();

	int dim1 = data_helper.GetWidth();
	int dim2 = data_helper.GetHeight();
	int dim3 = data_helper.GetSliceCount();
	int dim4 = data_helper.GetDim4();

	unsigned buffer_size = dim1 * dim2 * dim3 * dim4;
	complex<float> * fid_data = GetDataArray<complex<float>>(data);

	ofstream file(file_path, ios::binary);

	file.write(reinterpret_cast<char*>(&file_version), 4);
	file.write(reinterpret_cast<char*>(&section1size), 4);
	file.write(reinterpret_cast<char*>(&section2size), 4);
	file.write(reinterpret_cast<char*>(&section3size), 4);
	file.write(reinterpret_cast<char*>(&section4size), 4);
	file.write(reinterpret_cast<char*>(&section5size), 4);

	file.seekp(section5_offset, ios::beg);
	file.write(reinterpret_cast<char*>(&dim1), 4);
	file.write(reinterpret_cast<char*>(&dim2), 4);
	file.write(reinterpret_cast<char*>(&dim3), 4);
	file.write(reinterpret_cast<char*>(&dim4), 4);
	file.write(reinterpret_cast<char*>(fid_data), buffer_size * sizeof(complex<float>));
	file.close();

	if (OutportLinked(L"Output"))
	{
		return Feed(L"Output", data);
	}
		
	return true;	
}

std::wstring Yap::NiumagFidWriter::GetFilePath(const wchar_t * output_folder, const wchar_t * output_name)
{
	wstring file_path = output_folder;
	if (wcslen(output_folder) > 3)
	{
		file_path += L"\\";
	}

	if (wcslen(output_name) > 0)
	{
		file_path += output_name;
		file_path += L".";
	}

	struct tm t;
	time_t now;
	localtime_s(&t, &now);
	wstring time{ to_wstring(t.tm_year + 1900) };
	time += to_wstring(t.tm_mon + 1);
	time += to_wstring(t.tm_mday);
	time += L"-";
	time += to_wstring(t.tm_hour);
	time += to_wstring(t.tm_min);
	time += to_wstring(t.tm_sec);
	file_path += time;

	file_path += L".fid";

	return file_path;
}