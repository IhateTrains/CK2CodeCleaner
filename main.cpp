#include <stdio.h>  /* defines FILENAME_MAX */
#define WINDOWS  /* uncomment this line to use it for windows.*/
#ifdef WINDOWS
#include <direct.h>
#define GetCurrentDir _getcwd
#else
#include <unistd.h>
#define GetCurrentDir getcwd
#endif
#include<iostream>

#include <windows.h> /// for DWORD

#include <conio.h>
#include <iomanip>
#include <fstream>
#include <cstdio>

using namespace std;

string GetCurrentWorkingDir( void ) {
  char buff[FILENAME_MAX];
  GetCurrentDir( buff, FILENAME_MAX );
  std::string current_working_dir(buff);
  return current_working_dir;
}

string GetLastErrorAsString()
{
    //Get the error message, if any.
    DWORD errorMessageID = ::GetLastError();
    if (errorMessageID == 0)
        return std::string(); //No error message has been recorded

    LPSTR messageBuffer = nullptr;
    size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

    std::string message(messageBuffer, size);

    //Free the buffer.
    LocalFree(messageBuffer);

    return message;
}

void CleanFile ()
{
    char WhatToDo;
    ifstream File;
    ofstream CleanedFile;
    string ModWorkingDirectory;
    string FileDirectory;
    string SingleLine = "";
    int CrapCount=0;
    unsigned int CrapLocation1;
    cout<<"Specify the file directory (example: C:\\MyMod\\events\\MyEvent.txt): ";
    getline(cin, FileDirectory);
    string CleanedFileDirectory = GetCurrentWorkingDir()+"\\FILEBEINGCLEANEDBYILIKETRAINS.txt";
    File.open(FileDirectory.c_str());
    CleanedFile.open(CleanedFileDirectory.c_str());

    cout<<CleanedFileDirectory<<endl;
    if (File.good())
    {
        cout<<"File opened successfully."<<endl<<endl<<"If you want to continue cleaning, press 1. If you want to cancel cleaning, press 2."<<endl;
        do
            {
                cin.clear();
                cin.sync();
                WhatToDo = getch();

                switch(WhatToDo)
                {
                    case '1':
                    {
                        cout<<WhatToDo<<endl;
                        do
                        {
                            CrapCount=0;
                            while(!File.eof())
                            {
                                getline(File, SingleLine);
                                /// first function: replacing 4 spaces or from 1 to 3 spaces+TAB with 1 TAB (seriously, in many events in history there are tons of spaces instead of tabs...)
                                while (SingleLine.find("    ") != string::npos)
                                {
                                    CrapCount++;
                                    CrapLocation1 = SingleLine.find("    ");
                                    SingleLine.erase(CrapLocation1, 4);
                                    SingleLine = SingleLine.substr(0,CrapLocation1) + "	" + SingleLine.substr(CrapLocation1,SingleLine.length()-CrapLocation1);
                                }
                                while (SingleLine.find("   	") != string::npos)
                                {
                                    CrapCount++;
                                    CrapLocation1 = SingleLine.find("   	");
                                    SingleLine.erase(CrapLocation1, 4);
                                    SingleLine = SingleLine.substr(0,CrapLocation1) + "	" + SingleLine.substr(CrapLocation1,SingleLine.length()-CrapLocation1);
                                }
                                while (SingleLine.find("  	") != string::npos)
                                {
                                    CrapCount++;
                                    CrapLocation1 = SingleLine.find("  	");
                                    SingleLine.erase(CrapLocation1, 3);
                                    SingleLine = SingleLine.substr(0,CrapLocation1) + "	" + SingleLine.substr(CrapLocation1,SingleLine.length()-CrapLocation1);
                                }
                                while (SingleLine.find(" 	") != string::npos)
                                {
                                    CrapCount++;
                                    CrapLocation1 = SingleLine.find(" 	");
                                    SingleLine.erase(CrapLocation1, 2);
                                    SingleLine = SingleLine.substr(0,CrapLocation1) + "	" + SingleLine.substr(CrapLocation1,SingleLine.length()-CrapLocation1);
                                }
                                /// simple check to keep lines with only TABs where they are supposed to stay
                                int PreviousLineTabCount = 0;
                                while (SingleLine[PreviousLineTabCount]=='	') PreviousLineTabCount++; /// I like this line, sometimes I can be smart
                                /// second function: removing empty spaces and TABs at the end of line
                                string ExpectedStringOfTabs = "";
                                for (int i=0;i<PreviousLineTabCount;i++) ExpectedStringOfTabs=ExpectedStringOfTabs+'	';
                                while ((SingleLine[SingleLine.size()-1] == ' '  || SingleLine[SingleLine.size()-1] == '	') && SingleLine != ExpectedStringOfTabs)
                                {
                                    CrapCount++;
                                    SingleLine.erase(SingleLine.size()-1, 1);
                                }
                                if (!File.eof()) CleanedFile<<SingleLine<<endl; ///the eof check is to avoid adding empty line at the end of file
                                else if (SingleLine!="") CleanedFile<<SingleLine<<endl;
                            }
                        }
                        while (CrapCount>0);

                        cout<<"Cleaning finished."<<endl;
                        break;
                    }
                    case '2':
                    {
                        cout<<WhatToDo<<endl;
                        break;
                    }
                }
            }
        while (WhatToDo!='1' && WhatToDo!='2');

        File.close();
        CleanedFile.close();

        remove( FileDirectory.c_str() ); /// removed the old version of the file

        if (!MoveFile(CleanedFileDirectory.c_str(), FileDirectory.c_str())) /// move the cleaned version in place of the old one
        {
            string msg = GetLastErrorAsString();
            cout << "Fail: " << msg << endl;
            system("pause");
        }
        else {
            cout << "File saved." << endl<<endl;
        }
    }
    else
    {
        cout<<"Specified file couldn't be opened."<<endl<<endl;
    }
}

int main()
{
    cout << "===============================" << endl;
    cout << " Crusader Kings 2 Code Cleaner" << endl;
    cout << "===============================" << endl<<endl;
    GetCurrentWorkingDir();
    CleanFile();
    return main();
}
