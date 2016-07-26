#include "cbase.h"
#include "tf_advitembutton.h"

using namespace vgui;

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

DECLARE_BUILD_FACTORY_DEFAULT_TEXT( CTFAdvItemButton, CTFAdvItemButtonBase );

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CTFAdvItemButton::CTFAdvItemButton( Panel *parent, const char *panelName, const char *text ) : CTFButton( parent, panelName, text )
{
	Init();
}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
CTFAdvItemButton::~CTFAdvItemButton()
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFAdvItemButton::Init()
{
	m_pItemDefinition = NULL;
	m_iLoadoutSlot = TF_LOADOUT_SLOT_PRIMARY;
}

void CTFAdvItemButton::ApplySchemeSettings( IScheme *pScheme )
{
	BaseClass::ApplySchemeSettings( pScheme );

	// Don't want to darken weapon images.
	m_colorImageDefault = COLOR_WHITE;
	m_colorImageArmed = COLOR_WHITE;

	SetContentAlignment( Label::a_south );
	SetTextInset( 0, -10 );
}

void CTFAdvItemButton::PerformLayout()
{
	BaseClass::PerformLayout();

	int inset = YRES( 45 );
	int wide = GetWide() - inset;

	SetImageSize( wide, wide );
	SetImageInset( inset / 2, -1 * wide / 5 );
}

// ---------------------------------------------------------------------------- -
// Purpose: 
//-----------------------------------------------------------------------------
void CTFAdvItemButton::ShowToolTip( void )
{
	// Using a custom tooltip.
	MAINMENU_ROOT->ShowItemToolTip( m_pItemDefinition );
}

void CTFAdvItemButton::SetItemDefinition( CEconItemDefinition *pItemData )
{
	m_pItemDefinition = pItemData;

	char szIcon[128];
	Q_snprintf( szIcon, sizeof( szIcon ), "../%s_large", pItemData->image_inventory );
	SetImage( szIcon );

	SetText( pItemData->GenerateLocalizedFullItemName() );

	SetDepressedSound( pItemData->mouse_pressed_sound );
	SetReleasedSound( NULL );
}

void CTFAdvItemButton::SetLoadoutSlot( int iSlot, int iPreset )
{
	m_iLoadoutSlot = iSlot;

	char szCommand[64];
	Q_snprintf( szCommand, sizeof( szCommand ), "loadout %d %d", iSlot, iPreset );
	SetCommand( szCommand );
}
