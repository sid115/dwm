#include <X11/XF86keysym.h>

/* appearance */
static unsigned int borderpx     = 2;        /* border pixel of windows */
static unsigned int snap         = 32;       /* snap pixel */
static const unsigned int minwsz = 20;       /* Minimal height of a client for smfact */
static int showbar               = 1;        /* 0 means no bar */
static int topbar                = 1;        /* 0 means bottom bar */
static const int splitstatus     = 1;        /* 1 for split status items */
static const char *splitdelim    = ";";      /* Character used for separating status */
static char font[]               = "Hack:size=14";
static char dmenufont[]          = "Hack:size=14";
static const char *fonts[]       = { font, "Font Awesome 5 Free:size=14:autohint=true" };
static char normbgcolor[]        = "#222222";
static char normbordercolor[]    = "#444444";
static char normfgcolor[]        = "#bbbbbb";
static char selfgcolor[]         = "#eeeeee";
static char selbordercolor[]     = "#005577";
static char selbgcolor[]         = "#005577";
static char *colors[][3] = {
       /*               fg           bg           border   */
       [SchemeNorm] = { normfgcolor, normbgcolor, normbordercolor },
       [SchemeSel]  = { selfgcolor,  selbgcolor,  selbordercolor  },
};

/* tagging */
static const char *tags[] = { "", "", "", "", "", "", "", "", "" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
    /* class                   instance               title                      tags mask  isfloating  monitor  scratch key */
	{ "Element",               "element",             "Element",                 1 << 7,    0,          -1,      0   },
	{ "Jellyfin Media Player", "jellyfinmediaplayer", "Jellyfin Media Player",   1 << 6,    0,          -1,      0   },
	{ "Spotify",               "spotify",             "Spotify",                 1 << 6,    0,          -1,      0   },
	//{ NULL,                  NULL,                  "ncmpcpp",                 1 << 6,    0,          -1,      0   },
	{ "thunderbird",           "Mail",                "Thunderbird",             1 << 5,    0,          -1,      0   },
	{ NULL,                    NULL,                  "newsboat",                1 << 3,    0,          -1,      0   },
	{ "Virt-manager",          "virt-manager",        "Virtual Machine Manager", 1 << 8,    0,          -1,      0   },
	{ NULL,                    NULL,                  "filemanager",             0,         1,          -1,      'f' },
	{ NULL,                    NULL,                  "volumemixer",             0,         1,          -1,      'v' },
};

/* layout(s) */
static float mfact        = 0.50; /* factor of master area size [0.05..0.95] */
static const float smfact = 0.00; /* factor of tiled clients [0.00..0.95] */
static int nmaster        = 1;    /* number of clients in master area */
static int resizehints    = 1;    /* 1 means respect size hints in tiled resizals */
static const int lockfullscreen = 1;    /* 1 will force focus on the fullscreen window */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },    /* first entry is default */
	{ "><>",      NULL },    /* no layout function means floating behavior */
	{ "[M]",      monocle },
};

/* key definitions */
#define ALT Mod1Mask
#define MODKEY Mod4Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *browsercmd[]        = { "librewolf", NULL };
static const char *calculatorcmd[]     = { "st", "-t", "calculator", "-e", "insect", NULL };
static const char *calendarcmd[]       = { "st", "-t", "calendar", "-e", "calcurse", NULL };
static const char *chatclientcmd[]     = { "st", "-t", "gomuks", "-e", "gomuks", NULL };
static const char *chatgptcmd[]        = { "st", "-t", "chatgpt", "-e", "chatgpt-shell-cli", NULL };
static const char *dmenucmd[]          = { "dmenu_run", NULL };
static const char *elementcmd[]        = { "element-desktop", NULL };
static const char *mailcmd[]           = { "thunderbird", NULL };
static const char *musicplayercmd[]    = { "st", "-t", "ncmpcpp", "-e", "ncmpcpp", NULL };
static const char *networkmanagercmd[] = { "networkmanager_dmenu", NULL };
static const char *officecmd[]         = { "libreoffice", NULL };
static const char *powercmd[]          = { "dmenu-power", NULL };
static const char *rssreadercmd[]      = { "st", "-t", "newsboat", "-e", "newsboat", NULL };
static const char *termcmd[]           = { "st", NULL };
static const char *upgradecmd[]        = { "st", "-t", "upgrade system", "-e", "upgrade_pkgs", NULL };
static const char *vmcmd[]             = { "virt-manager", NULL };

