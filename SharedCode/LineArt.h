#pragma once
#include "ofMain.h"

/*
 LineArt renders an ofMesh as a wireframe with occlusion, and optionally only
 the depth edges. It happens in vertex space, so one model that partially
 occludes another will not create a depth edge.
 
 To use LineArt, just call LineArt::draw() on your ofMesh. You may optionally
 use ofEnableSmoothing() and ofSetLineWidth() with the function.
*/

namespace LineArt {
	inline void drawMesh(ofMesh& mesh, bool useNormals, bool useColors) {
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3, GL_FLOAT, sizeof(ofVec3f), mesh.getVerticesPointer());
		if(useNormals && mesh.getNumNormals()) {
			glEnableClientState(GL_NORMAL_ARRAY);
			glNormalPointer(GL_FLOAT, sizeof(ofVec3f), mesh.getNormalsPointer());
		}
		if(useColors && mesh.getNumColors()) {
			glEnableClientState(GL_COLOR_ARRAY);
			glColorPointer(4, GL_FLOAT, sizeof(ofFloatColor), mesh.getColorsPointer());
		}
		if(mesh.getNumIndices()){
			glDrawElements(ofGetGLPrimitiveMode(mesh.getMode()), mesh.getNumIndices(), GL_UNSIGNED_INT, mesh.getIndexPointer());
		}else{
			glDrawArrays(ofGetGLPrimitiveMode(mesh.getMode()), 0, mesh.getNumVertices());
		}
		glDisableClientState(GL_NORMAL_ARRAY);
		glDisableClientState(GL_COLOR_ARRAY);
	}
	inline void draw(ofMesh& mesh, bool depthEdgesOnly = true, ofColor fill = ofColor(0, 0, 0, 0), ofShader* shader = NULL) {
		ofColor stroke = ofGetStyle().color;
		
		glPushAttrib(GL_ALL_ATTRIB_BITS);
		
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		if(ofGetStyle().smoothing) {
			glEnable(GL_LINE_SMOOTH);
		}

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glPolygonMode(GL_FRONT, GL_FILL);
		glDepthFunc(GL_LESS);
		glCullFace(GL_BACK);
		ofSetColor(fill);
		drawMesh(mesh, true, false);
		
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		ofMatrix4x4 mat;
		mat(3, 3) = 1.01;
		glMultMatrixf(mat.getPtr());
		glMatrixMode(GL_MODELVIEW);
		
		glLineWidth(ofGetStyle().lineWidth);
		glDepthFunc(GL_LEQUAL);
		if(depthEdgesOnly) {
			glPolygonMode(GL_BACK, GL_LINE);
			glCullFace(GL_FRONT);
		} else {
			glPolygonMode(GL_FRONT, GL_LINE);
			glCullFace(GL_BACK);
		}
		ofSetColor(stroke);
		if(shader != NULL) {
			shader->begin();
		}
		drawMesh(mesh, true, true);
		if(shader != NULL) {
			shader->end();
		}
		glPopAttrib();
		
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
	}
}
