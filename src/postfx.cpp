#include "skygfx.h"
#include "ModuleList.hpp"
#include <stdarg.h>

RwIm2DVertex *colorfilterVerts = (RwIm2DVertex*)0xC400D8;
RwImVertexIndex *colorfilterIndices = (RwImVertexIndex*)0x8D5174;

Imf &CPostEffects::ms_imf = *(Imf*)0xC40150;

WRAPPER void CPostEffects::DarknessFilter(uint8 alpha) { EAXJMP(0x702F00); }
WRAPPER void CPostEffects::Grain(int strengh, bool generate) { EAXJMP(0x7037C0); }
WRAPPER void CPostEffects::SpeedFX(float) { EAXJMP(0x7030A0); }
RwRaster *&CPostEffects::pRasterFrontBuffer = *(RwRaster**)0xC402D8;
float &CPostEffects::m_fInfraredVisionFilterRadius = *(float*)0x8D50B8;
RwRaster *&CPostEffects::m_pGrainRaster = *(RwRaster**)0xC402B0;
WRAPPER void CPostEffects::InfraredVision(RwRGBA color1, RwRGBA color2) { EAXJMP(0x703F80); }
WRAPPER void CPostEffects::ImmediateModeRenderStatesStore(void) { EAXJMP(0x700CC0); }
WRAPPER void CPostEffects::ImmediateModeRenderStatesSet(void) { EAXJMP(0x700D70); }
WRAPPER void CPostEffects::ImmediateModeRenderStatesReStore(void) { EAXJMP(0x700E00); }
WRAPPER void CPostEffects::SetFilterMainColour(RwRaster *raster, RwRGBA color) { EAXJMP(0x703520); }
WRAPPER void CPostEffects::DrawQuad(float x1, float y1, float x2, float y2, uchar r, uchar g, uchar b, uchar alpha, RwRaster *ras) { EAXJMP(0x700EC0); }
WRAPPER void CPostEffects::NightVision(RwRGBA color) { EAXJMP(0x7011C0); }
WRAPPER void CPostEffects::ColourFilter(RwRGBA rgb1, RwRGBA rgb2) { EAXJMP(0x703650); }
float &CPostEffects::m_fNightVisionSwitchOnFXCount = *(float*)0xC40300;
int &CPostEffects::m_InfraredVisionGrainStrength = *(int*)0x8D50B4;
int &CPostEffects::m_NightVisionGrainStrength = *(int*)0x8D50A8;
bool &CPostEffects::m_bInfraredVision = *(bool*)0xC402B9;

bool &CPostEffects::m_bDisableAllPostEffect = *(bool*)0xC402CF;

bool &CPostEffects::m_bColorEnable = *(bool*)0x8D518C;
int &CPostEffects::m_colourLeftUOffset = *(int*)0x8D5150;
int &CPostEffects::m_colourRightUOffset = *(int*)0x8D5154;
int &CPostEffects::m_colourTopVOffset = *(int*)0x8D5158;
int &CPostEffects::m_colourBottomVOffset = *(int*)0x8D515C;
float &CPostEffects::m_colour1Multiplier = *(float*)0x8D5160;
float &CPostEffects::m_colour2Multiplier = *(float*)0x8D5164;
float &CPostEffects::SCREEN_EXTRA_MULT_CHANGE_RATE = *(float*)0x8D5168;
float &CPostEffects::SCREEN_EXTRA_MULT_BASE_CAP = *(float*)0x8D516C;
float &CPostEffects::SCREEN_EXTRA_MULT_BASE_MULT = *(float*)0x8D5170;

bool &CPostEffects::m_bRadiosity = *(bool*)0xC402CC;
bool &CPostEffects::m_bRadiosityDebug = *(bool*)0xC402CD;
int &CPostEffects::m_RadiosityFilterPasses = *(int*)0x8D510C;
int &CPostEffects::m_RadiosityRenderPasses = *(int*)0x8D5110;
int &CPostEffects::m_RadiosityIntensityLimit = *(int*)0x8D5114;
int &CPostEffects::m_RadiosityIntensity = *(int*)0x8D5118;
bool &CPostEffects::m_bRadiosityBypassTimeCycleIntensityLimit = *(bool*)0xC402CE;
int &CPostEffects::m_RadiosityFilterUCorrection = *(int*)0x8D511C;
int &CPostEffects::m_RadiosityFilterVCorrection = *(int*)0x8D5120;

bool &CPostEffects::m_bDarknessFilter = *(bool*)0xC402C4;
int &CPostEffects::m_DarknessFilterAlpha = *(int*)0x8D5204;
int &CPostEffects::m_DarknessFilterAlphaDefault = *(int*)0x8D50F4;
int &CPostEffects::m_DarknessFilterRadiosityIntensityLimit = *(int*)0x8D50F8;

bool &CPostEffects::m_bCCTV = *(bool*)0xC402C5;
bool &CPostEffects::m_bFog = *(bool*)0xC402C6;
bool &CPostEffects::m_bNightVision = *(bool*)0xC402B8;
bool &CPostEffects::m_bHeatHazeFX = *(bool*)0xC402BA;
bool &CPostEffects::m_bHeatHazeMaskModeTest = *(bool*)0xC402BB;
bool &CPostEffects::m_bGrainEnable = *(bool*)0xC402B4;
bool &CPostEffects::m_waterEnable = *(bool*)0xC402D3;

bool &CPostEffects::m_bSpeedFX = *(bool*)0x8D5100;
bool &CPostEffects::m_bSpeedFXTestMode = *(bool*)0xC402C7;
uint8 &CPostEffects::m_SpeedFXAlpha = *(uint8*)0x8D5104;

/* My own */
bool CPostEffects::m_bBlurColourFilter = true;
bool CPostEffects::m_bYCbCrFilter = false;
float CPostEffects::m_lumaScale = 219.0f/255.0f;
float CPostEffects::m_lumaOffset = 16.0f/255.0f;
float CPostEffects::m_cbScale = 1.23f;
float CPostEffects::m_cbOffset = 0.0f;
float CPostEffects::m_crScale = 1.23f;
float CPostEffects::m_crOffset = 0.0f;



/////
///// Im2D overrides
/////


int overrideColorMod = -1;
int overrideAlphaMod = -1;
void *overrideIm2dPixelShader;

void Im2DColorModulationHook(RwUInt32 stage, RwUInt32 type, RwUInt32 value)
{
	if(overrideColorMod >= 0)
		RwD3D9SetTextureStageState(stage, type, overrideColorMod);
	else
		RwD3D9SetTextureStageState(stage, type, value);
}
void Im2DAlphaModulationHook(RwUInt32 stage, RwUInt32 type, RwUInt32 value)
{
	if(overrideAlphaMod >= 0)
		RwD3D9SetTextureStageState(stage, type, overrideAlphaMod);
	else
		RwD3D9SetTextureStageState(stage, type, value);
}

void
Im2dSetPixelShader_hook(void*)
{
	RwD3D9SetPixelShader(overrideIm2dPixelShader);
}


/////
/////
/////

// Credits: much of the code in this file was originally written by NTAuthority
// there's not a lot of that left now

void *iiiTrailsPS, *vcTrailsPS;
RwRaster *grainRaster;


// Mobile stuff
struct Grade
{
	float r, g, b, a;
};
void *gradingPS, *contrastPS;
#define NUMHOURS 8
#define NUMWEATHERS 23
#define EXTRASTART 21

struct GradeColorset
{
	Grade red;
	Grade green;
	Grade blue;

	GradeColorset(void) {}
	GradeColorset(int h, int w);
	void Interpolate(GradeColorset *a, GradeColorset *b, float fa, float fb);
};


struct Colorcycle
{
	static bool initialised;
	static Grade redGrade[24][NUMWEATHERS];
	static Grade greenGrade[24][NUMWEATHERS];
	static Grade blueGrade[24][NUMWEATHERS];

	static void Initialise(void);
	static void Update(GradeColorset *colorset);
};




// v9.30k: declared here (moved from the log section below) so that
// UpdateFrontBuffer's diagnostic can use it - its first use is above that
// section and MSVC rejects use-before-declaration (the v9.30j2 lesson)
static FILE *sfxLog;
// v9.34a: sfxProbeFrame lives up here - UpdateFrontBuffer gates its
// per-frame fill cache on it (C2065 in CI build a3254a3: it used to
// be declared ~1600 lines below this first use)
static int sfxProbeFrame;			// heartbeat, ++ per DFE call
// v9.69 probe: order stamps for the filter-source investigation.
// UpdateFrontBuffer counts which copy path ran (live readback /
// bb fallback / work-raster fallback) and Radiosity_VCS counts its
// calls; sfxProbeCFCMP prints the deltas so content divergence can
// be attributed to the writer that ran last before the filter.
static unsigned int sfxUfLive, sfxUfBack, sfxUfWork, sfxRadCalls;
static int sfxUfBackLog = 10, sfxUfWorkLog = 20;

// ---- v9.32: live front buffer for the hdr path ----------------------
// The 931-run log proved cam==bb:1 - the camera raster IS the
// swap-chain back buffer. The vanilla UpdateFrontBuffer then copies
// camR -> front buffer through RW's image path, which for this
// back-buffer-backed raster reads RW's own (never-resynced, stale)
// copy - feeding the whole colour-filter/radiosity/composite chain a
// static warm image, forever. That was the frozen beige. Fix: fill
// the front buffer from the LIVE back buffer instead -
// GetRenderTargetData into a systemmem surface, upload into our own
// texture raster (lock/unlock, the proven dither pattern) and blit
// that into the front buffer. Engaged only when the camera is on the
// swap-chain raster (the stale case); the radiosity ping-pong copies
// (camera on workBuffer) keep the stock path. hdrBuffer=0 never sees
// any of this.
static int sfxHDRready;				// set once the fp16 buffer exists
static RwRaster *sfxBBRaster;		// the swap-chain-backed camera raster
// v9.50: live-fill working set for UpdateFrontBuffer (see there) -
// a plain CPU raster the back buffer is read into via the proven
// GetRenderTargetData path before being blitted into the front raster
static RwRaster *sfxLiveRaster;
static int sfxLiveW, sfxLiveH;
static int sfxLogLive;
// v9.51: cached staging surface for the live fill (v9.50 created and
// released one three times per frame - most of the added FPS drop)
static IDirect3DSurface9 *sfxSysSurf;
static int sfxSysW, sfxSysH;
// v9.54: hybrid fill - the first refresh of a frame does the live
// readback (correctness anchor, recursion stays dead); the later
// refreshes re-upload THIS frame's cached clean copy (cheap CPU
// upload, no GPU->CPU readback). The v9.53 T lines measured the
// readback at 8-16ms per frame = 44-72% of the whole frame.
static unsigned int sfxUFLiveToken = 0;
static int sfxLogFast;
// v9.53: per-frame timing - microseconds spent in UpdateFrontBuffer
// (the live readback) and the full frame (DFE heartbeat), so the log
// quantifies exactly what the readback costs on this machine
static long long sfxUFUs;
static long long sfxFrameLastQPC;
static int sfxLogTiming;
// v9.50b: forward declaration - UpdateFrontBuffer logs before the
// declaration block further down (CI C3861)
static void sfxLogLine(const char *fmt, ...);
static int sfxLogU2;
static int sfxLogBR;
// v9.57: pause freeze. The game keeps rendering frames while paused
// (menu over a frozen scene), but the fp16 scene buffer is cleared and
// receives only the sky gradient, so a resolve overwrites the presented
// back buffer - which still holds the last real composite, HUD included
// - with that near-empty gradient. That is the black/white pause
// background. While either pause flag is set the whole post chain
// stands down: no resolve, no filter/glow redraw, no composite. The
// presented frame stays exactly the last composited one (vanilla pause
// behaviour) and the menu draws on top of it. Address pair verified
// against plugin-sdk CTimer.cpp (SA 1.0 US).
static int
sfxPaused(void)
{
	return (*(unsigned char*)0xB7CB48 || *(unsigned char*)0xB7CB49);
}
// v9.57: pure-GPU live fill, forward declaration (defined below after
// the quad helpers; called by UpdateFrontBuffer above them)
// v9.57b CI fix: sfxGPUFill calls setSceneRaster, which is defined much
// further down in this file - declare it here first (C3861; the recurring
// decl-before-use lesson, re-declaring a static prototype is legal).
static void setSceneRaster(RwRaster *r);
// v9.58: the GPU copy's own render-target TEXTURE. v9.57 sampled the
// swap-chain camera raster through RW's rwRENDERSTATETEXTURERASTER path
// - an undocumented binding, that raster has no native D3D9 texture of
// its own - and the log caught the result: with the camera FROZEN the
// final image flips between the real frame and a perfectly flat gray
// 178,178,178 (SNAPF f=634 +113,+101,+92 then f=642 back, H16 normal
// both times) - the composite intermittently sampled an empty/white
// source. The copy now goes through a texture WE created: StretchRect
// back buffer -> RT texture (pure GPU, device-level), then the same raw
// XYZRHW quad the fp16 resolve has used since v9.30 samples it into the
// padded front raster. No RW raster-as-texture magic anywhere.
static IDirect3DTexture9 *sfxCopyTex;
static IDirect3DSurface9 *sfxCopySurf;
static int sfxCopyW, sfxCopyH;
static int sfxCopyFailed;
static int sfxLogCopy;
// v9.59: the v9.58 log killed the flat-gray wash but the HUD still
// disappears in the ON state - the composite's source (the padded front
// raster) intermittently misses the HUD even though the fp16 scene and
// the back buffer are correct at the same moments. The refresh that
// runs right before the composite therefore goes back to the PROVEN
// v9.53 content path (GetRenderTargetData -> CPU raster -> blit), the
// last build whose ON state kept the HUD; refreshes 1-2 stay pure GPU.
static IDirect3DSurface9 *sfxRbSurf;
static int sfxRbW, sfxRbH;
// sfxLiveRaster/sfxLiveW/sfxLiveH already exist from the v9.54 block
// (declared at the top of this file) - they are reused here, NOT
// redeclared (C2086; grep the whole file before adding any static).
static int sfxLogRb;
// v9.66: menu state via the transition pulse at 0xBA67A4. The v9.65
// test proved that byte is NOT the persistent m_bMenuActive: it is 1
// for exactly one frame at menu open and one frame at menu close, 0
// in between (log: the open and close lines one frame apart). The
// open pulse lands inside the 3-4 frame pause blip, the close pulse
// comes without one - the gate samples the pulse and lets
// sfxPaused() pick which transition it was.
static int sfxMenuOpen;
// v9.65: rolling back-buffer copy, DOUBLE-BUFFERED. The hold presents
// the pair captured LAST frame while the fresh capture goes into the
// other pair: v9.64 stretched into the very texture it drew from in
// the same frame - a read-after-write hazard that produced the
// white/black garbage frames (Screenshot_110). Presented frames now
// always show a live (1 frame old) menu over the last finished game
// frame; the composite - the menu eraser - stays skipped while open,
// so hdrBuffer=1 behaves like the proven-clean hdrBuffer=0 path.
static IDirect3DTexture9 *sfxCopyTexB;
static IDirect3DSurface9 *sfxCopySurfB;
static void sfxHoldRolling(void);
static void sfxHoldRotate(void);
// v9.59: pause hold - defined further down (it needs the D3D vtable
// helper declarations). Re-presents the frozen GPU copy on the
// presented buffer every pause frame so the swap chain never serves
// cleared/stale frames (the black/white pause background of v9.57).
static void sfxPauseHold(RwRaster *camR);

// record which raster is the swap-chain-backed camera raster; the
// resolve calls this every frame right after landing
static void
sfxBBRegister(RwRaster *camR)
{
	if(sfxBBRaster != camR && sfxLog && sfxLogBR < 8){
		sfxLogBR++;
		fprintf(sfxLog, "BR bb raster %08x %dx%d\n",
			(unsigned int)(void*)camR,
			camR != nil ? camR->width : 0, camR != nil ? camR->height : 0);
	}
	sfxBBRaster = camR;
}

// v9.36e: the 9.36d run failed EVERY call - PushContext defers the
// render-target commit, so the 9.36d copy probed GetRenderTarget and
// always saw the back buffer, and the self-copy guard rejected all
// (0 "U2 live fb" lines). v9.36e switched the camera raster instead -
// the fill itself ran, but the bare DrawQuad call white-screened the
// whole game (screenshot 85): CPostEffects::DrawQuad does NOT set up
// the D3D vertex declaration, the quad UVs or the blend state on its
// own. EVERY other caller in this file wraps it in
// ImmediateModeRenderStatesStore/Set and resets the UVs afterwards;
// the bare call drew with whatever the fp16 pipeline had left,
// poisoned the front buffer with garbage and every composite came
// out white - in-game AND in the pause menu (the filter keeps
// compositing there). v9.36f keeps the live camera-raster source but
// copies inside the vanilla write bracket (RwRasterPushContext - the
// same pause-safe one the stock copy uses, with NO 9.36d
// GetRenderTarget probe) and draws the camera raster as one
// full-screen quad using the exact state pattern of the proven
// callers (default UVs, straight SRCALPHA/INVSRCALPHA copy, vertex
// colour 255).
// v9.36g: the f2 run (log aqKkGYeG) showed the fill target is the
// colour filter's own PADDED front buffer - 2048x1024 for a 1600x900
// screen (the "U2 live fb 2048x1024" / "CF fbchange ... 2048x1024"
// lines) - and the stock copy lands the image 1:1 in its top-left
// corner (RwRasterRenderFast(camR, 0, 0)); the filter UVs read exactly
// that sub-rect. f2 sized the quad from the TARGET (2048x1024) and
// stretched the 1600x900 camera raster across the whole padded raster,
// so the filter read a zoomed crop - the dim steady state, the blur
// snap (radiosity smearing the zoomed composite) and the
// angle-triggered white flashes. The quad now covers the camera
// raster's own rect - 1:1, exactly where the stock copy puts it.
void
CPostEffects::UpdateFrontBuffer(void)
{
	LARGE_INTEGER c0, c1;
	QueryPerformanceCounter(&c0);
	// v9.32: hdr path - the camera raster is the swap-chain back
	// buffer, so the stock copy below would feed the chain RW's stale
	// system copy of it (the frozen beige). Fill the front buffer from
	// the live back buffer instead. Radiosity ping-pong copies (camera
	// on workBuffer) and hdrBuffer=0 take the stock path unchanged.
	// v9.33: belt and suspenders - any fault in the copy path falls
	// back to the stock copy and disables the live path for good, so
	// the worst case is the old behaviour, never a crash.
	// v9.35: fill at EVERY camera-on-back-buffer refresh (the vanilla
	// semantics: after the colour filter the refresh must capture the
	// FILTERED image for radiosity, after the post-effect refresh the
	// FINAL image for the composite). The old once-per-frame cache
	// filled AFTER the filter had already read last frame's content -
	// a same-frame feedback loop that accumulated into the blurred
	// warm wash (screenshot 76), the white-out peaks and the warm
	// snap. v9.39: the fill experiment is CLOSED - every write method
	// (GPU StretchRect, camera-raster quad, CPU RenderFast upload) kept
	// the white/snap feedback alive or added its own artifacts (the
	// 9.38 run even ate the HUD text and the pause menu mid-snap, plus
	// fps). The stock copy below is the baseline; the white/warm-snap
	// family is radiosity/extra-colour feedback and gets tuned on top
	// of this build using the PROBE RB verdict data.
	// v9.30k: WHICH raster is the camera holding when the filter chain
	// refreshes its front buffer? The colour filter / radiosity chain
	// samples this raster, and with hdrBuffer the camera raster can
	// momentarily disagree with the resolved back buffer (blur/radiosity
	// swap the camera raster mid-chain). Log the identity for the first
	// calls - a raster here that is NOT the main back buffer is the bug.
	{ static int n = 0;
	  RwRaster *r = RwCameraGetRaster(Scene.camera);
	  if(sfxLog && r && n++ < 60)
		  fprintf(sfxLog, "UF raster=%08x %dx%dx%d n=%d\n",
			  (unsigned int)(void*)r, r->width, r->height, r->depth, n);
	}
	// v9.51: THE PAUSE-BLACK ROOT. v9.50 opened every copy with
	// RwCameraEndUpdate and closed it with RwCameraBeginUpdate - but
	// BeginUpdate CLEARS the camera raster, and here that raster is
	// the presented back buffer. UpdateFrontBuffer runs three times
	// per frame and the last call sits AFTER the composite, so a
	// freshly cleared (black/white) back buffer could reach the
	// screen: the intermittent black bursts (v9.50b log f=575,
	// recovered by the next frame's resolve) and the persistent
	// black/white pause background (while paused nothing refills the
	// cleared buffer). The live fill below no longer touches the
	// camera context at all on the back-buffer path - GetRenderTargetData
	// is device-level and the blit only pushes/pops a raster context.
	// v9.50: when the camera raster is the swap-chain back buffer
	// (hdrBuffer=1), the stock copy below reads RW's stale system copy
	// of it - an ancient frame. In the pause menu that is exactly the
	// reported black/white background: the composite blends whatever
	// ancient content the stale copy holds. Fill the front raster from
	// the LIVE back buffer through the proven GetRenderTargetData path
	// instead (same readback the probes use). Radiosity ping-pong
	// passes (camera on workBuffer) keep the stock path; any fault in
	// the readback falls back to the stock copy (v9.33 pattern).
	{
		IDirect3DSurface9 *bb = nil;
		D3DSURFACE_DESC d;
		D3DLOCKED_RECT lr;
		int done = 0, locked = 0;
		// v9.53: the v9.52 once-per-frame skip is REVERTED. The log
		// proved the later refreshes are load-bearing: the radiosity
		// downsample passes write their SCRATCH into this very raster,
		// and the colour filter + composite sample it afterwards -
		// skipping the refills made them read quarter-size blur
		// (Screenshot 103: blurred world, no HUD in the ON state).
		// Every refresh gets a live fill again (correct content,
		// vanilla semantics, no camera-context pair so the presented
		// back buffer is never cleared - the v9.51 pause fix stays).
		// The readback cost is now MEASURED per frame (the T line in
		// the log) so moving the copy fully onto the GPU is decided
		// on data, not guesswork.
		// v9.60: CONVERGENCE. Four fill architectures were tested since
		// v9.54 (per-frame cache, RW raster-as-texture quad, own-RT-texture
		// quad, hybrid selection); only this one - the v9.53 live readback
		// at EVERY camera-on-bb refresh - ever kept the HUD correct in the
		// ON state (v9.53 is the last build with no snap/HUD complaints).
		// The readback cost returns (~8-16ms, the postponed FPS item) and
		// is accepted until the visuals are confirmed fixed.
		if(sfxBBRaster != nil && RwCameraGetRaster(Scene.camera) == sfxBBRaster &&
		   d3d9device->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &bb) == D3D_OK && bb != nil &&
		   bb->GetDesc(&d) == D3D_OK){
			if(sfxSysSurf == nil ||
			   sfxSysW != (int)d.Width || sfxSysH != (int)d.Height){
				if(sfxSysSurf)
					sfxSysSurf->Release();
				sfxSysSurf = nil;
				if(d3d9device->CreateOffscreenPlainSurface(d.Width, d.Height, d.Format,
				                                           D3DPOOL_SYSTEMMEM, &sfxSysSurf, nil) == D3D_OK){
					sfxSysW = d.Width;
					sfxSysH = d.Height;
				}
			}
			if(sfxSysSurf != nil &&
			   d3d9device->GetRenderTargetData(bb, sfxSysSurf) == D3D_OK &&
			   sfxSysSurf->LockRect(&lr, nil, D3DLOCK_READONLY) == D3D_OK){
				locked = 1;
				if(sfxLiveRaster == nil ||
				   sfxLiveW != (int)d.Width || sfxLiveH != (int)d.Height){
					if(sfxLiveRaster)
						RwRasterDestroy(sfxLiveRaster);
					sfxLiveRaster = RwRasterCreate((int)d.Width, (int)d.Height, 32,
						rwRASTERTYPETEXTURE | rwRASTERFORMAT8888);
					sfxLiveW = d.Width;
					sfxLiveH = d.Height;
				}
				if(sfxLiveRaster != nil){
					unsigned char *dst = (unsigned char*)RwRasterLock(sfxLiveRaster, 0, 1);
					if(dst != nil){
						const unsigned char *srcrow = (const unsigned char*)lr.pBits;
						int dstpitch = RwRasterGetWidth(sfxLiveRaster) * 4;
						int y, rows = (int)d.Height, cw = (int)d.Width * 4;
						for(y = 0; y < rows; y++)
							memcpy(dst + (size_t)y*dstpitch, srcrow + (size_t)y*lr.Pitch, cw);
						RwRasterUnlock(sfxLiveRaster);
						RwRasterPushContext(CPostEffects::pRasterFrontBuffer);
						RwRasterRenderFast(sfxLiveRaster, 0, 0);
						RwRasterPopContext();
						done = 1;
						sfxUfLive++;
						if(sfxLogLive < 40){
							sfxLogLive++;
							sfxLogLine("UF2 fill n=%d %dx%d pf=%d\n", sfxLogLive, sfxLiveW, sfxLiveH, sfxProbeFrame);
						}
					}
				}
			}
		}
		if(!done && sfxBBRaster != nil && RwCameraGetRaster(Scene.camera) == sfxBBRaster){
			// readback fault with the camera on the presented buffer -
			// copy RW's cached copy WITHOUT the camera-context pair
			// (BeginUpdate would clear the presented back buffer)
			RwRasterPushContext(CPostEffects::pRasterFrontBuffer);
			RwRasterRenderFast(RwCameraGetRaster(Scene.camera), 0, 0);
			RwRasterPopContext();
			done = 1;
				sfxUfBack++;
				if(sfxUfBackLog > 0){
					sfxUfBackLog--;
					sfxLogLine("UFB f=%u\n", sfxFrameNo);
				}
		}
		if(!done){
			// camera on a work raster (radiosity ping-pong): the
			// vanilla copy path - BeginUpdate clears the WORK raster,
			// never the presented back buffer
			RwCameraEndUpdate(Scene.camera);
			RwRasterPushContext(CPostEffects::pRasterFrontBuffer);
			RwRasterRenderFast(RwCameraGetRaster(Scene.camera), 0, 0);
			RwRasterPopContext();
			RwCameraBeginUpdate(Scene.camera);
				sfxUfWork++;
				if(sfxUfWorkLog > 0){
					sfxUfWorkLog--;
					sfxLogLine("UFW f=%u\n", sfxFrameNo);
				}
		}
	}
	QueryPerformanceCounter(&c1);
	{
		static LARGE_INTEGER sfxQPF;
		if(sfxQPF.QuadPart == 0)
			QueryPerformanceFrequency(&sfxQPF);
		sfxUFUs += ((c1.QuadPart - c0.QuadPart) * 1000000) / sfxQPF.QuadPart;
	}
}

RwRaster *vcs_radiosity_target1, *vcs_radiosity_target2;
static RwIm2DVertex vcsVertices[24];
RwRect vcsRect;
RwImVertexIndex vcsIndices1[] = {
	0, 1, 2, 1, 2, 3,
		4, 5, 2, 5, 2, 3,
	4, 5, 6, 5, 6, 7,
		8, 9, 6, 9, 6, 7,
	8, 9, 10, 9, 10, 11,
		12, 13, 10, 13, 10, 11,
	12, 13, 14, 13, 14, 15,
};

RwImVertexIndex radiosityIndices[] = {
	0, 1, 2, 1, 2, 3
};

RwD3D9Vertex radiosity_vcs_vertices[44];

//#define LIMIT (config->trailsLimit)
//#define INTENSITY (config->trailsIntensity)

void
makequad(RwD3D9Vertex *v, int width, int height, int texwidth = 0, int texheight = 0)
{
	float w, h, tw, th;
	w = width;
	h = height;
	tw = texwidth > 0 ? texwidth : w;
	th = texheight > 0 ? texheight : h;
	v[0].x = 0;
	v[0].y = 0;
	v[0].z = 0.0f;
	v[0].rhw = 1.0f;
	v[0].u = 0.5f / tw;
	v[0].v = 0.5f / th;
	v[0].emissiveColor = 0xFFFFFFFF;
	v[1].x = 0;
	v[1].y = h;
	v[1].z = 0.0f;
	v[1].rhw = 1.0f;
	v[1].u = 0.5f / tw;
	v[1].v = (h + 0.5f) / th;
	v[1].emissiveColor = 0xFFFFFFFF;
	v[2].x = w;
	v[2].y = 0;
	v[2].z = 0.0f;
	v[2].rhw = 1.0f;
	v[2].u = (w + 0.5f) / tw;
	v[2].v = 0.5f / th;
	v[2].emissiveColor = 0xFFFFFFFF;
	v[3].x = w;
	v[3].y = h;
	v[3].z = 0.0f;
	v[3].rhw = 1.0f;
	v[3].u = (w + 0.5f) / tw;
	v[3].v = (h + 0.5f) / th;
	v[3].emissiveColor = 0xFFFFFFFF;
}

