#ifndef TF_ADVITEMBUTTON_H
#define TF_ADVITEMBUTTON_H
#ifdef _WIN32
#pragma once
#endif

#include "tf_advbutton.h"

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class CTFAdvItemButton : public CTFButton
{
	friend class CTFButton;
public:
	DECLARE_CLASS_SIMPLE(CTFAdvItemButton, CTFButton);

	CTFAdvItemButton(vgui::Panel *parent, const char *panelName, const char *text);
	~CTFAdvItemButton();

	virtual void Init();
	virtual void ApplySchemeSettings( vgui::IScheme *pScheme );
	virtual void PerformLayout();
	virtual void ShowToolTip();

	void SetItemDefinition(CEconItemDefinition *pItemData);
	void SetLoadoutSlot( int iSlot, int iPreset );

protected:
	CEconItemDefinition *m_pItemDefinition;
	int m_iLoadoutSlot;
};


#endif // TF_ADVITEMBUTTON_H
