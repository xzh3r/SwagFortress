//=============================================================================
//
// Purpose: Payload HUD
//
//=============================================================================

#ifndef HUD_ESCORT_H
#define HUD_ESCORT_H

#ifdef _WIN32
#pragma once
#endif

#include "hudelement.h"
#include <vgui_controls/Panel.h>
#include <vgui_controls/Label.h>
#include <vgui_controls/ImagePanel.h>
#include "vgui_controls/EditablePanel.h"
#include "vgui_controls/AnimationController.h"
#include "vgui_controls/CircularProgressBar.h"
#include <vgui/ISurface.h>
#include "GameEventListener.h"
#include "tf_controls.h"
#include "IconPanel.h"

class CTFHudEscort : public vgui::EditablePanel, public CGameEventListener
{
public:
	DECLARE_CLASS_SIMPLE( CTFHudEscort, vgui::EditablePanel );

	CTFHudEscort( vgui::Panel *pParent, const char *pszName );

	virtual void ApplySchemeSettings( vgui::IScheme *pScheme );
	virtual bool IsVisible( void );
	virtual void FireGameEvent( IGameEvent *event );
	virtual void OnTick( void );

private:
	vgui::ImagePanel *m_pLevelBar;

	vgui::EditablePanel *m_pEscortItemPanel;
	CExLabel *m_pCapNumPlayers;
	CExLabel *m_pRecedeTime;
	vgui::ImagePanel *m_pCapPlayerImage;
	vgui::ImagePanel *m_pBackwardsImage;
	vgui::ImagePanel *m_pBlockedImage;

	float m_flProgress;
	float m_flRecedeTime;
};

#endif // HUD_ESCORT_H