void
CPostEffects::Radiosity_VCS_init(void)
{
	static float uOffsets[] = { -1.0f, 1.0f, 0.0f, 0.0f,   -1.0f, 1.0f, -1.0f, 1.0f };
	static float vOffsets[] = { 0.0f, 0.0f, -1.0f, 1.0f,   -1.0f, -1.0f, 1.0f, 1.0f };
	int i;
	int resMult = config->trailsResolution;
	RwUInt32 c;
	float w, h;

	if(vcs_radiosity_target1)
		RwRasterDestroy(vcs_radiosity_target1);
	vcs_radiosity_target1 = RwRasterCreate(256 * resMult, 128 * resMult, RwCameraGetRaster(Scene.camera)->depth, rwRASTERTYPECAMERATEXTURE);
	if(vcs_radiosity_target2)
		RwRasterDestroy(vcs_radiosity_target2);
	vcs_radiosity_target2 = RwRasterCreate(256 * resMult, 128 * resMult, RwCameraGetRaster(Scene.camera)->depth, rwRASTERTYPECAMERATEXTURE);
//	RwD3D9CreateVertexBuffer(stride, size, &vbuf, &offset);

	w = 256 * resMult;
	h = 128 * resMult;

	// TODO: tex coords correct?
	makequad(radiosity_vcs_vertices, 256 * resMult, 128 * resMult);
	makequad(radiosity_vcs_vertices+4, RwCameraGetRaster(Scene.camera)->width, RwCameraGetRaster(Scene.camera)->height);

	// black vertices; at 8
	for(i = 0; i < 4; i++){
		radiosity_vcs_vertices[i+8] = radiosity_vcs_vertices[i];
		radiosity_vcs_vertices[i+8].emissiveColor = 0;
	}

	// two sets blur vertices; at 12
	c = D3DCOLOR_ARGB(0xFF, 36, 36, 36);
	for(i = 0; i < 2*4*4; i++){
		radiosity_vcs_vertices[i+12] = radiosity_vcs_vertices[i%4];
		radiosity_vcs_vertices[i+12].emissiveColor = c;
		switch(i%4){
		case 0:
			radiosity_vcs_vertices[i+12].u = (uOffsets[i/4] + 0.5f) / w;
			radiosity_vcs_vertices[i+12].v = (vOffsets[i/4] + 0.5f) / h;
			break;
		case 1:
			radiosity_vcs_vertices[i+12].u = (uOffsets[i/4] + 0.5f) / w;
			radiosity_vcs_vertices[i+12].v = (h + vOffsets[i/4] + 0.5f) / h;
			break;
		case 2:
			radiosity_vcs_vertices[i+12].u = (w + uOffsets[i/4] + 0.5f) / w;
			radiosity_vcs_vertices[i+12].v = (vOffsets[i/4] + 0.5f) / h;
			break;
		case 3:
			radiosity_vcs_vertices[i+12].u = (w + uOffsets[i/4] + 0.5f) / w;
			radiosity_vcs_vertices[i+12].v = (h + vOffsets[i/4] + 0.5f) / h;
			break;
		}
	}
}

void
CPostEffects::Radiosity_VCS(int limit, int intensity)
{
	sfxRadCalls++;

	static int lastWidth, lastHeight, lastConfigRes;
	int i;
	int resMult = config->trailsResolution;
	RwRaster *fb;
	RwRaster *fb1, *fb2, *tmp;

	fb = RwCameraGetRaster(Scene.camera);
	if(lastWidth != fb->width || lastHeight != fb->height || lastConfigRes != resMult){
		Radiosity_VCS_init();
		lastWidth = fb->width;
		lastHeight = fb->height;
		lastConfigRes = resMult;
	}

	RwRect r;
	r.x = 0;
	r.y = 0;
	r.w = 256 * resMult;
	r.h = 128 * resMult;

	CPostEffects::ImmediateModeRenderStatesStore();
	CPostEffects::ImmediateModeRenderStatesSet();
	RwD3D9SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, (void*)rwFILTERLINEAR);
	RwD3D9SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	RwCameraEndUpdate(Scene.camera);

	RwRasterPushContext(vcs_radiosity_target2);
	RwRasterRenderScaled(fb, &r);
	RwRasterPopContext();

	RwCameraSetRaster(Scene.camera, vcs_radiosity_target2);
	RwCameraBeginUpdate(Scene.camera);

	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, NULL);
	RwD3D9SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_REVSUBTRACT);
	RwD3D9SetRenderState(D3DRS_SRCBLEND, D3DBLEND_BLENDFACTOR);
	RwD3D9SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
	RwD3D9SetRenderState(D3DRS_BLENDFACTOR, D3DCOLOR_ARGB(0xFF, limit/2, limit/2, limit/2));
	RwIm2DRenderIndexedPrimitive(rwPRIMTYPETRILIST, radiosity_vcs_vertices, 4, radiosityIndices, 6);

	fb1 = vcs_radiosity_target1;
	fb2 = vcs_radiosity_target2;
	for(i = 0; i < 4; i++){
		RwD3D9SetRenderTarget(0, fb1);

		RwRenderStateSet(rwRENDERSTATETEXTURERASTER, NULL);
		RwD3D9SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		RwIm2DRenderIndexedPrimitive(rwPRIMTYPETRILIST, radiosity_vcs_vertices+8, 4, radiosityIndices, 6);

		RwRenderStateSet(rwRENDERSTATETEXTURERASTER, fb2);
		RwRenderStateSet(rwRENDERSTATETEXTUREADDRESSU, (void*)rwTEXTUREADDRESSCLAMP);
		RwRenderStateSet(rwRENDERSTATETEXTUREADDRESSV, (void*)rwTEXTUREADDRESSCLAMP);
		RwD3D9SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		RwD3D9SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
		RwD3D9SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
		RwD3D9SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
		if((i % 2) == 0)
			RwIm2DRenderIndexedPrimitive(rwPRIMTYPETRILIST, radiosity_vcs_vertices+12, 4*4, vcsIndices1, 6*7);
		else
			RwIm2DRenderIndexedPrimitive(rwPRIMTYPETRILIST, radiosity_vcs_vertices+28, 4*4, vcsIndices1, 6*7);

		tmp = fb1;
		fb1 = fb2;
		fb2 = tmp;
	}

	RwCameraEndUpdate(Scene.camera);
	RwCameraSetRaster(Scene.camera, fb);
	RwCameraBeginUpdate(Scene.camera);

	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, fb2);
	RwRenderStateSet(rwRENDERSTATETEXTUREADDRESSU, (void*)rwTEXTUREADDRESSCLAMP);
	RwRenderStateSet(rwRENDERSTATETEXTUREADDRESSV, (void*)rwTEXTUREADDRESSCLAMP);
	RwD3D9SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	RwD3D9SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	RwD3D9SetRenderState(D3DRS_SRCBLEND, D3DBLEND_BLENDFACTOR);
	RwD3D9SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
	RwD3D9SetRenderState(D3DRS_BLENDFACTOR, D3DCOLOR_ARGB(0xFF, intensity*4, intensity*4, intensity*4));
	RwIm2DRenderIndexedPrimitive(rwPRIMTYPETRILIST, radiosity_vcs_vertices+4, 4, radiosityIndices, 6);
	RwIm2DRenderIndexedPrimitive(rwPRIMTYPETRILIST, radiosity_vcs_vertices+4, 4, radiosityIndices, 6);

	RwD3D9SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	CPostEffects::ImmediateModeRenderStatesReStore();
}

RwD3D9Vertex blur_vcs_vertices[24];
RwImVertexIndex blur_vcs_Indices[] = {
	0, 1, 2, 2, 1, 3,
	4, 5, 6, 6, 5, 7,
	8, 9, 10, 10, 9, 11,
};
RwRaster *lastFrameBuffer;
RwRGBA vcsblurrgb;
RwRGBA rgbTweak;

#define BLUROFFSET (2.1f)
#define BLURINTENSITY (39.0f)

void
CPostEffects::Blur_VCS(void)
{
	static int lastWidth, lastHeight;
	static int justInitialized;
	int i;
	int bufw, bufh;
	int screenw, screenh;
	int intensity;
	bufw = CPostEffects::pRasterFrontBuffer->width;
	bufh = CPostEffects::pRasterFrontBuffer->height;

	/*if(GetAsyncKeyState(VK_F7) & 0x8000){
		justInitialized = 1;
		return;
	}*/

	if(lastWidth != bufw || lastHeight != bufh){
		if(lastFrameBuffer)
			RwRasterDestroy(lastFrameBuffer);
		lastFrameBuffer = RwRasterCreate(bufw, bufh, CPostEffects::pRasterFrontBuffer->depth, rwRASTERTYPECAMERATEXTURE);
		justInitialized = 1;
		lastWidth = bufw;
		lastHeight = bufh;
	}

	screenw = RwCameraGetRaster(Scene.camera)->width;
	screenh = RwCameraGetRaster(Scene.camera)->height;

	makequad(blur_vcs_vertices, screenw, screenh, bufw, bufh);
	for(i = 0; i < 4; i++)
		blur_vcs_vertices[i].x += BLUROFFSET;
	makequad(blur_vcs_vertices+4, screenw, screenh, bufw, bufh);
	for(i = 4; i < 8; i++){
		blur_vcs_vertices[i].x += BLUROFFSET;
		blur_vcs_vertices[i].y += BLUROFFSET;
	}
	makequad(blur_vcs_vertices+8, screenw, screenh, bufw, bufh);
	for(i = 8; i < 12; i++)
		blur_vcs_vertices[i].y += BLUROFFSET;
	makequad(blur_vcs_vertices+12, screenw, screenh, bufw, bufh);
	for(i = 12; i < 16; i++)
		blur_vcs_vertices[i].emissiveColor = D3DCOLOR_ARGB(0xff, vcsblurrgb.red, vcsblurrgb.green, vcsblurrgb.blue);
	makequad(blur_vcs_vertices+16, screenw, screenh, bufw, bufh);
	makequad(blur_vcs_vertices+20, screenw, screenh, bufw, bufh);
	for(i = 20; i < 24; i++)
		blur_vcs_vertices[i].emissiveColor = 0;

	CPostEffects::ImmediateModeRenderStatesStore();
	CPostEffects::ImmediateModeRenderStatesSet();
	RwD3D9SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, (void*)rwFILTERLINEAR);

	// get current frame
	RwCameraEndUpdate(Scene.camera);
	RwRasterPushContext(CPostEffects::pRasterFrontBuffer);
	RwRasterRenderFast(RwCameraGetRaster(Scene.camera), 0, 0);
	RwRasterPopContext();
	RwCameraBeginUpdate(Scene.camera);

	// blur frame
	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, CPostEffects::pRasterFrontBuffer);
	RwD3D9SetRenderState(D3DRS_SRCBLEND, D3DBLEND_BLENDFACTOR);
	RwD3D9SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVBLENDFACTOR);
	intensity = BLURINTENSITY*0.8f;
	RwD3D9SetRenderState(D3DRS_BLENDFACTOR, D3DCOLOR_ARGB(0xFF, intensity, intensity, intensity));
	RwIm2DRenderIndexedPrimitive(rwPRIMTYPETRILIST, blur_vcs_vertices, 12, blur_vcs_Indices, 3*6);

	// add colour filter color
	RwD3D9SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
	RwD3D9SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
	RwIm2DRenderIndexedPrimitive(rwPRIMTYPETRILIST, blur_vcs_vertices+12, 4, blur_vcs_Indices, 6);

	// blend with last frame
	if(justInitialized)
		justInitialized = 0;
	else{
		RwRenderStateSet(rwRENDERSTATETEXTURERASTER, lastFrameBuffer);
		RwD3D9SetRenderState(D3DRS_SRCBLEND, D3DBLEND_BLENDFACTOR);
		RwD3D9SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVBLENDFACTOR);
		RwD3D9SetRenderState(D3DRS_BLENDFACTOR, D3DCOLOR_ARGB(0xFF, 32, 32, 32));
		RwIm2DRenderIndexedPrimitive(rwPRIMTYPETRILIST, blur_vcs_vertices+16, 4, blur_vcs_Indices, 6);
	}

	// blend with black. Is this real?
if(0){
	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, NULL);
	RwD3D9SetRenderState(D3DRS_SRCBLEND, D3DBLEND_BLENDFACTOR);
	RwD3D9SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVBLENDFACTOR);
	RwD3D9SetRenderState(D3DRS_BLENDFACTOR, D3DCOLOR_ARGB(0xFF, 32, 32, 32));
	RwIm2DRenderIndexedPrimitive(rwPRIMTYPETRILIST, blur_vcs_vertices+20, 4, blur_vcs_Indices, 6);
}

	RwCameraEndUpdate(Scene.camera);
	RwRasterPushContext(lastFrameBuffer);
	RwRasterRenderFast(RwCameraGetRaster(Scene.camera), 0, 0);
	RwRasterPopContext();
	RwCameraBeginUpdate(Scene.camera);

	RwD3D9SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	CPostEffects::ImmediateModeRenderStatesReStore();
}

/* quad format:
 * 0--3
 * |\ |
 * | \|
 * 1--2 */
void
quadSetXY(RwIm2DVertex *verts, float x0, float y0, float x1, float y1)
{
	RwIm2DVertexSetScreenX(&verts[0], x0);
	RwIm2DVertexSetScreenY(&verts[0], y0);
	RwIm2DVertexSetScreenX(&verts[1], x0);
	RwIm2DVertexSetScreenY(&verts[1], y1);
	RwIm2DVertexSetScreenX(&verts[2], x1);
	RwIm2DVertexSetScreenY(&verts[2], y1);
	RwIm2DVertexSetScreenX(&verts[3], x1);
	RwIm2DVertexSetScreenY(&verts[3], y0);
}

void
quadSetUV(RwIm2DVertex *verts, float u0, float v0, float u1, float v1)
{
	RwIm2DVertexSetU(&verts[0], u0, 1.0f);
	RwIm2DVertexSetV(&verts[0], v0, 1.0f);
	RwIm2DVertexSetU(&verts[1], u0, 1.0f);
	RwIm2DVertexSetV(&verts[1], v1, 1.0f);
	RwIm2DVertexSetU(&verts[2], u1, 1.0f);
	RwIm2DVertexSetV(&verts[2], v1, 1.0f);
	RwIm2DVertexSetU(&verts[3], u1, 1.0f);
	RwIm2DVertexSetV(&verts[3], v0, 1.0f);
}

// v9.57: pure-GPU live fill. The camera raster (the swap-chain back
// buffer) is drawn as one textured quad INTO the padded front buffer
// through the proven setSceneRaster camera-raster swap - the exact
// bracket the radiosity passes and the bloom chain use every frame, so
// this mid-frame target juggling is as battle-tested as it gets in
// this build. No GetRenderTargetData, no CPU staging, no cached copy:
// every refresh sees the LIVE back buffer at the moment of the call,
// so the colour filter reads this frame's resolve and the end-of-frame
// composite reads the finished frame INCLUDING the HUD (the v9.53
// semantics without the 8ms readback that paid for them; the v9.54
// cache was what fed the composite pre-HUD content - the missing HUD -
// and pre-glow or ancient content on refill frames - the tint snap).
// The quad covers the SOURCE raster's own rect: 1:1 into the top-left
// of the padded target, exactly where the stock RwRasterRenderFast
// copy lands (v9.36g: a target-sized quad would stretch 1600x900
// across 2048x1024 and every consumer would read a zoomed crop).
void
CPostEffects::DrawQuadSetUVs(float utl, float vtl, float utr, float vtr, float ubr, float vbr, float ubl, float vbl)
{
	RwIm2DVertexSetU(&ms_imf.quad_verts[0], utl, ms_imf.recipZ);
	RwIm2DVertexSetV(&ms_imf.quad_verts[0], vtl, ms_imf.recipZ);
	RwIm2DVertexSetU(&ms_imf.quad_verts[1], utr, ms_imf.recipZ);
	RwIm2DVertexSetV(&ms_imf.quad_verts[1], vtr, ms_imf.recipZ);
	RwIm2DVertexSetU(&ms_imf.quad_verts[2], ubl, ms_imf.recipZ);
	RwIm2DVertexSetV(&ms_imf.quad_verts[2], vbl, ms_imf.recipZ);
	RwIm2DVertexSetU(&ms_imf.quad_verts[3], ubr, ms_imf.recipZ);
	RwIm2DVertexSetV(&ms_imf.quad_verts[3], vbr, ms_imf.recipZ);
}

void
CPostEffects::DrawQuadSetDefaultUVs(void)
{
	DrawQuadSetUVs(0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f);
}

// v9.44: radiosity/darkness diagnostics - must be declared before
// CPostEffects::Radiosity / DarknessFilter_fix below (the log helper and
// its counters live further down in this file).
static int sfxLogRad;
static int sfxLogRad2;
static int sfxLogDK;
static int sfxLogRadRun;
// v9.48: fresh-copy bookkeeping for the filter texture (see
// ColourFilter_switch) - its log line budget
// v9.45: call-presence counters - the DeferredStretch per-frame section
// compares them against last frame to detect the vanilla gate SKIPPING a
// call (a binary skip of the filter/glow draw = the suspected ON/OFF snap)
static unsigned int sfxRadSeq;
static unsigned int sfxDKSeq;
static void sfxLogLine(const char *fmt, ...);

void *blurPS, *radiosityPS;

void
CPostEffects::Radiosity_shader(int intensityLimit, int filterPasses, int renderPasses, int intensity)
{
	static RwRaster *workBuffer;
	// v9.46: effective (post-fade) radiosity add on a slow heartbeat
	if((sfxRadSeq & 15) == 0 && sfxLogRadRun < 200){
		sfxLogRadRun++;
		sfxLogLine("RADRUN seq=%u lim=%d inten=%d passes=%d/%d\n",
			sfxRadSeq, intensityLimit, intensity, filterPasses, renderPasses);
	}
	if(workBuffer)
		if(workBuffer->width != pRasterFrontBuffer->width ||
		   workBuffer->height != pRasterFrontBuffer->height ||
		   workBuffer->depth != pRasterFrontBuffer->depth){
			RwRasterDestroy(workBuffer);
			workBuffer = nil;
		}
	if(workBuffer == nil)
		workBuffer = RwRasterCreate(pRasterFrontBuffer->width, pRasterFrontBuffer->height, pRasterFrontBuffer->depth, rwRASTERTYPECAMERATEXTURE);

	RwRaster *drawBuffer = RwCameraGetRaster(Scene.camera);




	RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, (void*)rwFILTERLINEAR);
	RwRenderStateSet(rwRENDERSTATEFOGENABLE, (void*)FALSE);
	RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void*)FALSE);
	RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void*)FALSE);
	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void*)pRasterFrontBuffer);
	RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)FALSE);

	RwCameraEndUpdate(Scene.camera);
	RwCameraSetRaster(Scene.camera, workBuffer);
	RwCameraBeginUpdate(Scene.camera);

	float params[4];
	params[2] = 1<<filterPasses;
	params[2] *= drawBuffer->width/640.0f;

	overrideIm2dPixelShader = blurPS;
	// Blur vertically
	params[0] = 0;
	params[1] = 1.0f/RwRasterGetHeight(pRasterFrontBuffer);
	RwD3D9SetPixelShaderConstant(0, params, 1);
	RwIm2DRenderIndexedPrimitive(rwPRIMTYPETRILIST, colorfilterVerts, 4, colorfilterIndices, 6);
	UpdateFrontBuffer();
	// Blur horizontally
	params[0] = 1.0f/RwRasterGetWidth(pRasterFrontBuffer);
	params[1] = 0;
	RwD3D9SetPixelShaderConstant(0, params, 1);
	RwIm2DRenderIndexedPrimitive(rwPRIMTYPETRILIST, colorfilterVerts, 4, colorfilterIndices, 6);
	UpdateFrontBuffer();
	overrideIm2dPixelShader = nil;


	/* Restore original FB */
	RwCameraEndUpdate(Scene.camera);
	RwCameraSetRaster(Scene.camera, drawBuffer);
	RwCameraBeginUpdate(Scene.camera);
	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void*)pRasterFrontBuffer);

	/* Add to framebuffer */
	params[0] = intensityLimit/255.0f;
	params[1] = intensity/255.0f;
	params[2] = renderPasses;
	RwD3D9SetPixelShaderConstant(0, params, 1);

	float off = ((1<<filterPasses)-1);
	// only for upper left corner actually
	// other one has 2,2 harcoded but since these are 2 by default, we'll reuse them
	float offu = off*m_RadiosityFilterUCorrection;
	float offv = off*m_RadiosityFilterVCorrection;

	float minu = offu;
	float minv = offv;
	float maxu = drawBuffer->width - offu; //off*2;
	float maxv = drawBuffer->height - offv; //off*2;
	float cu = (offu*(drawBuffer->width+0.5f) + offu/*off*2*/*0.5f) / drawBuffer->width;
	float cv = (offv*(drawBuffer->height+0.5f) + offv/*off*2*/*0.5f) / drawBuffer->height;

	params[0] = cu / pRasterFrontBuffer->width;
	params[1] = cv / pRasterFrontBuffer->height;
	params[2] = (maxu-minu) / drawBuffer->width;
	params[3] = (maxv-minv) / drawBuffer->height;
	RwD3D9SetPixelShaderConstant(1, params, 1);

	RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)!m_bRadiosityDebug);
	RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDONE);
	RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDONE);
	overrideIm2dPixelShader = radiosityPS;
	RwIm2DRenderIndexedPrimitive(rwPRIMTYPETRILIST, colorfilterVerts, 4, colorfilterIndices, 6);
	overrideIm2dPixelShader = nil;

	RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, (void*)rwFILTERLINEAR);
	RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void*)TRUE);
	RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void*)TRUE);
	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void*)NULL);
	RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)TRUE);
	RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDSRCALPHA);
	RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDINVSRCALPHA);

	UpdateFrontBuffer();
}

void
CPostEffects::Radiosity(int intensityLimit, int filterPasses, int renderPasses, int intensity)
{
	sfxRadSeq++;
	// v9.57: paused - no glow redraw over the frozen frame
	if(sfxPaused())
		return;
/*
	{
		static bool keystate = false;
		if(GetAsyncKeyState(VK_F5) & 0x8000){
			if(!keystate){
				keystate = true;
				config->radiosity = !config->radiosity;
			}
		}else
			keystate = false;
	}
*/

	if (config->vcsTrails) {
		CPostEffects::Radiosity_VCS(config->trailsLimit, config->trailsIntensity);
		if (config->colorFilter == COLORFILTER_VCS)
			CPostEffects::Blur_VCS();
		return;
	}

	if(!config->doRadiosity)
		return;

	// v9.44: the vanilla caller can flip the radiosity glow hard ON/OFF
	// (night hours only, camera-angle dependent). Trace every target
	// change, and fade intensity/limit towards the vanilla target at
	// 16%/frame instead of following the flip instantly - the pop
	// becomes a sub-second fade. filterPasses/renderPasses stay as-is;
	// if the RAD target lines show those flipping, the next step
	// handles them.
	{
		static int tLim = -1, tInt = -1;
		if(intensityLimit != tLim || intensity != tInt){
			tLim = intensityLimit;
			tInt = intensity;
			if(sfxLogRad < 60){
				sfxLogRad++;
				sfxLogLine("RAD target lim=%d inten=%d passes=%d/%d gb=%d gi=%d\n",
					intensityLimit, intensity, filterPasses, renderPasses,
					(int)CPostEffects::m_bRadiosity,
					CPostEffects::m_RadiosityIntensity);
			}
		}
		{
			static int sLim = -1, sInt = -1;
			if(sLim < 0){
				// v9.46: the first call after the vanilla gate opens used
				// to draw the glow at full strength instantly - the RAD
				// start pop (v9.44 log: RAD@f=531 + SNAP@f=550; v9.45
				// log: RAD@f=371 + SNAP@f=373). Start the fade from zero
				// so the glow fades in over ~0.3 s instead of popping.
				sLim = 0;
				sInt = 0;
			}
			int dl = intensityLimit - sLim;
			int di = intensity - sInt;
			if(dl < -1 || dl > 1) sLim += dl * 16 / 100;
			if(di < -1 || di > 1) sInt += di * 16 / 100;
			if(sLim < 0) sLim = 0;
			if(sLim > 255) sLim = 255;
			if(sInt < 0) sInt = 0;
			if(sInt > 255) sInt = 255;
			if(sfxLogRad2 < 600 &&
			   ((di < -8 || di > 8) || (dl < -8 || dl > 8))){
				sfxLogRad2++;
				sfxLogLine("RADSMOOTH dl=%d di=%d lim=%d->%d inten=%d->%d\n",
					dl, di, intensityLimit, sLim, intensity, sInt);
			}
			intensityLimit = sLim;
			intensity = sInt;
		}
	}

	if(config->radiosity == 1){
		Radiosity_shader(intensityLimit, filterPasses, renderPasses, intensity);
		return;
	}

	static RwRaster *workBuffer;
	if(workBuffer)
		if(workBuffer->width != pRasterFrontBuffer->width ||
		   workBuffer->height != pRasterFrontBuffer->height ||
		   workBuffer->depth != pRasterFrontBuffer->depth){
			RwRasterDestroy(workBuffer);
			workBuffer = nil;
		}
	if(workBuffer == nil)
		workBuffer = RwRasterCreate(pRasterFrontBuffer->width, pRasterFrontBuffer->height, pRasterFrontBuffer->depth, rwRASTERTYPECAMERATEXTURE);

	RwRaster *renderBuffer, *textureBuffer;

	RwRaster *drawBuffer = RwCameraGetRaster(Scene.camera);

	RwInt32 w = RwRasterGetWidth(drawBuffer);
	RwInt32 h = RwRasterGetHeight(drawBuffer);
	RwReal width = RwRasterGetWidth(pRasterFrontBuffer);
	RwReal height = RwRasterGetHeight(pRasterFrontBuffer);
	float umin, umax, vmin, vmax;

	static RwIm2DVertex verts[4];

	float nearscreen = RwIm2DGetNearScreenZ();
	float nearcam = RwCameraGetNearClipPlane(Scene.camera);
	float recipz = 1.0f/nearcam;
	for(int i = 0; i < 4; i++){
		RwIm2DVertexSetScreenZ(&verts[i], nearscreen);
		RwIm2DVertexSetCameraZ(&verts[i], nearcam);
		RwIm2DVertexSetRecipCameraZ(&verts[i], recipz);
		RwIm2DVertexSetIntRGBA(&verts[i], 255, 255, 255, 255);
	}


	RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, (void*)rwFILTERLINEAR);
	RwRenderStateSet(rwRENDERSTATEFOGENABLE, (void*)FALSE);
	RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void*)FALSE);
	RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void*)FALSE);
	RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)FALSE);

	renderBuffer = workBuffer;
	textureBuffer  = pRasterFrontBuffer;
	RwCameraEndUpdate(Scene.camera);
	RwCameraSetRaster(Scene.camera, renderBuffer);
	RwCameraBeginUpdate(Scene.camera);
	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void*)textureBuffer);

	int downsampledwidth = w;
	int downsampledheight = h;

	// First step: Downsample
	for(int i = 0; i < filterPasses; i++){
		umin = (m_RadiosityFilterUCorrection + 0.5f)/width;
		umax = (downsampledwidth + 0.5f)/width;
		vmin = (m_RadiosityFilterVCorrection + 0.5f)/height;
		vmax = (downsampledheight + 0.5f)/height;

		downsampledwidth /= 2;
		downsampledheight /= 2;

		quadSetUV(verts, umin, vmin, umax, vmax);
		quadSetXY(verts, 0.0f, 0.0f, downsampledwidth+1, downsampledheight+1);

		RwIm2DRenderIndexedPrimitive(rwPRIMTYPETRILIST, verts, 4, colorfilterIndices, 6);

		// Switch buffers
		RwRaster *tmp = renderBuffer;
		renderBuffer = textureBuffer;
		textureBuffer = tmp;
		RwD3D9SetRenderTarget(0, renderBuffer);
		RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void*)textureBuffer);
	}

	// Second step: Subtract intensity value
	umin = (0 + 0.5f)/width;
	umax = (downsampledwidth+1 + 0.5f)/width;
	vmin = (0 + 0.5f)/height;
	vmax = (downsampledheight+1 + 0.5f)/height;

	quadSetUV(verts, umin, vmin, umax, vmax);
	quadSetXY(verts, 0.0f, 0.0f, downsampledwidth+1, downsampledheight+1);

	// D = 2*D - limit
	// We do 2*(D - limit/2) because the fixed function combiners can't do the above
	int limit = intensityLimit*128/255;
	RwD3D9SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_SUBTRACT);
	RwD3D9SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_CURRENT);
	RwD3D9SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_CONSTANT);
	RwD3D9SetTextureStageState(1, D3DTSS_CONSTANT, D3DCOLOR_ARGB(255, limit, limit, limit));
	RwD3D9SetTextureStageState(2, D3DTSS_COLOROP, D3DTOP_ADD);
	RwD3D9SetTextureStageState(2, D3DTSS_COLORARG1, D3DTA_CURRENT);
	RwD3D9SetTextureStageState(2, D3DTSS_COLORARG2, D3DTA_CURRENT);

	RwIm2DRenderIndexedPrimitive(rwPRIMTYPETRILIST, verts, 4, colorfilterIndices, 6);

	RwD3D9SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
	RwD3D9SetTextureStageState(2, D3DTSS_COLOROP, D3DTOP_DISABLE);

	RwCameraEndUpdate(Scene.camera);
	RwCameraSetRaster(Scene.camera, drawBuffer);
	RwCameraBeginUpdate(Scene.camera);
	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void*)renderBuffer);

	// Third step: add to framebuffer
	RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)!m_bRadiosityDebug);
	RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDSRCALPHA);
	RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDONE);
	umin = (0 + 0.5f)/width;
	umax = (downsampledwidth + 0.5f)/width;
	vmin = (0 + 0.5f)/height;
	vmax = (downsampledheight + 0.5f)/height;
	quadSetUV(verts, umin, vmin, umax, vmax);
	quadSetXY(verts, 0.0f, 0.0f, w, h);
	RwIm2DVertexSetIntRGBA(&verts[0], 255, 255, 255, intensity);
	RwIm2DVertexSetIntRGBA(&verts[1], 255, 255, 255, intensity);
	RwIm2DVertexSetIntRGBA(&verts[2], 255, 255, 255, intensity);
	RwIm2DVertexSetIntRGBA(&verts[3], 255, 255, 255, intensity);
	for(int i = 0; i < renderPasses; i++)
		RwIm2DRenderIndexedPrimitive(rwPRIMTYPETRILIST, verts, 4, colorfilterIndices, 6);


	RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void*)TRUE);
	RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void*)TRUE);
	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void*)NULL);
	RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDSRCALPHA);
	RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDINVSRCALPHA);

	UpdateFrontBuffer();
}

