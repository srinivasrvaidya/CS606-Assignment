#include<GL/glut.h>
#include<stdio.h>
#include<string.h>
#include<math.h>

#include "Utility.h"

extern float plyVertex[40000][5], plyVertexNormal[40000][4], nrml[80000][3];
extern int plyFace[80000][4], plyfaceCount, plyVertexCount;


void Utility::paintString(int x,int y,int size, char *str)
{
	glRasterPos2i(10,20);
	for(int i=0;i<size;i++)
	{
		char s = str[i];
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,s);
	}				
	glFlush();	
}
void Utility::loadPlyfile(char *filename)
{
	int headerEndFlag=0,vertexEndFlag=0;
	int temp,i=0,j=1;
	char str[500],tempStr1[20],tempStr2[20];
	char c;
	
	FILE *plyReader;
	
	if ((plyReader = fopen(filename, "r")) == NULL) {
		printf("File not found !");
		return;
	}
		
	// running down the comments.
	while (strstr(str, "comment")) {
	
		fgets(str, 100, plyReader);
	}	
	
	while(!feof(plyReader))
	{
		if(fgets(str,200,plyReader))	{	
			if(headerEndFlag!=1) {  // reading header.
				if(strstr(str,"element vert"))
					sscanf(str,"%s %s %d",tempStr1,tempStr2,&plyVertexCount); 	
				if(strstr(str,"element face"))
					sscanf(str,"%s %s %d",tempStr1,tempStr2,&plyfaceCount);
				if(strstr(str,"end_header"))
					headerEndFlag=1;
				normalise();
		}
		else 
		if(vertexEndFlag!=1){
			if(i<plyVertexCount){
				sscanf(str,"%f %f %f %f %f",&plyVertex[i][0],&plyVertex[i][1],&plyVertex[i][2],&plyVertex[i][3],&plyVertex[i][4]);
				i++;			
			}
			else{
				vertexEndFlag=1;
				sscanf(str,"%d %d %d %d ",&temp,&plyFace[0][0],&plyFace[0][1],&plyFace[0][2]);	
				calcNormal(0,plyVertex[plyFace[0][0]],plyVertex[plyFace[0][1]],plyVertex[plyFace[0][2]]);
				}
			}
			else{
				if(j<plyfaceCount){
					sscanf(str,"%d %d %d %d ",&temp,&plyFace[j][0],&plyFace[j][1],&plyFace[j][2]);
					calcNormal(j,plyVertex[plyFace[j][0]],plyVertex[plyFace[j][1]],plyVertex[plyFace[j][2]]);
					j++;
				}
			}
		}		
	}
}
void Utility::normalise()
{
	int i;
	for(i=0;i<plyVertexCount;i++)
	{
		plyVertexNormal[i][0]=plyVertexNormal[i][1]=plyVertexNormal[i][2]=plyVertexNormal[i][3]=0;
	}
}
	
void Utility::calcNormal( int faceNum,float *crd1, float *crd2, float *crd3 )
{
  
   float vectorA[3], vectorB[3], vectorResultant[3], val;
   // building vectors.
   vectorA[0] = crd1[0] - crd2[0];
   vectorA[1] = crd1[1] - crd2[1];
   vectorA[2] = crd1[2] - crd2[2];
 
   vectorB[0] = crd1[0] - crd3[0];
   vectorB[1] = crd1[1] - crd3[1];
   vectorB[2] = crd1[2] - crd3[2];
 
   // findint the cross product 
   vectorResultant[0] = vectorA[1] * vectorB[2] - vectorB[1] * vectorA[2];
   vectorResultant[1] = vectorB[0] * vectorA[2] - vectorA[0] * vectorB[2];
   vectorResultant[2] = vectorA[0] * vectorB[1] - vectorB[0] * vectorA[1];
 
	val = sqrt( vectorResultant[0]*vectorResultant[0] + vectorResultant[1]*vectorResultant[1] + vectorResultant[2]*vectorResultant[2] );
 
	nrml[faceNum][0] = vectorResultant[0]/val;
	nrml[faceNum][1] = vectorResultant[1]/val;
	nrml[faceNum][2] = vectorResultant[2]/val;
 	
	plyVertexNormal[plyFace[faceNum][0]][0]+=nrml[faceNum][0];
	plyVertexNormal[plyFace[faceNum][1]][0]+=nrml[faceNum][0];
	plyVertexNormal[plyFace[faceNum][2]][0]+=nrml[faceNum][0];
	
	plyVertexNormal[plyFace[faceNum][0]][1]+=nrml[faceNum][1];
	plyVertexNormal[plyFace[faceNum][1]][1]+=nrml[faceNum][1];
 	plyVertexNormal[plyFace[faceNum][2]][1]+=nrml[faceNum][1];

	plyVertexNormal[plyFace[faceNum][0]][2]+=nrml[faceNum][2];
	plyVertexNormal[plyFace[faceNum][1]][2]+=nrml[faceNum][2];
	plyVertexNormal[plyFace[faceNum][2]][2]+=nrml[faceNum][2];
	
	plyVertexNormal[plyFace[faceNum][0]][3]+=1.0;
	plyVertexNormal[plyFace[faceNum][1]][3]+=1.0;
 	plyVertexNormal[plyFace[faceNum][2]][3]+=1.0;
}
	

	

