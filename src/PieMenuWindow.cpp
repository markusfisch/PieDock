/*
 *   O         ,-
 *  ° o    . -´  '     ,-
 *   °  .´        ` . ´,´
 *     ( °   ))     . (
 *      `-;_    . -´ `.`.
 *          `._'       ´
 *
 * Copyright (c) 2007-2011 Markus Fisch <mf@markusfisch.de>
 *
 * Licensed under the MIT license:
 * http://www.opensource.org/licenses/mit-license.php
 */
#include "PieMenuWindow.h"
#include "WindowManager.h"
#include "Hotspot.h"

using namespace PieDock;

/**
 * Initialize this window
 *
 * @param a - Application object
 */
PieMenuWindow::PieMenuWindow( Application &a ) :
	TransparentWindow( a ),
	menu( &a, *canvas ),
	text( 0 ),
	textCanvas( 0 )
{
	XSelectInput(
		app->getDisplay(),
		window,
		LeaveWindowMask |
			ButtonReleaseMask |
			ButtonPressMask |
			KeyPressMask |
			KeyReleaseMask );

	WindowManager::setWindowType(
		app->getDisplay(),
		window,
		"_NET_WM_WINDOW_TYPE_DOCK" );
}

/**
 * Clean up
 */
PieMenuWindow::~PieMenuWindow()
{
	if( text )
	{
		XFreePixmap( app->getDisplay(), textCanvas );
		delete text;
	}

	for( CartoucheMap::iterator i = cartoucheMap.begin();
		i != cartoucheMap.end();
		i++ )
		delete i->second;
}

/**
 * Make window appear, returns false if there are no menu items
 *
 * @param n - menu name (optional)
 * @param p - where the menu should appear (optional)
 */
bool PieMenuWindow::appear( std::string n,
	PieMenuWindow::Placement p )
{
	if( !menu.update( n ) )
		return false;

	if( p == PieMenuWindow::IconBelowCursor )
		menu.setTwistForSelection();

	show( p );

	return true;
}

/**
 * (Re-)Draw contents
 */
void PieMenuWindow::draw()
{
	Hotspot p( app->getDisplay(), window );

	if( !menu.isObsolete( p.getX(), p.getY() ) )
		return;

	clear();
	menu.draw( p.getX(), p.getY() );

	if( !app->getSettings()->getShowTitle() )
		update();
	else
		updateWithText();
}

/**
 * Process event, this method returns true as long as it wants to keep
 * processing events
 *
 * @param event - X event
 */
bool PieMenuWindow::processEvent( XEvent &event )
{
	switch( event.type )
	{
		case LeaveNotify:
			if( event.xany.window == window &&
				!app->getSettings()->isFitts() )
			{
				hide();
				return false;
			}
			break;
		case ButtonRelease:
			{
				Settings::ButtonFunctions *bf =
					&app->getSettings()->getButtonFunctions();

				for( Settings::ButtonFunctions::iterator i = bf->begin();
					i != bf->end();
					i++ )
					if( (*i).button == event.xbutton.button )
						return performAction( (*i).action );
			}
			break;
		case KeyPress:
		case KeyRelease:
			{
				Settings::KeyFunctions *kf =
					&app->getSettings()->getKeyFunctions();

				for( Settings::KeyFunctions::iterator i = kf->begin();
					i != kf->end();
					i++ )
					if( event.type == (*i).eventType &&
						XKeysymToKeycode( event.xkey.display, (*i).keySym ) ==
							event.xkey.keycode )
						return performAction( (*i).action );
			}
			break;
	}

	return true;
}

/**
 * Perform action
 *
 * @param action - action to perform
 */
bool PieMenuWindow::performAction( Settings::Action action )
{
	switch( action )
	{
		case Settings::Launch:
		case Settings::ShowNext:
		case Settings::ShowPrevious:
		case Settings::Hide:
		case Settings::Close:
			if( menu.cursorInCenter() )
				switch( app->getSettings()->getCenterAction() )
				{
					default:
					case Settings::CenterNearestIcon:
						break;
					case Settings::CenterIgnore:
						return true;
					case Settings::CenterDisappear:
						return performAction( Settings::Disappear );
				}
			if( menu.change( action ) )
				break;
			hide();
			menu.execute( action );
			return false;
		case Settings::SpinUp:
			menu.turn( app->getSettings()->getSpinStep() );
			break;
		case Settings::SpinDown:
			menu.turn( -app->getSettings()->getSpinStep() );
			break;
		case Settings::SpinNext:
			menu.turn( 1 );
			break;
		case Settings::SpinPrevious:
			menu.turn( -1 );
			break;
		case Settings::Disappear:
			hide();
			return false;
		default:
			menu.execute( action );
			break;
	}

	return true;
}

/**
 * Show window
 *
 * @param p - where to show (optional)
 */