void
CPostEffects::DarknessFilter_fix(uint8 alpha)
{
	sfxDKSeq++;
	// v9.57: paused - no redraw over the frozen frame
	if(sfxPaused())
		return;
	// v9.44: trace what the vanilla caller feeds the darkness filter -
	// a binary flip here would also read as a hard night ON/OFF.
	{
		static int lastA = -1;
		if((int)alpha != lastA){
			lastA = (int)alpha;
			if(sfxLogDK < 40)
				sfxLogLine("DK alpha=%d bDF=%d\n",
					(int)alpha,
					(int)(CPostEffects::m_bDarknessFilter ? 1 : 0));
		}
	}
	DarknessFilter(alpha);
	UpdateFrontBuffer();
}

void
CPostEffects::ColourFilter_Generic(RwRGBA rgb1, RwRGBA rgb2, void *ps)
{
//	RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, (void*)rwFILTERNEAREST);
	RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, (void*)rwFILTERLINEAR);
	RwRenderStateSet(rwRENDERSTATEFOGENABLE, (void*)FALSE);
	RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void*)FALSE);
	RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void*)FALSE);
	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void*)CPostEffects::pRasterFrontBuffer);
	RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)FALSE);

	RwRGBAReal color, color2;
	RwRGBARealFromRwRGBA(&color, &rgb1);
	RwRGBARealFromRwRGBA(&color2, &rgb2);
	RwD3D9SetPixelShaderConstant(0, &color, 1);
	RwD3D9SetPixelShaderConstant(1, &color2, 1);

	overrideIm2dPixelShader = ps;
	RwIm2DRenderIndexedPrimitive(rwPRIMTYPETRILIST, colorfilterVerts, 4, colorfilterIndices, 6);
	overrideIm2dPixelShader = nil;

	RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, (void*)rwFILTERLINEAR);
	RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void*)TRUE);
	RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void*)TRUE);
	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void*)NULL);
	RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)TRUE);
}

void
CPostEffects::ColourFilter_Mobile(RwRGBA rgba1, RwRGBA rgba2)
{
//	RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, (void*)rwFILTERNEAREST);
	RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, (void*)rwFILTERLINEAR);
	RwRenderStateSet(rwRENDERSTATEFOGENABLE, (void*)FALSE);
	RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void*)FALSE);
	RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void*)FALSE);
	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void*)CPostEffects::pRasterFrontBuffer);
	RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)FALSE);

	if(!Colorcycle::initialised)
		Colorcycle::Initialise();

	GradeColorset cset;
	Colorcycle::Update(&cset);
	Grade red, green, blue;
	red = cset.red;
	green = cset.green;
	blue = cset.blue;

	// Mobile colors
	float r = rgba1.red + rgba2.red;
	float g = rgba1.green + rgba2.green;
	float b = rgba1.blue + rgba2.blue;
	float invsqrt = 1.0f/sqrt(r*r + g*g + b*b);
	r *= invsqrt;
	g *= invsqrt;
	b *= invsqrt;
	red.r = (1.5f + r*1.732f)*0.4f*red.r;
	green.g = (1.5f + g*1.732f)*0.4f*green.g;
	blue.b = (1.5f + b*1.732f)*0.4f*blue.b;

/*	// Fun trick: PS2 colour filter:
	float a = rgba2.alpha/128.0f;
	red.r = rgba1.red/128.0f + a*rgba2.red/128.0f;
	green.g = rgba1.green/128.0f + a*rgba2.green/128.0f;
	blue.b = rgba1.blue/128.0f + a*rgba2.blue/128.0f;
	red.g = red.b = red.a = 0.0f;
	green.r = green.b = green.a = 0.0f;
	blue.r = blue.g = blue.a = 0.0f;
*/
/*	// Also fun: PC colour filter:
	float a1 = rgba1.alpha/128.0f;
	float a2 = rgba2.alpha/128.0f;
	red.r = 1.0f + a1*rgba1.red/255.0f + a2*rgba2.red/255.0f;
	green.g = 1.0f + a1*rgba1.green/255.0f + a2*rgba2.green/255.0f;
	blue.b = 1.0f + a1*rgba1.blue/255.0f + a2*rgba2.blue/255.0f;
	red.g = red.b = red.a = 0.0f;
	green.r = green.b = green.a = 0.0f;
	blue.r = blue.g = blue.a = 0.0f;
*/


	RwD3D9SetPixelShaderConstant(0, &red, 1);
	RwD3D9SetPixelShaderConstant(1, &green, 1);
	RwD3D9SetPixelShaderConstant(2, &blue, 1);

	// contrast
	float mult[4];
	float add[4];
	mult[0] = red.r + red.g + red.b;
	mult[1] = green.r + green.g + green.b;
	mult[2] = blue.r + blue.g + blue.b;
	mult[3] = 1.0f;
	add[0] = red.a;
	add[1] = green.a;
	add[2] = blue.a;
	add[3] = 0.0f;

	RwD3D9SetPixelShaderConstant(3, mult, 1);
	RwD3D9SetPixelShaderConstant(4, add, 1);

	if(!(GetAsyncKeyState(VK_F5) & 0x8000))
		overrideIm2dPixelShader = gradingPS;
	else
		overrideIm2dPixelShader = contrastPS;
	RwIm2DRenderIndexedPrimitive(rwPRIMTYPETRILIST, colorfilterVerts, 4, colorfilterIndices, 6);
	overrideIm2dPixelShader = nil;

	RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, (void*)rwFILTERLINEAR);
	RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void*)TRUE);
	RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void*)TRUE);
	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void*)NULL);
	RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)TRUE);
}

void
CPostEffects::ColourFilter_PS2(RwRGBA rgba1, RwRGBA rgba2)
{
	RwIm2DVertex *verts;

	verts = colorfilterVerts;
	// Setup state
//	RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, (void*)rwFILTERNEAREST);
	RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, (void*)rwFILTERLINEAR);
	RwRenderStateSet(rwRENDERSTATEFOGENABLE, (void*)FALSE);
	RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void*)FALSE);
	RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void*)FALSE);
	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void*)CPostEffects::pRasterFrontBuffer);

	// Make Im2D use PS2 color range
	overrideColorMod = D3DTOP_MODULATE2X;
	overrideAlphaMod = D3DTOP_MODULATE2X;

	// First color - replace
	RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)FALSE);
	RwIm2DVertexSetIntRGBA(&verts[0], rgba1.red, rgba1.green, rgba1.blue, 255);
	RwIm2DVertexSetIntRGBA(&verts[1], rgba1.red, rgba1.green, rgba1.blue, 255);
	RwIm2DVertexSetIntRGBA(&verts[2], rgba1.red, rgba1.green, rgba1.blue, 255);
	RwIm2DVertexSetIntRGBA(&verts[3], rgba1.red, rgba1.green, rgba1.blue, 255);
	RwIm2DRenderIndexedPrimitive(rwPRIMTYPETRILIST, verts, 4, colorfilterIndices, 6);

	if(m_bBlurColourFilter){
		static RwIm2DVertex blurVerts[4];
		float rasterWidth = RwRasterGetWidth(CPostEffects::pRasterFrontBuffer);
		float rasterHeight = RwRasterGetHeight(CPostEffects::pRasterFrontBuffer);
		float scale = RwRasterGetWidth(RwCameraGetRaster(Scene.camera))/640.0f;
		float leftOff   = m_colourLeftUOffset*scale   / 16.0f / rasterWidth;
		float rightOff  = m_colourRightUOffset*scale  / 16.0f / rasterWidth;
		float topOff    = m_colourTopVOffset*scale    / 16.0f / rasterHeight;
		float bottomOff = m_colourBottomVOffset*scale / 16.0f / rasterHeight;
		memcpy(blurVerts, verts, sizeof(blurVerts));
		/* These are our vertices:
		 * 0--3
		 * |\ |
		 * | \|
		 * 1--2 */
		// We can get away without setting zrecip on D3D
		RwIm2DVertexSetU(&blurVerts[0], RwIm2DVertexGetU(&blurVerts[0]) + leftOff, 1.0f);
		RwIm2DVertexSetU(&blurVerts[1], RwIm2DVertexGetU(&blurVerts[1]) + leftOff, 1.0f);
		RwIm2DVertexSetU(&blurVerts[2], RwIm2DVertexGetU(&blurVerts[2]) + rightOff, 1.0f);
		RwIm2DVertexSetU(&blurVerts[3], RwIm2DVertexGetU(&blurVerts[3]) + rightOff, 1.0f);
		RwIm2DVertexSetV(&blurVerts[0], RwIm2DVertexGetV(&blurVerts[0]) + topOff, 1.0f);
		RwIm2DVertexSetV(&blurVerts[3], RwIm2DVertexGetV(&blurVerts[3]) + topOff, 1.0f);
		RwIm2DVertexSetV(&blurVerts[1], RwIm2DVertexGetV(&blurVerts[1]) + bottomOff, 1.0f);
		RwIm2DVertexSetV(&blurVerts[2], RwIm2DVertexGetV(&blurVerts[2]) + bottomOff, 1.0f);
		verts = blurVerts;
	}

	// Second color - add
	RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)TRUE);
	RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDSRCALPHA);
	RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDONE);
	RwIm2DVertexSetIntRGBA(&verts[0], rgba2.red, rgba2.green, rgba2.blue, rgba2.alpha);
	RwIm2DVertexSetIntRGBA(&verts[1], rgba2.red, rgba2.green, rgba2.blue, rgba2.alpha);
	RwIm2DVertexSetIntRGBA(&verts[2], rgba2.red, rgba2.green, rgba2.blue, rgba2.alpha);
	RwIm2DVertexSetIntRGBA(&verts[3], rgba2.red, rgba2.green, rgba2.blue, rgba2.alpha);
	RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDSRCALPHA);
 	RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDONE);
	RwIm2DRenderIndexedPrimitive(rwPRIMTYPETRILIST, verts, 4, colorfilterIndices, 6);

	// Restore state
	RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, (void*)rwFILTERLINEAR);
	RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void*)TRUE);
	RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void*)TRUE);
	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void*)NULL);
	RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDSRCALPHA);
	RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDINVSRCALPHA);

	overrideColorMod = -1;
	overrideAlphaMod = -1;
}

/* For reference only */
#if 0
void
CPostEffects::ColourFilter_PC(RwRGBA rgba1, RwRGBA rgba2)
{
	RwIm2DVertex *verts;

	verts = colorfilterVerts;
	// Setup state
	RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, (void*)rwFILTERNEAREST);
	RwRenderStateSet(rwRENDERSTATEFOGENABLE, (void*)FALSE);
	RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void*)FALSE);
	RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void*)FALSE);
	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void*)CPostEffects::pRasterFrontBuffer);
	RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)TRUE);
	RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDSRCALPHA);
	RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDONE);

	// First color
	RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)FALSE);
	RwIm2DVertexSetIntRGBA(&verts[0], rgba1.red, rgba1.green, rgba1.blue, rgba1.alpha);
	RwIm2DVertexSetIntRGBA(&verts[1], rgba1.red, rgba1.green, rgba1.blue, rgba1.alpha);
	RwIm2DVertexSetIntRGBA(&verts[2], rgba1.red, rgba1.green, rgba1.blue, rgba1.alpha);
	RwIm2DVertexSetIntRGBA(&verts[3], rgba1.red, rgba1.green, rgba1.blue, rgba1.alpha);
	RwIm2DRenderIndexedPrimitive(rwPRIMTYPETRILIST, verts, 4, colorfilterIndices, 6);

	// Second color
	RwIm2DVertexSetIntRGBA(&verts[0], rgba2.red, rgba2.green, rgba2.blue, rgba2.alpha);
	RwIm2DVertexSetIntRGBA(&verts[1], rgba2.red, rgba2.green, rgba2.blue, rgba2.alpha);
	RwIm2DVertexSetIntRGBA(&verts[2], rgba2.red, rgba2.green, rgba2.blue, rgba2.alpha);
	RwIm2DVertexSetIntRGBA(&verts[3], rgba2.red, rgba2.green, rgba2.blue, rgba2.alpha);
	RwIm2DRenderIndexedPrimitive(rwPRIMTYPETRILIST, verts, 4, colorfilterIndices, 6);

	// Restore state
	RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, (void*)rwFILTERLINEAR);
	RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void*)TRUE);
	RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void*)TRUE);
	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void*)NULL);
	RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDSRCALPHA);
	RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDINVSRCALPHA);
}
#endif

void
CPostEffects::SetFilterMainColour_PS2(RwRaster *raster, RwRGBA color)
{
//	RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, (void*)rwFILTERNEAREST);
	RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, (void*)rwFILTERLINEAR);
	RwRenderStateSet(rwRENDERSTATEFOGENABLE, (void*)FALSE);
	RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void*)FALSE);
	RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void*)FALSE);
	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, pRasterFrontBuffer);
	RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)FALSE);
	RwIm2DVertexSetIntRGBA(&colorfilterVerts[0], color.red, color.green, color.blue, color.alpha);
	RwIm2DVertexSetIntRGBA(&colorfilterVerts[1], color.red, color.green, color.blue, color.alpha);
	RwIm2DVertexSetIntRGBA(&colorfilterVerts[2], color.red, color.green, color.blue, color.alpha);
	RwIm2DVertexSetIntRGBA(&colorfilterVerts[3], color.red, color.green, color.blue, color.alpha);
	overrideColorMod = D3DTOP_MODULATE2X;
	RwIm2DRenderIndexedPrimitive(rwPRIMTYPETRILIST, colorfilterVerts, 4, colorfilterIndices, 6);
	overrideColorMod = -1;

	RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void*)TRUE);
	RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void*)TRUE);
	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, nil);
}

void
CPostEffects::InfraredVision_PS2(RwRGBA c1, RwRGBA c2)
{
	if(config->infraredVision != 0){
		InfraredVision(c1, c2);
		return;
	}

	CPostEffects::ImmediateModeRenderStatesStore();
	ImmediateModeRenderStatesSet();

	float r = m_fInfraredVisionFilterRadius;
	// not sure this scales correctly, but it looks ok (need better brain)
	float ru = r * RsGlobal->MaximumWidth  / RwRasterGetWidth(ms_imf.frontBuffer)  * 1024.0f / 640.0f;
	float rv = r * RsGlobal->MaximumHeight / RwRasterGetHeight(ms_imf.frontBuffer) * 512.0f  / 448.0f;
	float uoff[4] = { -ru, ru, ru, -ru };
	float voff[4] = { -rv, -rv, rv, rv };

	// PS2 draws the filter triangle triangle...we draw the quad
	RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDONE);
	RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDONE);
	for(int i = 0; i < 4; i++){
		DrawQuadSetUVs(ms_imf.tri_umin + uoff[i], ms_imf.tri_vmin + voff[i],
		               ms_imf.tri_umax + uoff[i], ms_imf.tri_vmin + voff[i],
		               ms_imf.tri_umax + uoff[i], ms_imf.tri_vmax + voff[i],
		               ms_imf.tri_umin + uoff[i], ms_imf.tri_vmax + voff[i]);
		DrawQuad(0, 0, RwRasterGetWidth(ms_imf.frontBuffer)*2, RwRasterGetHeight(ms_imf.frontBuffer)*2,
		                       c1.red, c1.green, c1.blue, 0xFFu, ms_imf.frontBuffer);

		UpdateFrontBuffer();
	}
	DrawQuadSetDefaultUVs();
	ImmediateModeRenderStatesReStore();

	SetFilterMainColour_PS2(ms_imf.frontBuffer, c2);
	UpdateFrontBuffer();
}

void
CPostEffects::NightVision_PS2(RwRGBA color)
{
	if(config->nightVision != 0){
		CPostEffects::NightVision(color);
		return;
	}

	if(CPostEffects::m_fNightVisionSwitchOnFXCount > 0.0f){
		CPostEffects::m_fNightVisionSwitchOnFXCount -= CTimer__ms_fTimeStep;
		if(CPostEffects::m_fNightVisionSwitchOnFXCount <= 0.0f)
			CPostEffects::m_fNightVisionSwitchOnFXCount = 0.0f;
		CPostEffects::ImmediateModeRenderStatesStore();
		CPostEffects::ImmediateModeRenderStatesSet();
		RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDONE);
		RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDONE);
		int n = CPostEffects::m_fNightVisionSwitchOnFXCount;
		while(n--)
		        CPostEffects::DrawQuad(0.0f, 0.0f,
				RwRasterGetWidth(ms_imf.frontBuffer), RwRasterGetHeight(ms_imf.frontBuffer),
				8, 8, 8, 255, ms_imf.frontBuffer);
		CPostEffects::ImmediateModeRenderStatesReStore();
	}

	UpdateFrontBuffer();
	CPostEffects::SetFilterMainColour_PS2(ms_imf.frontBuffer, color);
	UpdateFrontBuffer();
}


// VU style random number generator -- taken from pcsx2
uint R;
void vrinit(uint x){ R = 0x3F800000 | x & 0x007FFFFF; }
void vradvance(void){
	int x = (R >> 4) & 1;
	int y = (R >> 22) & 1;
	R <<= 1;
	R ^= x ^ y;
	R = (R&0x7fffff)|0x3f800000;
}
inline uint vrget(void){ return R; }
inline uint vrnext(void){ vradvance(); return R; }

void
CPostEffects::Grain_PS2(int strength, bool generate)
{
	if(config->grainFilter != 0){
		CPostEffects::Grain(strength, generate);
		return;
	}

	if(generate){
		RwUInt8 *pixels = RwRasterLock(grainRaster, 0, 1);
		vrinit(rand());
		int x = vrget();
		for(int i = 0; i < 64*64; i++){
			*pixels++ = x;
			*pixels++ = x;
			*pixels++ = x;
			*pixels++ = x & strength;
			x = vrnext();
		}
		RwRasterUnlock(grainRaster);
	}

	ImmediateModeRenderStatesStore();
	ImmediateModeRenderStatesSet();
	RwRenderStateSet(rwRENDERSTATETEXTUREADDRESS, (void*)rwTEXTUREADDRESSWRAP);
	RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, (void*)rwFILTERLINEAR);

	float umin = 0.0f;
	float vmin = 0.0f;
	float umax = 5.0f * RsGlobal->MaximumWidth/640.0f;
	float vmax = 7.0f * RsGlobal->MaximumHeight/448.0f;

	DrawQuadSetUVs(umin, vmin,
		umax, vmin,
		umax, vmax,
		umin, vmax);

	RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)D3DBLEND_DESTCOLOR);
	RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)D3DBLEND_SRCALPHA);

	overrideColorMod = D3DTOP_SELECTARG2;	// ignore texture color
	overrideAlphaMod = D3DTOP_MODULATE2X;
	CPostEffects::DrawQuad(0.0, 0.0, RsGlobal->MaximumWidth, RsGlobal->MaximumHeight,
	                       0xFFu, 0xFFu, 0xFFu, 0xFF, grainRaster);
	overrideColorMod = -1;
	overrideAlphaMod = -1;

	DrawQuadSetDefaultUVs();
	CPostEffects::ImmediateModeRenderStatesReStore();
}

// v9.30j2 build fix: the blank-front-buffer guard inside ColourFilter_switch
// runs BEFORE the log section below declares these - C2065/C3861. Declare
// them here instead (re-declaring a static function prototype is legal).
static int sfxLogCF;
static int sfxLogCSmooth;
// v9.45: bridge state - seq increments on every filter call, the last
// drawn colours let the always-running coronas stub redraw the filter
// on frames the vanilla gate skips it
static unsigned int sfxCFSeq;
static int sfxCFDrawOK;
static RwRGBA sfxCFLast1;
static RwRGBA sfxCFLast2;
static void sfxLogLine(const char *fmt, ...);

void
CPostEffects::ColourFilter_switch(RwRGBA rgb1, RwRGBA rgb2)
{
	sfxCFSeq++;
	// v9.57: paused - draw nothing. The frozen frame already carries the
	// last filter, and re-drawing it every pause frame would accumulate
	// the wash on the back buffer the freeze is preserving.
	if(sfxPaused())
		return;


	RwRGBA rgb1pc = rgb1;
	RwRGBA rgb2pc = rgb2;

	if(config->usePCTimecyc){
		// Gotta fix alpha for effects that assume PS2 alpha range
		rgb1.alpha /= 2;
		rgb2.alpha /= 2;
	}else{
		// Gotta fix alpha for effects that assume PC alpha range
		// clamping this is important!
		if(rgb1pc.alpha >= 128)
			rgb1pc.alpha = 255;
		else
			rgb1pc.alpha *= 2;
		if(rgb2pc.alpha >= 128)
			rgb2pc.alpha = 255;
		else
			rgb2pc.alpha *= 2;
	}

	rgb1.red *= config->rgb1Mult;
	rgb1.green *= config->rgb1Mult;
	rgb1.blue *= config->rgb1Mult;

	rgb2.red *= config->rgb2Mult;
	rgb2.green *= config->rgb2Mult;
	rgb2.blue *= config->rgb2Mult;

	// v9.43: the vanilla filter colours jump binary when the extra colour
	// engages (camera facing the low sun). On the fp16 path the resolved
	// frame is bright enough that the jump reads as a harsh warm ON/OFF
	// snap. Approach the target colours at 16%/frame instead - a sub-second
	// fade - and log sizeable jumps so the mechanism stays visible.
	{
		static int s1r = -1, s1g, s1b, s2r, s2g, s2b, s1a, s2a;
		int jR = (int)rgb1.red - s1r, jG = (int)rgb1.green - s1g, jB = (int)rgb1.blue - s1b;
		int kR = (int)rgb2.red - s2r, kG = (int)rgb2.green - s2g, kB = (int)rgb2.blue - s2b;
		int jA1 = (int)rgb1.alpha - s1a, jA2 = (int)rgb2.alpha - s2a;
		if(s1r < 0){
			s1r = rgb1.red; s1g = rgb1.green; s1b = rgb1.blue;
			s2r = rgb2.red; s2g = rgb2.green; s2b = rgb2.blue;
			s1a = rgb1.alpha; s2a = rgb2.alpha;
			jR = jG = jB = kR = kG = kB = jA1 = jA2 = 0;
		}
		if(jR < -1 || jR > 1) s1r += jR * 16 / 100;
		if(jG < -1 || jG > 1) s1g += jG * 16 / 100;
		if(jB < -1 || jB > 1) s1b += jB * 16 / 100;
		if(kR < -1 || kR > 1) s2r += kR * 16 / 100;
		if(kG < -1 || kG > 1) s2g += kG * 16 / 100;
		if(kB < -1 || kB > 1) s2b += kB * 16 / 100;
		if(jA1 < -1 || jA1 > 1) s1a += jA1 * 16 / 100;
		if(jA2 < -1 || jA2 > 1) s2a += jA2 * 16 / 100;
		rgb1.red = (unsigned char)s1r;
		rgb1.green = (unsigned char)s1g;
		rgb1.blue = (unsigned char)s1b;
		rgb2.red = (unsigned char)s2r;
		rgb2.green = (unsigned char)s2g;
		rgb2.blue = (unsigned char)s2b;
		rgb1.alpha = (unsigned char)s1a;
		rgb2.alpha = (unsigned char)s2a;
		if(sfxLogCSmooth < 40 &&
		   ((jR < -24 || jR > 24) || (jG < -24 || jG > 24) || (jB < -24 || jB > 24) ||
		    (kR < -24 || kR > 24) || (kG < -24 || kG > 24) || (kB < -24 || kB > 24) ||
		    (jA1 < -24 || jA1 > 24) || (jA2 < -24 || jA2 > 24))){
			sfxLogCSmooth++;
			sfxLogLine("CFSMOOTH jump1=%d,%d,%d,%d jump2=%d,%d,%d,%d\n",
				jR, jG, jB, jA1, kR, kG, kB, jA2);
		}
	}

	// v9.45: remember the colours actually drawn so a skipped frame can
	// be bridged from the coronas stub, and log the full filter colours
	// periodically (alpha included - previous logs were alpha-blind).
	sfxCFLast1 = rgb1;
	sfxCFLast2 = rgb2;
	sfxCFDrawOK = 1;
	if((sfxCFSeq & 15) == 0)
		sfxLogLine("CFC seq=%u c1=%d,%d,%d,%d c2=%d,%d,%d,%d\n",
			sfxCFSeq, rgb1.red, rgb1.green, rgb1.blue, rgb1.alpha,
			rgb2.red, rgb2.green, rgb2.blue, rgb2.alpha);

	vcsblurrgb = rgb2;

	int colorFilter = config->colorFilter;

	// VCS trails isn't compatible with PC/PS2 color filter, falls of to VCS color filter
	if (config->vcsTrails) {
		if (colorFilter == COLORFILTER_PC || colorFilter == COLORFILTER_PS2) {
			colorFilter = COLORFILTER_VCS;
		}
	}

	switch(colorFilter){
	case COLORFILTER_PS2:
		CPostEffects::ColourFilter_PS2(rgb1, rgb2);
		break;
	case COLORFILTER_PC:
		// this effects expects PC alphas
		CPostEffects::ColourFilter(rgb1pc, rgb2pc);
		break;
	case COLORFILTER_MOBILE:
		// this effects ignores alphas
		if(!UG_mod)
			CPostEffects::ColourFilter_Mobile(rgb1, rgb2);
		break;
	case COLORFILTER_III:
		// this effects expects PC alphas
		CPostEffects::ColourFilter_Generic(rgb1pc, rgb2pc, iiiTrailsPS);
		break;
	case COLORFILTER_VC:
		// this effects ignores alphas
		CPostEffects::ColourFilter_Generic(rgb1, rgb2, vcTrailsPS);
		break;
	case COLORFILTER_VCS:
		// this effects ignores alphas
		CPostEffects::ColourFilter_Generic(rgb1, rgb2, vcTrailsPS);
		break;
	default:
		return;
	}
	UpdateFrontBuffer();

	//static int doramp = 0;
	//{
	//	static bool keystate = false;
	//	if(GetAsyncKeyState(VK_F4) & 0x8000){
	//		if(!keystate){
	//			doramp = !doramp;
	//			keystate = true;
	//		}
	//	}else
	//		keystate = false;
	//}
	//if(doramp)
	//	renderRamp();
}

static RwMatrix RGB2YUV = {
	{  0.299f,	-0.168736f,	 0.500f }, 0,
	{  0.587f,	-0.331264f,	-0.418688f }, 0,
	{  0.114f,	 0.500f,	-0.081312f }, 0,
	{  0.000f,	 0.000f,	 0.000f }, 0,
};

static RwMatrix YUV2RGB = {
	{  1.000f,	 1.000f,	 1.000f }, 0,
	{  0.000f,	-0.344136f,	 1.772f }, 0,
	{  1.402f,	-0.714136f,	 0.000f }, 0,
	{  0.000f,	 0.000f,	 0.000f }, 0,
};

////
//// Bloom / exposure / tone map / PS2 dither
////

void *brightPS, *bloomBlurPS, *finalPS;

RwRaster *bloomRasterA, *bloomRasterB;
RwTexture *bloomTextureA, *bloomTextureB;
RwRaster *ditherRaster;
RwTexture *ditherTexture;
static int bloomLastW, bloomLastH, ditherLastW, ditherLastH;

static uint32 ditherRngState = 0x12345678;
static uint32
ditherRng(void)
{
	ditherRngState ^= ditherRngState << 13;
	ditherRngState ^= ditherRngState >> 17;
	ditherRngState ^= ditherRngState << 5;
	return ditherRngState;
}

// 8x8 blue noise via Munch's algorithm: start from a random permutation and
// repeatedly swap the value closest to the mean with the one farthest from it
static void
makeBlueNoise8x8(uint8 *out)
{
	uint8 v[64];
	int i;
	for(i = 0; i < 64; i++)
		v[i] = (uint8)i;
	for(i = 63; i > 0; i--){
		int j = ditherRng() % (i + 1);
		uint8 t = v[i]; v[i] = v[j]; v[j] = t;
	}
	for(int iter = 0; iter < 4096; iter++){
		int iclose = 0, ifar = 0;
		float dclose = 1e9f, dfar = -1e9f;
		for(i = 0; i < 64; i++){
			float d = fabsf((float)v[i] - 31.5f);
			if(d < dclose){ dclose = d; iclose = i; }
			if(d > dfar){ dfar = d; ifar = i; }
		}
		if(iclose != ifar){
			uint8 t = v[iclose]; v[iclose] = v[ifar]; v[ifar] = t;
		}
	}
	for(i = 0; i < 64; i++)
		out[i] = (uint8)((v[i] * 255) / 63);
}

