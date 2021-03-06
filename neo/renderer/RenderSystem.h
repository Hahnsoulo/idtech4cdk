/*
===========================================================================

Doom 3 GPL Source Code
Copyright (C) 1999-2011 id Software LLC, a ZeniMax Media company. 

This file is part of the Doom 3 GPL Source Code (?Doom 3 Source Code?).  

Doom 3 Source Code is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Doom 3 Source Code is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Doom 3 Source Code.  If not, see <http://www.gnu.org/licenses/>.

In addition, the Doom 3 Source Code is also subject to certain additional terms. You should have received a copy of these additional terms immediately following the terms and conditions of the GNU General Public License which accompanied the Doom 3 Source Code.  If not, please request a copy in writing from id Software at the address below.

If you have questions concerning this license or the applicable additional terms, you may contact in writing id Software LLC, c/o ZeniMax Media Inc., Suite 120, Rockville, Maryland 20850 USA.

===========================================================================
*/

#ifndef __RENDERER_H__
#define __RENDERER_H__

class idImage;

/*
===============================================================================

	idRenderSystem is responsible for managing the screen, which can have
	multiple idRenderWorld and 2D drawing done on it.

===============================================================================
*/


// Contains variables specific to the OpenGL configuration being run right now.
// These are constant once the OpenGL subsystem is initialized.
typedef struct glconfig_s {
	const char			*renderer_string;
	const char			*vendor_string;
	const char			*version_string;
	const char			*extensions_string;
	const char			*wgl_extensions_string;

	float				glVersion;				// atof( version_string )


	int					maxTextureSize;			// queried from GL
	int					maxTextureUnits;
	int					maxTextureCoords;
	int					maxTextureImageUnits;
	float				maxTextureAnisotropy;

	int					colorBits, depthBits, stencilBits;

	bool				multitextureAvailable;
	bool				textureCompressionAvailable;
	bool				anisotropicAvailable;
	bool				textureLODBiasAvailable;
	bool				textureEnvAddAvailable;
	bool				textureEnvCombineAvailable;
	bool				registerCombinersAvailable;
	bool				cubeMapAvailable;
	bool				envDot3Available;
	bool				texture3DAvailable;
	bool				sharedTexturePaletteAvailable;
	bool				ARBVertexBufferObjectAvailable;
	bool				ARBVertexProgramAvailable;
	bool				ARBFragmentProgramAvailable;
	bool				twoSidedStencilAvailable;
	bool				textureNonPowerOfTwoAvailable;
	bool				depthBoundsTestAvailable;

	// ati r200 extensions
	bool				atiFragmentShaderAvailable;

	// ati r300
	bool				atiTwoSidedStencilAvailable;

	int					vidWidth, vidHeight;	// passed to R_BeginFrame

	int					displayFrequency;

	bool				isFullscreen;

	bool				allowNV30Path;
	bool				allowNV20Path;
	bool				allowNV10Path;
	bool				allowR200Path;
	bool				allowARB2Path;

	bool				isInitialized;
} glconfig_t;


// font support 
const int GLYPH_START			= 0;
const int GLYPH_END				= 255;
const int GLYPH_CHARSTART		= 32;
const int GLYPH_CHAREND			= 127;
const int GLYPHS_PER_FONT		= GLYPH_END - GLYPH_START + 1;

typedef struct {
	int					height;			// number of scan lines
	int					top;			// top of glyph in buffer
	int					bottom;			// bottom of glyph in buffer
	int					pitch;			// width for copying
	int					xSkip;			// x adjustment
	int					imageWidth;		// width of actual image
	int					imageHeight;	// height of actual image
	float				s;				// x offset in image where glyph starts
	float				t;				// y offset in image where glyph starts
	float				s2;
	float				t2;
	const idMaterial *	glyph;			// shader with the glyph
	char				shaderName[32];
} glyphInfo_t;

typedef struct {
	glyphInfo_t			glyphs [GLYPHS_PER_FONT];
	float				glyphScale;
	char				name[64];
} fontInfo_t;

typedef struct {
	fontInfo_t			fontInfoSmall;
	fontInfo_t			fontInfoMedium;
	fontInfo_t			fontInfoLarge;
	int					maxHeight;
	int					maxWidth;
	int					maxHeightSmall;
	int					maxWidthSmall;
	int					maxHeightMedium;
	int					maxWidthMedium;
	int					maxHeightLarge;
	int					maxWidthLarge;
	char				name[64];
} fontInfoEx_t;

const int SMALLCHAR_WIDTH		= 8;
const int SMALLCHAR_HEIGHT		= 16;
const int BIGCHAR_WIDTH			= 16;
const int BIGCHAR_HEIGHT		= 16;

