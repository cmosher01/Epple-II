/*
    epple2
    Copyright (C) 2008 by Chris Mosher <chris@mosher.mine.nu>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY, without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/
#include "clipboardhandler.h"

#include <SDL/SDL.h>
#include <SDL/SDL_syswm.h>
#include <limits.h>

// taken from: http://www.libsdl.org/projects/scrap (original author: Sam Lantinga)


/* Determine what type of clipboard we are using */
#if defined(__WIN32__) || defined(__CYGWIN__)
#define WIN_SCRAP
#elif defined(__unix__)
#define X11_SCRAP
#else
#error Unknown window manager for clipboard handling
#endif /* scrap type */



/* System dependent variables */
#if defined(X11_SCRAP)
static Display *SDL_Display;
static Window SDL_Window;
static void (*Lock_Display)();
static void (*Unlock_Display)();
#elif defined(WIN_SCRAP)
static HWND SDL_Window;
#endif /* scrap type */
static bool initialized(false);
static bool have(false);


#ifdef X11_SCRAP
static int clipboard_filter(const SDL_Event *event)
{
  /* Post all non-window manager specific events */
  if ( event->type != SDL_SYSWMEVENT ) {
    return(1);
  }

  /* Handle window-manager specific clipboard events */
  switch (event->syswm.msg->event.xevent.type) {
    /* Copy the selection from XA_CUT_BUFFER0 to the requested property */
    case SelectionRequest: {
      XSelectionRequestEvent *req;
      XEvent sevent;
      int seln_format;
      unsigned long nbytes;
      unsigned long overflow;
      unsigned char *seln_data;

      req = &event->syswm.msg->event.xevent.xselectionrequest;
      sevent.xselection.type = SelectionNotify;
      sevent.xselection.display = req->display;
      sevent.xselection.selection = req->selection;
      sevent.xselection.target = None;
      sevent.xselection.property = None;
      sevent.xselection.requestor = req->requestor;
      sevent.xselection.time = req->time;
      if ( XGetWindowProperty(SDL_Display, DefaultRootWindow(SDL_Display),
                              XA_CUT_BUFFER0, 0, INT_MAX/4, False, req->target,
                              &sevent.xselection.target, &seln_format,
                              &nbytes, &overflow, &seln_data) == Success )
        {
          if ( sevent.xselection.target == req->target )
            {
              if ( sevent.xselection.target == XA_STRING )
                {
                  if ( seln_data[nbytes-1] == '\0' )
                    --nbytes;
                }
              XChangeProperty(SDL_Display, req->requestor, req->property,
                sevent.xselection.target, seln_format, PropModeReplace,
                                                      seln_data, nbytes);
              sevent.xselection.property = req->property;
            }
          XFree(seln_data);
        }
      XSendEvent(SDL_Display,req->requestor,False,0,&sevent);
      XSync(SDL_Display, False);
    }
    break;
  }

  /* Post the event for X11 clipboard reading above */
  return(1);
}
#endif /* X11_SCRAP */




ClipboardHandler::ClipboardHandler()
{
	if (initialized)
		return;
	initialized = true;

	SDL_SetError("SDL is not running on known window manager");

	/* Grab the window manager specific information */
	SDL_SysWMinfo info;
	SDL_VERSION(&info.version);
	if (SDL_GetWMInfo(&info))
	{
		/* Save the information for later use */
#if defined(X11_SCRAP)
		if (info.subsystem == SDL_SYSWM_X11)
		{
			SDL_Display = info.info.x11.display;
			SDL_Window = info.info.x11.window;
			Lock_Display = info.info.x11.lock_func;
			Unlock_Display = info.info.x11.unlock_func;

			/* Enable the special window hook events */
			SDL_EventState(SDL_SYSWMEVENT, SDL_ENABLE);
			SDL_SetEventFilter(clipboard_filter);

			have = true;
		}
		else
		{
			SDL_SetError("SDL is not running on X11");
		}
#elif defined(WIN_SCRAP)
		SDL_Window = info.window;
		have = true;
#endif /* scrap type */
	}
}


ClipboardHandler::~ClipboardHandler()
{
}


std::string ClipboardHandler::getText()
{
	std::string ret;
	if (!have)
		return ret;

#if defined(X11_SCRAP)
	const Atom format(XA_STRING);

	Atom selection;
	Lock_Display();
	Window owner = XGetSelectionOwner(SDL_Display, XA_PRIMARY);
	Unlock_Display();
	if (owner == None || owner == SDL_Window)
	{
		owner = DefaultRootWindow(SDL_Display);
		selection = XA_CUT_BUFFER0;
	}
	else
	{
		int selection_response = 0;
		SDL_Event event;
	
		owner = SDL_Window;
		Lock_Display();
		selection = XInternAtom(SDL_Display, "SDL_SELECTION", False);
		XConvertSelection(SDL_Display, XA_PRIMARY, format, selection, owner, CurrentTime);
		Unlock_Display();
		while (!selection_response)
		{
			SDL_WaitEvent(&event);
			if (event.type == SDL_SYSWMEVENT)
			{
				XEvent xevent = event.syswm.msg->event.xevent;
		
				if (xevent.type == SelectionNotify && xevent.xselection.requestor == owner)
					selection_response = 1;
			}
		}
	}



	Lock_Display();
	Atom seln_type;
	int seln_format;
	unsigned long nbytes;
	unsigned long overflow;
	char *src;
	if (XGetWindowProperty(SDL_Display, owner, selection, 0, INT_MAX/4, False, format, &seln_type, &seln_format, &nbytes, &overflow, (unsigned char **)&src) == Success)
	{
		if (seln_type == format)
		{
			ret.assign(src,nbytes);
		}
		XFree(src);
	}
	Unlock_Display();
#elif defined(WIN_SCRAP)
	const UINT format(CF_TEXT);
	if (IsClipboardFormatAvailable(format) && OpenClipboard(SDL_Window))
	{
		const HANDLE hMem = GetClipboardData(format);
		if (hMem)
		{
			char *src = (char*)GlobalLock(hMem);
			ret.assign(src);
			GlobalUnlock(hMem);
		}
		CloseClipboard();
	}
#endif /* scrap type */

	return ret;
}