static bool
ensureBloomBuffers(int w, int h)
{
	if(bloomRasterA && bloomLastW == w && bloomLastH == h)
		return true;
	// envmap.cpp pattern: the RwTexture persists, only the raster is
	// recreated and re-attached (RwTextureDestroy isn't wrapped)
	if(!bloomTextureA){
		bloomTextureA = RwTextureCreate(nil);
		bloomTextureB = RwTextureCreate(nil);
	}
	if(bloomRasterA) RwRasterDestroy(bloomRasterA);
	if(bloomRasterB) RwRasterDestroy(bloomRasterB);
	int depth = CPostEffects::pRasterFrontBuffer->depth;
	bloomRasterA = RwRasterCreate(w, h, depth, rwRASTERTYPECAMERATEXTURE);
	bloomRasterB = RwRasterCreate(w, h, depth, rwRASTERTYPECAMERATEXTURE);
	if(!bloomRasterA || !bloomRasterB)
		return false;
	RwTextureSetRaster(bloomTextureA, bloomRasterA);
	RwTextureSetRaster(bloomTextureB, bloomRasterB);
	bloomLastW = w;
	bloomLastH = h;
	return true;
}

// Render targets are switched the way the proven Radiosity_shader code does
// it: by swapping the camera raster. Never point RwD3D9SetRenderTarget at the
// camera raster itself - in this RW build its D3D render target surface is
// managed by the game, not the raster, and that code path dereferences a NULL
// surface (access violation inside RwD3D9SetRenderTarget, reported by users).
static void
setSceneRaster(RwRaster *r)
{
	RwCameraEndUpdate(Scene.camera);
	RwCameraSetRaster(Scene.camera, r);
	RwCameraBeginUpdate(Scene.camera);
}

// forward decl - the diagnostics helper lives further down in this file
static void sfxLogLine(const char *fmt, ...);

// dither pattern: the 8x8 blue-noise tile is written per SCREEN PIXEL into a
// full-size raster, so sampling it with the quad's 0..1 screen-space UVs
// gives one noise texel per pixel - per-pixel dithering like the PS2
// framebuffer. An earlier version stored the tile in a w/8 x h/8 raster
// instead: every texel then covered a flat 8x8-pixel block and the tile
// repeated every 64 pixels - big regular blotches all over the screen.
static bool
ensureDitherTexture(int w, int h)
{
	if(ditherRaster && ditherLastW == w && ditherLastH == h)
		return true;
	if(!ditherTexture)
		ditherTexture = RwTextureCreate(nil);
	if(ditherRaster) RwRasterDestroy(ditherRaster);
	ditherRaster = RwRasterCreate(w, h, 32, rwRASTERTYPECAMERATEXTURE);
	if(!ditherRaster){
		sfxLogLine("D dither: FAIL create %dx%d\n", w, h);
		return false;
	}
	uint8 noise[64];
	makeBlueNoise8x8(noise);
	RwUInt8 *pixels = RwRasterLock(ditherRaster, 0, 1);
	RwUInt8 *start = pixels;
	for(int y = 0; y < h; y++)
		for(int x = 0; x < w; x++){
			uint8 v = noise[(y & 7) * 8 + (x & 7)];
			*pixels++ = v;
			*pixels++ = v;
			*pixels++ = v;
			*pixels++ = 0xFF;
		}
	sfxLogLine("D dither: raster %dx%d per-pixel\n", w, h);
	sfxLogLine("D dither: tile row0 %02x %02x %02x %02x %02x %02x %02x %02x\n",
		noise[0], noise[1], noise[2], noise[3],
		noise[4], noise[5], noise[6], noise[7]);
	sfxLogLine("D dither: raster row0 %02x %02x %02x %02x %02x %02x %02x %02x\n",
		start[0], start[4], start[8], start[12],
		start[16], start[20], start[24], start[28]);
	RwRasterUnlock(ditherRaster);
	RwTextureSetRaster(ditherTexture, ditherRaster);
	ditherLastW = w;
	ditherLastH = h;
	return true;
}

////
//// Screen FX 2: PS2 grain+scanlines, auto exposure, night bloom boost,
//// renderScale (internal resolution)
////

// 128x128 texture with a per-pixel grain and a 2px scanline pattern pre-baked:
// g in [-1,1] = (noise-0.5)*1.2 (grain, range [-0.6, 0.6])
//               + (row%2 ? -0.5 : 0) (scanline darkens every other 2px row)
// stored as (g+1)/2 in 8-bit. 128 is even, so tiling keeps the 2px row parity
// aligned on screen at any resolution.
RwRaster *sfxGrainRaster;
RwTexture *sfxGrainTexture;
static int sfxGrainLastW, sfxGrainLastH;

// PS2-style grain + 2px scanlines, baked per screen pixel (same pattern as
// the dither texture: full screen size, rebuilt on resolution change).
static uint32 sfxGrainRng = 0x9E3779B9;
static uint32
sfxGrainRngNext(void)
{
	sfxGrainRng ^= sfxGrainRng << 13;
	sfxGrainRng ^= sfxGrainRng >> 17;
	sfxGrainRng ^= sfxGrainRng << 5;
	return sfxGrainRng;
}

static bool
ensureSfxGrainTexture(int w, int h)
{
	if(sfxGrainRaster && sfxGrainLastW == w && sfxGrainLastH == h)
		return true;
	if(!sfxGrainTexture)
		sfxGrainTexture = RwTextureCreate(nil);
	if(sfxGrainRaster) RwRasterDestroy(sfxGrainRaster);
	sfxGrainRaster = RwRasterCreate(w, h, 32, rwRASTERTYPECAMERATEXTURE);
	if(!sfxGrainRaster)
		return false;
	sfxGrainRng = 0x9E3779B9;
	uint8 *px = RwRasterLock(sfxGrainRaster, 0, 1);
	for(int y = 0; y < h; y++)
		for(int x = 0; x < w; x++){
			float n = (float)((sfxGrainRngNext() >> 8) & 0xFF) / 255.0f;
			float g = (n - 0.5f) * 1.2f;
			if(y & 1)
				g -= 0.5f;
			if(g < -1.0f) g = -1.0f;
			if(g > 1.0f) g = 1.0f;
			uint8 v = (uint8)((g + 1.0f) * 0.5f * 255.0f + 0.5f);
			*px++ = v;
			*px++ = v;
			*px++ = v;
			*px++ = 0xFF;
		}
	RwRasterUnlock(sfxGrainRaster);
	RwTextureSetRaster(sfxGrainTexture, sfxGrainRaster);
	sfxGrainLastW = w;
	sfxGrainLastH = h;
	return true;
}

// renderScale: render the scene into a smaller camera texture and upscale it
// in the final composite (softer PS2-like look + fewer scene pixels).
// RenderScale_Begin() is called from RenderScene_before() in main.cpp before
// the scene is drawn; DrawFinalEffects() stretches the scene into the front
// buffer again.
//
// v9 design: the scene raster, the depth surface and the camera are left
// EXACTLY as the game set them up - nothing is swapped. v8 proved that
// rendering into a swapped-in smaller camera raster is what breaks the frame
// under D3D9On12: its log showed raster and viewport perfectly in sync
// (1440x810/1440x810), yet every depth-tested mesh vanished while sky and
// ground haze, which render without z-test, survived - the depth surface
// that comes with the swapped raster does not work there. So the downscale
// is done with the D3D viewport alone: a 1440x810 viewport on the untouched
// 1600x900 raster remaps the whole NDC cube into that rectangle - same
// projection, same FOV, same image, just fewer pixels (fill rate is still
// saved, rasterization is bounded by the viewport). DrawFinalEffects() then
// stretches exactly that rectangle over the whole raster at the END of the
// scene (RenderScale_EndOfScene), so the frame is already full-size when the
// game draws its HUD - the HUD and every other 2D element keep their regular
// full-raster coordinates and can never be cropped or zoomed. (An earlier
// attempt kept the scaled viewport alive for the HUD phase instead, but RW
// immediate-mode 2D vertices are raster space and ignore the D3D viewport
// entirely, so that cannot move the HUD.) While the scene renders, every
// SetViewport issued at exactly the scene raster size is rewritten through
// the hooked vtable slot (anything else belongs to another pass's own
// target and passes through), and the scaled viewport is re-asserted after
// each projection change.
static uint8 sfxScaleActive, sfxScaleVtPatched, sfxScaleVtTried, sfxVpScaled;
static uint8 sfxScaleApplied; // frame uses the scaled viewport (DrawFinalEffects)
static uint8 sfxScaleInScene; // log only: scene phase vs HUD phase
static unsigned int sfxScaleW, sfxScaleH; // scaled viewport size (even)
static unsigned int sfxSceneW, sfxSceneH; // full scene raster size

struct SfxD3DViewport
{
	long x, y;
	unsigned int width, height;
	float minz, maxz;
}; // D3DVIEWPORT9 is 24 bytes - GetViewport() writes all of it
static struct SfxD3DViewport sfxVpFull;

// scratch for the upscale: the camera raster is the back buffer and can NOT
// be sampled as a texture (v9.1 sampled it and every sample came back
// white), so the frame is copied here 1:1 first and the scaled rectangle is
// stretched from here into the front buffer
static RwRaster *sfxStretchRaster;
static int sfxStretchW, sfxStretchH, sfxStretchDepth;

// D3D9 device vtable slots (IDirect3DDevice9 layout, d3d9.h order:
// ... EndScene=42, Clear=43, then):
//   44 = SetTransform(int type, const D3DMATRIX *)
//   45 = GetTransform(int type, D3DMATRIX *)
//   47 = SetViewport(const D3DVIEWPORT9 *)
//   48 = GetViewport(D3DVIEWPORT9 *)
// Earlier revisions hooked 8/9/15/16, which are GetDisplayMode /
// GetCreationParameters / GetNumberOfSwapChains / Reset - that is why the
// v7 log showed "install: OK" but never a single viewport/transform event.
// Device methods are declared without D3D9 SDK types (the plugin SDK
// include chain does not reliably provide the interfaces in this
// translation unit): opaque pointers + x86 __stdcall only.
// D3DMATRIX layout: { float m[4][4]; }
// D3DTS_VIEW = 2, D3DTS_PROJECTION = 3, D3D_OK = 0
typedef int (__stdcall *sfxD3D2ArgFn)(void *, void *);
typedef int (__stdcall *sfxD3D3ArgFn)(void *, int, void *);
static sfxD3D2ArgFn d3dSetViewportOrig;
static sfxD3D2ArgFn d3dGetViewport;
static sfxD3D3ArgFn d3dSetTransformOrig;
// v9.11: D3DTS_PROJECTION=44 in this table, so Clear=43 and
// GetDepthStencilSurface=40 (verified by the working SetTransform44 /
// SetViewport47 / GetViewport48 mapping). GetDepthStencilSurface is only
// called, never patched.
typedef int (__stdcall *sfxClearFn)(void*, unsigned int, void*, unsigned int, unsigned int, float, unsigned int);
static sfxClearFn d3dClearOrig;
static sfxD3D2ArgFn d3dGetDepthStencil;

// --- diagnostics (renderScaleDebugLog=1): append D3D state events to
// skygfx_renderScale.log in the game folder (first 20000 lines) ---
// (sfxLog moved above UpdateFrontBuffer in v9.30k - its diagnostic uses it)
static int sfxLogCount;
static int sfxLogV0, sfxLogT0, sfxLogOther; // caps for off-window events
static int sfxLogT, sfxLogC, sfxLogS, sfxLogP; // caps for in-window events
// v9.9: while the pre-stretch overlay pass runs, the vtable hooks must
// keep enforcing the scaled viewport exactly like during the scene -
// RW (Im3D/pipeline draws) can re-issue the camera's full-size viewport
// mid-pass, which silently undid v9.8's scaled one. sfxW2DPass marks
// that window for the vp-fix counter in the SetViewport hook.
static int sfxW2DPass;
static int sfxW2DVpFix;
// v9.11: while sfxW2DNoClear is set, the Clear hook (vtable slot 43)
// strips D3DCLEAR_ZBUFFER (0x100) from every Clear - the scene depth
// must survive from the end of the scene render until the world overlay
// pass has finished drawing (BeginUpdate in setSceneRaster may otherwise
// execute the camera's pending clear and wipe it). The sfxLogZ counters
// gate the depth diagnostics in the log.
static int sfxW2DNoClear;
static int sfxW2DZStripped;
static int sfxLogZ, sfxLogZ0, sfxLogDS;
static int sfxLogQ;
static int sfxLogB, sfxLogR, sfxLogPS;
static int sfxLogH;
static void sfxHDRbind(void);
// v9.19: the end-of-frame stretch is deferred out of RenderScale_EndOfScene
// into the swallowed CCoronas::Render stub - see the long note in
// RenderScale_EndOfScene. While sfxStretchPending is set the scale window
// (scaled viewport, shrunk RsGlobal and camera raster) stays open, so the
// world-space draws of RenderEffects - above all CMovingThings::Render,
// which is where Project2DFX renders its LOD lights - land in the sub-rect
// and z-test against the intact sub-rect depth instead of against the
// misaligned full-viewport depth the old immediate stretch left behind.
static int sfxStretchPending;
static RwRaster *sfxSavedFB;
static DWORD sfxSavedRsW, sfxSavedRsH;
static void RenderScale_DeferredStretch(void);
static void
sfxLogLine(const char *fmt, ...)
{
	if(!config->renderScaleDebugLog || sfxLogCount >= 20000)
		return;
	if(sfxLog == nil){
		sfxLog = fopen("skygfx_renderScale.log", "a");
		if(sfxLog == nil)
			return;
		fprintf(sfxLog, "==== skygfx renderScale diagnostics (build v9.69) ====\n");
	}
	va_list ap;
	va_start(ap, fmt);
	vfprintf(sfxLog, fmt, ap);
	va_end(ap);
	if((++sfxLogCount % 32) == 0)
		fflush(sfxLog);
}

// ------------------------------------------------------------------
// v9.30: HDR scene buffer, stage 1 (opt-in: hdrBuffer = 1)
//
// The scene always rendered into the 8-bit back buffer, so every value
// brighter than 1.0 was clamped before any post effect could see it.
// Stage 1 redirects the scale window into a full-size FP16 render
// target and resolves it back to the back buffer at the deferred
// stretch - one hardware linear quad, the format conversion and clamp
// happen in the sampler, so the picture is the stock one by
// construction. The POINT of stage 1 is the plumbing; the visible
// gains come in stage 2 when bloom/tonemap sample the FP16 data.
// Falls back to the stock path whenever hdrBuffer = 0 (default), the
// renderScale window is not active, the format can not be created, or
// a device call fails (stale resource after a reset - recreated).
// The depth stencil is kept bound across every target switch.
// ------------------------------------------------------------------
static void *sfxHDRtex;		// IDirect3DTexture9*, A16B16G16R16F, back buffer size
static void *sfxHDRsurf;	// its level-0 surface
static int sfxHDRon;		// active for this frame's scale window
static int sfxHDRw, sfxHDRh;
static int sfxHDRfailed;	// CreateTexture rejected the format
static int sfxHDRresolve(RwRaster *camR);
// v9.30f: sky capture. SA never colour-clears the main camera (log
// "Zc f=6" - depth+stencil only): the gradient, clouds, stars and
// sun/moon are drawn to the BACK BUFFER before the scale window opens
// (v9.30b they ghosted there, v9.30d the clear wiped them, v9.30e
// tried rerouting the CClouds::Render call - but the Idle region is
// already rewritten by other ASI mods: hook NOT FOUND, zero game
// bytes changed). v9.30f stops patching game code entirely: at window
// open the back buffer is copied (StretchRect + one quad) into the
// FP16 sub-rect, so the pre-window sky lands exactly where the
// resolve stretches it back - vanilla "nothing is ever cleared"
// semantics, no game bytes touched.
static int sfxLogCap;	// v9.30g: sky-call log cap (sfxLogS is taken, C2086)
// v9.30i blink/shadow diagnostics: frame counter + per-stream line budgets
static unsigned int sfxFrameNo;
static int sfxLogXF;
static int sfxLogDFE;
static int sfxLogVP;
static int sfxLogCL;
// (sfxLogCF moved to the top of ColourFilter_switch's section in v9.30j2 -
// its first use is above this point and MSVC rejects use-before-declaration)
static void sfxHDRclearFull(void);
static void sfxSkyDraw(const struct SfxD3DViewport *svp);

static void
sfxHDRrelease(void)
{
	if(sfxHDRtex)
		((IDirect3DTexture9*)sfxHDRtex)->Release();
	sfxHDRtex = nil;
	sfxHDRsurf = nil;
	sfxHDRready = 0;
}

static int
sfxHDRensure(int w, int h)
{
	HRESULT hr;
	if(sfxHDRtex){
		if(sfxHDRw == w && sfxHDRh == h)
			return sfxHDRsurf != nil;
		sfxHDRrelease();
	}
	if(sfxHDRfailed || d3d9device == nil)
		return 0;
	hr = d3d9device->CreateTexture(w, h, 1, D3DUSAGE_RENDERTARGET,
		D3DFMT_A16B16G16R16F, D3DPOOL_DEFAULT,
		(IDirect3DTexture9**)&sfxHDRtex, nil);
	if(hr != D3D_OK || sfxHDRtex == nil){
		sfxLogLine("HDR: fp16 %dx%d ABORT CreateTexture hr=%08x - feature off\n",
			w, h, (unsigned int)hr);
		sfxHDRfailed = 1;
		return 0;
	}
	((IDirect3DTexture9*)sfxHDRtex)->GetSurfaceLevel(0, (IDirect3DSurface9**)&sfxHDRsurf);
	if(sfxHDRsurf == nil){
		sfxLogLine("HDR: GetSurfaceLevel failed - feature off\n");
		sfxHDRrelease();
		sfxHDRfailed = 1;
		return 0;
	}
	sfxHDRw = w;
	sfxHDRh = h;
	sfxLogLine("HDR: fp16 %dx%d A16B16G16R16F ready\n", w, h);
	sfxHDRready = 1;
	return 1;
}

// bind the FP16 surface as render target 0, keeping the depth stencil
// bound across the switch (SetRenderTarget alone would drop it). Cheap
// and self-healing: shadow/env/radiosity passes bind their own targets
// mid-scene and RW re-binds the camera raster at camera begin, so this
// runs again from the SetViewport hook on every full-size rewrite.
static void
sfxHDRbind(void)
{
	IDirect3DSurface9 *cur = nil;
	IDirect3DSurface9 *ds = nil;
	HRESULT hr;
	if(!sfxHDRon || sfxHDRsurf == nil)
		return;
	if(d3d9device->GetRenderTarget(0, &cur) != D3D_OK)
		cur = nil;
	if(cur == (IDirect3DSurface9*)sfxHDRsurf){
		if(cur)
			cur->Release();
		return;
	}
	if(d3dGetDepthStencil)
		d3dGetDepthStencil(d3d9device, &ds);
	hr = d3d9device->SetRenderTarget(0, (IDirect3DSurface9*)sfxHDRsurf);
	if(ds){
		d3d9device->SetDepthStencilSurface(ds);
		ds->Release();
	}
	if(cur){
		cur->Release();
		if(hr == D3D_OK){
			if(sfxLogH++ < 2000)
				sfxLogLine("H bind -> fp16\n");
		}else{
			// stale resource after a device reset - drop and recreate
			sfxLogLine("H bind FAILED hr=%08x - recreating\n", (unsigned int)hr);
			sfxHDRrelease();
			sfxHDRon = 0;
		}
	}
}


// diagnostics only: the projection matrix itself is never modified (the
// viewport clamp below is the entire fix) - we just record what the game
// issues and while the scale window is open. D3DTS_PROJECTION = 3; the
// old code tested for 2, which is D3DTS_VIEW.
// diagnostics only for transforms - the projection matrix itself is never
// modified (the viewport is the entire fix). But right after each in-scene
// projection change we re-assert the scaled viewport, in case anything in
// between restored the full one. D3DTS_PROJECTION = 3.
static int __stdcall
sfxSetTransformHook(void *dev, int type, void *m)
{
	if(type == 3 /* D3DTS_PROJECTION */){
		if(sfxScaleActive){
			if(sfxLogT++ < 8)
				sfxLogLine("T proj win=1\n");
			if(sfxVpScaled && d3dSetViewportOrig && sfxLogP++ < 8){
				struct SfxD3DViewport c = {0, 0, sfxScaleW, sfxScaleH, 0.0f, 1.0f};
				d3dSetViewportOrig(dev, &c);
				sfxLogLine("P reassert %ux%u\n", sfxScaleW, sfxScaleH);
			}
			sfxHDRbind();
		}else if(sfxLogT0++ < 8)
			sfxLogLine("T0 proj win=0\n");
	}else if(sfxLogOther++ < 6)
		sfxLogLine("t type=%d win=%d\n", type, (int)sfxScaleActive);
	return d3dSetTransformOrig(dev, type, m);
}

// the whole fix: while the scale window is open (scene + HUD phase, up to
// DrawFinalEffects), every SetViewport issued at exactly the full scene
// raster size is rewritten to the scaled viewport before it reaches the
// device - that call is RW re-beginning the camera, or the game starting a
// full-screen pass (HUD, radar), and all of it must land in the scaled
// rectangle. Viewports of any other size belong to other passes' own
// targets (shadow maps, env maps, the 2048x1024 post buffer) and pass
// through untouched.
static int __stdcall
sfxSetViewportHook(void *dev, void *vp)
{
	struct SfxD3DViewport *v = (struct SfxD3DViewport*)vp;
	// v9.30i: every viewport for the first 20 frames - shows the shadow
	// passes' small viewports and any full-size passthrough vs the window
	if(sfxFrameNo < 20 && sfxLogVP++ < 1000)
		sfxLogLine("VP f=%u %ux%u win=%d inSc=%d\n",
			sfxFrameNo, v->width, v->height, (int)sfxScaleActive, (int)sfxScaleInScene);
	if(sfxScaleActive){
		if(v->width == sfxSceneW && v->height == sfxSceneH){
			struct SfxD3DViewport c = {0, 0, sfxScaleW, sfxScaleH, 0.0f, 1.0f};
			if(sfxW2DPass)
				sfxW2DVpFix++;
			if(sfxLogC++ < 16)
				sfxLogLine("%c vp=%ux%u -> %ux%u\n",
					sfxScaleInScene ? 'C' : 'H',
					v->width, v->height, c.width, c.height);
			sfxHDRbind();
			return d3dSetViewportOrig(dev, &c);
		}
		if(sfxLogS++ < 8)
			sfxLogLine("S vp=%ux%u win=%d\n",
				v->width, v->height, (int)sfxScaleInScene);
	}else if(sfxLogV0++ < 8)
		sfxLogLine("V0 vp=%ux%u\n", v->width, v->height);
	return d3dSetViewportOrig(dev, vp);
}

// v9.11: depth protection - during the world overlay window any
// D3DCLEAR_ZBUFFER is dropped so the sub-rect depth survives
static int __stdcall
sfxClearHook(void *dev, unsigned int count, void *rects,
	unsigned int flags, unsigned int color, float z, unsigned int stencil)
{
	// v9.30i: every clear for the first 20 frames - shadow/scene clears
	// vs the scale window, next to the VP trace
	if(sfxFrameNo < 20 && sfxLogCL++ < 1000)
		sfxLogLine("CL f=%u flags=%x win=%d\n", sfxFrameNo, flags, (int)sfxScaleActive);
	// v9.30c: RW binds the camera raster (the back buffer) at every
	// camera begin and THEN issues the camera's clear - so without
	// this, the FP16 target was never cleared and every frame drew
	// over all previous ones (the ghosting screenshots). If a colour
	// clear is issued while the back buffer is bound and the HDR
	// scene window is open, switch to the FP16 target first so the
	// clear - and the draws that follow, until the next full-size
	// viewport rewrite re-asserts the bind - land in the HDR buffer.
	// Any other bound target (env maps, shadow cameras) is untouched.
	if(sfxHDRon && sfxScaleInScene && (flags & 0x1u)){
		IDirect3DSurface9 *cur = nil, *bb = nil;
		if(d3d9device->GetRenderTarget(0, &cur) == D3D_OK && cur != nil){
			if(d3d9device->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &bb) == D3D_OK
					&& bb != nil){
				if(cur == bb)
					sfxHDRbind();
				bb->Release();
			}
			cur->Release();
		}
	}
	if(sfxW2DNoClear && (flags & 0x100u)){
		flags &= ~0x100u;
		sfxW2DZStripped++;
		if(sfxLogZ++ < 8)
			sfxLogLine("Zc strip f=%x\n", flags);
	}else if(sfxLogZ0++ < 8)
		sfxLogLine("Zc f=%x c=%x\n", flags, color);
	// v9.64: pure passthrough again - the v9.63 logs proved a paused
	// frame never issues a colour clear at all (only flags=6 Z+stencil
	// clears), so a clear-side hold had nothing to attach to. The fix
	// lives entirely in the DFE gate now.
	return d3dClearOrig(dev, count, rects, flags, color, z, stencil);
}

static void
sfxScaleInstallVtableHook(void)
{
	if(sfxScaleVtPatched || sfxScaleVtTried || d3d9device == nil)
		return;
	// the vtable is the FIRST member of the device object; the object
	// itself must never be written (earlier revisions corrupted it)
	void **vt = (void**)(*(void**)d3d9device);
	sfxLogLine("install: dev=%08x vt=%08x\n",
		(unsigned int)(void*)d3d9device, (unsigned int)vt);
	if(vt == nil){
		sfxScaleVtTried = 1;
		return;
	}
	d3dSetTransformOrig = (sfxD3D3ArgFn)vt[44];
	d3dSetViewportOrig = (sfxD3D2ArgFn)vt[47];
	d3dGetViewport = (sfxD3D2ArgFn)vt[48];
	d3dClearOrig = (sfxClearFn)vt[43];
	d3dGetDepthStencil = (sfxD3D2ArgFn)vt[40];
	sfxLogLine("install: orig SetTransform44=%08x SetViewport47=%08x GetViewport48=%08x Clear43=%08x DS40=%08x\n",
		(unsigned int)(void*)d3dSetTransformOrig,
		(unsigned int)(void*)d3dSetViewportOrig,
		(unsigned int)(void*)d3dGetViewport,
		(unsigned int)(void*)d3dClearOrig,
		(unsigned int)(void*)d3dGetDepthStencil);
	// plausibility check: the "original" methods must be normal 32-bit
	// code pointers. No module is assumed here: a d3d9-on-d12 style
	// wrapper may implement the device in a different module.
	if((unsigned int)(void*)d3dSetViewportOrig < 0x10000
		|| (unsigned int)(void*)d3dSetViewportOrig >= 0x80000000
		|| (unsigned int)(void*)d3dGetViewport < 0x10000
		|| (unsigned int)(void*)d3dGetViewport >= 0x80000000
		|| (unsigned int)(void*)d3dSetTransformOrig < 0x10000
		|| (unsigned int)(void*)d3dSetTransformOrig >= 0x80000000
		|| (unsigned int)(void*)d3dClearOrig < 0x10000
		|| (unsigned int)(void*)d3dClearOrig >= 0x80000000){
		sfxLogLine("install: ABORT - original pointers not plausible\n");
		sfxScaleVtTried = 1;
		return;
	}
	// the vtable lives in a read-only section, so its page has to be
	// made writable just for the patch and restored afterwards
	size_t span = (size_t)((char*)&vt[48] - (char*)&vt[43]);
	DWORD oldProt = 0;
	if(!VirtualProtect(&vt[44], span, PAGE_READWRITE, &oldProt)){
		sfxLogLine("install: ABORT - VirtualProtect failed (err=%u)\n",
			(unsigned int)GetLastError());
		sfxScaleVtTried = 1;
		return;
	}
	Patch((void*)(&vt[43]), (void*)sfxClearHook);
	Patch((void*)(&vt[47]), (void*)sfxSetViewportHook);
	Patch((void*)(&vt[44]), (void*)sfxSetTransformHook);
	VirtualProtect(&vt[44], span, oldProt, &oldProt);
	// verify the entries really took (read them back through the table)
	if(vt[47] != (void*)sfxSetViewportHook || vt[44] != (void*)sfxSetTransformHook
			|| vt[43] != (void*)sfxClearHook){
		sfxLogLine("install: ABORT - readback mismatch s43=%08x s44=%08x s47=%08x\n",
			(unsigned int)(void*)vt[43], (unsigned int)(void*)vt[44],
			(unsigned int)(void*)vt[47]);
		sfxScaleVtTried = 1;
		return;
	}
	sfxLogLine("install: OK - vtable hooked SetTransform44=%08x SetViewport47=%08x Clear43=%08x\n",
		(unsigned int)(void*)sfxSetTransformHook,
		(unsigned int)(void*)sfxSetViewportHook,
		(unsigned int)(void*)sfxClearHook);
	sfxScaleVtPatched = 1;
	sfxScaleVtTried = 1;
}

