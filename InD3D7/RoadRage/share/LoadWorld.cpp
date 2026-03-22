//-----------------------------------------------------------------------------
// File: LoadWorld.cpp
//
// Desc: Code for loading the RoadRage 3D world
//
// Copyright (c) 1999 William Chin and Peter Kovach. All rights reserved.
//-----------------------------------------------------------------------------

#include "resource.h"
#include <math.h>
#include <time.h>
#include <stdio.h>
#include "D3DApp.h"
#include <dsound.h>
#include "D3DUtil.h"
#include "world.hpp"
#include "3Dengine.hpp"
#include "RoadRage.hpp"
#include "Import3DS.hpp"
#include "LoadWorld.hpp"

#define	k3DS_MODEL 1	

extern C3DS* pC3DS;
extern CMyD3DApplication* pCMyApp;

int player_count = 0;
int op_gun_count = 0;
int your_gun_count = 0;
int car_count = 0;
int type_num;
int num_imported_models_loaded = 0;
TCHAR g_model_filename[256];


CLoadWorld* pCWorld;


CLoadWorld::CLoadWorld()
{
	pCWorld = this;
}

BOOL CLoadWorld::LoadWorldMap(HWND hwnd, const TCHAR *filename)
{ 
	FILE *fp = NULL;    
	TCHAR s[256] = {}; int sSIZE=ARRAYSIZE(s);
	TCHAR p[256] = {}; int pSIZE=ARRAYSIZE(p);
	TCHAR buffer[100] = {};
	int y_count=30;
	int done=0;
	int object_count=0;
	int vert_count=0;
	int pv_count=0;
	int poly_count=0;
	int object_id;
	BOOL lwm_start_flag=TRUE;
	int num_lverts;
	int i;
	int mem_counter = 0;
	int lit_vert;
	BYTE red, green, blue;


    errno_t ferr = _tfopen_s(&fp, filename, _T("r"));
    if(fp==NULL)
    {     
		PrintMessage(hwnd, _T("Error can't load file "), filename, SCN_AND_FILE);
        MessageBox(hwnd, filename, _T("Error can't load file"), MB_OK);
		return FALSE;
    }

	PrintMessage(hwnd, _T("Loading map "), filename, SCN_AND_FILE);
	pCMyApp->num_light_sources_in_map = 0;

	while(done==0)
	{
		_ftscanf_s( fp, _T("%s"), &s,sSIZE );

		if(_tcscmp(s,_T("OBJECT"))==0)
		{
			_ftscanf_s( fp, _T("%s"), &p,pSIZE );
			
			object_id = CheckObjectId(hwnd, p);
			if(object_id == -1)
			{
				PrintMessage(hwnd, _T("Error Bad Object ID in: LoadWorld "), p, SCN_AND_FILE);
				MessageBox(hwnd,_T("Error Bad Object ID in: LoadWorld"),NULL,MB_OK);
				return FALSE;
			}
			if(lwm_start_flag == FALSE)
				object_count++;

			pCMyApp->oblist[object_count].type = object_id;
			
			num_lverts = pCMyApp->num_vert_per_object[object_id];
			pCMyApp->oblist[object_count].lit = new LIGHT[num_lverts];
			mem_counter += sizeof(LIGHT) * num_lverts;

			pCMyApp->oblist[object_count].light_source = new LIGHTSOURCE;
			pCMyApp->oblist[object_count].light_source->command = 0;
			pCMyApp->num_light_sources_in_map++;	
		
			for(i = 0; i < num_lverts; i++)
			{
				pCMyApp->oblist[object_count].lit[i].r = 0;
				pCMyApp->oblist[object_count].lit[i].g = 0;
				pCMyApp->oblist[object_count].lit[i].b = 0;
			}

			lwm_start_flag = FALSE;
		}
		
		if(_tcscmp(s,_T("CO_ORDINATES"))==0)
		{
			_ftscanf_s( fp, _T("%s"), &p,pSIZE );
			pCMyApp->oblist[object_count].x = (float)_ttof(p);
			
			_ftscanf_s( fp, _T("%s"), &p,pSIZE );
			pCMyApp->oblist[object_count].y = (float)_ttof(p);
			
			_ftscanf_s( fp, _T("%s"), &p,pSIZE );
			pCMyApp->oblist[object_count].z = (float)_ttof(p);
		}

		if(_tcscmp(s,_T("ROT_ANGLE"))==0)
		{
			_ftscanf_s( fp, _T("%s"), &p,pSIZE );
			pCMyApp->oblist[object_count].rot_angle = (float)_ttof(p);
		}

		if(_tcscmp(s,_T("LIGHT_ON_VERT"))==0)
		{	
			_ftscanf_s( fp, _T("%s"), &p,pSIZE );
			lit_vert = _ttoi(p);

			_ftscanf_s( fp, _T("%s"), &p,pSIZE );
			red = _ttoi(p);
			pCMyApp->oblist[object_count].lit[lit_vert].r = red;

			_ftscanf_s( fp, _T("%s"), &p,pSIZE );
			green = _ttoi(p);
			pCMyApp->oblist[object_count].lit[lit_vert].g = green;

			_ftscanf_s( fp, _T("%s"), &p,pSIZE );
			blue = _ttoi(p);
			pCMyApp->oblist[object_count].lit[lit_vert].b = blue;
		}
		
		
		if(_tcscmp(s,_T("LIGHT_SOURCE"))==0)
		{	
			_ftscanf_s( fp, _T("%s"), &p,pSIZE );

			if(_tcscmp(p,_T("Spotlight")) == 0)
				pCMyApp->oblist[object_count].light_source->command = SPOT_LIGHT_SOURCE;
				
			if(_tcscmp(p,_T("directional")) == 0)
				pCMyApp->oblist[object_count].light_source->command = DIRECTIONAL_LIGHT_SOURCE;

			if(_tcscmp(p,_T("point")) == 0)
				pCMyApp->oblist[object_count].light_source->command = POINT_LIGHT_SOURCE;
			
			_ftscanf_s( fp, _T("%s"), &p,pSIZE );

			if(_tcscmp(p,_T("POS"))==0)	
			{
				_ftscanf_s( fp, _T("%s"), &p,pSIZE );
				pCMyApp->oblist[object_count].light_source->position_x = (float)_ttof(p);
				_ftscanf_s( fp, _T("%s"), &p,pSIZE );
				pCMyApp->oblist[object_count].light_source->position_y = (float)_ttof(p);
				_ftscanf_s( fp, _T("%s"), &p,pSIZE );
				pCMyApp->oblist[object_count].light_source->position_z = (float)_ttof(p);
			}

			_ftscanf_s( fp, _T("%s"), &p,pSIZE );

			if(_tcscmp(p,_T("DIR"))==0)
			{
				_ftscanf_s( fp, _T("%s"), &p,pSIZE );
				pCMyApp->oblist[object_count].light_source->direction_x = (float)_ttof(p);
				_ftscanf_s( fp, _T("%s"), &p,pSIZE );
				pCMyApp->oblist[object_count].light_source->direction_y = (float)_ttof(p);
				_ftscanf_s( fp, _T("%s"), &p,pSIZE );
				pCMyApp->oblist[object_count].light_source->direction_z = (float)_ttof(p);
			}

			
		}

		if(_tcscmp(s,_T("END_FILE"))==0)
		{
			_ftscanf_s( fp, _T("%s"), &p,pSIZE );	
			pCMyApp->oblist_length = object_count+1;
			done=1;
		}
	}

	fclose(fp);	
	
	_itot_s(pCMyApp->oblist_length, buffer, 10);
	PrintMessage(hwnd, buffer, _T(" map objects loaded (oblist_length)"),SCN_AND_FILE);
	PrintMessage(hwnd, _T("\n"), NULL, LOGFILE_ONLY);
	
	return TRUE;
}

