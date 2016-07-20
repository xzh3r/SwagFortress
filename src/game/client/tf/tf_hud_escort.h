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
#include <vgui/ISurface.h>
#include "GameEventListener.h"
#include "tf_controls.h"

class CEscortHillPanel;
class CTFHudEscortProgressBar;


class CTFHudEscort : public vgui::EditablePanel, public CGameEventListener
{
public:
	DECLARE_CLASS_SIMPLE( CTFHudEscort, vgui::EditablePanel );

	CTFHudEscort( vgui::Panel *pParent, const char *pszName, int iTeam, bool bMultipleTrains );

	virtual void ApplySchemeSettings( vgui::IScheme *pScheme );
	virtual void OnChildSettingsApplied( KeyValues *pInResourceData, Panel *pChild );
	virtual void PerformLayout( void );
	virtual bool IsVisible( void );
	virtual void FireGameEvent( IGameEvent *event );
	virtual void OnTick( void );

	void UpdateCPImages( bool bUpdatePositions, int iIndex );
	void UpdateAlarmAnimations( void );
	void SetTeam( int iTeam ) { m_iTeamNum = iTeam; }
	void SetMultipleTrains( bool bEnabled ) { m_bMultipleTrains = bEnabled; }
	void SetOnTop( bool bTop ) { m_bOnTop = bTop; }

private:
	vgui::ImagePanel *m_pLevelBar;

	vgui::EditablePanel *m_pEscortItemPanel;
	vgui::ImagePanel *m_pEscortItemImage;
	vgui::ImagePanel *m_pEscortItemImageBottom;
	vgui::ImagePanel *m_pEscortItemImageAlert;
	CExLabel *m_pCapNumPlayers;
	CExLabel *m_pRecedeTime;
	vgui::ImagePanel *m_pCapPlayerImage;
	vgui::ImagePanel *m_pBackwardsImage;
	vgui::ImagePanel *m_pBlockedImage;

	vgui::ImagePanel *m_pCPImageTemplate;
	vgui::ImagePanel *m_pCPImages[MAX_CONTROL_POINTS];

	CEscortHillPanel *m_pHillPanels[TEAM_TRAIN_MAX_HILLS];

	CTFHudEscortProgressBar *m_pProgressBar;

	float m_flProgress;
	float m_flRecedeTime;

	int m_iTeamNum;
	bool m_bMultipleTrains;
	bool m_bOnTop;
	bool m_bAlarm;
	int m_iNumHills;
};


class CTFHudMultipleEscort : public vgui::EditablePanel, public CGameEventListener
{
public:
	DECLARE_CLASS_SIMPLE( CTFHudMultipleEscort, vgui::EditablePanel );

	CTFHudMultipleEscort( vgui::Panel *pParent, const char *pszName );

	virtual void ApplySchemeSettings( vgui::IScheme *pScheme );
	virtual void FireGameEvent( IGameEvent *event );
	virtual void SetVisible( bool bVisible );
	virtual bool IsVisible( void );

private:
	CTFHudEscort *m_pRedEscort;
	CTFHudEscort *m_pBlueEscort;
};


class CEscortHillPanel : public vgui::Panel, public CGameEventListener
{
public:
	DECLARE_CLASS_SIMPLE( CEscortHillPanel, vgui::Panel );

	CEscortHillPanel( vgui::Panel *pParent, const char *pszName );

	virtual void Paint( void );
	virtual void PerformLayout( void );
	virtual void OnTick( void );
	virtual void FireGameEvent( IGameEvent *event );
	
	void SetHillIndex( int index ) { m_iHillIndex = index; }
	void SetTeam( int iTeam ) { m_iTeamNum = iTeam; }

private:
	int m_iTextureId;
	bool m_bActive;
	bool m_bLowerAlpha;
	int m_iWidth;
	int m_iHeight;
	float m_flScrollPerc;
	float m_flTextureScale;

	int m_iTeamNum;
	int m_iHillIndex;
};


class CTFHudEscortProgressBar : public vgui::ImagePanel
{
public:
	DECLARE_CLASS_SIMPLE( CTFHudEscortProgressBar, vgui::ImagePanel );

	CTFHudEscortProgressBar( vgui::Panel *pParent, const char *pszName, int iTeam );

	virtual void Paint( void );
	void SetProgress( float flValue ) { m_flProgress = flValue; }

private:
	int m_iTextureId;

	int m_iTeamNum;
	float m_flProgress;
};

#endif // HUD_ESCORT_H
