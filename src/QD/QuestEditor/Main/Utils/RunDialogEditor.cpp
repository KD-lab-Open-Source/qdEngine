#include "stdafx.h"
#include "resource.h"
#include "CurDirSafer.h"
#include <ShlWapi.h>
#include ".\rundialogeditor.h"
#include "disk_op.h"
#include "ProcessLauncher.h"

RunDialogEditor::RunDialogEditor(void)
{
}

RunDialogEditor::~RunDialogEditor(void)
{
}

bool RunDialogEditor::run(CString const& projFileName)
{
	CString strExeName;
	strExeName.LoadString(IDS_DIALOG_EDITOR);

	CString app_path;
	GetModuleFileName(AfxGetApp()->m_hInstance, CStrBuf(app_path, 1024), 1024);

	TCHAR drive[_MAX_DRIVE];
	TCHAR dir[_MAX_DIR];
	TCHAR fname[_MAX_FNAME];
	TCHAR ext[_MAX_EXT];
	_tsplitpath( app_path, drive, dir, fname, ext );

	app_path = drive;
	app_path += dir;
	app_path += strExeName;

	CString comand_line = _T("\"");
	comand_line += app_path;
	comand_line += _T("\"");

	CString fileName = projFileName;
	PathRenameExtension(CStrBuf(fileName, fileName.GetLength() + 1), _T(".tdb"));

	comand_line += _T(" \"");
	comand_line += fileName;
	comand_line += _T("\"");

	comand_line += _T(" -GM");

	comand_line += _T(" -E\"");
	fileName = CurDirSafer::GetCurrentDirectory();
	fileName += _T("\\Resource\\");
	fileName += disk_op::GetFileName(projFileName);
	PathRenameExtension(CStrBuf(fileName, fileName.GetLength() + 2), _T(".btdb"));

	comand_line += fileName;
	comand_line += _T("\"");

	ProcessLauncher launch;
	return launch.createProcess(app_path, CStrBuf(comand_line));
}