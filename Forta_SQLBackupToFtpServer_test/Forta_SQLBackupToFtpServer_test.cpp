#include "CppUnitTest.h"

#include "FakeSocket.h"
#include "../Forta_SQLBackuptoFTPserver/FTPClient.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace FortaSQLBackupToFtpServertest
{
	TEST_CLASS(FortaSQLBackupToFtpServertest)
	{
	public:
		
		TEST_METHOD(FTPClientTestPositive)
		{
			FTPClient<FakeSocket> ftp;
			ftp.connctToHost("host",20);
			ftp.login("test", "root");
		}

		TEST_METHOD(FTPClientTestNegative)
		{
			FTPClient<FakeSocket> ftp;
			ftp.connctToHost("host", 20);
			try
			{
				ftp.login("error", "root");
			}
			catch (const FTPException& e)
			{
				Assert::AreEqual(e.getResponse().back().first, 503);
			}
			
		}
	};
}