/*First arg only serves to match against key in rules*/
static const char *filemanagercmd[] = { "f", "st", "-t", "filemanager", "-e", "lf", NULL }; 
static const char *volumemixercmd[] = { "v", "st", "-t", "volumemixer", "-e", "pulsemixer", NULL }; 

/*
 * Xresources preferences to load at startup
 */
ResourcePref resources[] = {
	{ "font",               STRING,  &font },
	{ "dmenufont",          STRING,  &dmenufont },
	{ "normbgcolor",        STRING,  &normbgcolor },
	{ "normbordercolor",    STRING,  &normbordercolor },
	{ "normfgcolor",        STRING,  &normfgcolor },
	{ "selbgcolor",         STRING,  &selbgcolor },
	{ "selbordercolor",     STRING,  &selbordercolor },
	{ "selfgcolor",         STRING,  &selfgcolor },
	{ "borderpx",          	INTEGER, &borderpx },
	{ "snap",          		INTEGER, &snap },
	{ "showbar",          	INTEGER, &showbar },
	{ "topbar",          	INTEGER, &topbar },
	{ "nmaster",          	INTEGER, &nmaster },
	{ "resizehints",       	INTEGER, &resizehints },
	{ "mfact",      	 	FLOAT,   &mfact },
};

static const Key keys[] = {
	/* modifier                     key                       function        argument */
  	{ MODKEY,                       XK_d,                     spawn,          {.v = dmenucmd} },
  	{ MODKEY,                       XK_a,                     spawn,          {.v = volumemixercmd} },
	{ MODKEY,                       XK_Return,                spawn,          {.v = termcmd} },
	{ MODKEY,                       XK_e,	                  togglescratch,  {.v = filemanagercmd} },
	{ MODKEY|ShiftMask,             XK_e,	                  spawn,          {.v = elementcmd} },
	{ MODKEY,                       XK_b,                     spawn,          {.v = browsercmd} },
	{ MODKEY,                       XK_o,                     spawn,          {.v = officecmd} },
	{ MODKEY,                       XK_m,	                  spawn,          {.v = mailcmd} },
	{ MODKEY,                       XK_w,	                  spawn,          {.v = chatclientcmd} },
	{ MODKEY|ShiftMask,             XK_n,	                  spawn,          {.v = networkmanagercmd} },
	{ MODKEY,                       XK_c,	                  spawn,          {.v = calendarcmd} },
	{ MODKEY|ShiftMask,             XK_m,	                  spawn,          {.v = musicplayercmd} },
	{ MODKEY,                       XK_n,	                  spawn,          {.v = rssreadercmd} },
	{ MODKEY|ShiftMask,             XK_u,                     spawn,          {.v = upgradecmd} },
	{ MODKEY|ControlMask,           XK_c,                     spawn,          {.v = chatgptcmd} },
	{ MODKEY,                       XK_v,                     spawn,          {.v = vmcmd} },
	{ MODKEY|ShiftMask,             XK_j,                     spawn,          {.v = (const char*[]){ "jellyfinmediaplayer", NULL }} },
	{ MODKEY|ControlMask,           XK_r,                     spawn,          {.v = (const char*[]){ "remmina", NULL }} },
	{ MODKEY,                       XK_s,                     spawn,          {.v = (const char*[]){ "dmenu-settings", NULL }} },
	{ MODKEY|ShiftMask,             XK_s,                     spawn,          {.v = (const char*[]){ "screenshot", NULL }} },
	{ MODKEY,                       XK_r,                     spawn,          {.v = (const char*[]){ "dmenu-recording", NULL }} },
	{ MODKEY|ShiftMask,             XK_r,                     spawn,          {.v = (const char*[]){ "dmenu-recording", "kill", NULL }} },
	{ MODKEY|ShiftMask,             XK_p,                     spawn,          {.v = (const char*[]){ "dmenu-display", NULL }} },
	{ MODKEY,                       XK_p,                     spawn,          {.v = (const char*[]){ "passmenu", NULL }} },
	{ MODKEY|ControlMask,           XK_l,                     spawn,          {.v = (const char*[]){ "pic2tex", NULL }} },
	{ MODKEY,                       XK_f,                     spawn,          SHCMD("libreoffice --calc ~/doc/finances.ods") },
/* F1  */{ 0,                       XF86XK_AudioMute,         spawn,          SHCMD("amixer set Master toggle; pkill -RTMIN+10 dwmblocks") },
/* F2  */{ 0,                       XF86XK_AudioLowerVolume,  spawn,          SHCMD("amixer set Master 5%-; pkill -RTMIN+10 dwmblocks") },
/* F2  */{ ShiftMask,               XF86XK_AudioLowerVolume,  spawn,          SHCMD("amixer set Capture 1%-; pkill -RTMIN+11 dwmblocks") },
/* F3  */{ 0,                       XF86XK_AudioRaiseVolume,  spawn,          SHCMD("amixer set Master 5%+; pkill -RTMIN+10 dwmblocks") },
/* F3  */{ ShiftMask,               XF86XK_AudioRaiseVolume,  spawn,          SHCMD("amixer set Capture 1%+; pkill -RTMIN+11 dwmblocks") },
/* F4  */{ 0,                       XF86XK_AudioMicMute,      spawn,          SHCMD("amixer set Capture toggle; pkill -RTMIN+11 dwmblocks") },
/* F5  */{ 0,                       XF86XK_MonBrightnessDown, spawn,          SHCMD("light -U 5") },
/* F6  */{ 0,                       XF86XK_MonBrightnessUp,   spawn,          SHCMD("light -A 5") },
/* F12 */{ 0,                       XF86XK_Calculator,        spawn,          {.v = calculatorcmd} },
	{ 0,                            XF86XK_AudioPlay,	      spawn,          SHCMD("mpc toggle") },
	{ 0,                            XF86XK_AudioStop,	      spawn,          SHCMD("mpc stop") },
	{ 0,                            XF86XK_AudioPrev,	      spawn,          SHCMD("mpc prev") },
	{ 0,                            XF86XK_AudioNext,	      spawn,          SHCMD("mpc next") },
	{ MODKEY|ShiftMask,             XK_b,                     togglebar,      {0} },
	{ MODKEY,                       XK_j,                     focusstack,     {.i = +1 } },
	{ MODKEY,                       XK_k,                     focusstack,     {.i = -1 } },
    { MODKEY|ControlMask,           XK_j,                     pushdown,       {0} },
	{ MODKEY|ControlMask,           XK_k,                     pushup,         {0} },
    { MODKEY,                       XK_i,                     incnmaster,     {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_i,                     incnmaster,     {.i = -1 } },
	{ MODKEY,                       XK_h,                     setmfact,       {.f = -0.05} },
	{ MODKEY,                       XK_l,                     setmfact,       {.f = +0.05} },
	{ MODKEY|ShiftMask,             XK_h,                     setsmfact,      {.f = +0.05} },
	{ MODKEY|ShiftMask,             XK_l,                     setsmfact,      {.f = -0.05} },
	{ MODKEY|ShiftMask,             XK_Return,                zoom,           {0} },
	{ MODKEY,                       XK_Tab,                   view,           {0} },
	{ MODKEY|ShiftMask,             XK_c,                     killclient,     {0} },
	{ MODKEY|ALT,                   XK_t,                     setlayout,      {.v = &layouts[0]} },
	{ MODKEY|ALT,                   XK_f,                     setlayout,      {.v = &layouts[1]} },
	{ MODKEY|ALT,                   XK_m,                     setlayout,      {.v = &layouts[2]} },
	{ MODKEY|ALT,                   XK_space,                 setlayout,      {0} },
	{ MODKEY|ShiftMask,             XK_space,                 togglefloating, {0} },
    { MODKEY|ShiftMask,             XK_f,                     togglefullscr,  {0} },
	{ MODKEY,                       XK_0,                     view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_0,                     tag,            {.ui = ~0 } },
	{ MODKEY,                       XK_comma,                 focusmon,       {.i = -1 } },
	{ MODKEY,                       XK_period,                focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_comma,                 tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_period,                tagmon,         {.i = +1 } },
	TAGKEYS(                        XK_1,                     0)
	TAGKEYS(                        XK_2,                     1)
	TAGKEYS(                        XK_3,                     2)
	TAGKEYS(                        XK_4,                     3)
	TAGKEYS(                        XK_5,                     4)
	TAGKEYS(                        XK_6,                     5)
	TAGKEYS(                        XK_7,                     6)
	TAGKEYS(                        XK_8,                     7)
	TAGKEYS(                        XK_9,                     8)
   	{ MODKEY|ShiftMask,             XK_q,                     spawn,          {.v = powercmd} },
	{ MODKEY|ControlMask|ShiftMask, XK_q,                     quit,           {0} },
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static const Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button2,        spawn,          {.v = termcmd } },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};

