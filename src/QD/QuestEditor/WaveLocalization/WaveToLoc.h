#pragma once
#include "qd_fwd.h"

class CProgressDlg;

/// Объект, копирующий куда-то и откуда-то звуки. Не помню, для чего это было сделано
class WaveToLoc
{
public:
	WaveToLoc(void);
	~WaveToLoc(void);
	bool run(CWnd* parent, qdGameDispatcher& gameDisp, bool copyToResource);
protected:
private:
	CString baseDir_;
};

