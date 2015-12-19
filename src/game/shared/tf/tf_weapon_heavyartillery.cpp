//====== Copyright � 1996-2005, Valve Corporation, All rights reserved. =======
//
// Purpose: A lightweight minigun variant for use in DM
//
//=============================================================================

#include "cbase.h"
#ifdef GAME_DLL
#include "tf_player.h"
#else
#include "c_tf_player.h"
#endif
#include "tf_weapon_heavyartillery.h"

IMPLEMENT_NETWORKCLASS_ALIASED( TFHeavyArtillery, DT_TFHeavyArtillery )
															
BEGIN_NETWORK_TABLE( CTFHeavyArtillery, DT_TFHeavyArtillery )			
END_NETWORK_TABLE()										

BEGIN_PREDICTION_DATA( CTFHeavyArtillery )				
END_PREDICTION_DATA()									
														
LINK_ENTITY_TO_CLASS( tf_weapon_heavyartillery, CTFHeavyArtillery );		
PRECACHE_WEAPON_REGISTER( tf_weapon_heavyartillery );

//-----------------------------------------------------------------------------
// Purpose: Constructor.
//-----------------------------------------------------------------------------
CTFHeavyArtillery::CTFHeavyArtillery()
{
	//WeaponReset();
}

//-----------------------------------------------------------------------------
// Purpose: Destructor.
//-----------------------------------------------------------------------------
CTFHeavyArtillery::~CTFHeavyArtillery()
{
	//WeaponReset();
}

void CTFHeavyArtillery::PrimaryAttack( void )
{
	CTFPlayer *pPlayer = ToTFPlayer( GetOwner() );

	if ( !pPlayer )
		return;

	BaseClass::PrimaryAttack();

	float maxVerticalKickAngle = m_pWeaponInfo->GetWeaponData(TF_WEAPON_PRIMARY_MODE).m_flPunchAngle;
	float slideLimitTime = 7.f;

	static float lastAttack, baseAccuracy, nextAccuracy;
	
	// Lazy init. Kinda.
	if (lastAttack < 0.1f)
		lastAttack = gpGlobals->curtime;
	
	// If this is a new attack, apply base inaccuracy degradation from previous shot if necessary.
	if (lastAttack < gpGlobals->curtime - 0.1f)
		baseAccuracy = max(0.f, nextAccuracy - (gpGlobals->curtime - lastAttack) / slideLimitTime / nextAccuracy);
	
	// Find how far into our accuracy degradation we are
	float kickPerc = nextAccuracy = min(1.f, m_fFireDuration / slideLimitTime + baseAccuracy);
	lastAttack = gpGlobals->curtime;
	
	// do this to get a hard discontinuity
	pPlayer->ViewPunchReset();

	//Apply this to the view angles as well
	QAngle vecScratch;
	vecScratch.x = -( RandomFloat(0.f, 1.f) + ( maxVerticalKickAngle * kickPerc ) * RandomFloat(0.5f, 1.f) );
	vecScratch.y = -( RandomFloat(0.f, 1.f) + ( maxVerticalKickAngle * kickPerc ) * RandomFloat(0.5f, 1.f) );
	vecScratch.z = RandomFloat(0.f, 2.f) + (maxVerticalKickAngle * kickPerc);

	//Wibble left and right
	if ( random->RandomInt( 0, 1 ) != 0 )
		vecScratch.y *= -1.f;
	
	//Wobble up and down
	if ( random->RandomInt( 0, 1 ) != 0 )
		vecScratch.z *= -1.f;

	if ( random->RandomInt( 0, 1 ) != 0 )
		vecScratch.x *= -1.f;
	
	vecScratch.x -= kickPerc * 20.f;

	//Clip this to our desired min/max
	ClipPunchAngleOffset( vecScratch, pPlayer->m_Local.m_vecPunchAngle, QAngle( 64.0f, 64.0f, 1.0f ) );

	//Add it to the view punch
	// NOTE: 0.5 is just tuned to match the old effect before the punch became simulated
	pPlayer->ViewPunch( vecScratch * 0.5 );
}

void CTFHeavyArtillery::ClipPunchAngleOffset( QAngle &in, const QAngle &punch, const QAngle &clip )
{
	QAngle	final = in + punch;

	//Clip each component
	for ( int i = 0; i < 3; i++ )
	{
		if ( final[i] > clip[i] )
		{
			final[i] = clip[i];
		}
		else if ( final[i] < -clip[i] )
		{
			final[i] = -clip[i];
		}

		//Return the result
		in[i] = final[i] - punch[i];
	}
}
