#include "cbase.h"
#include "tf_advbuttonbase.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

using namespace vgui;

DECLARE_BUILD_FACTORY_DEFAULT_TEXT( CTFAdvButtonBase, CTFAdvButtonBase );

#define ADVBUTTON_DEFAULT_IMAGE		""

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CTFAdvButtonBase::CTFAdvButtonBase( Panel *parent, const char *panelName, const char *text ) : EditablePanel( parent, panelName )
{
	Init();
}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
CTFAdvButtonBase::~CTFAdvButtonBase()
{
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFAdvButtonBase::Init()
{
	SetTall( 50 );
	SetWide( 100 );

	m_szCommand[0] = '\0';
	m_szToolTip[0] = '\0';
	m_bAutoChange = false;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFAdvButtonBase::ApplySettings( KeyValues *inResourceData )
{
	BaseClass::ApplySettings( inResourceData );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFAdvButtonBase::ApplySchemeSettings( vgui::IScheme *pScheme )
{
	BaseClass::ApplySchemeSettings( pScheme );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFAdvButtonBase::PerformLayout()
{
	BaseClass::PerformLayout();
}

void CTFAdvButtonBase::SetCommandString( const char *sCommand )
{
	Q_strncpy( m_szCommand, sCommand, sizeof( m_szCommand ) );
}

const char* CTFAdvButtonBase::GetCommandString()
{
	return m_szCommand;
}

void CTFAdvButtonBase::SetToolTip( const char *sText )
{
	V_strncpy( m_szToolTip, sText, sizeof( m_szToolTip ) );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFAdvButtonBase::OnThink()
{
	BaseClass::OnThink();
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFAdvButtonBase::SetDefaultAnimation()
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFAdvButtonBase::SendAnimation( MouseState flag )
{
	switch ( flag )
	{
	case MOUSE_DEFAULT:
		if ( m_szToolTip[0] != '\0' )
			MAINMENU_ROOT->HideToolTip();

		break;
	case MOUSE_ENTERED:
		if ( m_szToolTip[0] != '\0' )
			MAINMENU_ROOT->ShowToolTip( m_szToolTip );

		break;
	case MOUSE_EXITED:
		if ( m_szToolTip[0] != '\0' )
			MAINMENU_ROOT->HideToolTip();

		break;
	}
}

///
//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CTFButtonBase::CTFButtonBase( Panel *parent, const char *panelName, const char *text ) : Button( parent, panelName, text )
{
	m_pButtonImage = new ImagePanel( this, "SubImage" );

	iState = MOUSE_DEFAULT;
	m_bBorderVisible = true;
	_armedBorder = NULL;
	_selectedBorder = NULL;

	m_szToolTip[0] = '\0';

	m_iImageWidth = 0;

	// Set default border.
	V_strncpy( m_szDefaultBG, ADVBUTTON_DEFAULT_BG, sizeof( m_szDefaultBG ) );
	V_strncpy( m_szArmedBG, ADVBUTTON_ARMED_BG, sizeof( m_szArmedBG ) );
	V_strncpy( m_szDepressedBG, ADVBUTTON_DEPRESSED_BG, sizeof( m_szDepressedBG ) );

	REGISTER_COLOR_AS_OVERRIDABLE( m_colorImageDefault, "image_drawcolor" );
	REGISTER_COLOR_AS_OVERRIDABLE( m_colorImageArmed, "image_armedcolor" );
	REGISTER_COLOR_AS_OVERRIDABLE( m_colorImageDepressed, "image_depressedcolor" );
}

//-----------------------------------------------------------------------------
// Purpose: Set armed button border attributes.
//-----------------------------------------------------------------------------
void CTFButtonBase::SetArmedBorder( IBorder *border )
{
	_armedBorder = border;
	InvalidateLayout( false );
}

//-----------------------------------------------------------------------------
// Purpose: Set selected button border attributes.
//-----------------------------------------------------------------------------
void CTFButtonBase::SetSelectedBorder( IBorder *border )
{
	_selectedBorder = border;
	InvalidateLayout( false );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFButtonBase::ApplySchemeSettings( IScheme *pScheme )
{
	BaseClass::ApplySchemeSettings( pScheme );

	SetDefaultBorder( pScheme->GetBorder( m_szDefaultBG ) );
	SetArmedBorder( pScheme->GetBorder( m_szArmedBG ) );
	SetDepressedBorder( pScheme->GetBorder( m_szDepressedBG ) );

	// Image shouldn't interfere with the button.
	m_pButtonImage->SetMouseInputEnabled( false );
	m_pButtonImage->SetShouldScaleImage( true );

	// Set default image colors.
	m_colorImageDefault = pScheme->GetColor( ADVBUTTON_DEFAULT_COLOR, COLOR_WHITE );
	m_colorImageArmed = pScheme->GetColor( ADVBUTTON_ARMED_COLOR, COLOR_WHITE );
	m_colorImageDepressed = pScheme->GetColor( ADVBUTTON_DEPRESSED_COLOR, COLOR_WHITE );
}

void CTFButtonBase::ApplySettings( KeyValues *inResourceData )
{
	BaseClass::ApplySettings( inResourceData );

	SetPaintBackgroundEnabled( false );

	m_bBorderVisible = inResourceData->GetBool( "bordervisible", m_bBorderVisible );
	_activationType = (ActivationType_t)inResourceData->GetInt( "button_activation_type", ACTIVATE_ONPRESSEDANDRELEASED );

	V_strncpy( m_szToolTip, inResourceData->GetString( "tooltip" ), sizeof( m_szToolTip ) );

	V_strncpy( m_szDefaultBG, inResourceData->GetString( "border_default", ADVBUTTON_DEFAULT_BG ), sizeof( m_szDefaultBG ) );
	V_strncpy( m_szArmedBG, inResourceData->GetString( "border_armed", ADVBUTTON_ARMED_BG ), sizeof( m_szArmedBG ) );
	V_strncpy( m_szDepressedBG, inResourceData->GetString( "border_depressed", ADVBUTTON_DEPRESSED_BG ), sizeof( m_szDepressedBG ) );

	KeyValues *pImageKey = inResourceData->FindKey( "SubImage" );
	if ( pImageKey )
	{
		// Workaround for this not being an EditablePanel.
		if ( m_pButtonImage )
			m_pButtonImage->ApplySettings( pImageKey );

		m_iImageWidth = pImageKey->GetInt( "imagewidth", 0 );
	}

	InvalidateLayout( false, true ); // Force ApplySchemeSettings to run.
}

//-----------------------------------------------------------------------------
// Purpose: Get button border attributes.
//-----------------------------------------------------------------------------
IBorder *CTFButtonBase::GetBorder( bool depressed, bool armed, bool selected, bool keyfocus )
{
	if ( !m_bBorderVisible )
		return NULL;

	if ( depressed )
		return _depressedBorder;
	if ( armed )
		return _armedBorder;
	if ( selected )
		return _armedBorder;
		// return _selectedborder;

	return _defaultBorder;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFButtonBase::OnCursorEntered()
{
	BaseClass::OnCursorEntered();

	if ( iState != MOUSE_ENTERED )
	{
		SetMouseEnteredState( MOUSE_ENTERED );
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFButtonBase::OnCursorExited()
{
	BaseClass::OnCursorExited();

	if ( iState != MOUSE_EXITED )
	{
		SetMouseEnteredState( MOUSE_EXITED );
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFButtonBase::OnMousePressed( MouseCode code )
{
	BaseClass::OnMousePressed( code );

	if ( code == MOUSE_LEFT && iState != MOUSE_PRESSED )
	{
		SetMouseEnteredState( MOUSE_PRESSED );
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFButtonBase::OnMouseReleased( MouseCode code )
{
	BaseClass::OnMouseReleased( code );

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
void CTFButtonBase::SetMouseEnteredState( MouseState flag )
{
	iState = flag;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFButtonBase::SetArmed( bool bState )
{
	BaseClass::SetArmed( bState );

	// Show tooltip if we have one.
	if ( m_szToolTip[0] != '\0' )
	{
		if ( bState )
		{
			ShowToolTip();
		}
		else
		{
			MAINMENU_ROOT->HideToolTip();
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFButtonBase::SetSelected( bool bState )
{
	BaseClass::SetSelected( bState );

	// Sometimes SetArmed( false ) won't get called...
	if ( !bState )
	{
		SetArmed( false );
	}
}

//-----------------------------------------------------------------------------
// Purpose: For quickly setting button borders.
//-----------------------------------------------------------------------------
void CTFButtonBase::SetBordersByName( const char *pszBorderDefault, const char *pszBorderArmed, const char *pszBorderDepressed )
{
	V_strncpy( m_szDefaultBG, pszBorderDefault, sizeof( m_szDefaultBG ) );
	V_strncpy( m_szArmedBG,  pszBorderArmed, sizeof( m_szArmedBG ) );
	V_strncpy( m_szDepressedBG, pszBorderDepressed, sizeof( m_szDepressedBG ) );

	InvalidateLayout( false, true );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFButtonBase::SetFontByString( const char *sFont )
{
	IScheme *pScheme = scheme()->GetIScheme( GetScheme() );
	if ( !pScheme )
		return;

	SetFont( pScheme->GetFont( sFont, true ) );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFButtonBase::SetImage( const char *pszImage )
{
	if ( m_pButtonImage )
		m_pButtonImage->SetImage( pszImage );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFButtonBase::SetImageInset( int iInsetX, int iInsetY )
{
	if ( m_pButtonImage )
		m_pButtonImage->SetPos( iInsetX, iInsetY );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFButtonBase::SetImageSize( int iWide, int iTall )
{
	if ( m_pButtonImage )
		m_pButtonImage->SetSize( iWide, iTall );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFButtonBase::ShowToolTip( void )
{
	MAINMENU_ROOT->ShowToolTip( m_szToolTip );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFButtonBase::SetToolTip( const char *pszText )
{
	V_strncpy( m_szToolTip, pszText, sizeof( m_szToolTip ) );
}