// ---------------------------------------------------------------------------
// v9.8: the night world overlays that depth-test against the scene
// (CCoronas::Render - street/car lamp coronas, CBrightLights::Render -
// building windows, CShinyTexts::Render - neon signs, C3dMarkers::Render)
// must be drawn BEFORE the end-of-scene stretch AND in the sub-rect
// coordinate space. The scene renders into the top-left sfxScaleW x
// sfxScaleH of the back buffer and only the colour is stretched to full
// size; the depth buffer stays in the sub-rect layout. The overlays use
// three different coordinate sources, so all three have to point into
// the sub-rect while they are drawn early:
//  - pipeline and RwIm3D draws (shiny texts, bright lights, markers)
//    follow the D3D viewport;
//  - CSprite::CalcScreenCoors, which positions the coronas, scales by
//    RsGlobal->MaximumWidth/Height (the SCREEN_WIDTH/HEIGHT macros);
//  - anything reading RwCameraGetRaster(Scene.camera) follows the
//    camera raster dims (flare layout, off-screen culling).
// Drawn after the stretch their z no longer matches the colour frame -
// the "lights shine through buildings at night" bug at renderScale < 1.0
// (v9.7 already drew them early, but still in full-raster coordinates,
// so the lights moved with the stretch and z-tested against the wrong
// pixels). The hooks below let skygfx run the real render functions
// right before the stretch with viewport/raster/RsGlobal shrunk to the
// sub-rect, and swallow the game's own (now duplicate) calls for the
// rest of the frame. When renderScale is 1.0 nothing is ever flagged
// drawn and the hooks just pass through.
typedef void (*sfxVoidCall)();
struct SfxW2DHook {
	unsigned int addr;		// game function, 1.0 US
	unsigned char orig[5];	// original first 5 bytes
	unsigned char jmp[5];		// our E9 jmp
	int ok;
};
static SfxW2DHook sfxW2Dhooks[4] = {
	{0x6FAEC0, {0}, {0}, 0},	// CCoronas::Render
	{0x7241C0, {0}, {0}, 0},	// CBrightLights::Render
	{0x724890, {0}, {0}, 0},	// CShinyTexts::Render
	{0x725040, {0}, {0}, 0},	// C3dMarkers::Render
};
static int sfxW2Dinstalled;
static int sfxWorld2DDrawn;	// overlays already drawn early this frame
static int sfxW2Dlogs;
static RwRaster *sfxW2DDimsRaster;	// sub-rect sized raster - only its
static int sfxW2DDimsW, sfxW2DDimsH;	// width/height are ever read

static RwRaster *
ensureW2DDimsRaster(int w, int h, RwInt32 depth)
{
	if(sfxW2DDimsRaster && sfxW2DDimsW == w && sfxW2DDimsH == h)
		return sfxW2DDimsRaster;
	if(sfxW2DDimsRaster)
		RwRasterDestroy(sfxW2DDimsRaster);
	sfxW2DDimsRaster = RwRasterCreate(w, h, depth, rwRASTERTYPECAMERATEXTURE);
	if(!sfxW2DDimsRaster)
		return nil;
	sfxW2DDimsW = w;
	sfxW2DDimsH = h;
	return sfxW2DDimsRaster;
}

static void
sfxW2Dwrite(int i, const unsigned char *bytes)
{
	DWORD old;
	VirtualProtect((void*)sfxW2Dhooks[i].addr, 5, PAGE_EXECUTE_READWRITE, &old);
	memcpy((void*)sfxW2Dhooks[i].addr, bytes, 5);
	VirtualProtect((void*)sfxW2Dhooks[i].addr, 5, old, &old);
}

// run the original body with the hook temporarily removed
static void
sfxW2DcallOrig(int i)
{
	sfxW2Dwrite(i, sfxW2Dhooks[i].orig);
	((sfxVoidCall)sfxW2Dhooks[i].addr)();
	sfxW2Dwrite(i, sfxW2Dhooks[i].jmp);
}

static void sfxW2Dcoronas(void){ if(sfxStretchPending) RenderScale_DeferredStretch(); if(!sfxWorld2DDrawn) sfxW2DcallOrig(0); }
static void sfxW2Dbright(void){ if(!sfxWorld2DDrawn) sfxW2DcallOrig(1); }
static void sfxW2Dshiny(void){ if(!sfxWorld2DDrawn) sfxW2DcallOrig(2); }
static void sfxW2Dmarkers(void){ if(!sfxWorld2DDrawn) sfxW2DcallOrig(3); }

static void
sfxW2Dinstall(void)
{
	void *hooks[4] = {
		(void*)sfxW2Dcoronas, (void*)sfxW2Dbright,
		(void*)sfxW2Dshiny, (void*)sfxW2Dmarkers
	};
	int i;
	if(sfxW2Dinstalled)
		return;
	sfxW2Dinstalled = 1;
	for(i = 0; i < 4; i++){
		SfxW2DHook *h = &sfxW2Dhooks[i];
		memcpy(h->orig, (void*)h->addr, 5);
		if(h->orig[0] == 0xE9){
			// something else hooked this function already - leave it alone
			sfxLogLine("install: world2d %d @%08x skip - already hooked\n", i, h->addr);
			continue;
		}
		InjectHook(h->addr, hooks[i], PATCH_JUMP);
		memcpy(h->jmp, (void*)h->addr, 5);
		if(h->jmp[0] != 0xE9 || memcmp(h->jmp, h->orig, 5) == 0){
			sfxW2Dwrite(i, h->orig);
			sfxLogLine("install: world2d %d @%08x ABORT - patch did not take\n", i, h->addr);
			continue;
		}
		h->ok = 1;
	}
	sfxLogLine("install: world2d ok=%d%d%d%d (coronas bright shiny markers)\n",
		sfxW2Dhooks[0].ok, sfxW2Dhooks[1].ok,
		sfxW2Dhooks[2].ok, sfxW2Dhooks[3].ok);
}

// ------------------------------------------------------------------
// v9.29: real-time shadow upgrade (opt-in, hardened full-chain patch + softness)
//
// History: v9.21 (entry trampolines) crashed inside
// CRealTimeShadow::Create. v9.22 required a push DIRECTLY in front of
// the call and matched zero sites (MSVC puts the thiscall ECX load
// between the argument push and the call). v9.22c found the pushes
// with a 16 byte window but crashed inside Manager::Init. v9.23
// tightened to one byte with the same 16 byte window and crashed with
// EIP = 0x1 - a jump into garbage, which is what a byte written into
// the middle of an unrelated instruction produces: the loose window
// could match a stale 6A 07 that was not the argument push at all.
// v9.24 therefore hardens everything:
//   - the push must sit within 7 bytes in front of the call
//     (push 7 + up to 5 bytes of thiscall ECX setup)
//   - if the gap is non empty its first byte must be a plausible
//     setup opcode (8B/8D mov-lea or 50..57 push)
//   - the create functions must not already be jmp-hooked (E9) by
//     another mod, otherwise the code region may be relocated
//   - everything is logged: the push address and its distance to the
//     call, so the log can be diagnosed without a debugger
// The patched byte is still only the "push 7" argument of the FIRST
// CShadowCamera::Create call inside CRealTimeShadow::Create - the
// entity shadow raster, the source of the visible ground shadow.
// Manager::Init, gradient/blur rasters and blur passes stay 100%
// vanilla. DEFAULT IS NOW 7 = VANILLA: nothing is patched unless the
// ini explicitly opts in with shadowResolution >= 8, which should be
// treated as experimental (three crashes on three designs point at
// the environment, not the idea - the new logging will tell).
// shadowSoftness and shadowAllEntities stay reserved (unused).
// v9.26: ROOT CAUSE OF EVERY CRASH SINCE v9.22 FOUND - it was our own
// write, not the environment. sfxRTfindPush7 returns the address of
// the 0x6A OPCODE byte, but the value was written to that same
// address instead of the immediate at p+1: "push 7" (6A 07) turned
// into "or [edi], al" (08 07 with res 8), the argument push vanished
// and CShadowCamera::Create was called with a stale stack argument.
// res = 7 never wrote anything, which is why it was always stable,
// and any other value always crashed the same way everywhere. The
// write now targets p+1 and is gated by an explicit 6A 07 check so a
// wrong site aborts instead of corrupting the instruction stream.
// v9.27: v9.26 proved the mechanism on this setup (stable at res 8),
// so the SECOND byte goes in as well: the blur camera raster inside
// CRealTimeShadow::Create (the vanilla "push 6" - that texture is
// what is actually drawn on the ground) scales to res-1, doubling
// the visible ground shadow resolution from 64x64 to 128x128 at
// res = 8. Every write is read back and verified in the log.
// v9.27 result: stable, both writes verified - but the shadows
// DISAPPEARED. Raising only the per object blur raster breaks the
// chain: the manager level blur and gradient cameras are still
// 64x64 and the soften/gradient path mixes surfaces of mismatched
// size. v9.28 therefore also scales the two "push 6" cameras inside
// CRealTimeShadowManager::Init so every camera in the pipeline
// matches. If the shadows still do not come back, the ground
// texture theory is wrong and the final build stays entity-only.
// v9.29: v9.28 completed the chain and the shadows came back sharper
// (four sites, all readback verified) - the feature is validated on
// this setup. Fifth and last knob: the blur pass count of
// CRealTimeShadow::Create (the vanilla "push 4") = shadowSoftness,
// -1 = vanilla 4 passes (nothing written), 1..8: fewer passes are
// crisper, more are softer. The byte dumps now only run for non
// vanilla configurations to keep the default log small.
// shadowAllEntities stays NOT IMPLEMENTED (no effect).
// ------------------------------------------------------------------
static unsigned int sfxShadowCamCreateAddr = 0x705B60;	// CShadowCamera::Create(int)
static unsigned char sfxRTShadowApplied;

static void
sfxRTwriteByte(unsigned int addr, unsigned char b)
{
	DWORD old;
	VirtualProtect((void*)addr, 1, PAGE_EXECUTE_READWRITE, &old);
	*(unsigned char*)addr = b;
	VirtualProtect((void*)addr, 1, old, &old);
}

// collect the addresses of "E8 call target" inside the given range
static int
sfxRTfindCalls(unsigned int start, unsigned int end, unsigned int target,
		unsigned int *out, int maxOut)
{
	unsigned int a;
	int n = 0;
	for(a = start; a < end - 6 && n < maxOut; a++){
		unsigned char *p = (unsigned char*)a;
		if(p[0] == 0xE8 && (unsigned int)(a + 5 + *(int *)(p + 1)) == target)
			out[n++] = a;
	}
	return n;
}

// the argument push for the call at callAddr: a "6A 07" within 7 bytes
// in front of it, optionally followed by a thiscall ECX setup. Returns
// the push address or 0.
static unsigned int
sfxRTfindPushImm(unsigned int callAddr, unsigned char imm)
{
	unsigned int p, best = 0;
	unsigned int minA = callAddr >= 7 ? callAddr - 7 : callAddr;
	for(p = minA; p + 2 <= callAddr; p++){
		unsigned char *q = (unsigned char*)p;
		if(q[0] == 0x6A && q[1] == imm)
			best = p;
	}
	if(best != 0){
		int dist = (int)(callAddr - best);
		if(dist > 2){
			unsigned char g = ((unsigned char*)best)[2];
			if(g != 0x8B && g != 0x8D && (g < 0x50 || g > 0x57))
				return 0;	// gap does not look like a this pointer setup
		}
	}
	return best;
}

// one-time hex dump of the shadow code regions, so the byte layout of
// THIS exe can be checked in the log without a debugger - if these
// bytes do not match the vanilla 1.0 US layout, the exe is modified
// and every literal patch assumption is void
static void
sfxRTdump(const char *name, unsigned int start, unsigned int end)
{
	unsigned int a;
	for(a = start; a < end; a += 16){
		sfxLogLine("RT dump %s @%08x: %02X%02X %02X%02X %02X%02X %02X%02X %02X%02X %02X%02X %02X%02X %02X%02X\n",
			name, a,
			((unsigned char*)a)[0], ((unsigned char*)a)[1],
			((unsigned char*)a)[2], ((unsigned char*)a)[3],
			((unsigned char*)a)[4], ((unsigned char*)a)[5],
			((unsigned char*)a)[6], ((unsigned char*)a)[7],
			((unsigned char*)a)[8], ((unsigned char*)a)[9],
			((unsigned char*)a)[10], ((unsigned char*)a)[11],
			((unsigned char*)a)[12], ((unsigned char*)a)[13],
			((unsigned char*)a)[14], ((unsigned char*)a)[15]);
	}
}

void
rtshadowhooks(void)
{
	int res = config->shadowResolution;
	unsigned int calls[4], mfcalls[4], ccalls[4];
	unsigned int p, bp;
	int blurPow, soft;
	if(sfxRTShadowApplied)
		return;
	sfxRTShadowApplied = 1;
	if(res != 7 || config->shadowSoftness >= 0){
		sfxRTdump("Create", 0x706460, 0x706520);
		sfxRTdump("Init", 0x7067C0, 0x706870);
		sfxRTdump("CamCreate", 0x705B60, 0x705B80);
		sfxRTdump("CamDtor", 0x705990, 0x705A20);
		sfxRTdump("CamCreate2", 0x705B80, 0x705C80);
		sfxRTdump("Gap520", 0x706520, 0x7067C0);
		sfxRTdump("ReInit+DoShadow", 0x706870, 0x706D40);
		sfxRTdump("HelperLight", 0x751A90, 0x751AD0);
		sfxRTdump("HelperFirst", 0x752110, 0x752150);
		sfxRTdump("RasterFn", 0x7EE4F0, 0x7EE550);
		sfxRTdump("LightFn", 0x7F0410, 0x7F0430);
		sfxRTdump("CopyFn", 0x804EF0, 0x804F10);
	}
	if(res < 6) res = 6;
	if(res > 10) res = 10;
	soft = config->shadowSoftness;
	if(soft == 0) soft = 1;
	if(soft > 8) soft = 8;

	if(((unsigned char*)0x706460)[0] == 0xE9 ||
	   ((unsigned char*)sfxShadowCamCreateAddr)[0] == 0xE9){
		sfxLogLine("install: rtshadow ABORT - create functions already hooked\n");
		return;
	}
	if(sfxRTfindCalls(0x706460, 0x706520, sfxShadowCamCreateAddr, calls, 4) != 2){
		sfxLogLine("install: rtshadow ABORT - Create call count\n");
		return;
	}
	p = sfxRTfindPushImm(calls[0], 7);
	if(p == 0){
		sfxLogLine("install: rtshadow ABORT - push 7 not found (hardened window, call@%08x)\n", calls[0]);
		return;
	}
	if(((unsigned char*)p)[0] != 0x6A || ((unsigned char*)p)[1] != 0x07){
		sfxLogLine("install: rtshadow ABORT - site is not 6A 07 @%08x\n", p);
		return;
	}
	if(res == 7 && soft < 0){
		sfxLogLine("install: rtshadow ok res=7 soft=vanilla (fully vanilla - nothing patched, push@%08x dist=%d)\n",
			p, (int)(calls[0] - p));
		return;
	}
	sfxLogLine("install: rtshadow EXPERIMENTAL res=%d push@%08x call@%08x dist=%d\n",
		res, p, calls[0], (int)(calls[0] - p));

	// site 1: the entity shadow raster (source of the blur chain)
	sfxRTwriteByte(p + 1, (unsigned char)res);	// v9.26 FIX: the immediate, not the opcode
	sfxLogLine("RT shadow: entity push 7 -> %d @%08x imm@%08x readback=%d (dist %d)\n",
		res, p, p + 1, (int)((unsigned char*)p)[1], (int)(calls[0] - p));

	// site 2 (v9.27): the blur camera raster - the texture actually
	// drawn on the ground; scale it to res-1 (min 6 = vanilla 64x64)
	blurPow = res - 1;
	if(blurPow < 6) blurPow = 6;
	bp = sfxRTfindPushImm(calls[1], 6);
	if(bp == 0 || ((unsigned char*)bp)[0] != 0x6A || ((unsigned char*)bp)[1] != 0x06){
		sfxLogLine("install: rtshadow blur site not found (call@%08x) - entity raster only\n", calls[1]);
	}else if(blurPow == 6){
		sfxLogLine("install: rtshadow blur ok res=%d (vanilla 6 - nothing patched)\n", res);
	}else{
		sfxRTwriteByte(bp + 1, (unsigned char)blurPow);
		sfxLogLine("RT shadow: blur push 6 -> %d @%08x imm@%08x readback=%d\n",
			blurPow, bp, bp + 1, (int)((unsigned char*)bp)[1]);
	}
	// sites 3+4 (v9.28): the manager level blur and gradient cameras
	// (CRealTimeShadowManager::Init) - raise them too so the whole
	// chain runs at matching sizes
	if(sfxRTfindCalls(0x7067C0, 0x706870, sfxShadowCamCreateAddr, mfcalls, 4) == 2){
		int mi;
		for(mi = 0; mi < 2; mi++){
			unsigned int mp = sfxRTfindPushImm(mfcalls[mi], 6);
			if(mp == 0 || ((unsigned char*)mp)[0] != 0x6A || ((unsigned char*)mp)[1] != 0x06){
				sfxLogLine("install: rtshadow manager site %d not found (call@%08x)\n", mi, mfcalls[mi]);
			}else if(blurPow == 6){
				sfxLogLine("install: rtshadow manager site %d ok res=%d (vanilla 6 - nothing patched)\n", mi, res);
			}else{
				sfxRTwriteByte(mp + 1, (unsigned char)blurPow);
				sfxLogLine("RT shadow: manager push 6 -> %d @%08x imm@%08x readback=%d\n",
					blurPow, mp, mp + 1, (int)((unsigned char*)mp)[1]);
			}
		}
	}else{
		sfxLogLine("install: rtshadow manager call count mismatch - skipped\n");
	}
	// site 5 (v9.29): the blur pass count of CRealTimeShadow::Create
	// (the vanilla "push 4") - shadowSoftness. The call is the only
	// one in Manager::Init that targets 0x706460, and the push sits a
	// few bytes in front of it between two "push 1"s, so it is
	// verified by its 01 6A 04 6A context before anything is written.
	if(soft >= 0){
		if(sfxRTfindCalls(0x7067C0, 0x706870, 0x706460, ccalls, 4) == 1){
			unsigned int q, sp = 0;
			for(q = ccalls[0] >= 10 ? ccalls[0] - 10 : ccalls[0]; q + 3 <= ccalls[0]; q++){
				unsigned char *b = (unsigned char*)q;
				if(b[0] == 0x01 && b[1] == 0x6A && b[2] == 0x04 && b[3] == 0x6A)
					sp = q + 1;
			}
			if(sp != 0 && ((unsigned char*)sp)[0] == 0x6A && ((unsigned char*)sp)[1] == 0x04){
				sfxRTwriteByte(sp + 1, (unsigned char)soft);
				sfxLogLine("RT shadow: blur passes 4 -> %d @%08x imm@%08x readback=%d\n",
					soft, sp, sp + 1, (int)((unsigned char*)sp)[1]);
			}else{
				sfxLogLine("install: rtshadow softness site not found (call@%08x) - passes untouched\n", ccalls[0]);
			}
		}else{
			sfxLogLine("install: rtshadow softness call count mismatch - passes untouched\n");
		}
	}
	sfxLogLine("install: rtshadow ok res=%d blur=%d soft=%d (literal patch, readback verified)\n", res, blurPow, soft);
}

// v9.30f: clear the FP16 buffer over the whole raster rect (Clear
// honours the viewport). It deliberately does NOT save/restore the
// viewport - SetRenderTarget left the FULL one on the device and the
// CALLER must re-assert the scaled one right after (v9.30e lesson:
// "restore what GetViewport says" restores the FULL one after a bind
// and the frame renders unscaled = zoom).
static void
sfxHDRclearFull(void)
{
	struct SfxD3DViewport full = {0, 0, (unsigned int)sfxHDRw, (unsigned int)sfxHDRh, 0.0f, 1.0f};
	d3dSetViewportOrig(d3d9device, &full);
	d3d9device->Clear(0, nil, D3DCLEAR_TARGET, 0xFF000000, 1.0f, 0);
}

// ---- v9.30n: safe content probes (beige-aware, freeze-aware) --------
// v9.30m proved the readback path (GetRenderTargetData into a SYSTEMMEM
// surface) works on the user's machine and touches nothing. Its
// classification was wrong though: the beige-out is NOT pure white
// (red channel roughly 150-210 with a vignette gradient), so it fell
// between the WHITE and BLACK tests and stayed silent. v9.30n adds an
// explicit BEIGE band, samples the back buffer EVERY frame to detect
// frozen output (identical hash while the user keeps moving the
// camera - the warm snap marker) and prints on every verdict change.
// Menu frames (uniform black) print nothing at all.
static int sfxProbeBBLast = -1;	// verdict: -1 unknown, 0 normal, 1 white, 2 black, 3 beige
static int sfxProbeBBBurst;
static unsigned int sfxProbeBBHash;	// previous frame's hash
static int sfxProbeBBSame;			// consecutive identical-frame count
static int sfxProbeBBFail;
static int sfxProbeHLast = -1;
static int sfxProbeHBurst;
static int sfxProbeHFail;
static int sfxProbeGame;			// set once the resolve has run (gameplay)
static int sfxProbeBudget = 3000;

static void
sfxProbeOut(const char *tag, unsigned int h, int mn, int mx, int av,
            unsigned int p0, unsigned int pm, const char *vs, const char *fs)
{
	if(sfxProbeBudget-- <= 0)
		return;
	sfxLogLine("PROBE %s f=%u pf=%d h=%08x mn=%d mx=%d av=%d p0=%08x pm=%08x%s%s\n",
		tag, sfxFrameNo, sfxProbeFrame, h, mn, mx, av, p0, pm, vs, fs);
}

// Truth probe: what the player actually sees - the back buffer right
// after the final composite quad, before HUD/fade draw on top.
// Sampled EVERY frame (one GPU->CPU copy), printed on verdict changes,
// short bursts, baselines every 90 frames and on freeze repeats.
static void
sfxProbeBackBuffer(void)
{
	IDirect3DDevice9 *dev = d3d9device;
	IDirect3DSurface9 *bb = nil, *sys = nil;
	D3DSURFACE_DESC d;
	D3DLOCKED_RECT lr;
	unsigned int s = 0, p0 = 0, pm = 0;
	unsigned int f = sfxProbeFrame++;
	const char *vs, *fs;
	int i, mn = 255, mx = 0, sum = 0, sumR = 0, v;
	if(dev == nil)
		return;
	// v9.34: sample every 2nd frame - freeze episodes last many
	// frames, a 2-frame grid still catches them, at half the cost.
	if((sfxProbeFrame & 1) != 0){
		sfxProbeFrame++;
		return;
	}
	if(dev->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &bb) != D3D_OK || bb == nil)
		return;
	if(bb->GetDesc(&d) != D3D_OK ||
	   dev->CreateOffscreenPlainSurface(d.Width, d.Height, d.Format,
	                                    D3DPOOL_SYSTEMMEM, &sys, nil) != D3D_OK ||
	   dev->GetRenderTargetData(bb, sys) != D3D_OK ||
	   sys->LockRect(&lr, nil, D3DLOCK_READONLY) != D3D_OK){
		if(!sfxProbeBBFail){
			sfxProbeBBFail = 1;
			sfxLogLine("PROBE BB readback unavailable - probe off\n");
		}
		if(sys) sys->Release();
		bb->Release();
		return;
	}
	for(i = 0; i < 64; i++){
		unsigned int c = *(unsigned int*)((unsigned char*)lr.pBits
			+ (d.Height/16 + (i>>3)*(d.Height/8))*lr.Pitch
			+ (d.Width/16 + (i&7)*(d.Width/8))*4);
		int r = (int)(c & 0xFF);
		s = s*31 + c;
		sum += r;
		if(r > mx) mx = r;
		sumR += (int)((c >> 16) & 0xFF);
		if(r < mn) mn = r;
		if(i == 0) p0 = c;
		if(i == 36) pm = c;
	}
	sys->UnlockRect();
	sys->Release();
	bb->Release();
	// classify. v9.36c: the yellow full-screen state is a CHANNEL
	// property - saturated yellow has blue ~0 and red ~255, which
	// blue-only stats reported as BLACK or missed entirely; red vs
	// blue averages separate it from night (all channels low). The
	// 9.36a run also proved the old BEIGE test fires on a bright sky
	// (f=529 p0=ff6199e1), so beige now requires warmth: avgR > avgB.
	if(sumR/64 > 110 && sum/64 < 60)	v = 4;	// yellow-out
	else if(mx < 6)				v = 2;
	else if(mn > 230)		v = 1;
	else if(mn > 90 && (sum/64) > 120
				       && sumR/64 > sum/64)	v = 3;	// warm beige
	else					v = 0;
	vs = v == 1 ? " WHITE" : v == 2 ? " BLACK" : v == 3 ? " BEIGE" : v == 4 ? " YELLOW" : "";
	fs = "";
	// v9.34: at any non-normal verdict, identify the filter chain's
	// input raster - pointer, size, and whether the engage gate holds.
	// Catches the live path silently dropping to the stale stock copy
	// mid-episode. Cheap: no readback.
	if(v != 0 && sfxProbeBudget-- > 0)
		sfxLogLine("PROBE FBX f=%u pf=%d fb=%08x %dx%d cam==bb:%d\n",
			sfxFrameNo, sfxProbeFrame,
			(unsigned int)(void*)CPostEffects::pRasterFrontBuffer,
			CPostEffects::pRasterFrontBuffer != nil
				? CPostEffects::pRasterFrontBuffer->width : 0,
			CPostEffects::pRasterFrontBuffer != nil
				? CPostEffects::pRasterFrontBuffer->height : 0,
			RwCameraGetRaster(Scene.camera) == sfxBBRaster);
	// frozen output: identical hash on consecutive frames while the
	// game is running. Only meaningful while the camera is moving;
	// throttled to one line per four repeats.
	if(sfxProbeGame && s != 0 && s == sfxProbeBBHash){
		sfxProbeBBSame++;
		if((sfxProbeBBSame & 3) == 1)
			sfxProbeOut("BB", s, mn, mx, sum/64, p0, pm, vs, " SAME");
		vs = nil;
	}else
		sfxProbeBBSame = 0;
	sfxProbeBBHash = s;
	if(vs == nil)
		return;
	// print policy
	if(v != sfxProbeBBLast){
		if(!sfxProbeGame && v == 2){	// menu: black is expected, stay quiet
			sfxProbeBBLast = v;
			return;
		}
		sfxProbeBBBurst = 30;
		sfxProbeOut("BB", s, mn, mx, sum/64, p0, pm, vs, fs);
	}else if(v != 0){
		if(sfxProbeBBBurst > 0 && (f & 3) == 0)
			sfxProbeOut("BB", s, mn, mx, sum/64, p0, pm, vs, fs);
	}else if(sfxProbeGame && (f % 90) == 0)
		sfxProbeOut("BB", s, mn, mx, sum/64, p0, pm, vs, fs);
	if(sfxProbeBBBurst > 0)
		sfxProbeBBBurst--;
	sfxProbeBBLast = v;
}

