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

/// Settings
bool ExcludeCommentedLines = 1; /// exclude by default
bool AddingSpacesBetweensyntax = 1;
string CommentSignsString = "#"; /// NOTE: only one-sign characters are supported

const string OperatorsAndBrackets = "><={}";

string GetCurrentWorkingDir( void ) {
  char buff[FILENAME_MAX];
  GetCurrentDir( buff, FILENAME_MAX );
  std::string current_working_dir(buff);
  return current_working_dir;
}

bool ToBool (string value)
{
    if(value == "1") return true;
    return false;
}



void Menu (void)
{
    cout << "=========================================" << endl;
    cout << "      Crusader Kings 2 Code Cleaner" << endl;
    cout << "=========================================" << endl;
    cout<<"C - initiate new cleaning"<<endl;
    ///cout<<"S - open settings"<<endl;
    cout<<"ESC - exit"<<endl;
    cout << "========================================="<<endl;
    cout<<endl;
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

int FirstCommentSignLocation (string SingleLine)
{
    /// returns -1 if comment sign not found
    int location = -1;
    for (unsigned int i=0; i<SingleLine.size(); i++)
        if (CommentSignsString.find(SingleLine[i]) != string::npos) location = i;
    return location;
}

void Settings (char WhatToDo)
{
    ifstream ConfigFile;
    ofstream TempConfigFile;
    string SingleSettingsFlag = "";
    string Syntax = "";
    string ExpectedSyntax = "=";
    string SettingsFlagValue = "";
    string SingleLine = "";
    string ConfigFileDirectory = GetCurrentWorkingDir()+"\\settings.txt";
    string TempConfigFileDirectory = GetCurrentWorkingDir()+"\\settings_temp.txt";
    ConfigFile.open(ConfigFileDirectory.c_str());

    cout<<ConfigFileDirectory<<endl;

    if (ConfigFile.good())
    {
                switch(WhatToDo)
                {
                    case '1': /// load settings
                    {
                            while(!ConfigFile.eof())
                            {
                                ConfigFile>>SingleSettingsFlag;
                                ConfigFile>>Syntax;

                                if (SingleSettingsFlag.find("ExcludeCommentedLines") == 0 && Syntax == ExpectedSyntax) /// the flag is found at the beginning of the line and syntax is OK
                                {
                                    ConfigFile>>SettingsFlagValue;
                                    if (SettingsFlagValue=="0" || SettingsFlagValue == "1") ExcludeCommentedLines = ToBool(SettingsFlagValue); /// save value
                                }
                                else if (SingleSettingsFlag.find("AddingSpacesBetweensyntax") == 0 && Syntax == ExpectedSyntax)
                                {
                                    ConfigFile>>SettingsFlagValue;
                                    if (SettingsFlagValue=="0" || SettingsFlagValue == "1") AddingSpacesBetweensyntax = ToBool(SettingsFlagValue); /// save value
                                }
                                else if (SingleSettingsFlag.find("CommentSignsString") == 0 && Syntax == ExpectedSyntax)
                                {
                                    ConfigFile>>SettingsFlagValue;
                                    CommentSignsString = SettingsFlagValue; /// save string
                                }
                                else getline(ConfigFile, SingleLine); /// no point in reading the rest of line if it's useless
                            }

                        cout<<"Settings loaded from "<<ConfigFileDirectory<<endl;

                        ConfigFile.close();

                        break;
                    }
                    case '2': /// save settings, not written and not needed ATM
                    {
                        ///TempConfigFile.open(TempConfigFileDirectory.c_str());

                        ConfigFile.close();
                        ///TempConfigFile.close();
                        ///remove( CleanedFileDirectory.c_str() ); /// removes the temporary file

                        break;
                    }
                }
    }
    else
    {
        cout<<"Settings file couldn't be loaded."<<endl;
        cout<<"Don't worry, you can still run the program with default settings :)" <<endl<<endl;
    }
}

void CleanFile ()
{
    char WhatToDo;
    ifstream File;
    ofstream CleanedFile;
    string ModWorkingDirectory; /// unused ATM
    string FileDirectory;
    string SingleLine = "";
    int CrapCount=0;
    unsigned int CrapLocation1; /// for first function

    cout<<"Specify the file directory (example: C:\\MyMod\\events\\MyEvent.txt): ";
    getline(cin, FileDirectory);
    string CleanedFileDirectory = GetCurrentWorkingDir()+"\\FILEBEINGCLEANEDBYILIKETRAINS.txt"; /// lul name but at least almost ensures no such file exists already (yeah too lazy to add proper safety checks...)
    File.open(FileDirectory.c_str());
    CleanedFile.open(CleanedFileDirectory.c_str());

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
                            while(getline(File, SingleLine))
                            {
                                    /// first function: replacing 4 spaces or from 1 to 3 spaces+TAB with 1 TAB (seriously, in many events in history there are tons of spaces instead of tabs...)
                                    while (SingleLine.find("    ") != string::npos && (SingleLine.find("    ")<FirstCommentSignLocation(SingleLine) || ExcludeCommentedLines == false)) /// here comparison between unsigned int and int is actually helpful (negative int is greater than unsigned int, so -1 is treated like a great number)
                                    {
                                        CrapCount++;
                                        CrapLocation1 = SingleLine.find("    ");
                                        SingleLine.erase(CrapLocation1, 4);
                                        SingleLine = SingleLine.substr(0,CrapLocation1) + "	" + SingleLine.substr(CrapLocation1,SingleLine.length()-CrapLocation1);
                                    }
                                    while (SingleLine.find("   	") != string::npos && (SingleLine.find("   	")<FirstCommentSignLocation(SingleLine) || ExcludeCommentedLines == false))
                                    {
                                        CrapCount++;
                                        CrapLocation1 = SingleLine.find("   	");
                                        SingleLine.erase(CrapLocation1, 4);
                                        SingleLine = SingleLine.substr(0,CrapLocation1) + "	" + SingleLine.substr(CrapLocation1,SingleLine.length()-CrapLocation1);
                                    }
                                    while (SingleLine.find("  	") != string::npos && (SingleLine.find("  	")<FirstCommentSignLocation(SingleLine) || ExcludeCommentedLines == false))
                                    {
                                        CrapCount++;
                                        CrapLocation1 = SingleLine.find("  	");
                                        SingleLine.erase(CrapLocation1, 3);
                                        SingleLine = SingleLine.substr(0,CrapLocation1) + "	" + SingleLine.substr(CrapLocation1,SingleLine.length()-CrapLocation1);
                                    }
                                    while (SingleLine.find(" 	") != string::npos && (SingleLine.find(" 	")<FirstCommentSignLocation(SingleLine) || ExcludeCommentedLines == false))
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
                                    /// this funtion is not excluded from cleaning
                                    string ExpectedStringOfTabs = "";
                                    for (int i=0;i<PreviousLineTabCount;i++) ExpectedStringOfTabs=ExpectedStringOfTabs+'	';
                                    while ((SingleLine[SingleLine.size()-1] == ' '  || SingleLine[SingleLine.size()-1] == '	') && SingleLine != ExpectedStringOfTabs)
                                    {
                                        CrapCount++;
                                        SingleLine.erase(SingleLine.size()-1, 1);
                                    }

                                    ///third funtion: adding spaces between syntax
                                    for (unsigned int i=0;i<SingleLine.size();i++)
                                    {
                                        if (OperatorsAndBrackets.find(SingleLine[i])!=string::npos&& (i<FirstCommentSignLocation(SingleLine) || ExcludeCommentedLines == false))
                                        {
                                            if (SingleLine[i] == '=')
                                            {
                                                if (i>0 && SingleLine[i-1] != ' ' && SingleLine[i-1] != '	')
                                                {
                                                    SingleLine = SingleLine.substr(0, i) + " " + SingleLine.substr(i, SingleLine.size()-i);
                                                    i++;
                                                }
                                                if (i<SingleLine.size()-1 && SingleLine[i+1] != ' ')
                                                {
                                                    SingleLine = SingleLine.substr(0, i+1) + " " + SingleLine.substr(i+1, SingleLine.size()-i-1);
                                                    i++;
                                                }
                                            }
                                            if (SingleLine[i] == '>' || SingleLine[i] == '<')
                                            {
                                                if (i>0 && SingleLine[i-1] != ' ' && SingleLine[i-1] != '	')
                                                {
                                                    SingleLine = SingleLine.substr(0, i) + " " + SingleLine.substr(i, SingleLine.size()-i);
                                                    i++;
                                                }
                                                if (i<SingleLine.size()-1 && SingleLine[i+1] != ' ' && SingleLine[i+1] != '=')
                                                {
                                                    SingleLine = SingleLine.substr(0, i+1) + " " + SingleLine.substr(i+1, SingleLine.size()-i-1);
                                                    i++;
                                                }
                                            }
                                            if (SingleLine[i] == '{' || SingleLine[i] == '}')
                                            {
                                                if (i>0 && SingleLine[i-1] != ' ' && SingleLine[i-1] != '	')
                                                {
                                                    SingleLine = SingleLine.substr(0, i) + " " + SingleLine.substr(i, SingleLine.size()-i);
                                                    i++;
                                                }
                                                if (i<SingleLine.size()-1 && SingleLine[i+1] != ' ')
                                                {
                                                    SingleLine = SingleLine.substr(0, i+1) + " " + SingleLine.substr(i+1, SingleLine.size()-i-1);
                                                    i++;
                                                }
                                            }
                                        }
                                    }

                                CleanedFile<<SingleLine;
                                if (!File.eof()) CleanedFile<<endl;

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

        remove( FileDirectory.c_str() ); /// removes the old version of the file

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
    Menu();
}

int main()
{
    char d;

    Settings(1); /// load settings
    GetCurrentWorkingDir();
    Menu();

    do
    {
        cin.clear();
        cin.sync();
        d = toupper(getch());

        switch(d)
        {
            ///case 'S': Settings(1); break; /// for now it's only done through the text file
            case 'C': CleanFile(); break;
            case 27: cout<<"Closing the program..."<<endl; break;
            default: cout<<"You pressed a wrong key"<<endl;
        }
    }
    while (d!=char(27));

    return 0;
}
