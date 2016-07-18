//=============================================================================
//
// Purpose: Payload HUD
//
//=============================================================================
#include "cbase.h"
#include "tf_hud_escort.h"
#include "tf_hud_freezepanel.h"

using namespace vgui;

CTFHudEscort::CTFHudEscort( Panel *pParent, const char *pszName ) : EditablePanel( pParent, pszName )
{
	m_pEscortItemPanel = NULL;
	m_pLevelBar = NULL;
	m_pRecedeTime = NULL;
	m_pCapPlayerImage = NULL;
	m_pBackwardsImage = NULL;
	m_pBlockedImage = NULL;

	m_flProgress = -1.0f;
	m_flRecedeTime = 0.0f;

	ivgui()->AddTickSignal( GetVPanel() );

	ListenForGameEvent( "escort_progress" );
	ListenForGameEvent( "escort_speed" );
	ListenForGameEvent( "escort_recede" );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFHudEscort::ApplySchemeSettings( IScheme *pScheme )
{
	BaseClass::ApplySchemeSettings( pScheme );

	LoadControlSettings( "Resource/UI/ObjectiveStatusEscort.res" );

	m_pLevelBar = dynamic_cast<ImagePanel *>( FindChildByName( "LevelBar" ) );
	m_pEscortItemPanel = dynamic_cast<EditablePanel *>( FindChildByName( "EscortItemPanel" ) );

	if ( m_pEscortItemPanel )
	{
		m_pCapNumPlayers = dynamic_cast<CExLabel *>( m_pEscortItemPanel->FindChildByName( "CapNumPlayers" ) );
		m_pRecedeTime = dynamic_cast<CExLabel *>( m_pEscortItemPanel->FindChildByName( "RecedeTime" ) );
		m_pCapPlayerImage = dynamic_cast<ImagePanel *>( m_pEscortItemPanel->FindChildByName( "CapPlayerImage" ) );
		m_pBackwardsImage = dynamic_cast<ImagePanel *>( m_pEscortItemPanel->FindChildByName( "Speed_Backwards" ) );
		m_pBlockedImage = dynamic_cast<ImagePanel *>( m_pEscortItemPanel->FindChildByName( "Blocked" ) );
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CTFHudEscort::IsVisible( void )
{
	if ( IsTakingAFreezecamScreenshot() )
		return false;

	return BaseClass::IsVisible();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFHudEscort::FireGameEvent( IGameEvent *event )
{
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
	if ( m_pEscortItemPanel && m_pLevelBar )
	{
		// Position the cart icon so the arrow points at its position on the track.
		int x, y, wide, tall, pos;
		m_pLevelBar->GetBounds( x, y, wide, tall );

		pos = Lerp( m_flProgress, x, x + wide ) - m_pEscortItemPanel->GetWide() / 2;

		m_pEscortItemPanel->SetPos( pos, m_pEscortItemPanel->GetYPos() );
	}

	float flRecedeTimeLeft = ( m_flRecedeTime != 0.0f ) ? m_flRecedeTime - gpGlobals->curtime : 0.0f;

	if ( m_pEscortItemPanel )
	{
		m_pEscortItemPanel->SetDialogVariable( "recede", (int)ceil( flRecedeTimeLeft ) );
	}

	// Show the timer if the cart is close to starting to recede.
	if ( m_pRecedeTime )
	{
		bool bShow = flRecedeTimeLeft > 0 && flRecedeTimeLeft < 30.0f;
		if ( m_pRecedeTime->IsVisible() != bShow )
		{
			m_pRecedeTime->SetVisible( bShow );
		}
	}
}