// v9.30o: the back buffer RIGHT AFTER the resolve quad. Compared with
// the end-of-composite BB probe of the same frame this bisects the
// freeze: RB frozen = the resolve itself stopped landing; RB live +
// BB frozen = something between resolve and composite paints frozen
// content over the live scene.
static int sfxProbeRBLast = -1;
static int sfxProbeRBBurst;
static unsigned int sfxProbeRBHash;
static int sfxProbeRBSame;
static int sfxProbeRBFail;
static int sfxProbeWhiteDiag;
// v9.42: full-frame colour-average tracking for snap detection; the 64-dot
// grid verdict only catches extreme episodes, a warm/pale snap during camera
// motion slips through between samples - the frame-to-frame delta does not.
static int sfxProbeAvR = -1, sfxProbeAvG, sfxProbeAvB;
static int sfxProbeSnapSeq, sfxProbeSnapBudget = 120;
static int sfxProbeAvSeq;
// v9.69 probe v2: content-only comparison. v9.68 proved the filter's
// source raster is 2048x1024 while the live copy only covers the
// top-left back-buffer-sized rectangle - the v1 grid also sampled the
// never-refreshed padding, so avFB mixed content with stale memory.
// v2 samples ONLY the rectangle the filter's UVs actually cover (the
// content area, same fractions as the BB grid) and prints the order
// stamps (UF live / bb-fallback / work-raster fallback since the last
// sample, radiosity calls since the last sample) so a content
// divergence can be attributed to the writer that ran last before
// the filter sampled the raster. Read-only; logs on divergence
// (mag 12) or every 16th sample; budget 80 lines.
static void
sfxProbeCFCMP(void)
{
	IDirect3DDevice9 *dev = d3d9device;
	IDirect3DSurface9 *bb = nil, *sys = nil;
	D3DSURFACE_DESC d;
	D3DLOCKED_RECT lr;
	RwRaster *fb = CPostEffects::pRasterFrontBuffer;
	unsigned int i;
	int avR, avG, avB, fbR, fbG, fbB;
	int cw, ch;
	static int sfxCfLog = 80;
	static int sfxCfSeq;
	static unsigned int sfxCfMuL, sfxCfMuB, sfxCfMuW, sfxCfMr;
	if(dev == nil || fb == nil || sfxCfLog <= 0)
		return;
	if((sfxFrameNo & 7) != 2)
		return;
	if(dev->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &bb) != D3D_OK || bb == nil)
		return;
	if(bb->GetDesc(&d) != D3D_OK ||
	   dev->CreateOffscreenPlainSurface(d.Width, d.Height, d.Format,
	                                    D3DPOOL_SYSTEMMEM, &sys, nil) != D3D_OK ||
	   dev->GetRenderTargetData(bb, sys) != D3D_OK ||
	   sys->LockRect(&lr, nil, D3DLOCK_READONLY) != D3D_OK){
		if(sys) sys->Release();
		bb->Release();
		return;
	}
	{
		int sr = 0, sg = 0, sb = 0;
		for(i = 0; i < 144; i++){
			unsigned int sc = *(unsigned int*)((unsigned char*)lr.pBits
				+ (d.Height/18 + (i/16)*(d.Height/9))*lr.Pitch
				+ (d.Width/32 + (i%16)*(d.Width/16))*4);
			sr += (int)((sc >> 16) & 0xFF);
			sg += (int)((sc >> 8) & 0xFF);
			sb += (int)(sc & 0xFF);
		}
		avR = sr/144; avG = sg/144; avB = sb/144;
	}
	sys->UnlockRect();
	sys->Release();
	bb->Release();
	cw = (int)d.Width;
	ch = (int)d.Height;
	if(cw > (int)RwRasterGetWidth(fb))
		cw = (int)RwRasterGetWidth(fb);
	if(ch > (int)RwRasterGetHeight(fb))
		ch = (int)RwRasterGetHeight(fb);
	{
		// content-rectangle lock - the same lock class the live fill uses
		unsigned char *px = (unsigned char*)RwRasterLock(fb, 0, 2);
		int fw, fh, pitch, sr = 0, sg = 0, sb = 0;
		if(px == nil){
			if(sfxCfSeq % 16 == 0 && sfxCfLog > 0){
				sfxCfLog--;
				sfxLogLine("CFC2 fb lock failed\n");
			}
			sfxCfSeq++;
			return;
		}
		fw = RwRasterGetWidth(fb);
		fh = RwRasterGetHeight(fb);
		pitch = fw * 4;
		for(i = 0; i < 144; i++){
			unsigned int sc = *(unsigned int*)(px
				+ (ch/18 + (i/16)*(ch/9))*pitch
				+ (cw/32 + (i%16)*(cw/16))*4);
			sr += (int)((sc >> 16) & 0xFF);
			sg += (int)((sc >> 8) & 0xFF);
			sb += (int)(sc & 0xFF);
		}
		fbR = sr/144; fbG = sg/144; fbB = sb/144;
		RwRasterUnlock(fb);
	}
	{
		int dR = fbR - avR, dG = fbG - avG, dB = fbB - avB;
		int mag = (dR < 0 ? -dR : dR) + (dG < 0 ? -dG : dG) + (dB < 0 ? -dB : dB);
		unsigned int ul = sfxUfLive - sfxCfMuL, ub = sfxUfBack - sfxCfMuB;
		unsigned int uw = sfxUfWork - sfxCfMuW, ur = sfxRadCalls - sfxCfMr;
		sfxCfMuL = sfxUfLive; sfxCfMuB = sfxUfBack;
		sfxCfMuW = sfxUfWork; sfxCfMr = sfxRadCalls;
		sfxCfSeq++;
		if((mag > 12 || (sfxCfSeq & 15) == 1) && sfxCfLog > 0){
			sfxCfLog--;
			sfxLogLine("CFC2 f=%u fb=%dx%d cn=%dx%d avFB=%d,%d,%d avBB=%d,%d,%d d=%d,%d,%d u=%u/%u/%u r=%u\n",
				sfxFrameNo, RwRasterGetWidth(fb), RwRasterGetHeight(fb), cw, ch,
				fbR, fbG, fbB, avR, avG, avB, dR, dG, dB,
				ul, ub, uw, ur);
		}
	}
}

// v9.43: camera direction helper shared by the colour/angle logs
static void
sfxCamAngles(int *pit, int *hea)
{
	RwFrame *sfxAf = RwCameraGetFrame(Scene.camera);
	RwMatrix *sfxAm = sfxAf ? RwFrameGetMatrix(sfxAf) : nil;
	*pit = 0; *hea = 0;
	if(sfxAm){
		float sfxAz = sfxAm->at.z;
		if(sfxAz > 1.0f) sfxAz = 1.0f;
		if(sfxAz < -1.0f) sfxAz = -1.0f;
		*pit = (int)(asinf(sfxAz) * 57.2958f);
		*hea = (int)(atan2f(sfxAm->at.y, sfxAm->at.x) * 57.2958f);
	}
}
static void
sfxProbeResolveBB(void)
{
	IDirect3DDevice9 *dev = d3d9device;
	IDirect3DSurface9 *bb = nil, *sys = nil;
	D3DSURFACE_DESC d;
	D3DLOCKED_RECT lr;
	unsigned int s = 0, p0 = 0, pm = 0;
	int i, mn = 255, mx = 0, sum = 0, sumR = 0, v;
	const char *vs;
	if(dev == nil || sfxProbeBudget <= 0)
		return;
	// v9.34: every 4th frame (BB advances sfxProbeFrame by 2 per
	// sample, so this lands once per two BB samples).
	if((sfxProbeFrame & 3) != 2)
		return;
	if(dev->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &bb) != D3D_OK || bb == nil)
		return;
	if(bb->GetDesc(&d) != D3D_OK ||
	   dev->CreateOffscreenPlainSurface(d.Width, d.Height, d.Format,
	                                    D3DPOOL_SYSTEMMEM, &sys, nil) != D3D_OK ||
	   dev->GetRenderTargetData(bb, sys) != D3D_OK ||
	   sys->LockRect(&lr, nil, D3DLOCK_READONLY) != D3D_OK){
		if(!sfxProbeRBFail){
			sfxProbeRBFail = 1;
			sfxLogLine("PROBE RB readback unavailable - probe off\n");
		}
		if(sys) sys->Release();
		bb->Release();
		return;
	}
	{
		// v9.42: full-frame colour average (16x9 grid over the already-locked
		// surface - no extra readback). A jump larger than 30 total units
		// between two samples while the game is running is a SNAP candidate;
		// logged with the extra-colour state so the cause is identifiable.
		int sr = 0, sg = 0, sb = 0, si;
		for(si = 0; si < 144; si++){
			unsigned int sc = *(unsigned int*)((unsigned char*)lr.pBits
				+ (d.Height/18 + (si/16)*(d.Height/9))*lr.Pitch
				+ (d.Width/32 + (si%16)*(d.Width/16))*4);
			sr += (int)((sc >> 16) & 0xFF);
			sg += (int)((sc >> 8) & 0xFF);
			sb += (int)(sc & 0xFF);
		}
		{
			int avR = sr/144, avG = sg/144, avB = sb/144;
			if(sfxProbeAvR >= 0 && sfxProbeGame){
				int dR = avR - sfxProbeAvR, dG = avG - sfxProbeAvG, dB = avB - sfxProbeAvB;
				int mag = (dR < 0 ? -dR : dR) + (dG < 0 ? -dG : dG) + (dB < 0 ? -dB : dB);
				if(mag > 12 && sfxProbeSnapBudget > 0 && (sfxProbeSnapSeq++ % 8) == 0){
					sfxProbeSnapBudget--;
					sfxLogLine("SNAP f=%u d=%d,%d,%d av=%d,%d,%d xf=%.2f/%d\n",
						sfxFrameNo, dR, dG, dB, avR, avG, avB,
						*(float*)0xB79E3C, *(int*)0xB7C484);
				}
				// v9.43: continuous colour-vs-angle timeline (every 4th probe
				// sample, ~0.6 s apart) - catches subtle warm shifts the
				// SNAP threshold misses and pairs them with the camera angle.
				if((sfxProbeAvSeq++ & 3) == 0){
					int avP, avH;
					sfxCamAngles(&avP, &avH);
					sfxLogLine("AV f=%u av=%d,%d,%d d=%d,%d,%d p=%d h=%d\n",
						sfxFrameNo, avR, avG, avB, dR, dG, dB, avP, avH);
				}
			}
			sfxProbeAvR = avR; sfxProbeAvG = avG; sfxProbeAvB = avB;
		}
	}
	for(i = 0; i < 64; i++){
		unsigned int c = *(unsigned int*)((unsigned char*)lr.pBits
			+ (d.Height/16 + (i>>3)*(d.Height/8))*lr.Pitch
			+ (d.Width/16 + (i&7)*(d.Width/8))*4);
		int r = (int)(c & 0xFF);
		s = s*31 + c;
		sum += r;
		if(r > mx) mx = r;
		sumR += (int)((c >> 16) & 0xFF);
		if(r < mn) mn = r;
		if(i == 0) p0 = c;
		if(i == 36) pm = c;
	}
	sys->UnlockRect();
	sys->Release();
	bb->Release();
	if(sumR/64 > 110 && sum/64 < 60)	v = 4;	// yellow-out
	else if(mx < 6)				v = 2;
	else if(mn > 230)		v = 1;
	else if(mn > 90 && (sum/64) > 120
				       && sumR/64 > sum/64)	v = 3;	// warm beige
	else					v = 0;
	vs = v == 1 ? " WHITE" : v == 2 ? " BLACK" : v == 3 ? " BEIGE" : v == 4 ? " YELLOW" : "";
	// v9.41: at a white resolve output, snapshot the FP16 content and the
	// live stage-0 state. fp16 values are read as the red half float;
	// 15360 == 1.0, so mn>15360 proves the source texel data itself was
	// over-bright, while normal fp16 indicts the state path. Throttled
	// hard - this is an episode-only readback.
	if(v == 1 && sfxProbeWhiteDiag < 4){
		sfxProbeWhiteDiag++;
		IDirect3DSurface9 *hsrc = (IDirect3DSurface9*)sfxHDRsurf, *hsys = nil;
		D3DSURFACE_DESC hd;
		D3DLOCKED_RECT hlr;
		DWORD hcOp = 0, hArg1 = 0, hArg2 = 0, hS1 = 0;
		d3d9device->GetTextureStageState(0, D3DTSS_COLOROP, &hcOp);
		d3d9device->GetTextureStageState(0, D3DTSS_COLORARG1, &hArg1);
		d3d9device->GetTextureStageState(0, D3DTSS_COLORARG2, &hArg2);
		d3d9device->GetTextureStageState(1, D3DTSS_COLOROP, &hS1);
		if(hsrc != nil && hsrc->GetDesc(&hd) == D3D_OK &&
		   d3d9device->CreateOffscreenPlainSurface(hd.Width, hd.Height, hd.Format,
		                                    D3DPOOL_SYSTEMMEM, &hsys, nil) == D3D_OK &&
		   d3d9device->GetRenderTargetData(hsrc, hsys) == D3D_OK &&
		   hsys->LockRect(&hlr, nil, D3DLOCK_READONLY) == D3D_OK){
			int hm = 32767, hx = -32768, hsum = 0, hi;
			for(hi = 0; hi < 32; hi++){
				unsigned int hc = *(unsigned int*)((unsigned char*)hlr.pBits
					+ (hd.Height/16 + (hi>>2)*(hd.Height/8))*hlr.Pitch
					+ (hd.Width/16 + (hi&3)*(hd.Width/8))*8);
				int hr = (int)(unsigned short)(hc & 0xFFFF);
				hsum += hr;
				if(hr > hx) hx = hr;
				if(hr < hm) hm = hr;
			}
			hsys->UnlockRect();
			sfxLogLine("WHITE diag f=%u: fp16 mn=%d mx=%d av=%d (1.0=15360) | TSS cop=%u arg1=%u arg2=%u s1cop=%u\n",
				sfxFrameNo, hm, hx, hsum/32, hcOp, hArg1, hArg2, hS1);
		}else
			sfxLogLine("WHITE diag f=%u: fp16 readback failed | TSS cop=%u arg1=%u arg2=%u s1cop=%u\n",
				sfxFrameNo, hcOp, hArg1, hArg2, hS1);
		if(hsys)
			hsys->Release();
	}
	if(s != 0 && s == sfxProbeRBHash){
		sfxProbeRBSame++;
		if((sfxProbeRBSame & 3) == 1)
			sfxProbeOut("RB", s, mn, mx, sum/64, p0, pm, vs, " SAME");
		vs = nil;
	}else
		sfxProbeRBSame = 0;
	sfxProbeRBHash = s;
	if(vs == nil)
		return;
	if(v != sfxProbeRBLast){
		if(!sfxProbeGame && v == 2){
			sfxProbeRBLast = v;
			return;
		}
		sfxProbeRBBurst = 30;
		sfxProbeOut("RB", s, mn, mx, sum/64, p0, pm, vs, "");
	}else if(v != 0){
		if(sfxProbeRBBurst > 0 && (sfxProbeFrame & 3) == 0)
			sfxProbeOut("RB", s, mn, mx, sum/64, p0, pm, vs, "");
	}else if(sfxProbeGame && (sfxProbeFrame % 90) == 0)
		sfxProbeOut("RB", s, mn, mx, sum/64, p0, pm, vs, "");
	if(sfxProbeRBBurst > 0)
		sfxProbeRBBurst--;
	sfxProbeRBLast = v;
}

// FP16 scene content at resolve time (A16B16G16R16F, 8 bytes/px;
// the low 16 bits of the first dword are the red channel as half).
static void
sfxProbeFP16(void)
{
	IDirect3DDevice9 *dev = d3d9device;
	IDirect3DSurface9 *src = (IDirect3DSurface9*)sfxHDRsurf, *sys = nil;
	D3DSURFACE_DESC d;
	D3DLOCKED_RECT lr;
	unsigned int s = 0, p0 = 0, pm = 0;
	int i, mn = 32767, mx = -32768, sum = 0, v;
	if(dev == nil || src == nil)
		return;
	sfxProbeGame = 1;		// resolve runs -> gameplay frames
	if((sfxProbeFrame % 20) != 0)
		return;
	if(src->GetDesc(&d) != D3D_OK ||
	   dev->CreateOffscreenPlainSurface(d.Width, d.Height, d.Format,
	                                    D3DPOOL_SYSTEMMEM, &sys, nil) != D3D_OK ||
	   dev->GetRenderTargetData(src, sys) != D3D_OK ||
	   sys->LockRect(&lr, nil, D3DLOCK_READONLY) != D3D_OK){
		if(!sfxProbeHFail){
			sfxProbeHFail = 1;
			sfxLogLine("PROBE H16 readback unavailable - probe off\n");
		}
		if(sys) sys->Release();
		return;
	}
	for(i = 0; i < 32; i++){
		unsigned int c = *(unsigned int*)((unsigned char*)lr.pBits
			+ (d.Height/16 + (i>>2)*(d.Height/8))*lr.Pitch
			+ (d.Width/16 + (i&3)*(d.Width/8))*8);
		int r = (int)(unsigned short)(c & 0xFFFF);
		s = s*31 + c;
		sum += r;
		if(r > mx) mx = r;
		if(r < mn) mn = r;
		if(i == 0) p0 = c;
		if(i == 17) pm = c;
	}
	sys->UnlockRect();
	sys->Release();
	if(mx < 100)			v = 2;
	else if(mn > 14000)		v = 1;
	else if(mn > 1500 && (sum/32) > 3000)	v = 3;
	else					v = 0;
	if(v != sfxProbeHLast){
		sfxProbeHBurst = 30;
		sfxProbeOut("H16", s, mn, mx, sum/32, p0, pm,
			v == 1 ? " WHITE" : v == 2 ? " BLACK" : v == 3 ? " BEIGE" : "", "");
	}else if(v != 0 && sfxProbeHBurst > 0 && (sfxProbeFrame & 7) == 0)
		sfxProbeOut("H16", s, mn, mx, sum/32, p0, pm,
			v == 1 ? " WHITE" : v == 2 ? " BLACK" : v == 3 ? " BEIGE" : "", "");
	else if(v == 0 && (sfxProbeFrame % 90) == 0)
		sfxProbeOut("H16", s, mn, mx, sum/32, p0, pm, "", "");
	if(sfxProbeHBurst > 0)
		sfxProbeHBurst--;
	sfxProbeHLast = v;
}

// v9.30g: the sky, drawn the vanilla way, into the FP16 sub-rect.
// CClouds::Render (0x713950) projects its vertices through the CURRENT
// viewport, so with the scaled viewport set the gradient, clouds,
// stars and sun land in exactly the coordinate space the world
// renders into - and vanilla calls the same function right before
// RenderScene anyway (to the back buffer; that copy still happens and
// is harmless - the resolve overwrites it). No game bytes are
// patched; the black clear runs first so frames where the sky
// early-outs (interiors, no-sky cameras) get the v9.30d behaviour
// instead of stale content.
static void
sfxSkyDraw(const struct SfxD3DViewport *svp)
{
	sfxHDRclearFull();
	d3dSetViewportOrig(d3d9device, (void *)svp);	// v9.30g2: void* param - const T* -> void* is C2664
	// v9.30h: the sky GRADIENT is NOT part of CClouds::Render. Vanilla
	// draws it in DoRWStuffStartOfFrame_Horizon (0x53D7A0):
	//   DefinedState -> camera view update -> CClouds::RenderSkyPolys
	// i.e. BEFORE our fp16 bind - it landed on the back buffer and the
	// fp16 resolve wiped it every frame (hence the black sky, while
	// sun/moon/clouds - real CClouds::Render content - survived).
	// Replay the same vanilla sequence here, straight into the fp16
	// sub-rect. Pointer calls only - zero game bytes patched.
	((void (*)(void))0x734650)();	// DefinedState - vanilla render states
	((void (*)(void))0x714650)();	// CClouds::RenderSkyPolys - the gradient
	// v9.30i: the CClouds::Render (0x713950) replay is gone - Idle's
	// RenderScene already calls CClouds::Render mid-scene into the fp16
	// buffer (the g2 sun/moon/clouds came from THAT call), so the replay
	// rendered the whole sun/moon/cloud set twice per frame.
	if(sfxLogCap++ < 2000)
		sfxLogLine("SKY gradient drawn into fp16 sub-rect (vanilla fns, no patches)\n");
}

void
RenderScale_Begin(void)
{
	float s = config->renderScale;
	if(s < 0.5f)
		s = 0.5f;
	sfxWorld2DDrawn = 0;
	if(s >= 1.0f){
		sfxScaleActive = 0;
		sfxScaleApplied = 0;
		return;
	}
	RwRaster *camR = RwCameraGetRaster(Scene.camera);
	if(camR == nil || camR->width < 64 || camR->height < 64){
		sfxScaleActive = 0;
		sfxScaleApplied = 0;
		return;
	}
	int w = ((int)(camR->width * s) + 1) & ~1;
	int h = ((int)(camR->height * s) + 1) & ~1;
	if(w > camR->width) w = camR->width & ~1;
	if(h > camR->height) h = camR->height & ~1;
	sfxScaleInstallVtableHook();
	if(!sfxScaleVtPatched || !d3dSetViewportOrig || !d3dGetViewport){
		// without the vtable hooks we cannot control the viewport
		sfxScaleActive = 0;
		sfxScaleApplied = 0;
		return;
	}
	sfxW2Dinstall();
	sfxScaleW = w;
	sfxScaleH = h;
	sfxSceneW = camR->width;
	sfxSceneH = camR->height;
	memset(&sfxVpFull, 0, sizeof(sfxVpFull));
	if(d3dGetViewport(d3d9device, &sfxVpFull) != 0)
		memset(&sfxVpFull, 0, sizeof(sfxVpFull)); // unknown - don't restore garbage
	// open the window and put the scaled viewport on the device; the scene
	// raster and depth surface are NOT touched (see the v9 design note above)
	sfxScaleActive = 1;
	sfxScaleApplied = 1;
	struct SfxD3DViewport vp = {0, 0, (unsigned int)w, (unsigned int)h, 0.0f, 1.0f};
	d3dSetViewportOrig(d3d9device, &vp);
	sfxVpScaled = 1;
	sfxScaleInScene = 1;
	sfxHDRon = 0;
	if(config->hdrBuffer){
		if(sfxHDRensure(camR->width, camR->height)){
			sfxHDRon = 1;
			sfxHDRbind();
			// v9.30g: black clear + the game's own sky, straight into
			// the sub-rect (see sfxSkyDraw). Replaces the v9.30d flat
			// black (ate the real sky) and the v9.30f back buffer
			// capture (fed the PREVIOUS frame back in - the sky washed
			// toward white and stale content bled through the holes of
			// alpha-tested world geometry: the "roblox" look).
			sfxSkyDraw(&vp);
			// v9.30f CRITICAL: re-assert the SCALED viewport here.
			// SetRenderTarget (bind) and the full-viewport clear leave
			// the FULL one on the device; the sky call needs the
			// scaled one (v9.30e lesson).
			d3dSetViewportOrig(d3d9device, &vp);
			if(sfxLogH++ < 2000)
				sfxLogLine("H begin: scene window -> fp16 %dx%d\n", sfxHDRw, sfxHDRh);
		}else
			sfxLogLine("H begin: fp16 unavailable - stock path\n");
	}
	if(sfxLogB++ < 2000)
		sfxLogLine("B begin: scale=%.2f raster=%ux%u vp=%ux%u -> force %dx%d\n",
			s, sfxSceneW, sfxSceneH, sfxVpFull.width, sfxVpFull.height, w, h);
}

// full-size scratch raster for the stretch pass (same recipe as the bloom
// buffers: recreate when the size or depth of the front buffer changes)
static RwRaster *
ensureStretchRaster(int w, int h, RwInt32 depth)
{
	if(sfxStretchRaster && sfxStretchW == w && sfxStretchH == h
			&& sfxStretchDepth == depth)
		return sfxStretchRaster;
	if(sfxStretchRaster)
		RwRasterDestroy(sfxStretchRaster);
	sfxStretchRaster = RwRasterCreate(w, h, depth, rwRASTERTYPECAMERATEXTURE);
	if(!sfxStretchRaster)
		return nil;
	sfxStretchW = w;
	sfxStretchH = h;
	sfxStretchDepth = depth;
	return sfxStretchRaster;
}

// Called from RenderScene_after() once the 3D scene pass is done: close the
// window, take the full viewport back and immediately stretch the scaled
// rectangle over the whole raster. The game then draws its HUD and every
// other 2D element on top of the finished full-size frame with its regular
// full-raster coordinates (RW immediate-mode 2D is raster space - it ignores
// the D3D viewport, so the HUD can not be scaled along with the scene; it
// must be drawn AFTER the stretch).
void
RenderScale_EndOfScene(void)
{
	if(!sfxScaleApplied)
		return;
	sfxScaleApplied = 0;
	sfxScaleActive = 0;
	sfxScaleInScene = 0;
	// v9.11 diagnostics: is the depth stencil still bound when the scene
	// render is done? 00000000 here means the overlays could never have
	// z-tested against the scene in v9.10's position.
	{ void *ds = nil;
	  if(d3dGetDepthStencil)
		d3dGetDepthStencil(d3d9device, &ds);
	  if(sfxLogDS++ < 12)
		sfxLogLine("Z0 ds=%08x\n", (unsigned int)ds);
	}
	RwRaster *camR = RwCameraGetRaster(Scene.camera);
	if(camR == nil || (sfxStretchRaster == nil
			&& !ensureStretchRaster(camR->width, camR->height, camR->depth))){
		sfxLogLine("R no stretch (raster/scratch)\n");
		return;
	}
	// v9.11: from here to the end of the overlay pass, any
	// D3DCLEAR_ZBUFFER is stripped so the scene depth survives
	// (BeginUpdate below may otherwise execute the camera's pending clear)
	sfxW2DNoClear = 1;
	sfxW2DZStripped = 0;
	// camera back on the full raster - RW re-issues the full-size viewport
	setSceneRaster(camR);
	// v9.11 diagnostics: depth stencil right after the camera re-bind
	{ void *ds1 = nil;
	  if(d3dGetDepthStencil)
		d3dGetDepthStencil(d3d9device, &ds1);
	  if(sfxLogDS++ < 12)
		sfxLogLine("Z1 ds=%08x\n", (unsigned int)ds1);
	}
	// v9.11: draw the z-tested world overlays here - right after the
	// camera re-bind, where the depth buffer is definitely bound - in the
	// sub-rect coordinate space: scaled viewport for pipeline/Im3D draws,
	// camera raster dims + RsGlobal shrunk for the raster-space sprite
	// maths, vtable enforcement armed (vpfix), and any D3DCLEAR_ZBUFFER
	// stripped (zstrip) so the depth cannot be wiped mid-flight.
	if(sfxW2Dinstalled
		&& ensureW2DDimsRaster(sfxScaleW, sfxScaleH, camR->depth)){
		int i, n = 0;
		sfxSavedFB = Scene.camera->frameBuffer;
		sfxSavedRsW = RsGlobal->MaximumWidth;
		sfxSavedRsH = RsGlobal->MaximumHeight;
		// pipeline/Im3D draws follow the viewport
		struct SfxD3DViewport svp = {0, 0, (unsigned int)sfxScaleW, (unsigned int)sfxScaleH, 0.0f, 1.0f};
		d3dSetViewportOrig(d3d9device, &svp);
		sfxScaleActive = 1;
		sfxScaleInScene = 1;
		sfxW2DPass = 1;
		sfxW2DVpFix = 0;
		// v9.13: the v9.12 log proved the depth surface is bound and never
		// cleared during the window (Z0=Z1=M ds identical, zstrip 0) - so if
		// overlays still pass through geometry, the z TEST itself must be off
		// by the time RenderScene returns (device left with ZENABLE=0 or a
		// non-LESSEQUAL z func). Force it on for the pass, remember what it
		// actually was, restore afterwards.
		DWORD oldZen = 0, oldZwr = 0, oldZfn = 0;
		void *oldRwZTest = nil;
		if(d3d9device){
			d3d9device->GetRenderState(D3DRS_ZENABLE, &oldZen);
			d3d9device->GetRenderState(D3DRS_ZWRITEENABLE, &oldZwr);
			d3d9device->GetRenderState(D3DRS_ZFUNC, &oldZfn);
			d3d9device->SetRenderState(D3DRS_ZENABLE, TRUE);
			d3d9device->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
		}
		RwRenderStateGet(rwRENDERSTATEZTESTENABLE, &oldRwZTest);
		RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void*)TRUE);
		// raster-space maths (coronas, flares) must produce sub-rect
		// coordinates - lie about the screen and raster size
		Scene.camera->frameBuffer = sfxW2DDimsRaster;
		RsGlobal->MaximumWidth = (DWORD)sfxScaleW;
		RsGlobal->MaximumHeight = (DWORD)sfxScaleH;
		sfxWorld2DDrawn = 0;
		for(i = 0; i < 4; i++){
			if(sfxW2Dhooks[i].ok){
				// v9.15: CCoronas::Render leaves the z-test state flipped
				// (no restore at the end) - re-arm it for the next draws
				if(i > 0)
					RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void*)TRUE);
				((sfxVoidCall)sfxW2Dhooks[i].addr)();
				n++;
			}
		}
		sfxWorld2DDrawn = 1;
		sfxW2DPass = 0;
		if(d3d9device){
			d3d9device->SetRenderState(D3DRS_ZENABLE, oldZen);
			d3d9device->SetRenderState(D3DRS_ZFUNC, oldZfn);
		}
		RwRenderStateSet(rwRENDERSTATEZTESTENABLE, oldRwZTest);
		// v9.19: keep the scale window open through RenderEffects. The
		// game draws world-space overlays AFTER RenderScene returns
		// (skidmarks, glass, CMovingThings::Render - which is where
		// Project2DFX renders its LOD lights - really-draw-last
		// entities, fx). With the old immediate stretch those draws ran
		// at the full viewport against the sub-rect-only depth buffer:
		// colour and depth were misaligned and the lights z-tested
		// against the wrong pixels - the "coronas bleed through walls"
		// bug. The stretch is now done by the swallowed CCoronas::Render
		// stub, which RenderEffects calls right after
		// CMovingThings::Render, so those draws happen INSIDE the
		// window: the scaled viewport and the shrunk RsGlobal/raster put
		// their coordinates in the sub-rect and the intact sub-rect
		// depth occludes them correctly.
		sfxScaleInScene = 1;
		sfxScaleActive = 1;
		sfxStretchPending = 1;
		if(n > 0 && sfxW2Dlogs < 8){
			sfxW2Dlogs++;
			sfxLogLine("M world2d: %d overlays - window open for RenderEffects\n", n);
		}
		return;
	}else
		sfxW2DNoClear = 0;
	if(sfxHDRon && sfxHDRresolve(camR))
		return;
	// 1:1 copy of the frame into the scratch raster (the camera raster is
	// the back buffer and can not be sampled as a texture - that was the
	// v9.1 white screen)
	RwCameraEndUpdate(Scene.camera);
	RwRasterPushContext(sfxStretchRaster);
	RwRasterRenderFast(camR, 0, 0);
	RwRasterPopContext();
	RwCameraBeginUpdate(Scene.camera);
	// stretch the top-left sfxScaleW x sfxScaleH over the full raster
	static RwIm2DVertex sv[4];
	float nearscreen = RwIm2DGetNearScreenZ();
	float nearcam = RwCameraGetNearClipPlane(Scene.camera);
	float recipz = 1.0f/nearcam;
	float uw = sfxScaleW / (float)camR->width;
	float vh = sfxScaleH / (float)camR->height;
	quadSetUV(sv, 0.0f, 0.0f, uw, vh);
	quadSetXY(sv, 0.0f, 0.0f, (float)camR->width, (float)camR->height);
	for(int i = 0; i < 4; i++){
		RwIm2DVertexSetScreenZ(&sv[i], nearscreen);
		RwIm2DVertexSetCameraZ(&sv[i], nearcam);
		RwIm2DVertexSetRecipCameraZ(&sv[i], recipz);
		RwIm2DVertexSetIntRGBA(&sv[i], 255, 255, 255, 255);
	}
	RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, (void*)rwFILTERLINEAR);
	RwRenderStateSet(rwRENDERSTATEFOGENABLE, (void*)FALSE);
	RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void*)FALSE);
	RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void*)FALSE);
	RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)FALSE);
	RwD3D9SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void*)sfxStretchRaster);
	RwIm2DRenderIndexedPrimitive(rwPRIMTYPETRILIST, sv, 4, colorfilterIndices, 6);
	RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void*)TRUE);
	RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void*)TRUE);
	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void*)NULL);
	RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)TRUE);
	RwD3D9SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	sfxVpScaled = 0;
	if(sfxLogR++ < 2000)
		sfxLogLine("R stretch %ux%u -> %ux%u\n",
			sfxScaleW, sfxScaleH, camR->width, camR->height);
}

