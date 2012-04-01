/*
 * Copyright (c) 2008 Tatiana Azundris <hacks@azundris.com>
 *
 * Licensed under the MIT license:
 * http://www.opensource.org/licenses/mit-license.php
 */
#include "ModMask.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <stdexcept>

using namespace PieDock;

/**
 * Initialize mod mask
 *
 * @param display - display
 */
ModMask::ModMask( Display *display )
{
	// In a sane world, this will map onto itself. But
	// "This is insanity: This is X!"
	const char *keyName[] = {
		"Num_Lock",
		"ISO_Level3_Shift",
		"Shift_L",
		"Shift_R",
		"Control_L",
		"Control_R",
		"Meta_L",
		"Meta_R",
		"Alt_L",
		"Alt_R",
		"Super_L",
		"Super_R",
		"Hyper_L",
		"Hyper_R" };

	static const XlatEntry xlat[] = {
		{ "Shift", ShiftMapIndex, ShiftMask },
		{ "Lock", LockMapIndex, LockMask },
		{ "Control", ControlMapIndex, ControlMask },
		{ "Mod1", Mod1MapIndex, Mod1Mask },
		{ "Mod2", Mod2MapIndex, Mod2Mask },
		{ "Mod3", Mod3MapIndex, Mod3Mask },
		{ "Mod4", Mod4MapIndex, Mod4Mask },
		{ "Mod5", Mod5MapIndex, Mod5Mask } };

	modKeyCount = sizeof( keyName )/sizeof( const char * );

	if( !(modKey = (ModKeyEntry *) malloc(
		sizeof( ModKeyEntry )*modKeyCount )) )
		throw std::runtime_error( "out of memory" );

	XModifierKeymap *xmkm = XGetModifierMapping( display );

	// first, get the name and the KeySym
	for( int key = 0; key < modKeyCount; ++key )
	{
		modKey[key].name = keyName[key];
		modKey[key].code = XKeysymToKeycode(
			display,
			XStringToKeysym( modKey[key].name ) );
		modKey[key].xlat = 0;
		modKey[key].type = 0;
	}

	if( !xmkm )
		return;

	// iterate over all modifier-types
	for( int mod = 8; mod--; )
	{
		// iterate over all keys that can create this modifier-type
		for( int key = 0; key < xmkm->max_keypermod; ++key )
		{
			// iterate over all key-names; match up KeyCode and ModBit
			for( int code = 0; code < modKeyCount; ++code )
			{
				if( !modKey[code].code )
					// key doesn't exist, try next
					continue;

				// slot in mod-map may be empty, but then we'll just get
				// no match thanks to above continue
				if( xmkm->modifiermap[mod*xmkm->max_keypermod+key] ==
					modKey[code].code )
				{
					// get mod-index and mod-mask
					// this is X, don't rely on anything. not even proper
					// order. colour me bollocks.
					for( int i = 0;
						i < sizeof( xlat )/sizeof( XlatEntry );
						++i )
						if( xlat[i].index == mod )
						{
							char *n;
							size_t l;

							modKey[code].xlat = &xlat[i];

							if( !(n = strdup( modKey[code].name )) )
								throw std::runtime_error( "out of memory" );

							modKey[code].type = n;

							if( (l = strlen( n )) > 2 )
							{
								char *p = n+l-2;

								if( !strcasecmp( p, "_L" ) ||
									!strcasecmp( p, "_R" ) )
									*p = 0;
							}
						}
				}
			}
		}
	}

	XFreeModifiermap( xmkm );
}

/**
 * Clean up
 */
ModMask::~ModMask()
{
	if( !modKeyCount ||
		!modKey )
		return;

	for( int i = modKeyCount; i--; )
		if( modKey[i].type )
			free( modKey[i].type );

	free( modKey );

	modKey = 0;
	modKeyCount = 0;
}

/**
 * Return mod mask for given key
 *
 * @param name - key name
 */
unsigned int ModMask::getModMaskFor( const char *name )
{
	if( !name ||
		!*name ||
		!modKey )
		return 0;

	for( int i = modKeyCount; i--; )
		if( modKey[i].xlat &&
			modKey[i].type &&
			(!strcmp( modKey[i].xlat->name, name) ||
				!strcmp( modKey[i].type, name)) )
		return modKey[i].xlat->mask;

	return 0;
}