int CLoadWorld::CheckObjectId(HWND hwnd, const TCHAR *p) 
{
	int i;
	const TCHAR *buffer2;

	for(i = 0; i < pCMyApp->obdata_length; i++)
	{
		buffer2 = pCMyApp->obdata[i].name;

		if(_tcscmp(buffer2, p)==0)
		{
			return i;
		}
	}
	PrintMessage(hwnd, buffer2, _T("ERROR bad ID in: CheckObjectId"),SCN_AND_FILE);
	MessageBox(hwnd,buffer2,_T("Bad ID in: CheckObjectId"), MB_OK);
	
	return -1; //error
}

BOOL CLoadWorld::InitPreCompiledWorldMap(HWND hwnd, const TCHAR *filename)
{
	FILE *fp = NULL;    
	TCHAR s[256] = {}; const int sSIZE = ARRAYSIZE(s);
	int w;
	int done = 0;
	int count;
	int cell_x, cell_z;
	int mem_count;


	PrintMessage(hwnd, _T("InitPreCompiledWorldMap: starting\n"), NULL, LOGFILE_ONLY);
		
	_tfopen_s(&fp, filename, _T("r"));
    if(fp==NULL)
    {     
		PrintMessage(hwnd, _T("Error can't load "), filename, SCN_AND_FILE);
        MessageBox(hwnd,filename, _T("Error can't load file"),MB_OK);
		return FALSE;
	}

	for(cell_z = 0; cell_z < 200; cell_z++)
	{
		for(cell_x = 0; cell_x < 200; cell_x++)
		{
			pCMyApp->cell[cell_x][cell_z] = NULL;
			pCMyApp->cell_length[cell_x][cell_z] = 0;
		}
	}
	
	mem_count = 0;

	while(done==0)
	{
		_ftscanf_s( fp, _T("%s"), &s,sSIZE );

		if(_tcscmp(s,_T("END_FILE"))==0)
		{	
			done=1;
			fclose(fp);	
			PrintMessage(hwnd, _T("InitPreCompiledWorldMap: success\n\n"), NULL, LOGFILE_ONLY);
			return TRUE;
		}

		cell_x = _ttoi(s);

		if((cell_x >=0) && (cell_x <=200))
		{
			_ftscanf_s( fp, _T("%s"), &s,sSIZE );
			cell_z = _ttoi(s);

			if((cell_z >=0) && (cell_z <=200))
			{
				_ftscanf_s( fp, _T("%s"), &s,sSIZE );
				count = _ttoi(s);
			
				pCMyApp->cell[cell_x][cell_z] = new int[count];
				mem_count = mem_count + (count* sizeof(int));
							
				for(w=0;w<count;w++)
				{
					_ftscanf_s( fp, _T("%s"), &s,sSIZE );
					pCMyApp->cell[cell_x][cell_z][w] = _ttoi(s);
				}
				
				pCMyApp->cell_length[cell_x][cell_z] = count;
			}
		}
		
	} // end while		

	return FALSE;
}