// v9.30: resolve the FP16 scene buffer into the back buffer - one
// hardware linear quad (the same geometry the stock path draws from
// the scratch raster), so the FP16 -> 8-bit conversion happens in the
// sampler and the picture is the stock one by construction. Runs
// outside the RW update context (between EndUpdate and BeginUpdate)
// with plain D3D9; the device states it changes are restored.
static int
sfxHDRresolve(RwRaster *camR)
{
	struct HDRVtx { float x, y, z, rhw, u, v; } v[4];
	IDirect3DSurface9 *bb = nil, *ds = nil;
	DWORD oldZen = 0, oldCull = 0, oldBlend = 0;
	// v9.55: the three SILENT CULLERS. The resolve already neutralizes
	// z/cull/blend/PS/TSS, but a leftover scissor rect, alpha test or
	// stencil state culled the quad without any error - the log line
	// still printed, the back buffer never received the scene, and the
	// whole post chain then accumulated on the stale frame (green/
	// white/black states, HUD stuck in fp16, pause freezing the stale
	// frame). Which states are stale depends on what the camera sees -
	// hence the pitch correlation. Save them, log when armed, kill.
	DWORD oldSc = 0, oldAt = 0, oldSt = 0;
	// v9.56: the LAST unchecked silent killer. Shadow rendering (the
	// user runs stencil shadows) classically leaves COLORWRITEENABLE
	// at 0 - every draw after it "succeeds" but writes no pixels:
	// the resolve quad lands on nothing, the back buffer freezes, and
	// the whole post chain accumulates on the stale frame.
	DWORD oldCw = 0;
	HRESULT hr, qhr;
	float uw, vh;
	int i;
	if(d3d9device == nil || sfxHDRsurf == nil || camR == nil)
		return 0;
	// v9.57: paused - the back buffer still holds the last composited
	// frame; a resolve here would overwrite it with the near-empty fp16
	// content (cleared scene + sky gradient only - the world does not
	// redraw while paused). That overwrite was the black/white pause
	// background.
	if(sfxPaused())
		return 0;
	if(d3d9device->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &bb) != D3D_OK
		|| bb == nil)
		return 0;
	// v9.30m: safe FP16 content probe. v9.30l LockRect-ed this render
	// target surface directly, which is illegal on D3D9 and corrupted
	// the frame (user screenshots 71/72 - all black). GetRenderTargetData
	// copies GPU->CPU without touching the source.
	sfxProbeFP16();
	d3d9device->GetRenderState(D3DRS_ZENABLE, &oldZen);
	d3d9device->GetRenderState(D3DRS_CULLMODE, &oldCull);
	d3d9device->GetRenderState(D3DRS_ALPHABLENDENABLE, &oldBlend);
	d3d9device->GetRenderState(D3DRS_SCISSORTESTENABLE, &oldSc);
	d3d9device->GetRenderState(D3DRS_ALPHATESTENABLE, &oldAt);
	d3d9device->GetRenderState(D3DRS_STENCILENABLE, &oldSt);
	d3d9device->GetRenderState(D3DRS_COLORWRITEENABLE, &oldCw);
	{
		static int sfxLogClip;
		if((oldSc || oldAt || oldSt || oldCw != 0xF) && sfxLogClip < 12){
			sfxLogClip++;
			sfxLogLine("H2 clip states: scissor=%u atest=%u stencil=%u colwrite=%u\n",
				oldSc, oldAt, oldSt, oldCw);
		}
	}
	// full viewport first - the quad is placed in full-raster coordinates
	struct SfxD3DViewport fullvp = {0, 0, (unsigned int)camR->width, (unsigned int)camR->height, 0.0f, 1.0f};
	if(d3dSetViewportOrig)
		d3dSetViewportOrig(d3d9device, &fullvp);
	sfxVpScaled = 0;
	sfxScaleActive = 0;
	sfxScaleInScene = 0;
	RwCameraEndUpdate(Scene.camera);
	if(d3dGetDepthStencil)
		d3dGetDepthStencil(d3d9device, &ds);
	hr = d3d9device->SetRenderTarget(0, bb);
	bb->Release();
	if(ds){
		d3d9device->SetDepthStencilSurface(ds);
		ds->Release();
	}
	if(hr != D3D_OK){
		RwCameraBeginUpdate(Scene.camera);
		sfxLogLine("H2 resolve ABORT SetRenderTarget hr=%08x - stock path\n", (unsigned int)hr);
		sfxHDRrelease();
		sfxHDRon = 0;
		return 0;
	}
	d3d9device->SetRenderState(D3DRS_ZENABLE, FALSE);
	d3d9device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	d3d9device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	d3d9device->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
	d3d9device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	d3d9device->SetRenderState(D3DRS_STENCILENABLE, FALSE);
	d3d9device->SetRenderState(D3DRS_COLORWRITEENABLE, 0xF);
	d3d9device->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	d3d9device->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
	d3d9device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	d3d9device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	d3d9device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
	d3d9device->SetTexture(0, (IDirect3DTexture9*)sfxHDRtex);
	// v9.40: the resolve quad is fixed-function. The filter chain can leave
	// its own pixel shader bound (which pass runs last depends on what is on
	// screen - hence the pitch correlation), and a stray PS samples through
	// foreign sampler state: opaque white across the whole frame. Kill it.
	IDirect3DPixelShader9 *sfxOldPS = nil;
	d3d9device->GetPixelShader(&sfxOldPS);
	if(sfxOldPS != nil){
		d3d9device->SetPixelShader(nil);
		if(sfxLogPS < 6){
			sfxLogPS++;
			sfxLogLine("H2 stray pixel shader %p killed before resolve\n", sfxOldPS);
		}
		sfxOldPS->Release();
	}
	d3d9device->SetFVF(D3DFVF_XYZRHW | D3DFVF_TEX1);
	// v9.40: verify the FP16 texture really became sampler 0. D3D9 samples
	// an unbound/foreign sampler as opaque white, so a silent bind failure
	// would reproduce the same white episodes - log it once if so.
	IDirect3DBaseTexture9 *sfxBnd = nil;
	d3d9device->GetTexture(0, &sfxBnd);
	if(sfxBnd != (IDirect3DBaseTexture9*)sfxHDRtex && sfxLogPS < 6){
		sfxLogPS++;
		sfxLogLine("H2 bind mismatch got=%p want=%p\n", sfxBnd, sfxHDRtex);
	}
	if(sfxBnd)
		sfxBnd->Release();
	uw = (float)sfxScaleW / (float)sfxHDRw;
	vh = (float)sfxScaleH / (float)sfxHDRh;
	v[0].x = -0.5f;	v[0].y = -0.5f;	v[0].u = 0.0f;	v[0].v = 0.0f;
	v[1].x = (float)camR->width - 0.5f;	v[1].y = -0.5f;	v[1].u = uw;	v[1].v = 0.0f;
	v[2].x = -0.5f;	v[2].y = (float)camR->height - 0.5f;	v[2].u = 0.0f;	v[2].v = vh;
	v[3].x = (float)camR->width - 0.5f;	v[3].y = (float)camR->height - 0.5f;	v[3].u = uw;	v[3].v = vh;
	for(i = 0; i < 4; i++){
		v[i].z = 0.0f;
		v[i].rhw = 1.0f;
	}
	// v9.41: sanitize fixed-function texture stage state around the
	// resolve quad. Vanilla RwIm2D draws (sky gradient, world2d overlays,
	// marker coronas) reprogram stage-0 color args for untextured draws;
	// this quad's FVF has no DIFFUSE element, so a leftover argument
	// select (DIFFUSE/TFACTOR) paints constant opaque white - matching
	// the deterministic full-frame white episodes. Which Im2D draws ran
	// last depends on what the camera sees - hence the pitch correlation.
	{
		DWORD ocop, oarg1, oarg2, oaop, oaarg1, oaarg2, os1;
		d3d9device->GetTextureStageState(0, D3DTSS_COLOROP, &ocop);
		d3d9device->GetTextureStageState(0, D3DTSS_COLORARG1, &oarg1);
		d3d9device->GetTextureStageState(0, D3DTSS_COLORARG2, &oarg2);
		d3d9device->GetTextureStageState(0, D3DTSS_ALPHAOP, &oaop);
		d3d9device->GetTextureStageState(0, D3DTSS_ALPHAARG1, &oaarg1);
		d3d9device->GetTextureStageState(0, D3DTSS_ALPHAARG2, &oaarg2);
		d3d9device->GetTextureStageState(1, D3DTSS_COLOROP, &os1);
		if(sfxLogPS < 6 && (ocop != D3DTOP_MODULATE || oarg1 != D3DTA_TEXTURE)){
			sfxLogPS++;
			sfxLogLine("H2 stray TSS before resolve: cop=%u arg1=%u arg2=%u aop=%u s1cop=%u - sanitized\n",
				ocop, oarg1, oarg2, oaop, os1);
		}
		d3d9device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		d3d9device->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
		d3d9device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
		d3d9device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		d3d9device->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
		d3d9device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
		d3d9device->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
		qhr = d3d9device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, v, sizeof(struct HDRVtx));
		if(qhr != D3D_OK){
			static int sfxLogQ;
			if(sfxLogQ < 8){
				sfxLogQ++;
				sfxLogLine("H2 quad DrawPrimitiveUP FAILED hr=%08x\n", (unsigned int)qhr);
			}
		}
		// restore exactly what was there - RW keeps its own state cache
		d3d9device->SetTextureStageState(0, D3DTSS_COLOROP, ocop);
		d3d9device->SetTextureStageState(0, D3DTSS_COLORARG1, oarg1);
		d3d9device->SetTextureStageState(0, D3DTSS_COLORARG2, oarg2);
		d3d9device->SetTextureStageState(0, D3DTSS_ALPHAOP, oaop);
		d3d9device->SetTextureStageState(0, D3DTSS_ALPHAARG1, oaarg1);
		d3d9device->SetTextureStageState(0, D3DTSS_ALPHAARG2, oaarg2);
		d3d9device->SetTextureStageState(1, D3DTSS_COLOROP, os1);
	}
	// v9.30o: did the quad actually land? Content of the back buffer
	// right now - pairs with the end-of-composite BB probe of this frame.
	sfxProbeResolveBB();
	// v9.68 probe: is the filter's texture source fresh? (CFCMP)
	sfxProbeCFCMP();
	// detach the FP16 texture again and restore what was switched off;
	// RW re-issues its own state on the next camera/draw cycle
	d3d9device->SetTexture(0, nil);
	d3d9device->SetRenderState(D3DRS_ZENABLE, oldZen);
	d3d9device->SetRenderState(D3DRS_CULLMODE, oldCull);
	d3d9device->SetRenderState(D3DRS_ALPHABLENDENABLE, oldBlend);
	d3d9device->SetRenderState(D3DRS_SCISSORTESTENABLE, oldSc);
	d3d9device->SetRenderState(D3DRS_ALPHATESTENABLE, oldAt);
	d3d9device->SetRenderState(D3DRS_STENCILENABLE, oldSt);
	d3d9device->SetRenderState(D3DRS_COLORWRITEENABLE, oldCw);
	sfxHDRon = 0;
	RwCameraBeginUpdate(Scene.camera);
	if(sfxLogR++ < 2000)
		sfxLogLine("H2 resolve fp16 %ux%u -> %ux%u @coronas\n",
			sfxScaleW, sfxScaleH, camR->width, camR->height);
	return 1;
}

// v9.19: the deferred end-of-frame stretch. Runs from the swallowed
// CCoronas::Render stub inside RenderEffects - after CMovingThings::Render
// (Project2DFX LOD lights) and before the fx/HUD draws that need the full
// raster - and also from DrawFinalEffects as a safety net.
// v9.59: pause hold - see the comment at the forward declaration.
// Re-presents the frozen GPU copy onto the swap-chain back buffer every
// pause frame; the vanilla menu/HUD draws land on top of it right after.
// Same device-level bracket as sfxHDRresolve (proven since v9.30).
static void
sfxPauseHold(RwRaster *camR)
{
	struct HDRVtx { float x, y, z, rhw, u, v; } v[4];
	IDirect3DSurface9 *bb = nil, *ds = nil;
	DWORD oldZen = 0, oldCull = 0, oldBlend = 0;
	DWORD oldSc = 0, oldAt = 0, oldSt = 0, oldCw = 0;
	HRESULT hr;
	static int sfxHoldLogged;
	float w, h;
	int i;
	if(d3d9device == nil || camR == nil)
		return;
	if(d3d9device->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &bb) != D3D_OK
	   || bb == nil)
		return;
	// v9.61: the frozen copy is maintained by sfxHoldRolling (once per
	// unpaused frame), so the hold here only presents it - no capture:
	// capturing at this point would bake the menu into the held frame
	// (the v9.60 frozen pause selection).
	d3d9device->GetRenderState(D3DRS_ZENABLE, &oldZen);
	d3d9device->GetRenderState(D3DRS_CULLMODE, &oldCull);
	d3d9device->GetRenderState(D3DRS_ALPHABLENDENABLE, &oldBlend);
	d3d9device->GetRenderState(D3DRS_SCISSORTESTENABLE, &oldSc);
	d3d9device->GetRenderState(D3DRS_ALPHATESTENABLE, &oldAt);
	d3d9device->GetRenderState(D3DRS_STENCILENABLE, &oldSt);
	d3d9device->GetRenderState(D3DRS_COLORWRITEENABLE, &oldCw);
	RwCameraEndUpdate(Scene.camera);
	if(d3dGetDepthStencil)
		d3dGetDepthStencil(d3d9device, &ds);
	hr = d3d9device->SetRenderTarget(0, bb);
	bb->Release();
	if(ds){
		d3d9device->SetDepthStencilSurface(ds);
		ds->Release();
	}
	if(hr != D3D_OK){
		RwCameraBeginUpdate(Scene.camera);
		return;
	}
	{
		struct SfxD3DViewport fullvp = {0, 0, (unsigned int)camR->width,
			(unsigned int)camR->height, 0.0f, 1.0f};
		if(d3dSetViewportOrig)
			d3dSetViewportOrig(d3d9device, &fullvp);
	}
	d3d9device->SetRenderState(D3DRS_ZENABLE, FALSE);
	d3d9device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	d3d9device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	d3d9device->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
	d3d9device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	d3d9device->SetRenderState(D3DRS_STENCILENABLE, FALSE);
	d3d9device->SetRenderState(D3DRS_COLORWRITEENABLE, 0xF);
	d3d9device->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	d3d9device->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
	d3d9device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	d3d9device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	d3d9device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
	{
		IDirect3DPixelShader9 *ps = nil;
		d3d9device->GetPixelShader(&ps);
		if(ps != nil){
			d3d9device->SetPixelShader(nil);
			ps->Release();
		}
	}
	d3d9device->SetTexture(0, sfxCopyTex);
	d3d9device->SetFVF(D3DFVF_XYZRHW | D3DFVF_TEX1);
	w = (float)camR->width;
	h = (float)camR->height;
	v[0].x = -0.5f;    v[0].y = -0.5f;    v[0].u = 0.0f; v[0].v = 0.0f;
	v[1].x = w - 0.5f; v[1].y = -0.5f;    v[1].u = 1.0f; v[1].v = 0.0f;
	v[2].x = -0.5f;    v[2].y = h - 0.5f; v[2].u = 0.0f; v[2].v = 1.0f;
	v[3].x = w - 0.5f; v[3].y = h - 0.5f; v[3].u = 1.0f; v[3].v = 1.0f;
	for(i = 0; i < 4; i++){
		v[i].z = 0.0f;
		v[i].rhw = 1.0f;
	}
	d3d9device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, v,
		sizeof(struct HDRVtx));
	d3d9device->SetTexture(0, nil);
	d3d9device->SetRenderState(D3DRS_ZENABLE, oldZen);
	d3d9device->SetRenderState(D3DRS_CULLMODE, oldCull);
	d3d9device->SetRenderState(D3DRS_ALPHABLENDENABLE, oldBlend);
	d3d9device->SetRenderState(D3DRS_SCISSORTESTENABLE, oldSc);
	d3d9device->SetRenderState(D3DRS_ALPHATESTENABLE, oldAt);
	d3d9device->SetRenderState(D3DRS_STENCILENABLE, oldSt);
	d3d9device->SetRenderState(D3DRS_COLORWRITEENABLE, oldCw);
	RwCameraBeginUpdate(Scene.camera);
	if(!sfxHoldLogged && sfxLogCopy < 8){
		sfxHoldLogged = 1;
		sfxLogLine("PHOLD engaged\n");
	}
}

// v9.61: rolling frozen copy - one StretchRect per unpaused frame into
// the hold texture, so ESC always holds the LAST finished game frame
// (HUD included, menu never baked in).
static void
sfxHoldRolling(void)
{
	IDirect3DSurface9 *bb = nil;
	D3DSURFACE_DESC d;
	HRESULT hr;
	if(d3d9device == nil)
		return;
	// v9.67: capture into the BACK pair only. The front pair is what
	// the hold presents and must not be written in the same frame -
	// v9.66 rotated first, so the hold presented the texture the
	// capture had just written (steady black/white frames; the same
	// read-after-write hazard the v9.64 flicker proved). The gate
	// rotates the pairs AFTER the present, via sfxHoldRotate.
	if(d3d9device->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &bb) != D3D_OK
	   || bb == nil)
		return;
	if(bb->GetDesc(&d) != D3D_OK){
		bb->Release();
		return;
	}
	// v9.67: all capture state below targets the BACK pair (B); the
	// front pair is only touched by sfxHoldRotate.
	if(sfxCopyTexB != nil && (sfxCopyW != (int)d.Width || sfxCopyH != (int)d.Height)){
		if(sfxCopySurfB){ sfxCopySurfB->Release(); sfxCopySurfB = nil; }
		sfxCopyTexB->Release();
		sfxCopyTexB = nil;
	}
	if(sfxCopyTexB == nil && !sfxCopyFailed){
		hr = d3d9device->CreateTexture(d.Width, d.Height, 1,
			D3DUSAGE_RENDERTARGET, d.Format, D3DPOOL_DEFAULT,
			&sfxCopyTexB, nil);
		if(hr != D3D_OK || sfxCopyTexB == nil){
			sfxCopyFailed = 1;
			if(sfxLogCopy < 8)
				sfxLogLine("HOLD create FAILED hr=%08x\n", (unsigned int)hr);
			bb->Release();
			return;
		}
		sfxCopyTexB->GetSurfaceLevel(0, &sfxCopySurfB);
		sfxCopyW = d.Width;
		sfxCopyH = d.Height;
		if(sfxLogCopy < 8){
			sfxLogCopy++;
			sfxLogLine("HOLD copytexB %dx%d\n", sfxCopyW, sfxCopyH);
		}
	}
	if(sfxCopySurfB != nil){
		hr = d3d9device->StretchRect(bb, nil, sfxCopySurfB, nil, D3DTEXF_NONE);
		if(hr != D3D_OK){
			if(sfxLogCopy < 8){
				sfxLogCopy++;
				sfxLogLine("HOLD roll FAILED hr=%08x\n", (unsigned int)hr);
			}
			// a lost device invalidates DEFAULT-pool textures - drop
			// and recreate on the next frame (the sfxHDRtex pattern)
			if(sfxCopySurfB){ sfxCopySurfB->Release(); sfxCopySurfB = nil; }
			if(sfxCopyTexB){ sfxCopyTexB->Release(); sfxCopyTexB = nil; }
		}
	}
	bb->Release();
}

// v9.67: swap the capture and present pairs - call AFTER the hold
// presented the front pair, so the next frame presents what was
// captured this frame and the presented texture is never written
// and read in the same frame.
static void
sfxHoldRotate(void)
{
	IDirect3DTexture9 *tt = sfxCopyTex;
	IDirect3DSurface9 *ts = sfxCopySurf;
	sfxCopyTex = sfxCopyTexB;
	sfxCopySurf = sfxCopySurfB;
	sfxCopyTexB = tt;
	sfxCopySurfB = ts;
}

static void
RenderScale_DeferredStretch(void)
{
	RwRaster *camR;
	sfxStretchPending = 0;
	sfxFrameNo++;
	// v9.30i: golden-hour extra colour state - the vanilla timecycle
	// system ramps m_ExtraColourInter (0..1) when the camera faces the
	// low sun, warming the whole filter; correlates with the reported
	// ON/OFF "blink" (warm while driving towards the sun, pale away)
	if(sfxLogXF++ < 4000){
		int sfxPit, sfxHea;
		sfxCamAngles(&sfxPit, &sfxHea);
		sfxLogLine("XF f=%u inter=%.2f on=%d ec=%d p=%d h=%d\n",
			sfxFrameNo, *(float*)0xB79E3C, *(int*)0xB7C484, *(int*)0xB79E44, sfxPit, sfxHea);
	}
	// v9.45: did the vanilla post-effect calls actually run this frame?
	// A count that did not advance since last frame means the vanilla
	// gate in front of the call site skipped it - a binary skip of the
	// filter/glow draw reads as the ON/OFF snap. The colour filter is
	// BRIDGED: redraw the last colours for up to 24 frames, ramping to
	// neutral (the fade statics inside the filter chase the ramp, so
	// both the skip and the resume fade instead of popping).
	{
		static unsigned int xCF, xRad, xDK;
		static int cfSkip, radSkip, dkSkip;
		if(sfxCFSeq != xCF){
			if(cfSkip > 0)
				sfxLogLine("CFRESUME seq=%u skipped=%d\n", sfxCFSeq, cfSkip);
			cfSkip = 0;
		}else if(sfxCFDrawOK && sfxProbeGame){
			cfSkip++;
			// v9.61: PERMANENT bridge - the design goal: the colour
			// filter stays ON exactly like daytime, for as long as the
			// game runs. The old 24-frame fade-out WAS the prolonged
			// OFF half of every snap episode: past frame 24 the bridge
			// stopped drawing and the vanilla gate's long skips showed
			// as the full filter-off state, then the resume ramp showed
			// as the ON flip. Redraw the last colours at full strength
			// for as long as the vanilla gate skips - no fade, no cap.
			CPostEffects::ColourFilter_switch(sfxCFLast1, sfxCFLast2);
			if(cfSkip == 1 || (cfSkip & 127) == 0)
				sfxLogLine("CFGAP f=%u n=%d\n", sfxFrameNo, cfSkip);
		}else if(sfxCFDrawOK){
			cfSkip++;
			if(cfSkip == 1 || cfSkip == 120)
				sfxLogLine("CFGAP f=%u n=%d (no bridge: not in game)\n", sfxFrameNo, cfSkip);
		}
		xCF = sfxCFSeq;
		if(sfxRadSeq != xRad){
			if(radSkip > 0)
				sfxLogLine("RADRESUME seq=%u skipped=%d\n", sfxRadSeq, radSkip);
			radSkip = 0;
		}else{
			radSkip++;
			if(radSkip == 1 || radSkip == 30 || (radSkip & 127) == 0)
				sfxLogLine("RADGAP f=%u n=%d\n", sfxFrameNo, radSkip);
		}
		xRad = sfxRadSeq;
		if(sfxDKSeq == xDK){
			dkSkip++;
			if(dkSkip == 1 || dkSkip == 120)
				sfxLogLine("DKGAP f=%u n=%d\n", sfxFrameNo, dkSkip);
		}else
			dkSkip = 0;
		xDK = sfxDKSeq;
	}
	// v9.20: use the REAL camera raster and un-swap the frameBuffer
	// FIRST. v9.19 read the still-swapped 1200x676 dims raster here, so
	// the stretch quad only covered the sub-rect 1:1 (broken looking
	// resolution) and the copy source did not contain anything drawn
	// during RenderEffects - which wiped the Project2DFX LOD lights
	// that had just been drawn (correctly occluded) inside the window.
	// Copying the real back buffer bakes scene + overlays + in-window
	// draws into the stretched frame.
	camR = sfxSavedFB;
	Scene.camera->frameBuffer = camR;
	// v9.30d: the resolve happens HERE - after the overlay window, so
	// the window draws (coronas, LOD lights) are already inside the
	// FP16 buffer and are included in the single upscale. (v9.30c
	// resolved before the window and stretched the already upscaled
	// frame again - the double zoom in the screenshots.)
	if(sfxPaused()){
		// v9.57: paused - the post chain stands down
		// v9.59: but keep RE-PRESENTING the frozen frame - the swap
		// chain keeps serving buffers, and untouched ones alternate
		// black/white (the pause background). The vanilla menu/HUD
		// draws land on top of the held frame right after this.
		sfxPauseHold(camR);
	}else if(sfxHDRon && sfxHDRresolve(camR)){
		// resolved straight from the FP16 buffer; the RsGlobal and
		// NoClear cleanup at the end of this function is shared
		// v9.32/35: remember the swap-chain-backed raster, and fill the
		// front buffer from the live back buffer NOW - before the
		// colour filter chain reads it. This is the fill that breaks
		// the feedback loop: the filter must see THIS frame's resolve,
		// not last frame's (the 934a run proved the loop: FB one frame
		// behind = blurred warm wash + white-out peaks + warm snap).
		sfxBBRegister(camR);	// v9.39: the live fill is retired - see UpdateFrontBuffer
	}else if(camR != nil && (sfxStretchRaster != nil
			|| ensureStretchRaster(camR->width, camR->height, camR->depth))){
		// full viewport first - the stretch quad is placed in
		// full-raster coordinates
		struct SfxD3DViewport fullvp = {0, 0, (unsigned int)camR->width, (unsigned int)camR->height, 0.0f, 1.0f};
		if(d3dSetViewportOrig)
			d3dSetViewportOrig(d3d9device, &fullvp);
		sfxVpScaled = 0;
		sfxScaleActive = 0;
		sfxScaleInScene = 0;
		RwCameraEndUpdate(Scene.camera);
		RwRasterPushContext(sfxStretchRaster);
		RwRasterRenderFast(camR, 0, 0);
		RwRasterPopContext();
		RwCameraBeginUpdate(Scene.camera);
		{
			static RwIm2DVertex sv[4];
			float nearscreen = RwIm2DGetNearScreenZ();
			float nearcam = RwCameraGetNearClipPlane(Scene.camera);
			float recipz = 1.0f/nearcam;
			quadSetUV(sv, 0.0f, 0.0f,
				(float)sfxScaleW/(float)camR->width,
				(float)sfxScaleH/(float)camR->height);
			quadSetXY(sv, 0.0f, 0.0f, (float)camR->width, (float)camR->height);
			for(int i = 0; i < 4; i++){
				RwIm2DVertexSetScreenZ(&sv[i], nearscreen);
				RwIm2DVertexSetCameraZ(&sv[i], nearcam);
				RwIm2DVertexSetRecipCameraZ(&sv[i], recipz);
				RwIm2DVertexSetIntRGBA(&sv[i], 255, 255, 255, 255);
			}
			RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, (void*)rwFILTERLINEAR);
			RwRenderStateSet(rwRENDERSTATEFOGENABLE, (void*)FALSE);
			RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void*)FALSE);
			RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void*)FALSE);
			RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)FALSE);
			RwD3D9SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
			RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void*)sfxStretchRaster);
			RwIm2DRenderIndexedPrimitive(rwPRIMTYPETRILIST, sv, 4, colorfilterIndices, 6);
			RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void*)TRUE);
			RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void*)TRUE);
			RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void*)NULL);
			RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)TRUE);
			RwD3D9SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
		}
		if(sfxLogR++ < 2000)
			sfxLogLine("R2 stretch %ux%u -> %ux%u @coronas\n",
				sfxScaleW, sfxScaleH, camR->width, camR->height);
	}else
		sfxLogLine("R2 no stretch (raster/scratch)\n");
	// window fully closed - normal screen and raster sizes back
	// (frameBuffer was already un-swapped at the top, v9.20)
	RsGlobal->MaximumWidth = sfxSavedRsW;
	RsGlobal->MaximumHeight = sfxSavedRsH;
	sfxW2DNoClear = 0;
}

