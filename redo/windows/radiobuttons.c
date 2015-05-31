// 20 may 2015
#include "uipriv_windows.h"

struct radiobuttons {
	uiRadioButtons r;
	struct ptrArray *hwnds;
	uiControl *parent;
};

// TODO properly test parent changes (add an Add Item button to the test program)

uiDefineControlType(uiRadioButtons, uiTypeRadioButtons, struct radiobuttons)

static BOOL onWM_COMMAND(uiControl *c, HWND clicked, WORD code, LRESULT *lResult)
{
	struct radiobuttons *r = (struct radiobuttons *) c;
	WPARAM check;
	uintmax_t i;
	HWND hwnd;

	if (code != BN_CLICKED)
		return FALSE;
	for (i = 0; i < r->hwnds->len; i++) {
		hwnd = ptrArrayIndex(r->hwnds, HWND, i);
		check = BST_UNCHECKED;
		if (clicked == hwnd)
			check = BST_CHECKED;
		SendMessage(hwnd, BM_SETCHECK, check, 0);
	}
	*lResult = 0;
	return TRUE;
}

static void radiobuttonsCommitDestroy(uiControl *c)
{
	// TODO
}

// radio buttons have more than one handle
// if we allow deletion, the handles are not permanent
static uintptr_t radiobuttonsHandle(uiControl *c)
{
	return 0;
}

static void radiobuttonsCommitSetParent(uiControl *c, uiControl *parent)
{
	struct radiobuttons *r = (struct radiobuttons *) c;
	HWND hwnd;
	uintmax_t i;

	r->parent = parent;
	for (i = 0; i < r->hwnds->len; i++) {
		hwnd = ptrArrayIndex(r->hwnds, HWND, i);
		uiWindowsUtilSetParent(hwnd, r->parent);
	}
}

// from http://msdn.microsoft.com/en-us/library/windows/desktop/dn742486.aspx#sizingandspacing
#define radiobuttonHeight 10
// from http://msdn.microsoft.com/en-us/library/windows/desktop/bb226818%28v=vs.85%29.aspx
#define radiobuttonXFromLeftOfBoxToLeftOfLabel 12

// TODO vertical space between controls

static void radiobuttonsPreferredSize(uiControl *c, uiSizing *d, intmax_t *width, intmax_t *height)
{
	struct radiobuttons *r = (struct radiobuttons *) c;
	uintmax_t i;
	intmax_t wid, maxwid;

	maxwid = 0;
	for (i = 0; i < r->hwnds->len; i++) {
		wid = uiWindowsWindowTextWidth(ptrArrayIndex(r->hwnds, HWND, i));
		if (maxwid < wid)
			maxwid = wid;
	}
	*width = uiWindowsDlgUnitsToX(radiobuttonXFromLeftOfBoxToLeftOfLabel, d->Sys->BaseX) + maxwid;
	*height = uiWindowsDlgUnitsToY(radiobuttonHeight, d->Sys->BaseY) * r->hwnds->len;
}

// TODO clip to height
static void radiobuttonsResize(uiControl *c, intmax_t x, intmax_t y, intmax_t width, intmax_t height, uiSizing *d)
{
	struct radiobuttons *r = (struct radiobuttons *) c;
	intmax_t height1;
	uintmax_t i;
	HWND hwnd;

	height1 = uiWindowsDlgUnitsToY(radiobuttonHeight, d->Sys->BaseY);
	for (i = 0; i < r->hwnds->len; i++) {
		hwnd = ptrArrayIndex(r->hwnds, HWND, i);
		moveWindow(hwnd, x, y, width, height1, d);
		y += height1;
	}
}

static uiSizing *radiobuttonsSizing(uiControl *c)
{
	// TODO
	return NULL;
}

static void radiobuttonsSHED(uiControl *c)
{
	// TODO
}

static uintptr_t radiobuttonsStartZOrder(uiControl *c)
{
	// TODO
	return 0;
}

static uintptr_t radiobuttonsSetZOrder(uiControl *c, uintptr_t insertAfter)
{
	// TODO
	return 0;
}

static int radiobuttonsHasTabStops(uiControl *c)
{
	// TODO return 0 if no radio buttons
	return 1;
}

static void radiobuttonsAppend(uiRadioButtons *rr, const char *text)
{
	struct radiobuttons *r = (struct radiobuttons *) rr;
	HWND hwnd;
	WCHAR *wtext;

	wtext = toUTF16(text);
	hwnd = uiWindowsUtilCreateControlHWND(0,
		L"button", wtext,
		BS_RADIOBUTTON | WS_TABSTOP | WS_CHILD | WS_VISIBLE,
		hInstance, NULL,
		TRUE);
	uiWindowsUtilSetParent(hwnd, r->parent);
	uiWindowsRegisterWM_COMMANDHandler(hwnd, onWM_COMMAND, uiControl(r));
	ptrArrayAppend(r->hwnds, hwnd);
	uiControlQueueResize(uiControl(r));
}

uiRadioButtons *uiNewRadioButtons(void)
{
	struct radiobuttons *r;

	r = (struct radiobuttons *) uiNewControl(uiTypeRadioButtons());

	r->hwnds = newPtrArray();

	uiControl(r)->CommitDestroy = radiobuttonsCommitDestroy;
	uiControl(r)->Handle = radiobuttonsHandle;
	uiControl(r)->CommitSetParent = radiobuttonsCommitSetParent;
	uiControl(r)->PreferredSize = radiobuttonsPreferredSize;
	uiControl(r)->Resize = radiobuttonsResize;
	uiControl(r)->Sizing = radiobuttonsSizing;
	uiControl(r)->CommitShow = radiobuttonsSHED;
	uiControl(r)->CommitHide = radiobuttonsSHED;
	uiControl(r)->CommitEnable = radiobuttonsSHED;
	uiControl(r)->CommitDisable = radiobuttonsSHED;
	uiControl(r)->StartZOrder = radiobuttonsStartZOrder;
	uiControl(r)->SetZOrder = radiobuttonsSetZOrder;
	uiControl(r)->HasTabStops = radiobuttonsHasTabStops;

	uiRadioButtons(r)->Append = radiobuttonsAppend;

	return uiRadioButtons(r);
}