BOOL CLoadWorld::LoadObjectData(HWND hwnd, const TCHAR *filename)
{
	FILE *fp = NULL;
	int i;
	int done		  = 0;
	int object_id	  = 0;
	int object_count  = 0;
	int old_object_id = 0;
	int vert_count	  = 0;
	int pv_count	  = 0;
	int poly_count	  = 0;
	int conn_cnt      = 0;
	int num_v;
	BOOL command_error;
	float dat_scale;
	TCHAR buffer[256] = {};
	TCHAR s[256] = {}; int sSIZE = ARRAYSIZE(s);	
	TCHAR p[256] = {}; int pSIZE = ARRAYSIZE(p);


    _tfopen_s(&fp, filename, _T("r"));

    if(fp==NULL)
	{     
		PrintMessage(hwnd, _T("ERROR can't load "), filename, SCN_AND_FILE);
		MessageBox(hwnd, filename, _T("Error can't load file"),MB_OK);
		return FALSE;
    }

	PrintMessage(hwnd, _T("Loading "), filename, SCN_AND_FILE);
	
	while(done==0)
	{
		command_error = TRUE;

		_ftscanf_s( fp, _T("%s"), &s,sSIZE );

		if(_tcscmp(s,_T("OBJECT"))==0)
		{
			dat_scale = 1.0; 

			old_object_id = object_id;

			_ftscanf_s( fp, _T("%s"), &p,pSIZE );	// read object ID
			object_id = _ttoi(p);

			// find the highest object_id

			if(object_id > object_count)
				object_count = object_id;

			if((object_id < 0) || (object_id > 99))
			{
				MessageBox(hwnd,_T("Error Bad Object ID in: LoadObjectData"),NULL,MB_OK);
				return FALSE;
			}

			pCMyApp->num_vert_per_object [old_object_id] = vert_count;
			pCMyApp->num_polys_per_object[old_object_id] = poly_count;
			vert_count=0;
			poly_count=0;
			conn_cnt=0;
			
			_ftscanf_s( fp, _T("%s"), &p,pSIZE );	// read object name
			_tcscpy_s(pCMyApp->obdata[object_id].name, p);  
			command_error = FALSE;
		}

		if(_tcscmp(s,_T("SCALE"))==0)
		{
			_ftscanf_s( fp, _T("%s"), &p,pSIZE );
			dat_scale = (float)_ttof(p);
			command_error = FALSE;
		}

		if(_tcscmp(s,_T("TEXTURE"))== 0)
		{
			_ftscanf_s( fp, _T("%s"), &p,pSIZE );
			command_error = FALSE;
		}

		if(_tcscmp(s,_T("TYPE"))==0)
		{
			_ftscanf_s( fp, _T("%s"), &p,pSIZE );
			command_error = FALSE;
		}

		if(_tcscmp(s,_T("TRI"))==0)
		{
			for(i=0;i<3;i++)
			{
				ReadObDataVert(fp, object_id, vert_count, dat_scale);
				vert_count++;
			}
			
			pCMyApp->obdata[object_id].num_vert[poly_count] = 3;
			pCMyApp->obdata[object_id].poly_cmd[poly_count] = D3DPT_TRIANGLELIST; //POLY_CMD_TRI;
			poly_count++;
			command_error = FALSE;
		}

		if(_tcscmp(s,_T("QUAD"))==0)
		{
			ReadObDataVert(fp, object_id, vert_count,   dat_scale);
			ReadObDataVert(fp, object_id, vert_count+1, dat_scale);
			ReadObDataVert(fp, object_id, vert_count+3, dat_scale);
			ReadObDataVert(fp, object_id, vert_count+2, dat_scale);
			
			vert_count+=4;
			
			pCMyApp->obdata[object_id].num_vert[poly_count] = 4;
			pCMyApp->obdata[object_id].poly_cmd[poly_count] = D3DPT_TRIANGLESTRIP; //POLY_CMD_QUAD;
			poly_count++;
			command_error = FALSE;
		}

		if(_tcscmp(s,_T("TRITEX"))==0)
		{
			for(i=0;i<3;i++)
			{
				ReadObDataVertEx(fp, object_id, vert_count, dat_scale);
				vert_count++;
			}
			
			pCMyApp->obdata[object_id].num_vert[poly_count] = 3;
			pCMyApp->obdata[object_id].poly_cmd[poly_count] = D3DPT_TRIANGLELIST;// POLY_CMD_TRI_TEX;
			poly_count++;
			command_error = FALSE;
		}

		if(_tcscmp(s,_T("QUADTEX"))==0)
		{
			ReadObDataVertEx(fp, object_id, vert_count,   dat_scale);
			ReadObDataVertEx(fp, object_id, vert_count+1, dat_scale);
			ReadObDataVertEx(fp, object_id, vert_count+3, dat_scale);
			ReadObDataVertEx(fp, object_id, vert_count+2, dat_scale);
			
			vert_count+=4;
			
			pCMyApp->obdata[object_id].num_vert[poly_count] = 4;
			pCMyApp->obdata[object_id].poly_cmd[poly_count] = D3DPT_TRIANGLESTRIP; //POLY_CMD_QUAD_TEX;
			poly_count++;
			command_error = FALSE;
		}

		if(_tcscmp(s,_T("TRI_STRIP"))==0)
		{
			// Get numbers of verts in triangle strip
			_ftscanf_s( fp, _T("%s"), &p,pSIZE );	
			num_v = _ttoi(p);

			for(i = 0; i < num_v; i++)
			{
				ReadObDataVertEx(fp, object_id, vert_count, dat_scale);
				vert_count++;
			}

			pCMyApp->obdata[object_id].num_vert[poly_count] = num_v;
			pCMyApp->obdata[object_id].poly_cmd[poly_count] = D3DPT_TRIANGLESTRIP; //POLY_CMD_TRI_STRIP;
			poly_count++;
			command_error = FALSE;
		}

		if(_tcscmp(s,_T("TRI_FAN"))==0)
		{
			// Get numbers of verts in triangle fan
			_ftscanf_s( fp, _T("%s"), &p,pSIZE );	
			num_v = _ttoi(p);

			for(i = 0; i < num_v; i++)
			{
				ReadObDataVertEx(fp, object_id, vert_count, dat_scale);
				vert_count++;
			}

			pCMyApp->obdata[object_id].num_vert[poly_count] = num_v;
			pCMyApp->obdata[object_id].poly_cmd[poly_count] = D3DPT_TRIANGLEFAN; //POLY_CMD_TRI_FAN;
			poly_count++;
			command_error = FALSE;
		}

		if(_tcscmp(s,_T("CONNECTION"))==0)
		{
			if(conn_cnt<4)
			{
				_ftscanf_s( fp, _T("%s"), &p,pSIZE );
				pCMyApp->obdata[object_id].connection[conn_cnt].x = (float)_ttof(p);
				_ftscanf_s( fp, _T("%s"), &p,pSIZE );
				pCMyApp->obdata[object_id].connection[conn_cnt].y = (float)_ttof(p);
				_ftscanf_s( fp, _T("%s"), &p,pSIZE );
				pCMyApp->obdata[object_id].connection[conn_cnt].z = (float)_ttof(p);
				conn_cnt++;
			}
			else
			{
				_ftscanf_s( fp, _T("%s"), &p,pSIZE );
				_ftscanf_s( fp, _T("%s"), &p,pSIZE );
				_ftscanf_s( fp, _T("%s"), &p,pSIZE );
			}
			command_error = FALSE;
		}

		if(_tcscmp(s,_T("END_FILE"))==0)
		{		
			pCMyApp->num_vert_per_object[object_id]=vert_count;
			pCMyApp->num_polys_per_object[object_id]=poly_count;
			pCMyApp->obdata_length = object_count+1;
			command_error = FALSE;
			done=1;
		}

		if(command_error == TRUE)
		{
			_itot_s(object_id, buffer, 10);
			PrintMessage(NULL, _T("CLoadWorld::LoadObjectData - ERROR in objects.dat, object : "), buffer, LOGFILE_ONLY);
			MessageBox(hwnd, s,_T("Unrecognized command"),MB_OK);
			fclose(fp);	
			return FALSE;
		}
	}
	fclose(fp);	
	
	_itot_s(pCMyApp->obdata_length, buffer, 10);
	
	PrintMessage(hwnd, buffer, _T(" DAT objects loaded") ,SCN_AND_FILE);
	PrintMessage(hwnd, _T("\n"), NULL, LOGFILE_ONLY);
	
	return TRUE;
}

