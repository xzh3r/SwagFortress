#include "cbase.h"
#include "tf_advslider.h"
#include <vgui/ISurface.h>
#include <vgui/IVGui.h>
#include <vgui/IInput.h>
#include <filesystem.h>
#include <vgui_controls/AnimationController.h>

using namespace vgui;

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

DECLARE_BUILD_FACTORY_DEFAULT_TEXT( CTFAdvSlider, CTFAdvButtonBase );

#define ADVSLIDER_BG		"AdvSlider"

//#define ADVSLIDER_DEFAULT_BORDER	"TFFatLineBorder"
//#define ADVSLIDER_ARMED_BORDER		"TFFatLineBorderOpaque"
//#define ADVSLIDER_DEPRESSED_BORDER	"TFFatLineBorderRedBGOpaque"
#define ADVSLIDER_DEFAULT_BORDER		"AdvRoundedButtonDefault"
#define ADVSLIDER_ARMED_BORDER		"AdvRoundedButtonArmed"
#define ADVSLIDER_DEPRESSED_BORDER	"AdvRoundedButtonDepressed"

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CTFAdvSlider::CTFAdvSlider( vgui::Panel *parent, const char *panelName, const char *text ) : CTFAdvButtonBase( parent, panelName, text )
{
	m_pButton = new CTFScrollButton( this, "SubButton", "" );
	m_pTitleLabel = new CExLabel( this, "TitleLabel", text );
	m_pValueLabel = new CExLabel( this, "ValueLabel", "0" );
	m_pBGBorder = new EditablePanel( this, "BackgroundPanel" );

	Init();
}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
CTFAdvSlider::~CTFAdvSlider()
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFAdvSlider::Init()
{
	BaseClass::Init();

	m_flMinValue = 0.0;
	m_flMaxValue = 100.0;
	m_flLabelWidth = 0.0;
	m_flValue = -1.0;
	m_bVertical = false;
	m_bValueVisible = true;
	m_bShowFrac = false;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFAdvSlider::ApplySettings( KeyValues *inResourceData )
{
	BaseClass::ApplySettings( inResourceData );

	m_bVertical = inResourceData->GetBool( "vertical", false );
	m_bValueVisible = inResourceData->GetBool( "value_visible", true );
	m_flMinValue = inResourceData->GetFloat( "minvalue", 0.0 );
	m_flMaxValue = inResourceData->GetFloat( "maxvalue", 100.0 );
	m_flLabelWidth = inResourceData->GetFloat( "labelWidth", 0.0 );
	if ( m_flValue == -1.0 )
		m_flValue = m_flMinValue;

	InvalidateLayout( false, true ); // force ApplySchemeSettings to run
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFAdvSlider::ApplySchemeSettings( IScheme *pScheme )
{
	BaseClass::ApplySchemeSettings( pScheme );

	m_pTitleLabel->SetFgColor( pScheme->GetColor( ADVBUTTON_DEFAULT_COLOR, Color( 255, 255, 255, 255 ) ) );
	m_pTitleLabel->SetFont( m_pButton->GetFont() );
	m_pValueLabel->SetFgColor( pScheme->GetColor( ADVBUTTON_DEFAULT_COLOR, Color( 255, 255, 255, 255 ) ) );
	m_pValueLabel->SetFont( m_pButton->GetFont() );

	m_pButton->SetZPos( 3 );

	m_pBGBorder->SetBorder( pScheme->GetBorder( ADVSLIDER_BG ) );
	m_pBGBorder->SetVisible( true );
	m_pBGBorder->SetZPos( 1 );

	m_pTitleLabel->SetVisible( true );
	m_pTitleLabel->SetPos( 0, 0 );
	m_pTitleLabel->SetTextInset( 5, 0 );
	m_pTitleLabel->SetZPos( 3 );
	m_pTitleLabel->SetContentAlignment( Label::a_west );

	m_pValueLabel->SetVisible( m_bValueVisible );
	m_pValueLabel->SetZPos( 3 );
	m_pValueLabel->SetContentAlignment( Label::a_center );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFAdvSlider::PerformLayout()
{
	BaseClass::PerformLayout();

	float fBorder = ( m_flLabelWidth > 0.0 ? m_flLabelWidth : GetWide() / 2.0 + YRES( 8 ) );
	float fShift = YRES( 20 );

	SetBorder( NULL );

	m_pButton->SetPos( fBorder, 0 );
	if ( !m_bVertical )
		m_pButton->SetSize( YRES( 10 ), GetTall() );  //scroll wide
	else
		m_pButton->SetSize( GetPanelWide(), YRES( 10 ) );  //scroll wide	

	m_pBGBorder->SetPos( fBorder, 0 );
	m_pBGBorder->SetWide( GetWide() - fBorder - fShift );
	m_pBGBorder->SetTall( GetTall() );

	m_pTitleLabel->SetWide( fBorder );
	m_pTitleLabel->SetTall( GetTall() );

	m_pValueLabel->SetPos( GetWide() - fShift, 0 );
	m_pValueLabel->SetWide( fShift );
	m_pValueLabel->SetTall( GetTall() );


	UpdateValue();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFAdvSlider::SetFont( HFont font )
{
	// Set the font on all elements.
	m_pButton->SetFont( font );
	m_pTitleLabel->SetFont( font );
	m_pValueLabel->SetFont( font );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFAdvSlider::OnThink()
{
	BaseClass::OnThink();
	if ( m_pButton->GetState() == MOUSE_PRESSED && IsEnabled() )
	{
		SetPercentage();
	}
}

void CTFAdvSlider::UpdateValue()
{
	if ( GetCommandString()[0] != '\0' )
	{
		ConVarRef CheckButtonCommand( GetCommandString() );
		float fValue = CheckButtonCommand.GetFloat();
		SetValue( fValue );
	}
}

void CTFAdvSlider::RunCommand()
{
	PostActionSignal( new KeyValues( "ControlModified" ) );

	if ( GetCommandString()[0] == '\0' )
	{
		GetParent()->OnCommand( "scrolled" );
	}
	else if ( IsAutoChange() )
	{
		char szCommand[MAX_PATH];
		Q_snprintf( szCommand, sizeof( szCommand ), "%s %s", GetCommandString(), GetFinalValue() );
		engine->ExecuteClientCmd( szCommand );
	}
}


float CTFAdvSlider::GetValue()
{
	return m_flValue;
}

const char *CTFAdvSlider::GetFinalValue()
{
	static char szValue[32];

	if ( m_bShowFrac || m_flMaxValue <= 1.0f )
	{
		// Only show fractional part if it's 0 to 1 slider.
		V_snprintf( szValue, sizeof( szValue ), "%.02f", m_flValue );
	}
	else
	{
		V_snprintf( szValue, sizeof( szValue ), "%.f", m_flValue );
	}

	return szValue;
}

int CTFAdvSlider::GetScrollValue()
{
	return GetPercentage() * ( m_flMaxValue - m_flMinValue ) + m_flMinValue;
}

float CTFAdvSlider::GetPercentage()
{
	int _x = 0, _y = 0;
	int scroll_x = 0, scroll_y = 0;
	int mx = m_pBGBorder->GetWide() - m_pButton->GetWide();  //max local xpos
	m_pBGBorder->GetPos( _x, _y );
	m_pButton->GetPos( scroll_x, scroll_y ); //scroll local pos

	//Msg("Percentage: %f%%\n", pers * 100.0);
	float pers;
	if ( !m_bVertical )
		pers = (float)( scroll_x - _x ) / (float)mx;
	else
		pers = (float)( scroll_y - _y ) / (float)mx;

	return pers;
}

void CTFAdvSlider::SetValue( float fVal )
{
	m_flValue = clamp( fVal, m_flMinValue, m_flMaxValue );

	float fPerc = RemapValClamped( fVal, m_flMinValue, m_flMaxValue, 0.0f, 1.0f );
	SetPercentage( fPerc );
}

void CTFAdvSlider::SetPercentage()
{
	int _x = 0, _y = 0;
	int x = 0, y = 0;
	int ix = 0, iy = 0;
	int mx = m_pBGBorder->GetWide() - m_pButton->GetWide();  //max local xpos
	int my = m_pBGBorder->GetTall() - m_pButton->GetTall();  //max local xpos
	m_pBGBorder->GetPos( _x, _y );
	surface()->SurfaceGetCursorPos( x, y ); //cursor global pos
	GetParent()->ScreenToLocal( x, y );//cursor global to local
	GetPos( ix, iy ); //control global pos

	float fPerc;
	if ( !m_bVertical )
		fPerc = (float)( x - ix - _x - m_pButton->GetWide() / 2 ) / (float)mx;
	else
		fPerc = (float)( y - iy - _y - m_pButton->GetTall() / 2 ) / (float)my;

	m_flValue = RemapValClamped( fPerc, 0.0f, 1.0f, m_flMinValue, m_flMaxValue );

	SetPercentage( fPerc );
}

void CTFAdvSlider::SetPercentage( float fPerc )
{
	fPerc = clamp( fPerc, 0.0f, 1.0f );

	int _x = 0, _y = 0;
	int scroll_x = 0, scroll_y = 0;
	int	mx = m_pBGBorder->GetWide() - m_pButton->GetWide();  //max local xpos
	int my = m_pBGBorder->GetTall() - m_pButton->GetTall();  //max local xpos
	m_pBGBorder->GetPos( _x, _y );
	m_pButton->GetPos( scroll_x, scroll_y ); //scroll local pos

	float fPos;
	if ( !m_bVertical )
		fPos = fPerc * (float)mx + (float)_x;
	else
		fPos = fPerc * (float)my + (float)_y;

	m_pValueLabel->SetText( GetFinalValue() );

	if ( !m_bVertical )
	{
		AnimationController::PublicValue_t p_AnimHover( fPos, scroll_y );
		vgui::GetAnimationController()->RunAnimationCommand( m_pButton, "Position", p_AnimHover, 0.0f, 0.05f, vgui::AnimationController::INTERPOLATOR_LINEAR, NULL );
	}
	else
	{
		AnimationController::PublicValue_t p_AnimHover( scroll_x, fPos );
		vgui::GetAnimationController()->RunAnimationCommand( m_pButton, "Position", p_AnimHover, 0.0f, 0.05f, vgui::AnimationController::INTERPOLATOR_LINEAR, NULL );
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFAdvSlider::SetDefaultAnimation()
{
	BaseClass::SetDefaultAnimation();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFAdvSlider::SendAnimation( MouseState flag )
{
	BaseClass::SendAnimation( flag );
}

///
//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CTFScrollButton::CTFScrollButton( vgui::Panel *parent, const char *panelName, const char *text ) : CTFButtonBase( parent, panelName, text )
{
	m_pParent = dynamic_cast<CTFAdvSlider *>( parent );
	iState = MOUSE_DEFAULT;
	vgui::ivgui()->AddTickSignal( GetVPanel() );
}


void CTFScrollButton::ApplySettings( KeyValues *inResourceData )
{
	BaseClass::ApplySettings( inResourceData );
}
//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFScrollButton::ApplySchemeSettings( IScheme *pScheme )
{
	BaseClass::ApplySchemeSettings( pScheme );

	SetDefaultColor( pScheme->GetColor( ADVBUTTON_DEFAULT_COLOR, COLOR_WHITE ), Color( 0, 0, 0, 0 ) );
	SetArmedColor( pScheme->GetColor( ADVBUTTON_ARMED_COLOR, COLOR_WHITE ), Color( 0, 0, 0, 0 ) );
	SetDepressedColor( pScheme->GetColor( ADVBUTTON_DEPRESSED_COLOR, COLOR_WHITE ), Color( 0, 0, 0, 0 ) );
	SetSelectedColor( pScheme->GetColor( ADVBUTTON_DEPRESSED_COLOR, COLOR_WHITE ), Color( 0, 0, 0, 0 ) );

	SetDefaultBorder( pScheme->GetBorder( ADVSLIDER_DEFAULT_BORDER ) );
	SetArmedBorder( pScheme->GetBorder( ADVSLIDER_ARMED_BORDER ) );
	SetDepressedBorder( pScheme->GetBorder( ADVSLIDER_DEPRESSED_BORDER ) );
	SetSelectedBorder( pScheme->GetBorder( ADVSLIDER_DEPRESSED_BORDER ) );

	SetArmedSound( "ui/buttonrollover.wav" );
	SetDepressedSound( "ui/buttonclick.wav" );
	SetReleasedSound( "ui/buttonclickrelease.wav" );
}

void CTFScrollButton::PerformLayout()
{
	BaseClass::PerformLayout();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFScrollButton::OnCursorEntered()
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
void CTFScrollButton::OnCursorExited()
{
	Button::OnCursorExited();

	if ( iState != MOUSE_EXITED && iState != MOUSE_PRESSED )
	{
		SetMouseEnteredState( MOUSE_EXITED );
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFScrollButton::OnMousePressed( vgui::MouseCode code )
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
void CTFScrollButton::OnMouseReleased( vgui::MouseCode code )
{
	Button::OnMouseReleased( code );

	if ( m_pParent )
	{
		m_pParent->RunCommand();
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
void CTFScrollButton::SetMouseEnteredState( MouseState flag )
{
	BaseClass::SetMouseEnteredState( flag );

	if ( m_pParent->IsEnabled() )
		m_pParent->SendAnimation( flag );
}
