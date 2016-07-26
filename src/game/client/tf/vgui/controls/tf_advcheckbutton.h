#ifndef tf_advcheckbutton_H
#define tf_advcheckbutton_H
#ifdef _WIN32
#pragma once
#endif

#include <vgui_controls/EditablePanel.h>
#include "tf_imagepanel.h"
#include "tf_advbuttonbase.h"

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class CTFCheckButton : public CTFButtonBase
{
	friend class CTFAdvCheckButton;
public:
	DECLARE_CLASS_SIMPLE( CTFCheckButton, CTFButtonBase );

	CTFCheckButton( vgui::Panel *parent, const char *panelName, const char *text );

	virtual void ApplySettings( KeyValues *inResourceData );
	virtual void ApplySchemeSettings( vgui::IScheme *pScheme );
	virtual void PerformLayout();
	virtual void DoClick( void );

	void SetChecked( bool bState );
	bool IsChecked( void ) { return m_bChecked; }

private:
	MESSAGE_FUNC_PTR( OnCheckButtonChecked, "CheckButtonChecked", panel );

	vgui::ImagePanel		*m_pCheckImage;
	vgui::EditablePanel		*m_pBGBorder;

	bool					m_bChecked;
};


#endif // tf_advcheckbutton_H