BOOL CLoadWorld::ReadObDataVertEx(FILE *fp, int object_id, int vert_count, float dat_scale)
{ 
	float x,y,z;
	TCHAR p[256] = {}; const int pSIZE=ARRAYSIZE(p);

	_ftscanf_s( fp, _T("%s"), &p,pSIZE );
	x = dat_scale * (float)_ttof(p);
	pCMyApp->obdata[object_id].v[vert_count].x = x;
		
	_ftscanf_s( fp, _T("%s"), &p,pSIZE );
	y = dat_scale * (float)_ttof(p);
	pCMyApp->obdata[object_id].v[vert_count].y = y;
		
	_ftscanf_s( fp, _T("%s"), &p,pSIZE );
	z = dat_scale * (float)_ttof(p);
	pCMyApp->obdata[object_id].v[vert_count].z = z;


	_ftscanf_s( fp, _T("%s"), &p,pSIZE );
	pCMyApp->obdata[object_id].t[vert_count].x = (float)_ttof(p);

	_ftscanf_s( fp, _T("%s"), &p,pSIZE );
	pCMyApp->obdata[object_id].t[vert_count].y = (float)_ttof(p);
		
	return TRUE;
}

BOOL CLoadWorld::ReadObDataVert(FILE *fp, int object_id, int vert_count, float dat_scale)
{  
	float x,y,z;
	TCHAR p[256]={}; const int pSIZE=ARRAYSIZE(p);

	_ftscanf_s( fp, _T("%s"), &p,pSIZE );
	x = dat_scale * (float)_ttof(p);
	pCMyApp->obdata[object_id].v[vert_count].x = x;
		
	_ftscanf_s( fp, _T("%s"), &p,pSIZE );
	y = dat_scale * (float)_ttof(p);
	pCMyApp->obdata[object_id].v[vert_count].y = y;
		
	_ftscanf_s( fp, _T("%s"), &p,pSIZE );
	z = dat_scale * (float)_ttof(p);
	pCMyApp->obdata[object_id].v[vert_count].z = z;

	return TRUE;
}


