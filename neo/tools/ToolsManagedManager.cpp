// ToolsManagedManager.cpp
//

#include "precompiled.h"

#include "radiant/QERTYPES.H"
#include "radiant/editorEntity.h"
#include "radiant/EditorMap.h"

// jmarshall
#include "tools_managed.h"
#include "../renderer/Image.h"
// jmarshall end

using namespace ToolsManaged;
extern bool g_editorAlive;

IToolsManaged *toolsManaged;

TOOLS_EXPORTFUNC(cmdSystem, BufferCommandText, (const char *str), (CMD_EXEC_NOW, str) )
TOOLS_EXPORTFUNC_RET(fileSystem, OSPathToRelativePath, (const char *str), ( str) )
TOOLS_EXPORTFUNC_RET(fileSystem, RelativePathToOSPath, (const char *str), ( str) )
TOOLS_EXPORTFUNC_RET(fileSystem, OpenFileRead, (const char *relativePath, bool allowCopyFiles, const char* gamedir), ( relativePath, allowCopyFiles, gamedir ) )
TOOLS_EXPORTFUNC_RET(fileSystem, OpenFileWrite, (const char *relativePath,  const char* gamedir), ( relativePath,  gamedir ) )
TOOLS_EXPORTFUNC(fileSystem, CloseFile, (idFile *file), ( file ) )
TOOLS_EXPORTFUNC_RET(declManager, FindType, (declType_t type, const char *name, bool makeDefault), ( type, name, makeDefault ) )

// IdDict
TOOLS_EXPORTFUNC_NOOBJ( idDict, Set, (idDict *obj, const char *str, const char *str1), (str, str1) )
TOOLS_EXPORTFUNC_NOOBJ_RET( idDict, GetString, (idDict *obj, const char *key), (key) )
TOOLS_EXPORTFUNC_NOOBJ_RETTYPE( idDict, idVec3, GetVector, (idDict *obj, const char *key), (key) )
TOOLS_EXPORTFUNC_NOOBJ_RETTYPE( idDict, int, GetNumKeyVals, (idDict *obj), () )
TOOLS_EXPORTFUNC_NOOBJ_RETTYPE( idDict, idKeyValueInstance, GetKeyValInstance, (idDict *obj, int index), (index) )

float *GetCurrentViewPos();

void DrawRenderSurface( srfTriangles_t *surf, idImage *image, idVec3 origin, idAngles angle, bool cameraView );

extern "C" __declspec(dllexport) void TOOLAPI_Editor_DrawRenderSurf( srfTriangles_t *surf, idImage *image, float x, float y, float z, float yaw, float pitch, float roll, bool cameraView )
{
	DrawRenderSurface( surf, image, idVec3(x,y,z), idAngles(pitch, yaw, roll ), cameraView );
}

extern "C" __declspec(dllexport) idImage *TOOLAPI_Editor_FindImage(const char *path)
{
	return renderSystem->FindImage( path );
}

extern "C" __declspec(dllexport) float *TOOLAPI_Editor_GetViewPosition()
{
	return GetCurrentViewPos();
}

extern "C" __declspec(dllexport) srfTriangles_t *TOOLAPI_RenderWorld_GetVisibleVirtualTextureAreaSurface( idRenderWorld *world, int surfId )
{
	return world->GetVisibleVirtualTextureAreaSurface(surfId);
}

void SetProjectionMatrix(float width2, float height2) {
	float xfov = 90;
	float yfov = 2 * atan((float)height2 / width2) * idMath::M_RAD2DEG;
#if 0
	float screenaspect = (float)m_Camera.width / m_Camera.height;
	qglLoadIdentity();
	gluPerspective(yfov, screenaspect, 2, 8192);
#else
	float	xmin, xmax, ymin, ymax;
	float	width, height;
	float	zNear;
	float	projectionMatrix[16];

	//
	// set up projection matrix
	//
	zNear	= 1;

	ymax = zNear * tan( yfov * idMath::PI / 360.0f );
	ymin = -ymax;

	xmax = zNear * tan( xfov * idMath::PI / 360.0f );
	xmin = -xmax;

	width = xmax - xmin;
	height = ymax - ymin;

	projectionMatrix[0] = 2 * zNear / width;
	projectionMatrix[4] = 0;
	projectionMatrix[8] = ( xmax + xmin ) / width;	// normally 0
	projectionMatrix[12] = 0;

	projectionMatrix[1] = 0;
	projectionMatrix[5] = 2 * zNear / height;
	projectionMatrix[9] = ( ymax + ymin ) / height;	// normally 0
	projectionMatrix[13] = 0;

	// this is the far-plane-at-infinity formulation
	projectionMatrix[2] = 0;
	projectionMatrix[6] = 0;
	projectionMatrix[10] = -1;
	projectionMatrix[14] = -2 * zNear;

	projectionMatrix[3] = 0;
	projectionMatrix[7] = 0;
	projectionMatrix[11] = -1;
	projectionMatrix[15] = 0;

	qglLoadMatrixf( projectionMatrix );
#endif
}

extern "C" __declspec(dllexport) int *TOOLAPI_RenderWorld_GetVisibleVirtualTextureArea( idRenderWorld *world, int &numSurfaces, float width, float height, float x, float y, float z, float yaw, float pitch, float roll )
{
	renderView_t view;
	idAngles viewAngle;

	viewAngle.yaw = yaw;
	viewAngle.pitch = pitch;
	viewAngle.roll = roll;

	view.x = 0;
	view.y = 0;
	view.width = width;
	view.height = height;
	view.fov_x = 90.0f;
	view.fov_y = 2 * atan((float)height / width) * idMath::M_RAD2DEG;
	view.vieworg = idVec3(x, y, z);
	view.viewaxis = viewAngle.ToMat3();

	return world->GetVisibleVirtualTextureAreas( &view, numSurfaces );
}

