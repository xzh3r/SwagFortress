#ifndef tf_advbutton_H
#define tf_advbutton_H
#ifdef _WIN32
#pragma once
#endif

#include <vgui_controls/EditablePanel.h>
#include "tf_imagepanel.h"
#include "tf_advbuttonbase.h"

class CTFButton;

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class CTFAdvButton : public CTFAdvButtonBase
{
	friend class CTFButton;
public:
	DECLARE_CLASS_SIMPLE( CTFAdvButton, CTFAdvButtonBase );

	CTFAdvButton( vgui::Panel *parent, const char *panelName, const char *text );
	~CTFAdvButton();
	void Init();

	virtual void ApplySettings( KeyValues *inResourceData );
	virtual void ApplySchemeSettings( vgui::IScheme *pScheme );
	virtual void PerformLayout();
	virtual void OnThink();

	virtual void SendAnimation( MouseState flag );
	virtual void SetDefaultAnimation();

	void SetText( const char *tokenName );
	void SetGlowing( bool Glowing );
	void SetSelected( bool bState );
	void SetEnabled( bool bState );
	void SetBorderByString( const char *sBorderDefault, const char *sBorderArmed = NULL, const char *sBorderDepressed = NULL );

	CTFButton *GetButton() { return m_pButton; };

protected:
	CTFButton		*m_pButton;
	bool			m_bGlowing;
	bool			m_bAnimationIn;

	float			m_flActionThink;
	float			m_flAnimationThink;
};


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class CTFButton : public CTFButtonBase
{
	friend class CTFAdvButton;
public:
	DECLARE_CLASS_SIMPLE( CTFButton, CTFButtonBase );

	CTFButton( vgui::Panel *parent, const char *panelName, const char *text );

	void ApplySettings( KeyValues *inResourceData );
	void ApplySchemeSettings( vgui::IScheme *pScheme );
	void PerformLayout();
	void OnCursorExited();
	void OnCursorEntered();
	void OnMousePressed( vgui::MouseCode code );
	void OnMouseReleased( vgui::MouseCode code );
	void SetMouseEnteredState( MouseState flag );
	void SetParent( CTFAdvButton *m_pButton ) { m_pParent = m_pButton; };

private:
	CTFAdvButton   *m_pParent;
	float			m_fXShift;
	float			m_fYShift;
};


#endif // tf_advbutton_H