BOOL CLoadWorld::LoadImportedModelList(HWND hwnd, const TCHAR *filename)
{
	FILE *fp = NULL;    
	TCHAR p[256]={}; const int pSIZE=ARRAYSIZE(p);
	int done = 0;
	int i;
	TCHAR command[256]={}; const int commandSIZE=ARRAYSIZE(command);
	float f;
	int model_id;
	TCHAR model_filename[256]={};
	float scale;
	BOOL command_recognised;	
	BOOL Model_loaded_flags[1000] = {FALSE};

	pCMyApp->num_players   = 0;
	pCMyApp->num_your_guns = 0;
	pCMyApp->num_op_guns   = 0;

	for(i = 0; i < 10; i++)
	{
		pCMyApp->debug[i].current_sequence = 0;
		pCMyApp->debug[i].current_frame = 0;
	}

	_tfopen_s(&fp, filename, _T("r"));

    if(fp==NULL)
	{     
		PrintMessage(hwnd, _T("ERROR can't load "), filename, SCN_AND_FILE);
		MessageBox(hwnd, filename, _T("Error can't load file"),MB_OK);
		return FALSE;
    }

	PrintMessage(hwnd, _T("Loading "), filename, SCN_AND_FILE);

	_ftscanf_s( fp, _T("%s"), &command,commandSIZE );

	if(_tcscmp(command,_T("FILENAME")) == 0)
		_ftscanf_s( fp, _T("%s"), &p,pSIZE );	// ignore comment
	else
		return FALSE;

	while(done == 0)
	{
		command_recognised = FALSE;
		scale = (float)0.4;

		_ftscanf_s( fp, _T("%s"), &command,commandSIZE ); // get next command

		
		_ftscanf_s( fp, _T("%s"), &p,pSIZE );	// read player number
		type_num = _ttoi(p);
			
		_ftscanf_s( fp, _T("%s"), &p,pSIZE );	// find model file format - 3DS ?
					
		if(_tcscmp(p,_T("3DS")) == 0)
			pCMyApp->model_format = k3DS_MODEL;

		_ftscanf_s( fp, _T("%s"), &p,pSIZE );	//  ignore comment "model_id"  

		_ftscanf_s( fp, _T("%s"), &p,pSIZE );	// read model id
		model_id = _ttoi(p);
			
		_ftscanf_s( fp, _T("%s"), &model_filename,ARRAYSIZE(model_filename) ); // read filename for object

		_ftscanf_s( fp, _T("%s"), &p,pSIZE );	//  ignore comment "tex"  

		_ftscanf_s( fp, _T("%s"), &p,pSIZE );	// read texture alias id				

		if(_tcscmp(command,_T("PLAYER")) == 0)
		{
			_ftscanf_s( fp, _T("%s"), &p,pSIZE ); // ignore comment pos
					
			_ftscanf_s( fp, _T("%s"), &p,pSIZE ); // x pos
			pCMyApp->player_list[type_num].x = (float)_ttoi(p);
					
			_ftscanf_s( fp, _T("%s"), &p,pSIZE ); // y pos
			pCMyApp->player_list[type_num].y = (float)_ttoi(p);
					
			_ftscanf_s( fp, _T("%s"), &p,pSIZE ); // z pos
			pCMyApp->player_list[type_num].z = (float)_ttoi(p);
					
			_ftscanf_s( fp, _T("%s"), &p,pSIZE ); // ignore comment angle
			_ftscanf_s( fp, _T("%s"), &p,pSIZE ); // angle
			pCMyApp->player_list[type_num].rot_angle = (float)_ttoi(p);

			pCMyApp->player_list[type_num].model_id = model_id;
			pCMyApp->num_players++;

			_ftscanf_s( fp, _T("%s"), &p,pSIZE ); // ignore comment scale
			_ftscanf_s( fp, _T("%f"), &f );
			scale = f;

			_ftscanf_s( fp, _T("%s"), &p,pSIZE ); // Don't draw players external weapon
			if(_tcscmp(p, _T("NO_EXTERNAL_WEP")) == 0)
				pCMyApp->player_list[type_num].draw_external_wep = FALSE;
			
			if(_tcscmp(p, _T("USE_EXTERNAL_WEP")) == 0)
				pCMyApp->player_list[type_num].draw_external_wep = TRUE;
			

			player_count++;
			
			LoadPlayerAnimationSequenceList(model_id);
			LoadDebugAnimationSequenceList(hwnd, model_filename, model_id);
			command_recognised = TRUE;
		}

		if(_tcscmp(command,_T("CAR")) == 0)
		{
			_ftscanf_s( fp, _T("%s"), &p,pSIZE ); // ignore comment pos
					
			_ftscanf_s( fp, _T("%s"), &p,pSIZE ); // x pos
			pCMyApp->car_list[type_num].x = (float)_ttoi(p);
					
			_ftscanf_s( fp, _T("%s"), &p,pSIZE ); // y pos
			pCMyApp->car_list[type_num].y = (float)_ttoi(p);
					
			_ftscanf_s( fp, _T("%s"), &p,pSIZE ); // z pos
			pCMyApp->car_list[type_num].z = (float)_ttoi(p);
					
			_ftscanf_s( fp, _T("%s"), &p,pSIZE ); // ignore comment angle
			_ftscanf_s( fp, _T("%s"), &p,pSIZE ); // angle
			pCMyApp->car_list[type_num].rot_angle = (float)_ttoi(p);

			pCMyApp->car_list[type_num].model_id = model_id;
			pCMyApp->num_cars++;

			_ftscanf_s( fp, _T("%s"), &p,pSIZE ); // ignore comment scale
			_ftscanf_s( fp, _T("%f"), &f );
			scale = f;

			car_count++;
			command_recognised = TRUE;
		}

		if(_tcscmp(command,_T("YOURGUN")) == 0)
		{
			_ftscanf_s( fp, _T("%s"), &p,pSIZE ); // ignore comment pos
				
			_ftscanf_s( fp, _T("%s"), &p,pSIZE ); // x pos
			pCMyApp->your_gun[type_num].x_offset = (float)_ttoi(p);
					
			_ftscanf_s( fp, _T("%s"), &p,pSIZE ); // y pos
			pCMyApp->your_gun[type_num].z_offset = (float)_ttoi(p);
				
			_ftscanf_s( fp, _T("%s"), &p,pSIZE ); // z pos
			pCMyApp->your_gun[type_num].y_offset = (float)_ttoi(p);
					
			_ftscanf_s( fp, _T("%s"), &p,pSIZE ); // ignore comment angle
			_ftscanf_s( fp, _T("%s"), &p,pSIZE ); // angle
			pCMyApp->your_gun[type_num].rot_angle = (float)_ttoi(p);

			pCMyApp->your_gun[type_num].model_id = model_id;
			pCMyApp->num_your_guns++;

			_ftscanf_s( fp, _T("%s"), &p,pSIZE ); // ignore comment scale
			_ftscanf_s( fp, _T("%f"), &f );
			scale = f;

			your_gun_count++;
			LoadYourGunAnimationSequenceList(model_id);
			//LoadDebugAnimationSequenceList(hwnd, model_filename, model_id, wptr);
			command_recognised = TRUE;
		}

		if(_tcscmp(command,_T("DEBUG")) == 0)
		{
			_ftscanf_s( fp, _T("%s"), &p,pSIZE ); // ignore comment pos
				
			_ftscanf_s( fp, _T("%s"), &p,pSIZE ); // x pos
			pCMyApp->debug[type_num].x = (float)_ttoi(p);
					
			_ftscanf_s( fp, _T("%s"), &p,pSIZE ); // y pos
			pCMyApp->debug[type_num].y = (float)_ttoi(p);
				
			_ftscanf_s( fp, _T("%s"), &p,pSIZE ); // z pos
			pCMyApp->debug[type_num].z = (float)_ttoi(p);
					
			_ftscanf_s( fp, _T("%s"), &p,pSIZE ); // ignore comment angle
			_ftscanf_s( fp, _T("%s"), &p,pSIZE ); // angle
			pCMyApp->debug[type_num].rot_angle = (float)_ttoi(p);

			pCMyApp->debug[type_num].model_id = model_id;
			pCMyApp->num_debug_models++;
			
			_ftscanf_s( fp, _T("%s"), &p,pSIZE ); // ignore comment scale
			_ftscanf_s( fp, _T("%f"), &f );
			scale = f;

			LoadDebugAnimationSequenceList(hwnd, model_filename, model_id);

			command_recognised = TRUE;
		}

		if(_tcscmp(command,_T("OPGUN")) == 0)
		{			
			pCMyApp->other_players_guns[type_num].model_id = model_id;
			pCMyApp->num_op_guns++;
			
			_ftscanf_s( fp, _T("%s"), &p,pSIZE ); // ignore comment scale
			_ftscanf_s( fp, _T("%f"), &f );
			scale = f;

			op_gun_count++;
			command_recognised = TRUE;
		}
		
		if(_tcscmp(command,_T("END_FILE")) == 0)
		{
			done = 1;
			command_recognised = TRUE;
		}

		if(command_recognised == TRUE)
		{
			if(Model_loaded_flags[model_id] == FALSE)
			{
				PrintMessage(hwnd, _T("loading  "), model_filename, SCN_AND_FILE);
			
				if(pCMyApp->model_format == k3DS_MODEL)
					pC3DS->Import3DS(hwnd, model_filename, model_id, scale);
			
				Model_loaded_flags[model_id] = TRUE;
			}
		}
		else
		{
			PrintMessage(hwnd, _T("command unrecognized "), command, SCN_AND_FILE);
			MessageBox(hwnd, command, _T("command unrecognized"),MB_OK);
			return FALSE;
		}
		

	} // end while loop

	num_imported_models_loaded = player_count + your_gun_count + op_gun_count;
	PrintMessage(hwnd, _T("\n"), NULL, LOGFILE_ONLY);
	fclose(fp);	
	return TRUE;
}


