// ProgDlg.h : header file
// CG: This file was added by the Progress Dialog component

/////////////////////////////////////////////////////////////////////////////
// CProgressDlg dialog

#ifndef __PROGDLG_H__
#define __PROGDLG_H__

/// Класс диалога отображающий прогресс.
/**
	Сгенерирован 6й студией
	Лучше всего смотреть как используется в коде
 */
class CProgressDlg : public CDialog
{
// Construction / Destruction
public:
    CProgressDlg(UINT nCaptionID = 0);   // standard constructor
    ~CProgressDlg();

    BOOL Create(CWnd *pParent=NULL);

    // Progress Dialog manipulation
	/// Подпись 
    void SetStatus(LPCTSTR lpszMessage);
	/// Предел в которых изменяется значения
    void SetRange(int nLower,int nUpper);
	/// Устанавливает размер шага
    int  SetStep(int nStep);
	/// Устанавливает позицию
    int  SetPos(int nPos);
	/// Устанавливает текущую позицию путем добавления указанного числа к текущей позиции
    int  OffsetPos(int nPos);
	/// Шаг вперед
    int  StepIt();
        
// Dialog Data
    //{{AFX_DATA(CProgressDlg)
    enum { IDD = CG_IDD_PROGRESS };
    CProgressCtrl	m_Progress;
    //}}AFX_DATA

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CProgressDlg)
    public:
    virtual BOOL DestroyWindow();
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

// Implementation
protected:
	/// Заголовок
	UINT m_nCaptionID;
	/// Нижний предел
    int m_nLower;
	/// Верхний предел
    int m_nUpper;
	/// Размер шага
    int m_nStep;
	/// Служебный флаг
    BOOL m_bParentDisabled;

    void ReEnableParent();

    virtual void OnCancel();
    virtual void OnOK() {}; 
	/// Похоже вычисляет проценты
    void UpdatePercent(int nCurrent);
	/// Выбирает сообщения из очереди
    void PumpMessages();

    // Generated message map functions
    //{{AFX_MSG(CProgressDlg)
    virtual BOOL OnInitDialog();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

#endif // __PROGDLG_H__