void PieMenuWindow::show( PieMenuWindow::Placement p )
{
	if( app->getSettings()->isFitts() ||
		app->getSettings()->getKeyFunctions().size() )
	{
		XGrabKeyboard(
			app->getDisplay(),
			DefaultRootWindow( app->getDisplay() ),
			true,
			GrabModeAsync,
			GrabModeAsync,
			CurrentTime );

		XGrabPointer(
			app->getDisplay(),
			DefaultRootWindow( app->getDisplay() ),
			true,
			ButtonPressMask | ButtonReleaseMask,
			GrabModeAsync,
			GrabModeAsync,
			None,
			None,
			CurrentTime );
	}

	place( p );
	TransparentWindow::show();
}

/**
 * Hide window
 */
void PieMenuWindow::hide()
{
	if( app->getSettings()->isFitts() ||
		app->getSettings()->getKeyFunctions().size() )
	{
		XUngrabPointer( app->getDisplay(), CurrentTime );
		XUngrabKeyboard( app->getDisplay(), CurrentTime );
	}

	TransparentWindow::hide();
}

/**
 * Update window having text
 */
void PieMenuWindow::updateWithText()
{
	std::string title = menu.getItemTitle();

	if( title.empty() )
	{
		update();
		return;
	}

	if( !textCanvas )
	{
		// since Xft requires a Drawable, there needs to be this
		// detour through a Pixmap
		if(	!(textCanvas = XCreatePixmap(
				app->getDisplay(),
				window,
				width,
				height,
				canvas->getResource()->depth )) ||
			!(text = new Text(
				app,
				textCanvas,
				canvas->getVisual(),
				app->getSettings()->getTitleFont() )) )
			throw "out of memory";
	}

	// shorten title string since it fits inside the circle
	Text::Metrics m;

	for( int w = (width>>1)-40; title.length(); )
	{
		Text::Metrics p = text->getMetrics( title );

		if( p.getWidth() < w )
		{
			m = p;
			break;
		}

		title.erase( --title.end() );
	}

	int r = app->getSettings()->getCartoucheSettings().cornerRadius;

	// text should have always the same height to avoid flickering
	{
		Text::Metrics p = text->getMetrics( "Ag" );
		m.setY( p.getY()+(r>>1) );
		m.setHeight( p.getHeight() );
	}

	// draw cartouche
	{
		CartoucheMap::const_iterator i;
		Cartouche *c;

		if( (i = cartoucheMap.find( m.getWidth() )) == cartoucheMap.end() )
		{
			int rr = (r<<1)+r;

			cartoucheMap[m.getWidth()] = c = new Cartouche(
				m.getWidth()+rr,
				m.getHeight()+rr,
				r,
				app->getSettings()->getCartoucheSettings().color );
		}
		else
			c = i->second;

		menu.getBlender()->blend(
			*c,
			((width-c->getWidth())>>1),
			((height-c->getHeight())>>1),
			app->getSettings()->getCartoucheSettings().alpha );
	}

	// XftDrawString/XDrawString requires a Drawable (Window or Pixmap),
	// but update() deals only with XImage, hence this detour over the
	// textCanvas-Pixmap
	XPutImage(
		app->getDisplay(),
		textCanvas,
		gc,
		canvas->getResource(),
		0,
		0,
		0,
		0,
		width,
		height );

	text->draw(
		((width-m.getWidth())>>1)+m.getX(),
		((height-m.getHeight())>>1)+m.getY(),
		title );

	XCopyArea(
		app->getDisplay(),
		textCanvas,
		window,
		gc,
		0,
		0,
		width,
		height,
		0,
		0 );

#ifdef HAVE_XRENDER
	if( app->getSettings()->useCompositing() )
	{
		XCopyArea(
			app->getDisplay(),
			textCanvas,
			alphaPixmap,
			gc,
			0,
			0,
			width,
			height,
			0,
			0 );

		composite();
	}
#endif
}

/**
 * Place window on Screen
 *
 * @param placement - where to place window
 */
void PieMenuWindow::place( PieMenuWindow::Placement placement )
{
	int screen = DefaultScreen( app->getDisplay() );
	int desktopWidth = DisplayWidth( app->getDisplay(), screen );
	int desktopHeight = DisplayHeight( app->getDisplay(), screen );
	Hotspot p( app->getDisplay() );
	int x = p.getX();
	int y = p.getY();

	menu.setWindowBelowCursor( p.getChild() );

	x -= width>>1;
	y -= height>>1;

	if( placement == IconBelowCursor )
		x -= menu.getRadius();

	if( x < 0 )
		x = 0;
	else if( x+width >= desktopWidth )
		x = desktopWidth-width;

	if( y < 0 )
		y = 0;
	else if( y+height >= desktopHeight )
		y = desktopHeight-height;

	XMoveWindow( app->getDisplay(), window, x, y );
}
