#pragma once

HRESULT CreateShortcut(/*in*/ LPCTSTR lpszFileName, 
                    /*in*/ LPCTSTR lpszDesc, 
					/*in*/ LPCTSTR lpszWorkingDirectory,
                    /*in*/ LPCTSTR lpszShortcutPath,
					/*in*/ LPCTSTR lpszArguments=NULL);

HRESULT ResolveShortcut(/*in*/ LPCTSTR lpszShortcutPath,
                        /*out*/ LPTSTR lpszFilePath);