// all drawing is done to a 640 x 480 virtual screen size
// and will be automatically scaled to the real resolution
const int SCREEN_WIDTH			= 1280;
const int SCREEN_HEIGHT			= 720;

class idRenderWorld;


class idRenderSystem {
public:

	virtual					~idRenderSystem() {}

	// set up cvars and basic data structures, but don't
	// init OpenGL, so it can also be used for dedicated servers
	virtual void			Init( void ) = 0;

	// only called before quitting
	virtual void			Shutdown( void ) = 0;

	virtual void			InitOpenGL( void ) = 0;

	virtual void			ShutdownOpenGL( void ) = 0;

	virtual bool			IsOpenGLRunning( void ) const = 0;

	virtual bool			IsFullScreen( void ) const = 0;
	virtual int				GetScreenWidth( void ) const = 0;
	virtual int				GetScreenHeight( void ) const = 0;

	// allocate a renderWorld to be used for drawing
	virtual idRenderWorld *	AllocRenderWorld( void ) = 0;
	virtual	void			FreeRenderWorld( idRenderWorld * rw ) = 0;

	// All data that will be used in a level should be
	// registered before rendering any frames to prevent disk hits,
	// but they can still be registered at a later time
	// if necessary.
	virtual void			BeginLevelLoad( void ) = 0;
	virtual void			EndLevelLoad( void ) = 0;

	// font support
	virtual bool			RegisterFont( const char *fontName, fontInfoEx_t &font ) = 0;

	// GUI drawing just involves shader parameter setting and axial image subsections
	virtual void			SetColor( const idVec4 &rgba ) = 0;
	virtual void			SetColor4( float r, float g, float b, float a ) = 0;

	virtual void			DrawStretchPic( const idDrawVert *verts, const glIndex_t *indexes, int vertCount, int indexCount, const idMaterial *material,
											bool clip = true, float min_x = 0.0f, float min_y = 0.0f, float max_x = SCREEN_WIDTH, float max_y = SCREEN_HEIGHT ) = 0;
	virtual void			DrawStretchPic( float x, float y, float w, float h, float s1, float t1, float s2, float t2, const idMaterial *material ) = 0;

	virtual void			DrawStretchTri ( idVec2 p1, idVec2 p2, idVec2 p3, idVec2 t1, idVec2 t2, idVec2 t3, const idMaterial *material ) = 0;
	virtual void			GlobalToNormalizedDeviceCoordinates( const idVec3 &global, idVec3 &ndc ) = 0;
	virtual void			GetGLSettings( int& width, int& height ) = 0;
	virtual void			PrintMemInfo( MemInfo_t *mi ) = 0;

	virtual void			DrawSmallChar( int x, int y, int ch, const idMaterial *material ) = 0;
	virtual void			DrawSmallStringExt( int x, int y, const char *string, const idVec4 &setColor, bool forceColor, const idMaterial *material ) = 0;
	virtual void			DrawBigChar( int x, int y, int ch, const idMaterial *material ) = 0;
	virtual void			DrawBigStringExt( int x, int y, const char *string, const idVec4 &setColor, bool forceColor, const idMaterial *material ) = 0;

	// dump all 2D drawing so far this frame to the demo file
	virtual void			WriteDemoPics() = 0;

	// draw the 2D pics that were saved out with the current demo frame
	virtual void			DrawDemoPics() = 0;

	// FIXME: add an interface for arbitrary point/texcoord drawing
// jmarshall
	virtual struct viewDef_s*	GetNewFrameViewdef( void ) = 0;
// jmarshall end

	// a frame cam consist of 2D drawing and potentially multiple 3D scenes
	// window sizes are needed to convert SCREEN_WIDTH / SCREEN_HEIGHT values
	virtual void			BeginFrame( int windowWidth, int windowHeight ) = 0;

	// if the pointers are not NULL, timing info will be returned
	virtual void			EndFrame( int *frontEndMsec, int *backEndMsec, bool swapFrontBack = true ) = 0;

	// aviDemo uses this.
	// Will automatically tile render large screen shots if necessary
	// Samples is the number of jittered frames for anti-aliasing
	// If ref == NULL, session->updateScreen will be used
	// This will perform swapbuffers, so it is NOT an approppriate way to
	// generate image files that happen during gameplay, as for savegame
	// markers.  Use WriteRender() instead.
	virtual void			TakeScreenshot( int width, int height, const char *fileName, int samples, struct renderView_s *ref ) = 0;

	// the render output can be cropped down to a subset of the real screen, as
	// for save-game reviews and split-screen multiplayer.  Users of the renderer
	// will not know the actual pixel size of the area they are rendering to

	// the x,y,width,height values are in virtual SCREEN_WIDTH / SCREEN_HEIGHT coordinates

