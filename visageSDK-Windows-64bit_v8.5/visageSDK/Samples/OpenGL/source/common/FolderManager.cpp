#include <stdafx.h> 

#include "FolderManager.h"
#include "FolderManagerData.h"
#include "LicenseString.h"

#include <fstream>
#include <sstream>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <direct.h> 



namespace VisageSDK
{

	FolderManager::FolderManager()
	{
		strcpy(currentPath,".");
		strcpy(workPath,".");
	}

	FolderManager::~FolderManager()
	{
	}

#ifdef _UNICODE
	void FolderManager::ConvertToANSI(LPCWSTR string, char *ansiString)
	{
		int len = WideCharToMultiByte(CP_ACP, 0, string, -1, 0, 0, 0, 0);
		WideCharToMultiByte(CP_ACP, 0, string, -1, ansiString, len, 0, 0);
	}

	LPCWSTR FolderManager::ConvertToUNICODE(LPSTR string)
	{
		unsigned int len=strlen(string);
		wchar_t *result = new wchar_t[len+1];
		MultiByteToWideChar(CP_ACP,0,string,-1,result,len+1);
		return result;
	}
#endif

void FolderManager::init()
	{
		char visageRoot[500];
		FILE *f;
		int cnt = 0;
		bool found = false;
	
		initializeLicenseManager(licenseKey.c_str());

		//find the visage SDK root
		do
		{
			_getcwd( visageRoot, 500 );
			if(f = fopen("visageSDKRoot.txt","r"))
			{
				fclose(f);
				found = true;
				break; //we found the root folder
			}
			_chdir(".."); //look one level up
		}
		while(strlen(visageRoot)>3 && cnt++ < 200); // repeat until we come to something like c:/. Count the loop and avoid infinite loop - just in case.

		if(!found)
		{
			//try starting the search from the folder where the .exe file is actually located
#ifdef _UNICODE
			WCHAR visageRoot1[500];
			GetModuleFileName(NULL,visageRoot1,500);
			ConvertToANSI(visageRoot1,visageRoot);
#else
			GetModuleFileName(NULL,visageRoot,500);
#endif
			cnt = (int)strlen(visageRoot);
			while(cnt>0 && visageRoot[cnt]!='\\')
				cnt--;
			visageRoot[cnt] = 0;
			_chdir(visageRoot);
			cnt = 0;
			do
			{
				_getcwd( visageRoot, 500 );
				if(f = fopen("visageSDKRoot.txt","r"))
				{
					fclose(f);
					found = true;
					break; //we found the root folder
				}
				_chdir(".."); //look one level up
			}
			while(strlen(visageRoot)>3 && cnt++ < 200); // repeat until we come to something like c:/. Count the loop and avoid infinite loop - just in case.

		}

		if(!found)
		{
			char msg[500];
			sprintf(msg,"Could not find visage|SDK root folder. Exiting.\nMake sure that you are starting the application from within the visage|SDK folder hierarchy. If this message still appears, you may try to reinstall visage|SDK and/or contact support.");
#ifdef _UNICODE
			MessageBox(0,ConvertToUNICODE(msg),L"Error",MB_ICONERROR);
#else
			MessageBox(0,msg,"Error",MB_ICONERROR);
#endif
			exit(1);
		}

		_chdir(AppWorkingFolder); 
		_getcwd( workPath, 500 );

		for(int i=0; i<nRequiredFiles;i++)
		{
			if(!(f = fopen(RequiredFiles[i],"r")))
			{
				char msg[500];
				sprintf(msg,"The working folder \"%s\" does not contain the required file \"%s\". Exiting.\nMake sure that you are starting the application from within the visage|SDK folder hierarchy. If this message still appears, you may try to reinstall visage|SDK and/or contact support.",AppWorkingFolder,RequiredFiles[i]);
#ifdef _UNICODE
				MessageBox(0,ConvertToUNICODE(msg),L"Error",MB_ICONERROR);
#else
				MessageBox(0,msg,"Error",MB_ICONERROR);
#endif
				exit(1);
			}
			else fclose(f);
		}
	}

	void FolderManager::goToWorkingFolder()
	{
		_chdir(workPath); 
	}

	void FolderManager::pushPath()
	{
		_getcwd( currentPath, 500 );
	}

	void FolderManager::popPath()
	{
		_chdir(currentPath); 
	}

	/**
	* Folder manager.
	* The instance of the standard visage folder manager used by the sample projects in order to make sure that the sample application runs from the correct folder and that all required files are present.
	*/
	FolderManager visageFolderManager;

}