// v9.46: FINAL-image delta probe. The existing probes miss the snap the
// player sees: sfxProbeResolveBB samples right after the fp16 resolve
// (BEFORE the colour filter and radiosity glow draw) and sfxProbeBackBuffer
// only prints on white/black/beige verdict changes - a warm<->green tint
// flip never changes the verdict, so it stayed silent. This twin runs at
// the very end of DrawFinalEffects, averages the same 16x9 colour grid as
// the RB probe and logs every jump over the snap threshold together with
// the camera angles and the current filter colour - the first numeric
// capture of the user-visible snap.
static int sfxProbeFAvR = -1, sfxProbeFAvG, sfxProbeFAvB;
static int sfxProbeFSeq;
static int sfxLogH16S;
static void
sfxProbeFinalBB(void)
{
	IDirect3DDevice9 *dev = d3d9device;
	IDirect3DSurface9 *bb = nil, *sys = nil;
	D3DSURFACE_DESC d;
	D3DLOCKED_RECT lr;
	unsigned int sc;
	int sr = 0, sg = 0, sb = 0, si;
	int avR, avG, avB, dR, dG, dB, mag, avP, avH;
	if(dev == nil)
		return;
	if(((++sfxProbeFSeq) & 3) != 1)
		return;
	if(dev->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &bb) != D3D_OK || bb == nil)
		return;
	if(bb->GetDesc(&d) != D3D_OK ||
	   dev->CreateOffscreenPlainSurface(d.Width, d.Height, d.Format,
	                                    D3DPOOL_SYSTEMMEM, &sys, nil) != D3D_OK ||
	   dev->GetRenderTargetData(bb, sys) != D3D_OK ||
	   sys->LockRect(&lr, nil, D3DLOCK_READONLY) != D3D_OK){
		if(sys) sys->Release();
		if(bb) bb->Release();
		return;
	}
	for(si = 0; si < 144; si++){
		sc = *(unsigned int*)((unsigned char*)lr.pBits
			+ (d.Height/18 + (si/16)*(d.Height/9))*lr.Pitch
			+ (d.Width/32 + (si%16)*(d.Width/16))*4);
		sr += (int)((sc >> 16) & 0xFF);
		sg += (int)((sc >> 8) & 0xFF);
		sb += (int)(sc & 0xFF);
	}
	sys->UnlockRect();
	sys->Release();
	bb->Release();
	avR = sr/144; avG = sg/144; avB = sb/144;
	if(sfxProbeFAvR >= 0 && sfxProbeGame){
		dR = avR - sfxProbeFAvR;
		dG = avG - sfxProbeFAvG;
		dB = avB - sfxProbeFAvB;
		mag = (dR < 0 ? -dR : dR) + (dG < 0 ? -dG : dG) + (dB < 0 ? -dB : dB);
		sfxCamAngles(&avP, &avH);
		if(mag > 12){
			sfxLogLine("SNAPF f=%u d=%d,%d,%d av=%d,%d,%d p=%d h=%d c2=%d,%d,%d,%d\n",
				sfxFrameNo, dR, dG, dB, avR, avG, avB, avP, avH,
				vcsblurrgb.red, vcsblurrgb.green, vcsblurrgb.blue,
				vcsblurrgb.alpha);
			// v9.51: bifurcation probe - read the fp16 scene surface at
			// the SAME moment as a visible jump. If the jump is already
			// in the fp16 content, the scene/filter chain produces it;
			// if fp16 is normal while the back buffer jumped, the fault
			// is in the resolve/composite half. Budgeted.
			{
				IDirect3DSurface9 *hsrc = (IDirect3DSurface9*)sfxHDRsurf, *hsys = nil;
				D3DSURFACE_DESC hd;
				D3DLOCKED_RECT hlr;
				if(sfxLogH16S < 150 &&
				   hsrc != nil &&
				   hsrc->GetDesc(&hd) == D3D_OK &&
				   dev->CreateOffscreenPlainSurface(hd.Width, hd.Height, hd.Format,
				                                    D3DPOOL_SYSTEMMEM, &hsys, nil) == D3D_OK &&
				   dev->GetRenderTargetData(hsrc, hsys) == D3D_OK &&
				   hsys->LockRect(&hlr, nil, D3DLOCK_READONLY) == D3D_OK){
					int hmn = 32767, hmx = -32768, hi;
					long hsum = 0;
					for(hi = 0; hi < 32; hi++){
						unsigned int hc = *(unsigned int*)((unsigned char*)hlr.pBits
							+ (hd.Height/16 + (hi>>2)*(hd.Height/8))*hlr.Pitch
							+ (hd.Width/16 + (hi&3)*(hd.Width/8))*8);
						int hv = (int)(unsigned short)(hc & 0xFFFF);
						hsum += hv;
						if(hv > hmx) hmx = hv;
						if(hv < hmn) hmn = hv;
					}
					hsys->UnlockRect();
					sfxLogH16S++;
					sfxLogLine("H16S f=%u fp16mn=%d fp16mx=%d fp16av=%d\n",
						sfxFrameNo, hmn, hmx, (int)(hsum/32));
				}
				if(hsys)
					hsys->Release();
			}
		}else if((sfxProbeFSeq & 3) == 2)
			sfxLogLine("FB f=%u av=%d,%d,%d p=%d h=%d c2=%d,%d,%d,%d\n",
				sfxFrameNo, avR, avG, avB, avP, avH,
				vcsblurrgb.red, vcsblurrgb.green, vcsblurrgb.blue,
				vcsblurrgb.alpha);
	}
	sfxProbeFAvR = avR;
	sfxProbeFAvG = avG;
	sfxProbeFAvB = avB;
}

void
CPostEffects::DrawFinalEffects(void)
{
	// safety: make sure the scale window is closed and the full viewport is
	// back - RenderScale_EndOfScene normally did both (and stretched the
	// frame already); this only fires on odd frames that skip it
	// v9.19: safety - if the coronas stub never ran this frame, do the
	// deferred stretch here (the window is still open at this point)
	if(sfxStretchPending)
		RenderScale_DeferredStretch();
	if(sfxScaleActive){
		sfxScaleActive = 0;
		if(sfxVpScaled && sfxVpFull.width != 0 && d3dSetViewportOrig){
			d3dSetViewportOrig(d3d9device, &sfxVpFull);
			sfxLogLine("F0 close vp restored %ux%u\n",
				sfxVpFull.width, sfxVpFull.height);
		}
		sfxVpScaled = 0;
	}
	// v9.30i: proof-of-life marker - if the mod post chain (bloom,
	// exposure, vignette) ever stops on a machine, these lines stop
	// v9.53: frame + readback timing heartbeat (every ~30 frames)
	{
		static LARGE_INTEGER sfxQPF;
		LARGE_INTEGER n;
		if(sfxQPF.QuadPart == 0)
			QueryPerformanceFrequency(&sfxQPF);
		QueryPerformanceCounter(&n);
		if(sfxFrameLastQPC != 0 && sfxLogTiming < 400 &&
		   (sfxProbeFrame % 30) == 0){
			sfxLogTiming++;
			sfxLogLine("T f=%d uf=%lldus frame=%lldus\n",
				sfxProbeFrame, sfxUFUs,
				((n.QuadPart - sfxFrameLastQPC) * 1000000) / sfxQPF.QuadPart);
		}
		sfxFrameLastQPC = n.QuadPart;
		sfxUFUs = 0;
	}
	// v9.66: 0xBA67A4 is a transition PULSE in this exe - the v9.65 log
	// shows it 1 for exactly one frame at menu open and one frame at
	// menu close, 0 in between (so it is not the persistent
	// m_bMenuActive). The open pulse lands inside the 3-4 frame pause
	// blip (v9.63 log), the close pulse comes without a blip - sample
	// the pulse and let sfxPaused() say which transition it was.
	{
		static int sfxMenuPulsePrev = 0;
		int pulse = *(char *)0xBA67A4 != 0;
		if(pulse && !sfxMenuPulsePrev){
			sfxMenuOpen = sfxPaused();
			if(sfxLogCopy < 8){
				sfxLogCopy++;
				sfxLogLine("HOLD menu %s\n", sfxMenuOpen ? "open" : "close");
			}
		}
		sfxMenuPulsePrev = pulse;
	}
	if(sfxPaused() || sfxMenuOpen){
		// v9.57: paused - no composite; the presented frame stays the
		// last real one (with HUD)
		// v9.67: capture FIRST (clean buffer = this frame's live menu
		// over the scene), then present the OTHER pair - written last
		// frame, never this one - then rotate the pairs. The menu is
		// 1 frame old: steady and live. v9.66 rotated before the
		// present, so the hold read the capture's write target - the
		// steady black/white frames of the new screenshot.
		sfxHoldRolling();
		sfxPauseHold(RwCameraGetRaster(Scene.camera));
		sfxHoldRotate();
		sfxUFUs = 0;
		return;
	}
	// v9.61: keep the frozen copy rolling - one StretchRect per
	// unpaused frame, so ESC holds the LAST finished game frame (HUD
	// included, menu never baked in - the v9.60 first-pause capture
	// baked the menu, which froze the pause selection).
	sfxHoldRolling();
	sfxHoldRotate();
	if(sfxLogDFE++ < 600)
		sfxLogLine("DFE n=%d\n", sfxLogDFE);

	bool doYCbCr = m_bYCbCrFilter;
	bool doBloom = config->doBloom;
	bool doToneMap = config->doToneMap;
	bool doDither = config->ps2Dither;
	float exposure = config->exposure;

	bool doVignette = config->vignetteStrength > 0.0f;
	bool doCA = config->chromaticAberration > 0.0f;
	bool doGrain = config->ps2Grain != 0;
	float grainStrength = config->ps2GrainStrength;
	bool doAutoExp = config->doAutoExposure != 0;

	// renderScale: the frame was already stretched to full size by the
	// deferred stretch (RenderScale_DeferredStretch, before the game
	// drew its HUD), so this
	// function needs no scale-specific handling - it just runs the normal
	// post-FX chain on the finished frame

	if(!doYCbCr && !doBloom && !doToneMap && !doDither && !doVignette && !doCA
			&& !doGrain && !doAutoExp && exposure == 1.0f)
		return;
	if(finalPS == nil)
		return;

	int w = RwRasterGetWidth(pRasterFrontBuffer);
	int h = RwRasterGetHeight(pRasterFrontBuffer);
	if(w <= 0 || h <= 0)
		return;

	if(doBloom && !ensureBloomBuffers(w, h))
		doBloom = false;
	if(doDither && !ensureDitherTexture(w, h))
		doDither = false;
	if(doGrain && !ensureSfxGrainTexture(w, h))
		doGrain = false;

	RwRaster *drawBuffer = RwCameraGetRaster(Scene.camera);
	if(drawBuffer == nil)
		return;

	// scene, after all game post effects, into the front buffer
	UpdateFrontBuffer();

	RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, (void*)rwFILTERLINEAR);
	RwRenderStateSet(rwRENDERSTATEFOGENABLE, (void*)FALSE);
	RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void*)FALSE);
	RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void*)FALSE);
	RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)FALSE);
	RwD3D9SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	// ---- auto exposure + night detection (smoothed ambient light level) ----
	// CTimeCycle_GetAmbient* give the current timecycle ambient color (0..1);
	// their average is ~0.3-0.5 in daylight and ~0.05-0.15 at night.
	static float sfxNight = 0.0f;
	static float sfxAutoExpCur = 1.0f;
	{
		float amb = ((float)CTimeCycle_GetAmbientRed()
				+ (float)CTimeCycle_GetAmbientGreen()
				+ (float)CTimeCycle_GetAmbientBlue()) / 3.0f;
		float night = 1.0f - amb / 0.45f;
		if(night < 0.0f) night = 0.0f;
		if(night > 1.0f) night = 1.0f;
		sfxNight += (night - sfxNight) * 0.05f;
		if(doAutoExp){
			float target = 1.0f + sfxNight * config->autoExposureGain;
			sfxAutoExpCur += (target - sfxAutoExpCur) * 0.05f;
		}else
			sfxAutoExpCur = 1.0f;
	}
	if(doAutoExp)
		exposure *= sfxAutoExpCur;
	float bloomIntensity = doBloom
			? config->bloomIntensity * (1.0f + config->bloomNightBoost * sfxNight)
			: 0.0f;

	// ---- bloom: bright pass + separable blur iterations, ping-pong A/B ----
	// Each (vertical, horizontal) pair starts and ends in `src`, so after every
	// iteration `src` holds the newest result - no pointer swapping needed.
	RwRaster *bloomResult = nil;
	RwTexture *bloomResultTex = nil;
	if(doBloom){
		RwRaster *src, *dst;
		float th[4];
		float off[4];
		float invw = 1.0f / w;
		float invh = 1.0f / h;
		int i;

		// bright pass: front buffer -> A
		th[0] = config->bloomThreshold;
		th[1] = th[2] = th[3] = 0.0f;
		RwD3D9SetPixelShaderConstant(0, th, 1);
		RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void*)pRasterFrontBuffer);
		setSceneRaster(bloomRasterA);
		overrideIm2dPixelShader = brightPS;
		RwIm2DRenderIndexedPrimitive(rwPRIMTYPETRILIST, colorfilterVerts, 4, colorfilterIndices, 6);
		overrideIm2dPixelShader = nil;

		src = bloomRasterA; dst = bloomRasterB;
		for(i = 0; i < config->bloomIterations; i++){
			// vertical: read src, write dst
			off[0] = 0.0f; off[1] = invh; off[2] = off[3] = 0.0f;
			RwD3D9SetPixelShaderConstant(0, off, 1);
			RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void*)src);
			setSceneRaster(dst);
			overrideIm2dPixelShader = bloomBlurPS;
			RwIm2DRenderIndexedPrimitive(rwPRIMTYPETRILIST, colorfilterVerts, 4, colorfilterIndices, 6);
			overrideIm2dPixelShader = nil;
			// horizontal: read dst, write back to src
			off[0] = invw; off[1] = 0.0f;
			RwD3D9SetPixelShaderConstant(0, off, 1);
			RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void*)dst);
			setSceneRaster(src);
			overrideIm2dPixelShader = bloomBlurPS;
			RwIm2DRenderIndexedPrimitive(rwPRIMTYPETRILIST, colorfilterVerts, 4, colorfilterIndices, 6);
			overrideIm2dPixelShader = nil;
		}
		// each (vertical, horizontal) pair ends in src, so src holds the
		// newest result after the last iteration
		bloomResult = src;
		bloomResultTex = bloomTextureA;
	}

	// ---- final composite: scene (+bloom), exposure, tone map, grading,
	//      vignette, chromatic aberration, grain/scanlines, dither ----
	if(doBloom)
		setSceneRaster(drawBuffer); // restore the scene raster as render target
	RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, (void*)rwFILTERNEAREST);
	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void*)pRasterFrontBuffer);

	// grading matrix: YCbCr tweak, or identity when it's not enabled
	Grade red, green, blue;
	if(doYCbCr){
		RwMatrix m = RGB2YUV;
		RwMatrix m2;
		m2.right.x = m_lumaScale;
		m2.up.x = 0.0f;
		m2.at.x = 0.0f;
		m2.pos.x = m_lumaOffset;
		m2.right.y = 0.0f;
		m2.up.y = m_cbScale;
		m2.at.y = 0.0f;
		m2.pos.y = m_cbOffset;
		m2.right.z = 0.0f;
		m2.up.z = 0.0f;
		m2.at.z = m_crScale;
		m2.pos.z = m_crOffset;

		RwMatrixOptimize(&m2, nil);

		RwMatrixTransform(&m, &m2, rwCOMBINEPOSTCONCAT);
		RwMatrixTransform(&m, &YUV2RGB, rwCOMBINEPOSTCONCAT);
		red.r = m.right.x;
		red.g = m.up.x;
		red.b = m.at.x;
		red.a = m.pos.x;
		green.r = m.right.y;
		green.g = m.up.y;
		green.b = m.at.y;
		green.a = m.pos.y;
		blue.r = m.right.z;
		blue.g = m.up.z;
		blue.b = m.at.z;
		blue.a = m.pos.z;
	}else{
		red.r = 1.0f; red.g = red.b = red.a = 0.0f;
		green.g = 1.0f; green.r = green.b = green.a = 0.0f;
		blue.b = 1.0f; blue.r = blue.g = blue.a = 0.0f;
	}
	RwD3D9SetPixelShaderConstant(0, &red, 1);
	RwD3D9SetPixelShaderConstant(1, &green, 1);
	RwD3D9SetPixelShaderConstant(2, &blue, 1);

	{
		float params[4];
		params[0] = exposure;
		params[1] = doToneMap ? config->whitePoint : 0.0f;
		params[2] = bloomResult ? bloomIntensity : 0.0f;
		params[3] = doDither ? 1.0f : 0.0f;
		RwD3D9SetPixelShaderConstant(3, params, 1);
	}
	{
		float fx[4];
		fx[0] = doVignette ? config->vignetteStrength : 0.0f;
		fx[1] = doCA ? config->chromaticAberration : 0.0f;
		fx[2] = doGrain ? grainStrength : 0.0f;
		fx[3] = 0.0f;
		RwD3D9SetPixelShaderConstant(4, fx, 1);
	}

	RwD3D9SetTexture(bloomResultTex, 1);
	RwD3D9SetTexture(doDither ? ditherTexture : nil, 2);
	RwD3D9SetTexture(doGrain ? sfxGrainTexture : nil, 3);

	overrideIm2dPixelShader = finalPS;
	RwIm2DRenderIndexedPrimitive(rwPRIMTYPETRILIST, colorfilterVerts, 4, colorfilterIndices, 6);
	overrideIm2dPixelShader = nil;
	// v9.30o: which raster is the camera drawing into at composite
	// time? A swap that sticks (overlay dims raster never restored)
	// redirects the composite + HUD away from the presented surface.
	{ RwRaster *cr = RwCameraGetRaster(Scene.camera);
	  static RwRaster *lastCr; static int lastW, lastH;
	  if(cr != lastCr || (cr != nil && (cr->width != lastW || cr->height != lastH))){
		  if(sfxProbeBudget-- > 0)
			  sfxLogLine("PROBE CRPTR f=%u pf=%d raster=%08x %dx%dx%d CHANGED\n",
				  sfxFrameNo, sfxProbeFrame,
				  (unsigned int)(void*)cr,
				  cr != nil ? cr->width : 0, cr != nil ? cr->height : 0,
				  cr != nil ? cr->depth : 0);
		  lastCr = cr;
		  lastW = cr != nil ? cr->width : 0;
		  lastH = cr != nil ? cr->height : 0;
	  }
	}
	// v9.30m: content of the FINAL composite output - the truth probe.
	sfxProbeBackBuffer();
	// v9.46: final-image colour-average + user-visible snap capture
	sfxProbeFinalBB();

	RwD3D9SetTexture(nil, 1);
	RwD3D9SetTexture(nil, 2);
	RwD3D9SetTexture(nil, 3);

	RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, (void*)rwFILTERLINEAR);
	RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void*)TRUE);
	RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void*)TRUE);
	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void*)NULL);
	RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)TRUE);
	RwD3D9SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
}

void (*CPostEffects::Initialise_orig)(void);
void
CPostEffects::Initialise(void)
{
	Initialise_orig();
	Initialise_skygfx(nil);
}

bool
CPostEffects::Initialise_skygfx(void*)
{
	InjectHook(0x7FB824, Im2dSetPixelShader_hook);
	InjectHook(0x7FB885, Im2DColorModulationHook);
	InjectHook(0x7FB8A6, Im2DAlphaModulationHook);

	CreateShaders();

	grainRaster = RwRasterCreate(64, 64, 32, rwRASTERTYPETEXTURE | rwRASTERFORMAT8888);
	return true;
}


// Colorcycle stuff, partly taken from NTAuthority...at least originally

class CFileMgr
{
public:
	static void* OpenFile(const char* filename, const char* mode);

	static void  CloseFile(void* file);
};

class CFileLoader
{
public:
	static char* LoadLine(void* file);
};

WRAPPER void* CFileMgr::OpenFile(const char* filename, const char* mode) { EAXJMP(0x538900); }
WRAPPER void  CFileMgr::CloseFile(void* file) { EAXJMP(0x5389D0); }
WRAPPER char* CFileLoader::LoadLine(void* file) { EAXJMP(0x536F80); }

static int &CTimeCycle__m_ExtraColourWeatherType = *(int*)0xB79E40;
static int &CTimeCycle__m_ExtraColour = *(int*)0xB79E44;
static int &CTimeCycle__m_bExtraColourOn = *(int*)0xB7C484;
static float &CTimeCycle__m_ExtraColourInter = *(float*)0xB79E3C;
static float &CWeather__UnderWaterness = *(float*)0xC8132C;
static float &CWeather__InTunnelness = *(float*)0xC81334;
static int &tunnelWeather = *(int*)0x8CDEE0;


// 24 instead of NUMHOURS because we might be using timecycle_24h with extended extra colour hours
Grade Colorcycle::redGrade[24][NUMWEATHERS];
Grade Colorcycle::greenGrade[24][NUMWEATHERS];
Grade Colorcycle::blueGrade[24][NUMWEATHERS];
bool Colorcycle::initialised;

GradeColorset::GradeColorset(int h, int w)
{
	this->red = Colorcycle::redGrade[h][w];
	this->green = Colorcycle::greenGrade[h][w];
	this->blue = Colorcycle::blueGrade[h][w];
}

void
GradeColorset::Interpolate(GradeColorset *a, GradeColorset *b, float fa, float fb)
{
	this->red.r = fa * a->red.r + fb * b->red.r;
	this->red.g = fa * a->red.g + fb * b->red.g;
	this->red.b = fa * a->red.b + fb * b->red.b;
	this->red.a = fa * a->red.a + fb * b->red.a;
	this->green.r = fa * a->green.r + fb * b->green.r;
	this->green.g = fa * a->green.g + fb * b->green.g;
	this->green.b = fa * a->green.b + fb * b->green.b;
	this->green.a = fa * a->green.a + fb * b->green.a;
	this->blue.r = fa * a->blue.r + fb * b->blue.r;
	this->blue.g = fa * a->blue.g + fb * b->blue.g;
	this->blue.b = fa * a->blue.b + fb * b->blue.b;
	this->blue.a = fa * a->blue.a + fb * b->blue.a;
}

static int timecycleHours[] = { 0, 5, 6, 7, 12, 19, 20, 22, 24 };

void
Colorcycle::Update(GradeColorset *colorset)
{
	float time;
	int curHourSel, nextHourSel;
	int curHour, nextHour;
	float timeInterp, invTimeInterp, weatherInterp, invWeatherInterp;

	time = CClock__ms_nGameClockMinutes / 60.0f
	     + CClock__ms_nGameClockSeconds / 3600.0f
	     + CClock__ms_nGameClockHours;
	if(time >= 23.999f)
		time = 23.999f;

	for(curHourSel = 0; time >= timecycleHours[curHourSel+1]; curHourSel++);
	nextHourSel = (curHourSel + 1) % NUMHOURS;
	curHour = timecycleHours[curHourSel];
	nextHour = timecycleHours[curHourSel+1];
	timeInterp = (time - curHour) / (float)(nextHour - curHour);
	invTimeInterp = 1.0f - timeInterp;
	weatherInterp = CWeather__InterpolationValue;
	invWeatherInterp = 1.0f - weatherInterp;
	GradeColorset curold(curHourSel, CWeather__OldWeatherType);
	GradeColorset nextold(nextHourSel, CWeather__OldWeatherType);
	GradeColorset curnew(curHourSel, CWeather__NewWeatherType);
	GradeColorset nextnew(nextHourSel, CWeather__NewWeatherType);

	// Skipping smog weather handling
	GradeColorset oldInterp, newInterp;
	oldInterp.Interpolate(&curold, &nextold, invTimeInterp, timeInterp);
	newInterp.Interpolate(&curnew, &nextnew, invTimeInterp, timeInterp);
	colorset->Interpolate(&oldInterp, &newInterp, invWeatherInterp, weatherInterp);

	float inc = CTimer__ms_fTimeStep/120.0f;
	if(CTimeCycle__m_bExtraColourOn){
		CTimeCycle__m_ExtraColourInter += inc;
		if(CTimeCycle__m_ExtraColourInter > 1.0f)
			CTimeCycle__m_ExtraColourInter = 1.0f;
	}else{
		CTimeCycle__m_ExtraColourInter -= inc;
		if(CTimeCycle__m_ExtraColourInter < 0.0f)
			CTimeCycle__m_ExtraColourInter = 0.0f;
	}
	if(CTimeCycle__m_ExtraColourInter > 0.0f){
		GradeColorset extraset(CTimeCycle__m_ExtraColour, CTimeCycle__m_ExtraColourWeatherType);
		colorset->Interpolate(colorset, &extraset, 1.0f-CTimeCycle__m_ExtraColourInter, CTimeCycle__m_ExtraColourInter);
	}

	if(CWeather__UnderWaterness > 0.0f){
		GradeColorset curuwset(curHourSel, 20);
		GradeColorset nextuwset(nextHourSel, 20);
		GradeColorset tmpset;
		tmpset.Interpolate(&curuwset, &nextuwset, invTimeInterp, timeInterp);
		colorset->Interpolate(colorset, &tmpset, 1.0f-CWeather__UnderWaterness, CWeather__UnderWaterness);
	}

	if(CWeather__InTunnelness > 0.0f){
		GradeColorset tunnelset(tunnelWeather % NUMHOURS, tunnelWeather / NUMHOURS + EXTRASTART);
		colorset->Interpolate(colorset, &tunnelset, 1.0f-CWeather__InTunnelness, CWeather__InTunnelness);
	}

}

void
Colorcycle::Initialise(void)
{
	int have24h = ModuleList().Get(L"timecycle24") != 0;
	for(int i = 0; i < 24; i++)
		for(int j = 0; j < NUMHOURS; j++){
			redGrade[j][i].r = 1.0f;
			redGrade[j][i].g = 0.0f;
			redGrade[j][i].b = 0.0f;
			redGrade[j][i].a = 0.0f;
			greenGrade[j][i].r = 0.0f;
			greenGrade[j][i].g = 1.0f;
			greenGrade[j][i].b = 0.0f;
			greenGrade[j][i].a = 0.0f;
			blueGrade[j][i].r = 0.0f;
			blueGrade[j][i].g = 0.0f;
			blueGrade[j][i].b = 1.0f;
			blueGrade[j][i].a = 0.0f;
		}
	void *f = CFileMgr::OpenFile("data/colorcycle.dat", "r");
	if(f){
		char *line;
		for(int i = 0; i < NUMWEATHERS; i++){
			for(int j = 0; j < NUMHOURS; j++){
				line = CFileLoader::LoadLine(f);
				sscanf(line, "%f %f %f %f %f %f %f %f %f %f %f %f",
				       &redGrade[j][i].r, &redGrade[j][i].g,
				       &redGrade[j][i].b, &redGrade[j][i].a,
				       &greenGrade[j][i].r, &greenGrade[j][i].g,
				       &greenGrade[j][i].b, &greenGrade[j][i].a,
				       &blueGrade[j][i].r, &blueGrade[j][i].g,
				       &blueGrade[j][i].b, &blueGrade[j][i].a);
				float sum;
				sum = redGrade[j][i].r + redGrade[j][i].g + redGrade[j][i].b;
				if(sum > 1.7f)
					redGrade[j][i].a -= (sum - 1.7f)*0.13f;
				sum = greenGrade[j][i].r + greenGrade[j][i].g + greenGrade[j][i].b;
				if(sum > 1.7f)
					greenGrade[j][i].a -= (sum - 1.7f)*0.13f;
				sum = blueGrade[j][i].r + blueGrade[j][i].g + blueGrade[j][i].b;
				if(sum > 1.7f)
					blueGrade[j][i].a -= (sum - 1.7f)*0.13f;


				redGrade[j][i].r /= 1.5f;
				redGrade[j][i].g /= 1.5f;
				redGrade[j][i].b /= 1.5f;
				redGrade[j][i].a /= 1.5f;
				greenGrade[j][i].r /= 1.5f;
				greenGrade[j][i].g /= 1.5f;
				greenGrade[j][i].b /= 1.5f;
				greenGrade[j][i].a /= 1.5f;
				blueGrade[j][i].r /= 1.5f;
				blueGrade[j][i].g /= 1.5f;
				blueGrade[j][i].b /= 1.5f;
				blueGrade[j][i].a /= 1.5f;
				//printf("%f %f %f %f X %f %f %f %f X %f %f %f %f\n",
				//	redGrade[j][i].r, redGrade[j][i].g, redGrade[j][i].b, redGrade[j][i].a,
				//	greenGrade[j][i].r, greenGrade[j][i].g, greenGrade[j][i].b, greenGrade[j][i].a,
				//	blueGrade[j][i].r, blueGrade[j][i].g, blueGrade[j][i].b, blueGrade[j][i].a);
			}
		}
		if(have24h)
			for(int j = 0; j < NUMHOURS; j++){
				redGrade[j+8][21] = redGrade[j][22];
				greenGrade[j+8][21] = greenGrade[j][22];
				blueGrade[j+8][21] = blueGrade[j][22];
			}
		CFileMgr::CloseFile(f);
	}
	initialised = true;
}