void CLoadWorld::LoadDebugAnimationSequenceList(HWND hwnd, const TCHAR *filename, int model_id)
{
	FILE *fp = NULL;    
	int done = 0;
	int start_frame;
	int stop_frame;
	int sequence_number;
	int file_ex_start = 0;
	int i;
	TCHAR command[256]={};
	TCHAR p[256]={}; const int pSIZE=ARRAYSIZE(p);
	TCHAR name[256]={};

	BOOL command_recognised;	


	_tcscpy_s(g_model_filename, filename);

	for (i = 0; i < 255; i++)
	{
		if(g_model_filename[i] == '.')
		{
			if(g_model_filename[i+1] == '.')
			{
				i++;
			}
			else
			{
				file_ex_start = i;
				break;
			}
		}
	}

	_tcscpy_s(&g_model_filename[file_ex_start+1], ARRAYSIZE(g_model_filename)-file_ex_start-1, _T("seq"));

	_tfopen_s(&fp, g_model_filename, _T("r"));

    if(fp==NULL)
	{     
		PrintMessage(hwnd, _T("ERROR can't load "), g_model_filename, SCN_AND_FILE);
		MessageBox(hwnd, g_model_filename, _T("Error can't load file"),MB_OK);
		return;
    }

	PrintMessage(hwnd, _T("Loading "), g_model_filename, SCN_AND_FILE);

	_ftscanf_s( fp, _T("%s"), &command,ARRAYSIZE(command) );

	if(_tcscmp(command,_T("FILENAME")) == 0)
		_ftscanf_s( fp, _T("%s"), &p,pSIZE );	// ignore comment
	else
		return;

	// SEQUENCE 0  START_FRAME   0  STOP_FRAME  39 NAME Stand
	// SEQUENCE 1  START_FRAME  40  STOP_FRAME  45 NAME Run
	// SEQUENCE 2  START_FRAME  46  STOP_FRAME  53 NAME Attack

	while(done == 0)
	{
		command_recognised = FALSE;
	
		// read sequence number
		_ftscanf_s( fp, _T("%s"), &command,ARRAYSIZE(command) ); 
		if(_tcsicmp(command, _T("SEQUENCE")) == 0)
		{	
			_ftscanf_s( fp, _T("%d"), &sequence_number );

			// read start frame
			_ftscanf_s( fp, _T("%s"), &p,pSIZE );
			if(_tcsicmp(p, _T("START_FRAME")) != 0)
				return;

			_ftscanf_s( fp, _T("%d"), &start_frame );	
			pCMyApp->pmdata[model_id].sequence_start_frame[sequence_number] = start_frame;
			

			// read stop frame
			_ftscanf_s( fp, _T("%s"), &p,pSIZE );
			if(_tcsicmp(p, _T("STOP_FRAME")) != 0)
				return;

			_ftscanf_s( fp, _T("%d"), &stop_frame );	
			pCMyApp->pmdata[model_id].sequence_stop_frame[sequence_number] = stop_frame;

			// read name
			_ftscanf_s( fp, _T("%s"), &p,pSIZE );
			if(_tcsicmp(p, _T("NAME")) != 0)
				return;

			_ftscanf_s( fp, _T("%s"), &name,ARRAYSIZE(name) );	
			
		}

		if(_tcsicmp(command, _T("END_FILE")) == 0)
			done = 1;		
		
	} // end while loop

	fclose(fp);	
}


