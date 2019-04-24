#ifndef __FolderManager_h__
#define __FolderManager_h__


namespace VisageSDK
{
	void __declspec(dllimport) initializeLicenseManager(const char *licenseKeyFileFolder);

	/** visage|SDK folder manager.
	* Used to manage folders and files for all visage|SDK samples. All samples use it
	* to:
	* - make sure that they run from a correct exection folder
	* - make sure that all required files are present
	*/
	class FolderManager
	{
	public:
		/** Constructor
		*/
		FolderManager();

		/** Destructor
		*/
		~FolderManager();

#ifdef _UNICODE
		void FolderManager::ConvertToANSI(LPCWSTR string, char *ansiString);
		LPCWSTR FolderManager::ConvertToUNICODE(LPSTR string);
#endif		

		/** Go to working folder.
		* Go to the working folder of the application, defined in FolderManagerData.h. 
		*/
		void goToWorkingFolder();

		/** Initialise.
		* Initialise the folder management:
		* - find the visage|SDK root folder and go there
		* - from there, go to the working folder of the particular application, defined in FolderManagerData.h
		* - verify that all required files are present; required files are also defined in FolderManagerData.h; this file is unique for each application using FolderManager
		* 
		* If any of this fails, display an error message and exit the application.
		*/
		void init();

		/** Push path.
		* Save the current path (current working folder). popPath() restores it.
		*
		* @see popPath
		*/
		void pushPath();

		/** Pop path.
		* Restore the current path (current working folder) previously saved by pushPath().
		*
		* @see pushPath
		*/
		void popPath();


	private:
		char currentPath[500];
		char workPath[500]; //application working folder
	};

extern FolderManager visageFolderManager;

}
#endif