	// to render to a texture, first set the crop size with makePowerOfTwo = true,
	// then perform all desired rendering, then capture to an image
	// if the specified physical dimensions are larger than the current cropped region, they will be cut down to fit
	virtual void			CropRenderSize( int width, int height, bool makePowerOfTwo = false, bool forceDimensions = false ) = 0;
	virtual void			CaptureRenderToImage( const char *imageName ) = 0;
	// fixAlpha will set all the alpha channel values to 0xff, which allows screen captures
	// to use the default tga loading code without having dimmed down areas in many places
	virtual void			CaptureRenderToFile( const char *fileName, bool fixAlpha = false ) = 0;
	virtual void			UnCrop() = 0;
	virtual void			GetCardCaps( bool &oldCard, bool &nv10or20 ) = 0;

	// the image has to be already loaded ( most straightforward way would be through a FindMaterial )
	// texture filter / mipmapping / repeat won't be modified by the upload
	// returns false if the image wasn't found
	virtual bool			UploadImage( const char *imageName, const byte *data, int width, int height ) = 0;

// jmarshall
	// Called by the editor/ingame and dmap to operate on light volumes
	virtual void			RenderLightFrustum( const struct renderLight_t &renderLight, idPlane lightFrustum[6] ) = 0;

	// Loads in a image file.
	virtual idImage			*FindImage( const char *path, bool clampToEdge = false ) = 0;

	virtual void			DeriveLightData(class idRenderLightLocal *light) = 0;
	virtual void			FreeLightDefDerivedData(class idRenderLightLocal *light) = 0;

	// Clears the frame buffer data.
	virtual void			ClearFrameBuffer( void ) = 0;

	// Projects the mouse 2d xy coordinate to a 3d world position(context safe).
	virtual void			ProjectMouseToWorldCoord( idVec2 mouseXY, idVec3 &worldPos ) = 0;

	// Writes a tga out to the fileSystem
	virtual void			WriteTGA( const char *filename, const byte *data, int width, int height, bool flipVertical ) = 0;
// jmarshall end
};


//
// idRenderTriSurfaceManagerLocal
//
class idRenderTriSurfaceManager {
public:
	virtual struct srfTriangles_s *CopyStaticTriSurf(struct srfTriangles_s const *) = 0;
	virtual void				AllocStaticTriSurfIndexes(struct srfTriangles_s *,int) = 0;
	virtual void				AllocStaticTriSurfVerts(struct srfTriangles_s *,int) = 0;
	virtual struct srfTriangles_s * AllocStaticTriSurf(void) = 0;
	virtual void				FreeStaticTriSurf(struct srfTriangles_s *) = 0;
	virtual struct srfTriangles_s *PolytopeSurface(int,class idPlane const *,class idWinding * *) = 0;
};

extern idRenderTriSurfaceManager *renderTriSurfaceManager;
extern idRenderSystem *			renderSystem;

//
// functions mainly intended for editor and dmap integration
//

// returns the frustum planes in world space
void R_RenderLightFrustum( const struct renderLight_s &renderLight, idPlane lightFrustum[6] );

// for use by dmap to do the carving-on-light-boundaries and for the editor for display
/*
====================
R_LightProjectionMatrix

====================
*/
ID_INLINE void R_LightProjectionMatrix( const idVec3 &origin, const idPlane &rearPlane, idVec4 mat[4] ) {
	idVec4		lv;
	float		lg;

	// calculate the homogenious light vector
	lv.x = origin.x;
	lv.y = origin.y;
	lv.z = origin.z;
	lv.w = 1;

	lg = rearPlane.ToVec4() * lv;

	// outer product
	mat[0][0] = lg -rearPlane[0] * lv[0];
	mat[0][1] = -rearPlane[1] * lv[0];
	mat[0][2] = -rearPlane[2] * lv[0];
	mat[0][3] = -rearPlane[3] * lv[0];

	mat[1][0] = -rearPlane[0] * lv[1];
	mat[1][1] = lg -rearPlane[1] * lv[1];
	mat[1][2] = -rearPlane[2] * lv[1];
	mat[1][3] = -rearPlane[3] * lv[1];

	mat[2][0] = -rearPlane[0] * lv[2];
	mat[2][1] = -rearPlane[1] * lv[2];
	mat[2][2] = lg -rearPlane[2] * lv[2];
	mat[2][3] = -rearPlane[3] * lv[2];

	mat[3][0] = -rearPlane[0] * lv[3];
	mat[3][1] = -rearPlane[1] * lv[3];
	mat[3][2] = -rearPlane[2] * lv[3];
	mat[3][3] = lg -rearPlane[3] * lv[3];
}

// used by the view shot taker
void R_ScreenshotFilename( int &lastNumber, const char *base, idStr &fileName );

#endif /* !__RENDERER_H__ */
