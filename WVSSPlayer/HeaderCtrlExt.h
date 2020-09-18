#pragma once
class CHeaderCtrlEx : public CHeaderCtrl
{
private:
protected:
public:
	short m_nVerticalOffset;
	short m_nHorizontalOffset;
private:
protected:
public:
	CHeaderCtrlEx();
	virtual ~CHeaderCtrlEx();
 void SetOffsets(short p_nHorizontalOffset, short p_nVerticalOffset) { m_nHorizontalOffset = p_nHorizontalOffset; m_nVerticalOffset = p_nVerticalOffset; }
protected:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	DECLARE_MESSAGE_MAP()
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
};