void CLoadWorld::LoadPlayerAnimationSequenceList(int model_id)
{
	int i;

	i = model_id;

	pCMyApp->pmdata[i].sequence_start_frame[0] =0;  // Stand
	pCMyApp->pmdata[i].sequence_stop_frame [0] =39;

	pCMyApp->pmdata[i].sequence_start_frame[1] =40; // run
	pCMyApp->pmdata[i].sequence_stop_frame [1] =45;

	pCMyApp->pmdata[i].sequence_start_frame[2] =46; // attack
	pCMyApp->pmdata[i].sequence_stop_frame [2] =53;

	pCMyApp->pmdata[i].sequence_start_frame[3] =54; // pain1
	pCMyApp->pmdata[i].sequence_stop_frame [3] =57;

	pCMyApp->pmdata[i].sequence_start_frame[4] =58; // pain2
	pCMyApp->pmdata[i].sequence_stop_frame [4] =61;

	pCMyApp->pmdata[i].sequence_start_frame[5] =62; // pain3
	pCMyApp->pmdata[i].sequence_stop_frame [5] =65;

	pCMyApp->pmdata[i].sequence_start_frame[6] =66; // jump
	pCMyApp->pmdata[i].sequence_stop_frame [6] =71;

	pCMyApp->pmdata[i].sequence_start_frame[7] =72; // flip
	pCMyApp->pmdata[i].sequence_stop_frame [7] =83;

	pCMyApp->pmdata[i].sequence_start_frame[8] =84; // Salute
	pCMyApp->pmdata[i].sequence_stop_frame [8] =94;

	pCMyApp->pmdata[i].sequence_start_frame[9] =95; // Taunt
	pCMyApp->pmdata[i].sequence_stop_frame [9] =111;

	pCMyApp->pmdata[i].sequence_start_frame[10] =112; // Wave
	pCMyApp->pmdata[i].sequence_stop_frame [10] =122;

	pCMyApp->pmdata[i].sequence_start_frame[11] =123; // Point
	pCMyApp->pmdata[i].sequence_stop_frame [11] =134;
	
}

