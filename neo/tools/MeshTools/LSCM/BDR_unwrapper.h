/**
 * $Id: BDR_unwrapper.h,v 1.5 2005/11/22 15:00:32 broken Exp $
 *
 * ***** BEGIN GPL/BL DUAL LICENSE BLOCK *****
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version. The Blender
 * Foundation also sells licenses for use in proprietary software under
 * the Blender License.  See http://www.blender.org/BL/ for information
 * about this.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * The Original Code is Copyright (C) 2001-2002 by NaN Holding BV.
 * All rights reserved.
 *
 * The Original Code is: all of this file.
 *
 * Contributor(s): none yet.
 *
 * ***** END GPL/BL DUAL LICENSE BLOCK *****
 */




#ifndef BDR_UNWRAPPER_H
#define BDR_UNWRAPPER_H

#include "NewMesh.h"
#include "../lib/RKEdge.h"



void MeshUpdateUVs(int NumberFaces, int* FaceIndices, int* UVIndices, float* PinnedProj);

void MeshAddVerts(int NumberVerts, float* Vertices, Mesh* pMeshIn);
void MeshAddFaces(int NumberFaces, int* FaceIndices, int* UVIndices, float* PinnedProj, int* PinnedUVs, Mesh* pMeshIn);
void MeshAddEdges(int NumberEdges, RKEdge* EdgeVerts, Mesh* pMeshIn, bool NoSeams);
void MeshAddPinned(int NumberOfPinned, float* PinnedProj, int* PinnedIndex);
void CompressUVs();
int InsertUVIntoBank(float UV[2]);
void CleanUp(Mesh* pMesh);


void StartStretch();
void DoStretch(int BlendVal);
void EndStretch(bool KeepResults);




void set_seamtface(void); /* set TF_SEAM flags in tfaces */
void select_linked_tfaces_with_seams(int mode,  unsigned int index);

void unwrap_lscm(short seamcut); /* unwrap faces selected in 3d view */
void minimize_stretch_tface_uv(void); /* optimize faces selected in uv editor */

/* for live mode: no undo pushes, caching for quicky re-unwrap */
void unwrap_lscm_live_begin(void);
void unwrap_lscm_live_re_solve(void);
void unwrap_lscm_live_end(short cancel);


#endif /* BDR_UNWRAPPER_H */

