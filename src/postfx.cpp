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




void
CPostEffects::UpdateFrontBuffer(void)
{
	RwCameraEndUpdate(Scene.camera);
	RwRasterPushContext(CPostEffects::pRasterFrontBuffer);
	RwRasterRenderFast(RwCameraGetRaster(Scene.camera), 0, 0);
	RwRasterPopContext();
	RwCameraBeginUpdate(Scene.camera);
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

void *blurPS, *radiosityPS;

void
CPostEffects::Radiosity_shader(int intensityLimit, int filterPasses, int renderPasses, int intensity)
{
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

void
CPostEffects::ColourFilter_switch(RwRGBA rgb1, RwRGBA rgb2)
{
	{
		static bool keystate = false;
		if(GetAsyncKeyState(config->keys[0]) & 0x8000){
			if(!keystate){
				keystate = true;
				if(numConfigs){
					currentConfig = (currentConfig+1) % numConfigs;
					CMessages__AddMessageJumpQWithNumber("skygfx~1~.ini", 500, 0, currentConfig + 1, -1, -1, -1, -1, -1, false);
					setConfig();
				}
			}
		}else
			keystate = false;
	}

	{
		static bool keystate = false;
		if(GetAsyncKeyState(config->keys[1]) & 0x8000){
			if(!keystate){
				keystate = true;
				reloadAllInis();
			}
		}else
			keystate = false;
	}

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
static FILE *sfxLog;
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
static int sfxLogB, sfxLogR;
static int sfxLogH;
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
		fprintf(sfxLog, "==== skygfx renderScale diagnostics (build v9.30) ====\n");
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

static void
sfxHDRrelease(void)
{
	if(sfxHDRtex)
		((IDirect3DTexture9*)sfxHDRtex)->Release();
	sfxHDRtex = nil;
	sfxHDRsurf = nil;
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
	sfxHDRtex->GetSurfaceLevel(0, (IDirect3DSurface9**)&sfxHDRsurf);
	if(sfxHDRsurf == nil){
		sfxLogLine("HDR: GetSurfaceLevel failed - feature off\n");
		sfxHDRrelease();
		sfxHDRfailed = 1;
		return 0;
	}
	sfxHDRw = w;
	sfxHDRh = h;
	sfxLogLine("HDR: fp16 %dx%d A16B16G16R16F ready\n", w, h);
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
			if(sfxLogH++ < 16)
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
	if(sfxW2DNoClear && (flags & 0x100u)){
		flags &= ~0x100u;
		sfxW2DZStripped++;
		if(sfxLogZ++ < 8)
			sfxLogLine("Zc strip f=%x\n", flags);
	}else if(sfxLogZ0++ < 8)
		sfxLogLine("Zc f=%x c=%x\n", flags, color);
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
			if(sfxLogH++ < 8)
				sfxLogLine("H begin: scene window -> fp16 %dx%d\n", sfxHDRw, sfxHDRh);
		}else
			sfxLogLine("H begin: fp16 unavailable - stock path\n");
	}
	if(sfxLogB++ < 40)
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
	if(sfxLogR++ < 40)
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
	HRESULT hr;
	float uw, vh;
	int i;
	if(d3d9device == nil || sfxHDRsurf == nil || camR == nil)
		return 0;
	if(d3d9device->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &bb) != D3D_OK
		|| bb == nil)
		return 0;
	d3d9device->GetRenderState(D3DRS_ZENABLE, &oldZen);
	d3d9device->GetRenderState(D3DRS_CULLMODE, &oldCull);
	d3d9device->GetRenderState(D3DRS_ALPHABLENDENABLE, &oldBlend);
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
	d3d9device->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	d3d9device->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
	d3d9device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	d3d9device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	d3d9device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
	d3d9device->SetTexture(0, (IDirect3DTexture9*)sfxHDRtex);
	d3d9device->SetFVF(D3DFVF_XYZRHW | D3DFVF_TEX1);
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
	d3d9device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, v, sizeof(struct HDRVtx));
	// detach the FP16 texture again and restore what was switched off;
	// RW re-issues its own state on the next camera/draw cycle
	d3d9device->SetTexture(0, nil);
	d3d9device->SetRenderState(D3DRS_ZENABLE, oldZen);
	d3d9device->SetRenderState(D3DRS_CULLMODE, oldCull);
	d3d9device->SetRenderState(D3DRS_ALPHABLENDENABLE, oldBlend);
	RwCameraBeginUpdate(Scene.camera);
	if(sfxLogR++ < 40)
		sfxLogLine("H2 resolve fp16 %ux%u -> %ux%u @coronas\n",
			sfxScaleW, sfxScaleH, camR->width, camR->height);
	return 1;
}

// v9.19: the deferred end-of-frame stretch. Runs from the swallowed
// CCoronas::Render stub inside RenderEffects - after CMovingThings::Render
// (Project2DFX LOD lights) and before the fx/HUD draws that need the full
// raster - and also from DrawFinalEffects as a safety net.
static void
RenderScale_DeferredStretch(void)
{
	RwRaster *camR;
	sfxStretchPending = 0;
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
	if(sfxHDRon && sfxHDRresolve(camR)){
		// resolved straight from the FP16 buffer; the RsGlobal and
		// NoClear cleanup at the end of this function is shared
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
		if(sfxLogR++ < 40)
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