void CLoadWorld::LoadYourGunAnimationSequenceList(int model_id)
{
	int i;

	i = model_id;

	pCMyApp->your_gun[0].current_sequence = 2;
	pCMyApp->your_gun[0].current_frame = 13;
	pCMyApp->your_gun[1].current_sequence = 2;
	pCMyApp->your_gun[1].current_frame = 13;
	pCMyApp->your_gun[2].current_sequence = 0;
	pCMyApp->your_gun[2].current_frame = 0;
		
	pCMyApp->pmdata[i].sequence_start_frame[0] =0;  // Active1
	pCMyApp->pmdata[i].sequence_stop_frame [0] =10;

	pCMyApp->pmdata[i].sequence_start_frame[1] =11; // fire
	pCMyApp->pmdata[i].sequence_stop_frame [1] =12;

	pCMyApp->pmdata[i].sequence_start_frame[2] =13; // Idle
	pCMyApp->pmdata[i].sequence_stop_frame [2] =39;

	pCMyApp->pmdata[i].sequence_start_frame[3] =40; // putaway
	pCMyApp->pmdata[i].sequence_stop_frame [3] =44;

	pCMyApp->pmdata[i].sequence_start_frame[4] =45; // reload
	pCMyApp->pmdata[i].sequence_stop_frame [4] =63;

	pCMyApp->pmdata[i].sequence_start_frame[5] =64; // burstfire
	pCMyApp->pmdata[i].sequence_stop_frame [5] =69;

	pCMyApp->pmdata[i].sequence_start_frame[6] =70; // lastround
	pCMyApp->pmdata[i].sequence_stop_frame [6] =71;

}


