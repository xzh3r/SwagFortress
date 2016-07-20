//=============================================================================
//
// Purpose: Payload HUD
//
//=============================================================================
#include "cbase.h"
#include "tf_hud_escort.h"
#include "tf_hud_freezepanel.h"
#include "c_team_objectiveresource.h"
#include "tf_gamerules.h"
#include "iclientmode.h"
#include "vgui_controls/AnimationController.h"

using namespace vgui;


//=============================================================================
// CTFHudEscort
//=============================================================================
CTFHudEscort::CTFHudEscort( Panel *pParent, const char *pszName, int iTeam, bool bMultipleTrains ) : EditablePanel( pParent, pszName )
{
	m_iTeamNum = iTeam;

	m_pLevelBar = new ImagePanel( this, "LevelBar" );

	m_pEscortItemPanel = new EditablePanel( this, "EscortItemPanel" );
	m_pEscortItemImage = new ImagePanel( m_pEscortItemPanel, "EscortItemImage" );
	m_pEscortItemImageBottom = new ImagePanel( m_pEscortItemPanel, "EscortItemImageBottom" );
	m_pEscortItemImageAlert = new ImagePanel( m_pEscortItemPanel, "EscortItemImageAlert" );
	m_pCapNumPlayers = new CExLabel( m_pEscortItemPanel, "CapNumPlayers", "x0" );
	m_pRecedeTime = new CExLabel( m_pEscortItemPanel, "RecedeTime", "0" );
	m_pCapPlayerImage = new ImagePanel( m_pEscortItemPanel, "CapPlayerImage" );
	m_pBackwardsImage = new ImagePanel( m_pEscortItemPanel, "Speed_Backwards" );
	m_pBlockedImage = new ImagePanel( m_pEscortItemPanel, "Blocked" );

	for ( int i = 0; i < MAX_CONTROL_POINTS; i++ )
	{
		m_pCPImages[i] = new ImagePanel( this, VarArgs( "cp_%d", i ) );
	}

	m_pCPImageTemplate = new ImagePanel( this, "SimpleControlPointTemplate" );

	for ( int i = 0; i < TEAM_TRAIN_MAX_HILLS; i++ )
	{
		m_pHillPanels[i] = new CEscortHillPanel( this, VarArgs( "hill_%d", i ) );
	}

	m_pProgressBar = new CTFHudEscortProgressBar( this, "ProgressBar", m_iTeamNum );

	m_flProgress = -1.0f;
	m_flRecedeTime = 0.0f;

	m_bMultipleTrains = bMultipleTrains;
	m_bOnTop = true;
	m_bAlarm = false;
	m_iNumHills = 0;

	ivgui()->AddTickSignal( GetVPanel() );

	ListenForGameEvent( "escort_progress" );
	ListenForGameEvent( "escort_speed" );
	ListenForGameEvent( "escort_recede" );
	ListenForGameEvent( "controlpoint_initialized" );
	ListenForGameEvent( "controlpoint_updateimages" );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFHudEscort::ApplySchemeSettings( IScheme *pScheme )
{
	BaseClass::ApplySchemeSettings( pScheme );

	// Get hill data.
	if ( ObjectiveResource() )
	{
		m_iNumHills = ObjectiveResource()->GetNumNodeHillData( m_iTeamNum );
	}

	// Setup conditions.
	KeyValues *pConditions = NULL;
	if ( m_iTeamNum >= FIRST_GAME_TEAM )
	{
		pConditions = new KeyValues( "conditions" );

		AddSubKeyNamed( pConditions, m_iTeamNum == TF_TEAM_RED ? "if_team_red" : "if_team_blue" );

		if ( m_bMultipleTrains )
		{
			AddSubKeyNamed( pConditions, "if_multiple_trains" );
			AddSubKeyNamed( pConditions, m_iTeamNum == TF_TEAM_RED ? "if_multiple_trains_red" : "if_multiple_trains_blue" );
			AddSubKeyNamed( pConditions, m_bOnTop ? "if_multiple_trains_top" : "if_multiple_trains_bottom" );
		}
		else if ( m_iNumHills > 0 )
		{
			AddSubKeyNamed( pConditions, "if_single_with_hills" );
			AddSubKeyNamed( pConditions, m_iTeamNum == TF_TEAM_RED ? "if_single_with_hills_red" : "if_single_with_hills_blue" );
		}
	}

	LoadControlSettings( "resource/ui/ObjectiveStatusEscort.res", NULL, NULL, pConditions );

	if ( pConditions )
		pConditions->deleteThis();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFHudEscort::OnChildSettingsApplied( KeyValues *pInResourceData, Panel *pChild )
{
	// Apply settings from template to all CP icons.
	if ( pChild == m_pCPImageTemplate )
	{
		for ( int i = 0; i < MAX_CONTROL_POINTS; i++ )
		{
			if ( m_pCPImages[i] )
			{
				m_pCPImages[i]->ApplySettings( pInResourceData );
			}
		}
	}

	BaseClass::OnChildSettingsApplied( pInResourceData, pChild );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFHudEscort::PerformLayout( void )
{
	// If the tracker's at the bottom show the correct cart image.
	if ( m_pEscortItemImage )
		m_pEscortItemImage->SetVisible( m_bOnTop );

	if ( m_pEscortItemImageBottom )
		m_pEscortItemImageBottom->SetVisible( !m_bOnTop );

	// Update hill panels.
	for ( int i = 0; i < TEAM_TRAIN_MAX_HILLS; i++ )
	{
		CEscortHillPanel *pPanel = m_pHillPanels[i];
		if ( !pPanel )
			continue;

		if ( !ObjectiveResource() || i >= ObjectiveResource()->GetNumNodeHillData( m_iTeamNum ) || !m_pLevelBar )
		{
			if ( pPanel->IsVisible() )
				pPanel->SetVisible( false );

			continue;
		}

		pPanel->SetTeam( m_iTeamNum );
		pPanel->SetHillIndex( i );

		if ( !pPanel->IsVisible() )
			pPanel->SetVisible( true );

		// Set the panel's bounds according to starting and ending points of the hill.
		int x, y, wide, tall;
		m_pLevelBar->GetBounds( x, y, wide, tall );

		float flStart, flEnd;
		ObjectiveResource()->GetHillData( m_iTeamNum, i, flStart, flEnd );

		int iStartPos = flStart * wide;
		int iEndPos = flEnd * wide;

		pPanel->SetBounds( x + iStartPos, y, iEndPos - iStartPos, tall );

		// Show it on top of the track bar.
		pPanel->SetZPos( m_pLevelBar->GetZPos() + 1 );
	}

	UpdateCPImages( true, -1 );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CTFHudEscort::IsVisible( void )
{
	if ( IsInFreezeCam() )
		return false;

	return BaseClass::IsVisible();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFHudEscort::FireGameEvent( IGameEvent *event )
{
	if ( V_strcmp( event->GetName(), "controlpoint_initialized" ) == 0 )
	{
		InvalidateLayout( true, true );
		return;
	}

	if ( V_strcmp( event->GetName(), "controlpoint_updateimages" ) == 0 )
	{
		UpdateCPImages( false, event->GetInt( "index" ) );
		return;
	}

	// Ignore events not related to the watched team.
	if ( event->GetInt( "team" ) != m_iTeamNum )
		return;

	if ( V_strcmp( event->GetName(), "escort_progress" ) == 0 )
	{
		if ( event->GetBool( "reset" ) )
		{
			m_flProgress = 0.0f;
		}
		else
		{
			m_flProgress = event->GetFloat( "progress" );
		}
	}
	else if ( V_strcmp( event->GetName(), "escort_speed" ) == 0 )
	{
		// Get the number of cappers.
		int iNumCappers = event->GetInt( "players" );
		int iSpeedLevel = event->GetInt( "speed" );

		if ( m_pEscortItemPanel )
		{
			m_pEscortItemPanel->SetDialogVariable( "numcappers", iNumCappers );
		}

		// Show the number and icon if there any cappers present.
		bool bShowCappers = ( iNumCappers > 0 );
		if ( m_pCapNumPlayers )
		{
			m_pCapNumPlayers->SetVisible( bShowCappers );
		}
		if ( m_pCapPlayerImage )
		{
			m_pCapPlayerImage->SetVisible( bShowCappers );
		}

		// -1 cappers means the cart is blocked.
		if ( m_pBlockedImage )
		{
			m_pBlockedImage->SetVisible( iNumCappers == -1 );
		}

		// -1 speed level means the cart is receding.
		if ( m_pBackwardsImage )
		{
			// NO! CART MOVES WRONG VAY!
			m_pBackwardsImage->SetVisible( iSpeedLevel == -1 );
		}
	}
	else if ( V_strcmp( event->GetName(), "escort_recede" ) == 0 )
	{
		// Get the current recede time of the cart.
		m_flRecedeTime = event->GetFloat( "recedetime" );
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFHudEscort::OnTick( void )
{
	if ( !IsVisible() )
		return;

	if ( m_pEscortItemPanel && m_pLevelBar )
	{
		// Position the cart icon so the arrow points at its position on the track.
		int x, y, wide, tall, pos;
		m_pLevelBar->GetBounds( x, y, wide, tall );

		pos = (int)( wide * m_flProgress ) - m_pEscortItemPanel->GetWide() / 2;

		m_pEscortItemPanel->SetPos( x + pos, m_pEscortItemPanel->GetYPos() );
	}

	// Update the progress bar.
	if ( m_pProgressBar )
	{
		// Only show progress bar in Payload Race.
		if ( m_bMultipleTrains )
		{
			if ( !m_pProgressBar->IsVisible() )
			{
				m_pProgressBar->SetVisible( true );
			}

			m_pProgressBar->SetProgress( m_flProgress );
		}
		else if ( m_pProgressBar->IsVisible() )
		{
			m_pProgressBar->SetVisible( false );
		}
	}

	// Calculate time left until receding.
	float flRecedeTimeLeft = ( m_flRecedeTime != 0.0f ) ? m_flRecedeTime - gpGlobals->curtime : 0.0f;

	if ( m_pEscortItemPanel )
	{
		m_pEscortItemPanel->SetDialogVariable( "recede", (int)ceil( flRecedeTimeLeft ) );
	}

	if ( m_pRecedeTime )
	{
		// Show the timer if the cart is close to starting to recede.
		bool bShow = flRecedeTimeLeft > 0 && flRecedeTimeLeft < 20.0f;
		if ( m_pRecedeTime->IsVisible() != bShow )
		{
			m_pRecedeTime->SetVisible( bShow );
		}
	}

	// Check for alarm animation.
	bool bInAlarm = false;
	if ( ObjectiveResource() ) 
	{
		bInAlarm = ObjectiveResource()->GetTrackAlarm( m_iTeamNum );
	}

	if ( bInAlarm != m_bAlarm )
	{
		m_bAlarm = bInAlarm;
		UpdateAlarmAnimations();
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFHudEscort::UpdateCPImages( bool bUpdatePositions, int iIndex )
{
	if ( !ObjectiveResource() )
		return;

	for ( int i = 0; i < MAX_CONTROL_POINTS; i++ )
	{
		// If an index is specified only update the specified point.
		if ( iIndex != -1 && i != iIndex )
			continue;

		ImagePanel *pImage = m_pCPImages[i];
		if ( !pImage )
			continue;

		if ( bUpdatePositions )
		{
			// Check if this point exists and should be shown.
			if ( i >= ObjectiveResource()->GetNumControlPoints() ||
				ObjectiveResource()->IsInMiniRound( i ) == false ||
				ObjectiveResource()->IsCPVisible( i ) == false )
			{
				if ( pImage->IsVisible() )
					pImage->SetVisible( false );

				continue;
			}

			if ( !pImage->IsVisible() )
				pImage->SetVisible( true );

			// Get the control point position.
			float flDist = ObjectiveResource()->GetPathDistance( i );

			if ( m_pLevelBar )
			{
				int x, y, wide, tall, pos;
				m_pLevelBar->GetBounds( x, y, wide, tall );

				pos = (int)( wide * flDist ) - pImage->GetWide() / 2;

				pImage->SetPos( x + pos, pImage->GetYPos() );
			}
		}

		// Set the icon according to team.
		const char *pszImage = NULL;
		bool bOpaque = m_bMultipleTrains || m_iNumHills > 0;
		switch ( ObjectiveResource()->GetOwningTeam( i ) )
		{
		case TF_TEAM_RED:
			pszImage = bOpaque ? "../hud/cart_point_red_opaque" : "../hud/cart_point_red";
			break;
		case TF_TEAM_BLUE:
			pszImage = bOpaque ? "../hud/cart_point_blue_opaque" : "../hud/cart_point_blue";
			break;
		default:
			pszImage = bOpaque ? "../hud/cart_point_neutral_opaque" : "../hud/cart_point_neutral";
			break;
		}

		pImage->SetImage( pszImage );
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFHudEscort::UpdateAlarmAnimations( void )
{
	// Only do alert animations in Payload Race.
	if ( !m_pEscortItemImageAlert || !m_bMultipleTrains )
		return;

	if ( m_bAlarm )
	{
		if ( !m_pEscortItemImageAlert->IsVisible() )
			m_pEscortItemImageAlert->SetVisible( true );

		g_pClientMode->GetViewportAnimationController()->StartAnimationSequence( m_pEscortItemPanel, "HudCartAlarmPulse" );
	}
	else
	{
		if ( m_pEscortItemImageAlert->IsVisible() )
			m_pEscortItemImageAlert->SetVisible( false );

		g_pClientMode->GetViewportAnimationController()->StartAnimationSequence( m_pEscortItemPanel, "HudCartAlarmPulseStop" );
	}
}


//=============================================================================
// CTFHudMultipleEscort
//=============================================================================
CTFHudMultipleEscort::CTFHudMultipleEscort( Panel *pParent, const char *pszName ) : EditablePanel( pParent, pszName )
{
	m_pRedEscort = new CTFHudEscort( this, "RedEscortPanel", TF_TEAM_RED, true );

	m_pBlueEscort = new CTFHudEscort( this, "BlueEscortPanel", TF_TEAM_BLUE, true );
	m_pRedEscort->SetOnTop( false );

	ListenForGameEvent( "localplayer_changeteam" );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFHudMultipleEscort::ApplySchemeSettings( IScheme *pScheme )
{
	BaseClass::ApplySchemeSettings( pScheme );

	// Setup conditions.
	KeyValues *pConditions = new KeyValues( "conditions" );

	int iTeam = GetLocalPlayerTeam();
	AddSubKeyNamed( pConditions, iTeam == TF_TEAM_BLUE ? "if_blue_is_top" : "if_red_is_top" );

	LoadControlSettings( "resource/ui/ObjectiveStatusMultipleEscort.res", NULL, NULL, pConditions );

	pConditions->deleteThis();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFHudMultipleEscort::FireGameEvent( IGameEvent *event )
{
	if ( V_strcmp( event->GetName(), "localplayer_changeteam" ) == 0 )
	{
		// Show the cart of the local player's team on top.
		int iTeam = GetLocalPlayerTeam();
		if ( m_pRedEscort )
		{
			m_pRedEscort->SetOnTop( iTeam != TF_TEAM_BLUE );
		}
		if ( m_pBlueEscort )
		{
			m_pBlueEscort->SetOnTop( iTeam == TF_TEAM_BLUE );
		}

		// Re-arrange panels when player changes teams.
		InvalidateLayout( false, true );
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFHudMultipleEscort::SetVisible( bool bVisible )
{
	// Hide sub-panels as well.
	if ( m_pRedEscort )
	{
		m_pRedEscort->SetVisible( bVisible );
	}	
	if ( m_pBlueEscort )
	{
		m_pBlueEscort->SetVisible( bVisible );
	}

	BaseClass::SetVisible( bVisible );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CTFHudMultipleEscort::IsVisible( void )
{
	if ( IsInFreezeCam() )
		return false;

	return BaseClass::IsVisible();
}


//=============================================================================
// CEscortHillPanel
//=============================================================================
CEscortHillPanel::CEscortHillPanel( Panel *pParent, const char *pszName ) : Panel( pParent, pszName )
{
	// Load the texture.
	m_iTextureId = surface()->DrawGetTextureId( "hud/cart_track_arrow" );
	if ( m_iTextureId == -1 )
	{
		m_iTextureId = surface()->CreateNewTextureID( false );
		surface()->DrawSetTextureFile( m_iTextureId, "hud/cart_track_arrow", true, false );
	}

	m_bActive = false;
	m_bLowerAlpha = true;
	m_iWidth = 0;
	m_iHeight = 0;
	m_flScrollPerc = 0.0f;
	m_flTextureScale = 0.0f;

	m_iTeamNum = TEAM_UNASSIGNED;
	m_iHillIndex = 0;

	ivgui()->AddTickSignal( GetVPanel(), 750 );

	ListenForGameEvent( "teamplay_round_start" );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CEscortHillPanel::Paint( void )
{
	if ( ObjectiveResource() )
	{
		m_bActive = ObjectiveResource()->IsTrainOnHill( m_iTeamNum, m_iHillIndex );
	}
	else
	{
		m_bActive = false;
	}

	if ( m_bActive )
	{
		// Scroll the texture when the cart is on this hill.
		m_flScrollPerc += 0.02f;
		if ( m_flScrollPerc > 1.0f )
			m_flScrollPerc -= 1.0f;
	}

	surface()->DrawSetTexture( m_iTextureId );

	float flMod = m_flTextureScale + m_flScrollPerc;

	Vertex_t vert[4];

	vert[0].Init( Vector2D( 0.0f, 0.0f ), Vector2D( m_flScrollPerc, 0.0f ) );
	vert[1].Init( Vector2D( m_iWidth, 0.0f ), Vector2D( flMod, 0.0f ) );
	vert[2].Init( Vector2D( m_iWidth, m_iHeight ), Vector2D( flMod, 1.0f ) );
	vert[3].Init( Vector2D( 0.0f, m_iHeight ), Vector2D( m_flScrollPerc, 1.0f ) );

	surface()->DrawSetColor( COLOR_WHITE );
	surface()->DrawTexturedPolygon( 4, vert );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CEscortHillPanel::PerformLayout( void )
{
	int x, y, textureWide, textureTall;
	GetBounds( x, y, m_iWidth, m_iHeight );
	surface()->DrawGetTextureSize( m_iTextureId, textureWide, textureTall );

	m_flTextureScale = (float)m_iWidth / ( (float)textureWide * ( (float)m_iHeight / (float)textureTall ) );

	SetAlpha( 64 );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CEscortHillPanel::OnTick( void )
{
	if ( !IsVisible() )
		return;

	if ( m_bActive )
	{
		if ( m_bLowerAlpha )
		{
			// Lower alpha.
			GetAnimationController()->RunAnimationCommand( this, "alpha", 32.0f, 0.0f, 0.75f, AnimationController::INTERPOLATOR_LINEAR );
			m_bLowerAlpha = false;
		}
		else
		{
			// Rise alpha.
			GetAnimationController()->RunAnimationCommand( this, "alpha", 96.0f, 0.0f, 0.75f, AnimationController::INTERPOLATOR_LINEAR );
			m_bLowerAlpha = true;
		}
	}
	else
	{
		// Stop flashing.
		SetAlpha( 64 );
		m_bLowerAlpha = true;
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CEscortHillPanel::FireGameEvent( IGameEvent *event )
{
	if ( V_strcmp( event->GetName(), "teamplay_round_start" ) == 0 )
	{
		// Reset scrolling.
		m_flScrollPerc = 0.0f;
	}
}


//=============================================================================
// CTFHudEscortProgressBar
//=============================================================================
CTFHudEscortProgressBar::CTFHudEscortProgressBar( Panel *pParent, const char *pszName, int iTeam ) : ImagePanel( pParent, pszName )
{
	m_iTeamNum = iTeam;

	const char *pszTextureName = m_iTeamNum == TF_TEAM_RED ? "hud/cart_track_red_opaque" : "hud/cart_track_blue_opaque";

	m_iTextureId = surface()->DrawGetTextureId( pszTextureName );
	if ( m_iTextureId == -1 )
	{
		m_iTextureId = surface()->CreateNewTextureID( false );
		surface()->DrawSetTextureFile( m_iTextureId, pszTextureName, true, false );
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFHudEscortProgressBar::Paint( void )
{
	if ( m_flProgress == 0.0f )
		return;

	surface()->DrawSetTexture( m_iTextureId );

	int x, y, wide, tall;
	GetBounds( x, y, wide, tall );
	wide *= m_flProgress;

	// Draw the bar.
	Vertex_t vert[4];

	vert[0].Init( Vector2D( 0.0f, 0.0f ), Vector2D( 0.0f, 0.0f ) );
	vert[1].Init( Vector2D( wide, 0.0f ), Vector2D( 1.0f, 0.0f ) );
	vert[2].Init( Vector2D( wide, tall ), Vector2D( 1.0f, 1.0f ) );
	vert[3].Init( Vector2D( 0.0f, tall ), Vector2D( 0.0f, 1.0f ) );

	Color colBar( 255, 255, 255, 210 );
	surface()->DrawSetColor( colBar );
	surface()->DrawTexturedPolygon( 4, vert );

	// Draw a line at the end.
	Color colLine( 245, 229, 196, 210 );
	surface()->DrawSetColor( colLine );
	surface()->DrawLine( wide - 1, 0, wide - 1, tall );
}
