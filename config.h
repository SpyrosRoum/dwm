

#include "movestack.c"

/* appearance */
static const unsigned int borderpx  = 2;        /* border pixel of windows */
static const unsigned int snap      = 32;       /* snap pixel */
static const int swallowfloating    = 0;        /* 1 means swallow floating windows by default */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 1;        /* 0 means bottom bar */
static const char *fonts[]          = { "monospace:size=12", "JoyPixels:pixelsize=12:antialias=true:autohint=true"  };
static const char dmenufont[]       = "ubuntu-mono-nerd:size=12";

// static const char col_gray1[]       = "#282828"; // "#222222"
// static const char col_gray2[]       = "#49483e"; // "#444444"
// static const char col_gray3[]       = "#a1efe4"; // "#bbbbbb"
// static const char col_gray4[]       = "#383830"; // "#eeeeee"
// static const char col_cyan[]        = "#a6e22e"; // "#005577"
static char normbgcolor[] = "#222222";
static char normbordercolor[] = "#444444";
static char normfgcolor[] = "#bbbbbb";
static char selfgcolor[] = "#eeeeee";
static char selbordercolor[] = "#9558B2";
static char selbgcolor[] = "#9558B2";
static const char *colors[][3] = {
	/*               fg         bg         border   */
	[SchemeNorm] = {normfgcolor, normbgcolor, normbordercolor},
	[SchemeSel] = {selfgcolor, selbgcolor, selbordercolor},
};

/* tagging */
static const char *tags[] = { "1:Web", "2:Chat", "3:Dev", "4:Game", "5", "6", "7", "8:Music", "9" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class             instance    title       tags mask     isfloating   isterminal noswallow monitor */
	{ "Spotify",         "spotify",  NULL,       1 << 7,       0,           0,         -1,         0 },
	{ "Steam",           NULL,       NULL,       1 << 3,       0,           0,         -1,        -1 },
	{ "qBittorrent",     NULL,       NULL,       1 << 8,       0,           0,         -1,         0 },
	{ "discord",         NULL,       NULL,       1 << 1,       0,           0,         -1,         1 },
	{ "firefox",         NULL,       NULL,       1,            0,           0,         -1,        -1 },
    { "jetbrains-clion", NULL,       NULL,       1 << 2,       0,           0,         -1,         0 },
    { "jetbrains-pycharm", NULL,     NULL,       1 << 2,       0,           0,         -1,         0 },
    { "Gnome-todo",      NULL,       NULL,       1 << 6,       0,           0,         -1,         0 },
    { NULL,              NULL,       "Event Tester",0,         1,           0,          1,        -1 }, /* xev */
    { "Dwarf_Fortress",  NULL,       NULL,       0,            0,           0,          1,        -1 }, /* xev */
    { "Alacritty",       NULL,       NULL,       0,            0,           1,         -1,        -1 },
    { "GitKraken",       NULL,       NULL,       1 << 3,       0,           0,         -1,        -1 },
};

/* layout(s) */
static const float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 1;    /* 1 means respect size hints in tiled resizals */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },    // first entry is default
	{ "><>",      NULL },    // no layout function means floating behavior
	{ "[M]",      monocle },
	{ NULL,       NULL}      // For the cycle layout patch
};

/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define CMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "dmenu_run", "-m", dmenumon, "-fn", dmenufont, NULL };
static const char *termcmd[]  = { "alacritty", NULL };

static Key keys[] = {
	/* modifier                     key        function        argument */
	// Apps are launched using sxhkd

	// Change focus
	{ MODKEY,                       XK_j,      focusstack,     {.i = +1 } },
	{ MODKEY,                       XK_k,      focusstack,     {.i = -1 } },

	// Move in the stack
	{ MODKEY|ShiftMask,             XK_j,      movestack,      {.i = +1}  },
	{ MODKEY|ShiftMask,             XK_k,      movestack,      {.i = -1}  },
	{ MODKEY|ShiftMask,             XK_Return, zoom,           {0}  },

	// Resize
	{ MODKEY,                       XK_comma,  setmfact,       {.f = -0.05} },
	{ MODKEY,                       XK_period, setmfact,       {.f = +0.05} },

	// Kill current
	{ MODKEY,                       XK_q,      killclient,     {0} },

	// Toggle floating
	{ MODKEY,                       XK_space,  setlayout,      {0} },
	{ MODKEY|ShiftMask,             XK_space,  togglefloating, {0} },

	// Cycle layout
	{ MODKEY,                       XK_Tab,    cyclelayout,    {.i = +1} },

	{ MODKEY,                       XK_0,      view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_0,      tag,            {.ui = ~0 } },

	// Focus next/prev screen
	{ MODKEY,                       XK_z,  focusmon,           {.i = -1 } },
	{ MODKEY,                       XK_h,  focusmon,           {.i = -1 } },
	{ MODKEY,                       XK_Left, focusmon,         {.i = -1 } },
	{ MODKEY,                       XK_c,  focusmon,           {.i = +1 } },
	{ MODKEY,                       XK_l,  focusmon,           {.i = +1 } },
	{ MODKEY,                       XK_Right, focusmon,        {.i = +1 } },

	// Move to next/prev screen
	{ MODKEY|ShiftMask,             XK_z,  tagmon,             {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_h,  tagmon,             {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_c,  tagmon,             {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_l,  tagmon,             {.i = +1 } },

	// Quit
	{ MODKEY|ShiftMask,             XK_e,      quit,           {0} },

	// Restart
	{ MODKEY|ShiftMask,             XK_r,      quit,           {1} },

	// Focus to tag
	TAGKEYS(                        XK_1,                      0)
	TAGKEYS(                        XK_2,                      1)
	TAGKEYS(                        XK_3,                      2)
	TAGKEYS(                        XK_4,                      3)
	TAGKEYS(                        XK_5,                      4)
	TAGKEYS(                        XK_6,                      5)
	TAGKEYS(                        XK_7,                      6)
	TAGKEYS(                        XK_8,                      7)
	TAGKEYS(                        XK_9,                      8)
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button3,        spawn,          CMD("rofi -show p -modi p:~/.local/bin/rofi-power-menu -width 20 -lines 2") },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};


// Set the useless gap
static const unsigned int gappx = 4;
