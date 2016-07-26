#include "cbase.h"
#include "tf_advbutton.h"
#include "vgui_controls/Frame.h"
#include <vgui/ISurface.h>
#include <vgui/IVGui.h>
#include <vgui/IInput.h>
#include "vgui_controls/Button.h"
#include "vgui_controls/ImagePanel.h"
#include "tf_controls.h"
#include <filesystem.h>
#include <vgui_controls/AnimationController.h>
#include "basemodelpanel.h"
#include "panels/tf_dialogpanelbase.h"
#include "inputsystem/iinputsystem.h"

using namespace vgui;

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

DECLARE_BUILD_FACTORY_DEFAULT_TEXT( CTFAdvButton, CTFAdvButtonBase );

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CTFAdvButton::CTFAdvButton( vgui::Panel *parent, const char *panelName, const char *text ) : CTFAdvButtonBase( parent, panelName, text )
{
	m_pButton = new CTFButton( this, "SubButton", text );

	Init();
}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
CTFAdvButton::~CTFAdvButton()
{
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFAdvButton::Init()
{
	BaseClass::Init();
	m_bGlowing = false;
	m_flActionThink = -1;
	m_flAnimationThink = -1;
	m_bAnimationIn = true;
}

void CTFAdvButton::ApplySettings( KeyValues *inResourceData )
{
	BaseClass::ApplySettings( inResourceData );

	InvalidateLayout( false, true ); // force ApplySchemeSettings to run
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFAdvButton::ApplySchemeSettings( IScheme *pScheme )
{
	BaseClass::ApplySchemeSettings( pScheme );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFAdvButton::PerformLayout()
{
	BaseClass::PerformLayout();

	m_pButton->SetWide( GetWide() );
	m_pButton->SetTall( GetTall() );

	CTFDialogPanelBase *pParent = dynamic_cast<CTFDialogPanelBase *>( GetParent() );

	if ( pParent )
	{
		char sText[64];
		m_pButton->GetText( sText, sizeof( sText ) );
		if ( Q_strcmp( sText, "" ) )
		{
			char * pch;
			pch = strchr( sText, '&' );
			if ( pch != NULL )
			{
				int id = pch - sText + 1;
				//pch = strchr(pch + 1, '&');
				char* cTest = &sText[id];
				cTest[1] = '\0';
				pParent->AddShortcut( cTest, GetCommandString() );
			}
		}
	}
}

void CTFAdvButton::SetText( const char *tokenName )
{
	m_pButton->SetText( tokenName );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFAdvButton::OnThink()
{
	BaseClass::OnThink();

	if ( m_bGlowing && m_flAnimationThink < gpGlobals->curtime )
	{
		float m_fAlpha = ( m_bAnimationIn ? 50.0f : 255.0f );
		float m_fDelay = ( m_bAnimationIn ? 0.75f : 0.0f );
		float m_fDuration = ( m_bAnimationIn ? 0.15f : 0.25f );
		vgui::GetAnimationController()->RunAnimationCommand( this, "Alpha", m_fAlpha, m_fDelay, m_fDuration, vgui::AnimationController::INTERPOLATOR_LINEAR );
		m_bAnimationIn = !m_bAnimationIn;
		m_flAnimationThink = gpGlobals->curtime + 1.0f;
	}
}

void CTFAdvButton::SetGlowing( bool Glowing )
{
	m_bGlowing = Glowing;

	if ( !m_bGlowing )
	{
		float m_fAlpha = 255.0f;
		float m_fDelay = 0.0f;
		float m_fDuration = 0.0f;
		vgui::GetAnimationController()->RunAnimationCommand( this, "Alpha", m_fAlpha, m_fDelay, m_fDuration, vgui::AnimationController::INTERPOLATOR_LINEAR );
	}
};

void CTFAdvButton::SetSelected( bool bState )
{
	SendAnimation( MOUSE_DEFAULT );

	m_pButton->SetSelected( bState );
};

void CTFAdvButton::SetEnabled( bool bState )
{
	m_pButton->SetEnabled( bState );

	BaseClass::SetEnabled( bState );
}

void CTFAdvButton::SetBorderByString( const char *sBorderDefault, const char *sBorderArmed, const char *sBorderDepressed )
{
	IScheme *pScheme = scheme()->GetIScheme( GetScheme() );
	if ( !pScheme )
		return;

	if ( m_pButton )
	{
		m_pButton->SetDefaultBorder( pScheme->GetBorder( sBorderDefault ) );
		m_pButton->SetArmedBorder( pScheme->GetBorder( sBorderArmed ) );
		m_pButton->SetDepressedBorder( pScheme->GetBorder( sBorderDepressed ) );
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFAdvButton::SetDefaultAnimation()
{
	BaseClass::SetDefaultAnimation();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFAdvButton::SendAnimation( MouseState flag )
{
	BaseClass::SendAnimation( flag );

	if ( m_pButton->IsSelected() )
		return;

	bool bAnimation = ( ( m_pButton->m_fXShift == 0 && m_pButton->m_fYShift == 0 ) ? false : true );
	AnimationController::PublicValue_t p_AnimLeave( 0, 0 );
	AnimationController::PublicValue_t p_AnimHover( m_pButton->m_fXShift, m_pButton->m_fYShift );

	switch ( flag )
	{
		//We can add additional stuff like animation here
	case MOUSE_ENTERED:
		if ( bAnimation )
			vgui::GetAnimationController()->RunAnimationCommand( m_pButton, "Position", p_AnimHover, 0.0f, 0.1f, vgui::AnimationController::INTERPOLATOR_LINEAR, NULL );

		break;
	case MOUSE_EXITED:
		if ( bAnimation )
			vgui::GetAnimationController()->RunAnimationCommand( m_pButton, "Position", p_AnimLeave, 0.0f, 0.1f, vgui::AnimationController::INTERPOLATOR_LINEAR, NULL );

		break;
	}
}

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CTFButton::CTFButton( vgui::Panel *parent, const char *panelName, const char *text ) : CTFButtonBase( parent, panelName, text )
{
	m_pParent = dynamic_cast<CTFAdvButton *>( parent );
	iState = MOUSE_DEFAULT;
	m_fXShift = 0.0;
	m_fYShift = 0.0;
}

void CTFButton::ApplySettings( KeyValues *inResourceData )
{
	m_fXShift = inResourceData->GetFloat( "xshift", 0.0 );
	m_fYShift = inResourceData->GetFloat( "yshift", 0.0 );

	BaseClass::ApplySettings( inResourceData );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFButton::ApplySchemeSettings( IScheme *pScheme )
{
	BaseClass::ApplySchemeSettings( pScheme );

	SetDefaultColor( pScheme->GetColor( ADVBUTTON_DEFAULT_COLOR, Color( 255, 255, 255, 255 ) ), Color( 0, 0, 0, 0 ) );
	SetArmedColor( pScheme->GetColor( ADVBUTTON_ARMED_COLOR, Color( 255, 255, 255, 255 ) ), Color( 0, 0, 0, 0 ) );
	SetDepressedColor( pScheme->GetColor( ADVBUTTON_DEPRESSED_COLOR, Color( 255, 255, 255, 255 ) ), Color( 0, 0, 0, 0 ) );
	SetSelectedColor( pScheme->GetColor( ADVBUTTON_DEPRESSED_COLOR, Color( 255, 255, 255, 255 ) ), Color( 0, 0, 0, 0 ) );

	m_pButtonImage->SetDrawColor( pScheme->GetColor( m_szImageColorDefault, COLOR_WHITE ) );

	SetArmedSound( "ui/buttonrollover.wav" );
	SetDepressedSound( "ui/buttonclick.wav" );
	SetReleasedSound( "ui/buttonclickrelease.wav" );
}

void CTFButton::PerformLayout()
{
	BaseClass::PerformLayout();
	
	if ( m_iImageWidth != 0 )
	{
		int iWidth = YRES( m_iImageWidth );
		int iHeight = iWidth;
		int iShift = ( GetTall() - iWidth ) / 2;
		int x = iShift * 2 + iWidth;

		SetTextInset( x, 0 );

		m_pButtonImage->SetPos( iShift, iShift );
		m_pButtonImage->SetWide( iWidth );
		m_pButtonImage->SetTall( iHeight );
	}

	IScheme *pScheme = scheme()->GetIScheme( GetScheme() );
	if ( !pScheme )
		return;

	if ( m_pButtonImage )
	{
		// Set image color based on our state.
		if ( _buttonFlags.IsFlagSet( DEPRESSED ) )
		{
			m_pButtonImage->SetDrawColor( pScheme->GetColor( m_szImageColorDepressed, COLOR_WHITE ) );
		}
		else if ( _buttonFlags.IsFlagSet( ARMED ) )
		{
			m_pButtonImage->SetDrawColor( pScheme->GetColor( m_szImageColorArmed, COLOR_WHITE ) );
		}
		else if ( _buttonFlags.IsFlagSet( SELECTED ) )
		{
			m_pButtonImage->SetDrawColor( pScheme->GetColor( m_szImageColorDepressed, COLOR_WHITE ) );
		}
		else
		{
			m_pButtonImage->SetDrawColor( pScheme->GetColor( m_szImageColorDefault, COLOR_WHITE ) );
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFButton::OnCursorEntered()
{
	Button::OnCursorEntered();

	if ( iState != MOUSE_ENTERED && iState != MOUSE_PRESSED )
	{
		SetMouseEnteredState( MOUSE_ENTERED );
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFButton::OnCursorExited()
{
	Button::OnCursorExited();

	if ( iState != MOUSE_EXITED )
	{
		SetMouseEnteredState( MOUSE_EXITED );
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFButton::OnMousePressed( MouseCode code )
{
	Button::OnMousePressed( code );

	if ( code == MOUSE_LEFT && iState != MOUSE_PRESSED )
	{
		SetMouseEnteredState( MOUSE_PRESSED );
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFButton::OnMouseReleased( MouseCode code )
{
	Button::OnMouseReleased( code );

	if ( code == MOUSE_LEFT && ( iState == MOUSE_ENTERED || iState == MOUSE_PRESSED ) )
	{
		m_pParent->GetParent()->OnCommand( m_pParent->GetCommandString() );
		m_pParent->PostActionSignal( new KeyValues( "ButtonPressed" ) );
	}
	if ( code == MOUSE_LEFT && iState == MOUSE_ENTERED )
	{
		SetMouseEnteredState( MOUSE_ENTERED );
	}
	else
	{
		SetMouseEnteredState( MOUSE_EXITED );
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFButton::SetMouseEnteredState( MouseState flag )
{
	BaseClass::SetMouseEnteredState( flag );

	if ( m_pParent->IsEnabled() )
		m_pParent->SendAnimation( flag );
}