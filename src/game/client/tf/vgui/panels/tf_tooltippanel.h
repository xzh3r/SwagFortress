#ifndef TF_MAINMENUTOOLTIPPANEL_H
#define TF_MAINMENUTOOLTIPPANEL_H

#include "tf_dialogpanelbase.h"

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class CTFToolTipPanel : public CTFMenuPanelBase
{
	DECLARE_CLASS_SIMPLE(CTFToolTipPanel, CTFMenuPanelBase);

public:
	CTFToolTipPanel(vgui::Panel* parent, const char *panelName);
	virtual bool Init();
	virtual ~CTFToolTipPanel();
	virtual void PerformLayout();
	virtual void ApplySchemeSettings( vgui::IScheme *pScheme );
	virtual void OnThink();
	virtual void Show();
	virtual void Hide();
	virtual void ShowToolTip( char *sText );
	virtual void HideToolTip();

protected:
	char		m_sText[256];
	CExLabel	*m_pText;
};

#endif // TF_MAINMENUTOOLTIPPANEL_H