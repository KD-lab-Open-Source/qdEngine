#include <my_stl.h>
#include <windows.h>

#include "XUtil.h"

#include "..\XZip.h"
#include "..\FileUtils.h"

const char* test_zip_name = "resource0.pak";
const char* test_file_name = "Resource\\Sound\\bolls.wav";

const int test_count = 100;

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
/*
	XZipArchiveMaker make("test.zip", "Resource\\Models;Resource\\Textures", "*.*", "", 9, false);
	make.buildArchive();
*/
	XStream fh("xziptest.log", XS_OUT);

	int start_time = xclock();
	XZipArchiveManager::instance().openArchive(test_zip_name);
	int tm = xclock() - start_time;

	fh < "openArchive() time = " <= tm < " msec\r\n";

	XZipStream fz(0);

	start_time = xclock();
	fz.open(test_file_name, XS_IN);
	tm = xclock() - start_time;
	fh < "XZipStream::open() time = " <= tm < " msec\r\n";

	int size = fz.size();
	char* buf = new char[size];
	fz.close();

	tm = 0;

	XZipStream::setOpenMode(XZIP_ARCHIVE_ONLY);

	for(int i = 0; i < test_count; i++){
		fz.open(test_file_name, XS_IN);
		start_time = xclock();
		fz.read(buf, size);
		tm += xclock() - start_time;
		fz.close();
	}

	float read_speed = float(size) / tm * 1000.f / 1024.f / 1024.f * float(test_count);
	fh < "avg XZipStream::read() time = " <= tm / float(test_count) < " msec, speed = " <= read_speed < " mbytes/sec\r\n";

	fz.close();

	XStream fh1("test.out", XS_OUT);
	fh1.write(buf, size);
	fh1.close();

	MessageBox(0, "Done!", "XZipTest", MB_OK);

	return 0;
}
