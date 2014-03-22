//this file is part of notepad++
//Copyright (C)2003 Don HO <donho@altern.org>
//
//This program is free software; you can redistribute it and/or
//modify it under the terms of the GNU General Public License
//as published by the Free Software Foundation; either
//version 2 of the License, or (at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program; if not, write to the Free Software
//Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

#include "PluginDefinition.h"
#include "menuCmdID.h"
#include <memory>

//
// The plugin data that Notepad++ needs
//
FuncItem funcItem[nbFunc];
std::unique_ptr<ShortcutKey> toggleShortcut;

//
// The data of Notepad++ that you can use in your plugin commands
//
NppData nppData;

//
// Initialize your plugin data here
// It will be called while plugin loading   
void pluginInit(HANDLE hModule)
{
}

//
// Here you can do the clean up, save the parameters (if any) for the next session
//
void pluginCleanUp()
{
}

//
// Initialization of your plugin commands
// You should fill your plugins commands here
void commandMenuInit()
{

	toggleShortcut = std::make_unique<ShortcutKey>();
	toggleShortcut->_isCtrl = true;
	//toggleShortcut->_isShift = true;
	//toggleShortcut->_isAlt = true; 
	toggleShortcut->_key = 0x51; //VK_Q
	setCommand(0, L"Block Uncomment", performBlockCommentToggle, toggleShortcut.get(), false);
	setCommand(1, L"Reload and go to end", performReloadScrollToEnd, nullptr, false);
	setCommand(2, L"Normalize lines indents", performTabIndentNormalization, nullptr, false);


	setCommand(3, L"About", showAboutDialog, nullptr, false);
}

//
// Here you can do the clean up (especially for the shortcut)
//
void commandMenuCleanUp()
{
	toggleShortcut.reset();
}


//
// This function help you to initialize your plugin commands
//
bool setCommand(size_t index, TCHAR *cmdName, PFUNCPLUGINCMD pFunc, ShortcutKey *sk, bool check0nInit)
{
	if (index >= nbFunc)
		return false;

	if (!pFunc)
		return false;

	lstrcpy(funcItem[index]._itemName, cmdName);
	funcItem[index]._pFunc = pFunc;
	funcItem[index]._init2Check = check0nInit;
	funcItem[index]._pShKey = sk;

	return true;
}


void showAboutDialog()
{
	::MessageBox(nullptr, L"This plugin contains a few tiny commands:\
\r\n\
\r\n1. Proxy command 'Uncomment Block' which just call same command of NPP. The main purpose is to allow users to use the shortcut (there is no way to set shortcut for the default command).\
\r\n2. 'Reload and go to end' command which reloads file and just scrolls to the document end.\
\r\n3. 'Normalize lines indents' command which normalizes line indents. All the lines will the same indents will be normalized to 2*(number of indent). \
\r\n\
\r\nWas develped by Alex Povar.", L"NPP Tiny Helpers", MB_OK);
}

void performBlockCommentToggle()
{
	::SendMessage(nppData._nppHandle, NPPM_MENUCOMMAND, 0, IDM_EDIT_BLOCK_UNCOMMENT);
}

void performReloadScrollToEnd()
{
	//Get active document
	int activeScintillaRes;
	::SendMessage(nppData._nppHandle, NPPM_GETCURRENTSCINTILLA, (WPARAM)0, (LPARAM)&activeScintillaRes);
	HWND activeScintilla = activeScintillaRes == 0 ? nppData._scintillaMainHandle : nppData._scintillaSecondHandle;

	//Send reload & scroll to end messages
	::SendMessage(nppData._nppHandle, NPPM_MENUCOMMAND, 0, IDM_FILE_RELOAD);
	::SendMessage(activeScintilla, SCI_DOCUMENTEND, 0, 0);
}

void performTabIndentNormalization()
{
	//Get active document
	int activeScintillaRes;
	::SendMessage(nppData._nppHandle, NPPM_GETCURRENTSCINTILLA, (WPARAM)0, (LPARAM)&activeScintillaRes);
	HWND activeScintilla = activeScintillaRes == 0 ? nppData._scintillaMainHandle : nppData._scintillaSecondHandle;

	long tabWidth = ::SendMessage(activeScintilla, SCI_GETTABWIDTH, 0, 0);
	long allTextLen = ::SendMessage(activeScintilla, SCI_GETLENGTH, 0, 0);

	if (allTextLen == 0)
		return;

	std::unique_ptr<char[]> allTextBuff(new char[allTextLen + 1]);
	::SendMessage(activeScintilla, SCI_GETTEXT, allTextLen + 1, reinterpret_cast<LPARAM>(allTextBuff.get()));
	
	TabIndentsNormalizer indentsFixer = TabIndentsNormalizer(static_cast<int>(tabWidth), std::move(allTextBuff), allTextLen);

	//Get text fix fixed indents
	std::vector<char> normalizedText = indentsFixer.GetTextWithNormalizedIndents();

	//Add final null character
	if (normalizedText.size() > 0)
	{
		char finalChar = normalizedText.at(normalizedText.size() - 1);
		if (finalChar != '\0')
		{
			normalizedText.push_back('\0');
		}
	}

	::SendMessage(activeScintilla, SCI_BEGINUNDOACTION, 0, 0);
	::SendMessage(activeScintilla, SCI_SETTEXT, 0, reinterpret_cast<LPARAM>(normalizedText.data()));
	::SendMessage(activeScintilla, SCI_ENDUNDOACTION, 0, 0);
}