extern "C" __declspec(dllexport) HDC TOOLAPI_Device_BeginRender( HWND hwnd, int width, int height )
{
	HDC hDC = ::GetDC(hwnd);

	if (!renderDevice->BindDeviceToWindow( hDC ) ) {
		return NULL;
	}

	qglMatrixMode( GL_PROJECTION );
	qglLoadIdentity();
	qglMatrixMode( GL_MODELVIEW );
	qglLoadIdentity();



	qglClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
	qglViewport(0, 0, width, height);
	qglScissor(0, 0, width, height);
	qglClear( GL_COLOR_BUFFER_BIT  | GL_DEPTH_BUFFER_BIT );

	qglCullFace(GL_FRONT);
	qglEnable(GL_CULL_FACE);
	qglShadeModel(GL_FLAT);
	qglPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	qglDisable(GL_BLEND);
	qglEnable(GL_DEPTH_TEST);
	qglDepthFunc(GL_LEQUAL);

	qglDisable(GL_LIGHTING);
	qglMatrixMode(GL_PROJECTION);

	SetProjectionMatrix(width, height);

	qglMatrixMode(GL_MODELVIEW);
	qglEnable(GL_TEXTURE_2D);

	qglEnableClientState(GL_VERTEX_ARRAY);
	qglEnableClientState(GL_COLOR_ARRAY);
	qglEnableClientState(GL_TEXTURE_COORD_ARRAY);
	
	// render it
	//renderSystem->BeginFrame( width , height );

	return hDC;
}

extern "C" __declspec(dllexport) void TOOLAPI_Device_EndRender( HWND hwnd, HDC hDC )
{
	//renderSystem->EndFrame( NULL, NULL );
	renderDevice->SwapBuffers(hDC);
	renderDevice->BindDeviceToWindow( NULL );
	::ReleaseDC( hwnd, hDC );
}

extern "C" __declspec(dllexport) idRenderWorld *TOOLAPI_Editor_LoadWorld( const char *name )
{
	static idRenderWorld *world = NULL;
	
	if(world != NULL)
	{
		renderSystem->FreeRenderWorld( world );
	}
	world = renderSystem->AllocRenderWorld();

	if(!world->InitFromMap( va("generated/maps%s", name) )) {
		renderSystem->FreeRenderWorld( world );
		world = NULL;
		return NULL;
	}

	return world;
}

extern "C" __declspec(dllexport) byte *TOOLAPI_Editor_GetDiffuseImageForMaterial( const char *mtr, int &width, int &height )
{
	const idMaterial *mat = declManager->FindMaterial( mtr );

	idImage *editorImage = mat->GetEditorImage();

	width = editorImage->uploadWidth;
	height = editorImage->uploadHeight;

	return editorImage->ReadDriverPixels( 0, 0 );
}

extern "C" __declspec(dllexport) int TOOLAPI_Editor_GetNumMaterials( void )
{
	return declManager->GetNumDecls( DECL_MATERIAL );
}

extern "C" __declspec(dllexport) const char *TOOLAPI_Editor_GetMaterialNameByIndex( int indx )
{
	const idMaterial *material = declManager->MaterialByIndex( indx, false );

	return material->GetName();
}

extern "C" __declspec(dllexport) const char *TOOLAPI_Editor_GetMapName( void )
{
	static idStr s;
	
	s = &currentmap[0];

	
	return s.StripFileExtension().c_str();
}


extern "C" __declspec(dllexport) idDict *TOOLAPI_Entity_GetTemplate( const char *type )
{
	idDeclEntityDef *def = (idDeclEntityDef *)declManager->FindType( DECL_ENTITYDEF, type );
	
	assert( def != NULL );

	return &def->dict;
}

extern "C" __declspec(dllexport) idDict *TOOLAPI_Entity_GetEntityDict( const char *name )
{
	entity_t *ent = FindEntity( "name", name );
	if(!ent) {
		return NULL;
	}

	return &ent->epairs;
}


/*
=================
idToolInterfaceLocal::GetValueFromManagedEnum
=================
*/
int idToolInterfaceLocal::GetValueFromManagedEnum(const char * enumTypeStrv, const char * enumValStrv)
{
	long value = 0;


	toolsManaged->GetValueFromManagedEnum( _com_util::ConvertStringToBSTR(enumTypeStrv), _com_util::ConvertStringToBSTR(enumValStrv), &value );
	return value;
}

void Brush_Update();
extern "C" __declspec(dllexport) void TOOLAPI_Brush_Update( void )
{
	Brush_Update();
}

// Export 

/*
==================
idToolInterfaceLocal::InitToolsManaged
==================
*/
void idToolInterfaceLocal::InitToolsManaged( void ) {
	CoCreateInstance( __uuidof(ToolsManagedPrivate), NULL, CLSCTX_INPROC_SERVER, __uuidof(IToolsManaged), (void **)&toolsManaged);
}

/*
==================
idToolInterfaceLocal::ShowDebugConsole
==================
*/
void idToolInterfaceLocal::ShowDebugConsole( void ) {
	toolsManaged->ShowDebugConsole();
}
/*
==================
idToolInterfaceLocal::RadiantPrint
==================
*/
void idToolInterfaceLocal::RadiantPrint( const char *text ) {
	if ( g_editorAlive ) {
		toolsManaged->PrintToConsole( _com_util::ConvertStringToBSTR(text) );
	}

	
}
